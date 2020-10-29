#include <stdexcept>
#include <iostream>
#include "RaspiStill.hpp"

extern "C" {
    #include "RaspiHelpers.h"
}

RaspiStill::RaspiStill() {
    bcm_host_init();

    // Register our application with the logging system
    vcos_log_register("RaspiStillCppLib", VCOS_LOG_CATEGORY);


    signal(SIGINT, default_signal_handler);

    // Disable USR1 and USR2 for the moment - may be reenabled if go in to signal capture mode
    signal(SIGUSR1, SIG_IGN);
    signal(SIGUSR2, SIG_IGN);

    default_status(&(this->m_State));

    // Get camera params for easy access
    m_pCameraParams = &(this->m_State.camera_parameters);

    // Setup sensor specific parameters
    get_sensor_defaults(this->m_State.common_settings.cameraNum, this->m_State.common_settings.camera_name,
                       &(this->m_State.common_settings.width), &(this->m_State.common_settings.height));
}

RaspiStill::~RaspiStill() {

}

bool RaspiStill::Initialize() {

    if (create_camera_component(&this->m_State) != MMAL_SUCCESS)
    {
        vcos_log_error("%s: Failed to create camera component", __func__);
        throw std::runtime_error("%s: Failed to create camera component");
    }
    else if ((status = raspipreview_create(&this->m_State.preview_parameters)) != MMAL_SUCCESS)
    {
        vcos_log_error("%s: Failed to create preview component", __func__);
        destroy_camera_component(&this->m_State);
        throw std::runtime_error("%s: Failed to create preview component");
    }
    else if ((status = create_encoder_component(&this->m_State)) != MMAL_SUCCESS)
    {
        vcos_log_error("%s: Failed to create encode component", __func__);
        raspipreview_destroy(&this->m_State.preview_parameters);
        destroy_camera_component(&this->m_State);
        throw std::runtime_error("%s: Failed to create encode component");
    }
    else
    {
        // PORT_USERDATA callback_data;

        // if (this->m_State.common_settings.verbose)
        //     fprintf(stderr, "Starting component connection stage\n");

        m_pCameraPreviewPort = this->m_State.camera_component->output[MMAL_CAMERA_PREVIEW_PORT];
        m_pCameraVideoPort = this->m_State.camera_component->output[MMAL_CAMERA_VIDEO_PORT];
        m_pCameraStillPort = this->m_State.camera_component->output[MMAL_CAMERA_CAPTURE_PORT];
        m_pEncoderInputPort = this->m_State.encoder_component->input[0];
        m_pEncoderOutputPort = this->m_State.encoder_component->output[0];

        if (this->m_State.burstCaptureMode &&
            this->m_State.camera_parameters.exposureMode == MMAL_PARAM_EXPOSUREMODE_OFF &&
            this->m_State.camera_parameters.shutter_speed &&
            this->m_State.camera_parameters.analog_gain && this->m_State.camera_parameters.stats_pass)
        {
            mmal_port_parameter_set_boolean(this->m_State.camera_component->control, MMAL_PARAMETER_CAMERA_BURST_CAPTURE, 1);
        }

        // Connecting preview port
        {
            m_pPreviewInputPort = this->m_State.preview_parameters.preview_component->input[0];
            status = connect_ports(m_pCameraPreviewPort, m_pPreviewInputPort, &this->m_State.preview_connection);
        }

        if (status == MMAL_SUCCESS)
        {
            // VCOS_STATUS_T vcos_status;

            if (this->m_State.common_settings.verbose)
                fprintf(stderr, "Connecting camera stills port to encoder input port\n");

            // Now connect the camera to the encoder
            status = connect_ports(m_pCameraStillPort, m_pEncoderInputPort, &this->m_State.encoder_connection);

            if (status != MMAL_SUCCESS)
            {
                vcos_log_error("%s: Failed to connect camera video port to encoder input", __func__);
                throw std::runtime_error("Failed to connect camera video port to encoder input");
            }

        }
        else
        {
            mmal_status_to_int(status);
            vcos_log_error("%s: Failed to connect camera to preview", __func__);
        }
    }
    return true;
}

bool RaspiStill::CaptureToFile(std::string sFile){
    this->SetOutputFile(sFile);
    return this->Capture(NULL, 0, NULL);
}

bool RaspiStill::Capture(std::vector<uint8_t>& data){
    size_t alloc_len = this->GetWidth()*this->GetHeight()*3 + 54;
    unsigned char *data_arr = new unsigned char[alloc_len];
    size_t final_len;
    this->Capture(data_arr, alloc_len, &final_len);
    data.insert(data.end(), data_arr, data_arr + final_len);
    delete[] data_arr;
}

bool RaspiStill::Capture(unsigned char *pPreallocatedData, size_t nPreallocatedLength, size_t* pFinalLength){
    if (pPreallocatedData)
    {
        if (nPreallocatedLength == 0)
        {
            throw std::runtime_error("Preallocated memory provided but of size 0");
        }
    }

    VCOS_STATUS_T vcos_status;
    PORT_USERDATA callback_data;

    // Set up our userdata - this is passed though to the callback where we need the information.
    // Null until we open our filename
    callback_data.file_handle = NULL;
    callback_data.pstate = &this->m_State;
    callback_data.data = pPreallocatedData;
    callback_data.final_offset = pFinalLength;
    callback_data.preallocated_length = nPreallocatedLength;
    callback_data.offset = 0;
    vcos_status = vcos_semaphore_create(&callback_data.complete_semaphore, "RaspiStill-sem", 0);

    vcos_assert(vcos_status == VCOS_SUCCESS);

    if (status != MMAL_SUCCESS)
    {
        vcos_log_error("Failed to setup encoder output");
        throw std::runtime_error("Failed to setup encoder output");
    }

    int frame, keep_looping = 1;
    FILE *output_file = NULL;
    char *use_filename = NULL;   // Temporary filename while image being written
    char *final_filename = NULL; // Name that file gets once writing complete

    frame = this->m_State.frameStart - 1;

    while (keep_looping)
    {
        keep_looping = wait_for_next_frame(&this->m_State, &frame);

        if (this->m_State.datetime)
        {
            time_t rawtime;
            struct tm *timeinfo;

            time(&rawtime);
            timeinfo = localtime(&rawtime);

            frame = timeinfo->tm_mon + 1;
            frame *= 100;
            frame += timeinfo->tm_mday;
            frame *= 100;
            frame += timeinfo->tm_hour;
            frame *= 100;
            frame += timeinfo->tm_min;
            frame *= 100;
            frame += timeinfo->tm_sec;
        }
        if (this->m_State.timestamp)
        {
            frame = (int)time(NULL);
        }

        // Open the file
        if (this->m_State.common_settings.filename)
        {
            if (this->m_State.common_settings.filename[0] == '-')
            {
                output_file = stdout;
            }
            else
            {
                vcos_assert(use_filename == NULL && final_filename == NULL);
                status = create_filenames(&final_filename, &use_filename, this->m_State.common_settings.filename, frame);
                if (status != MMAL_SUCCESS)
                {
                    vcos_log_error("Unable to create filenames");
                    throw std::runtime_error("Unable to create filenames");
                }

                if (this->m_State.common_settings.verbose)
                    fprintf(stderr, "Opening output file %s\n", final_filename);
                // Technically it is opening the temp~ filename which will be renamed to the final filename

                output_file = fopen(use_filename, "wb");

                if (!output_file)
                {
                    // Notify user, carry on but discarding encoded output buffers
                    vcos_log_error("%s: Error opening output file: %s\nNo output file will be generated\n", __func__, use_filename);
                }
            }

            callback_data.file_handle = output_file;
        }

        if ( output_file || pPreallocatedData )
        {
            int num, q;

            // Must do this before the encoder output port is enabled since
            // once enabled no further exif data is accepted
            if (this->m_State.enableExifTags)
            {
                struct gps_data_t *gps_data = raspi_gps_lock();
                add_exif_tags(&this->m_State, gps_data);
                raspi_gps_unlock();
            }
            else
            {
                mmal_port_parameter_set_boolean(
                    this->m_State.encoder_component->output[0], MMAL_PARAMETER_EXIF_DISABLE, 1);
            }

            // Same with raw, apparently need to set it for each capture, whilst port
            // is not enabled
            if (this->m_State.wantRAW)
            {
                if (mmal_port_parameter_set_boolean(m_pCameraStillPort, MMAL_PARAMETER_ENABLE_RAW_CAPTURE, 1) != MMAL_SUCCESS)
                {
                    vcos_log_error("RAW was requested, but failed to enable");
                }
            }

            // There is a possibility that shutter needs to be set each loop.
            if (mmal_status_to_int(mmal_port_parameter_set_uint32(this->m_State.camera_component->control, MMAL_PARAMETER_SHUTTER_SPEED, this->m_State.camera_parameters.shutter_speed)) != MMAL_SUCCESS)
                vcos_log_error("Unable to set shutter speed");

            // Enable the encoder output port
            m_pEncoderOutputPort->userdata = reinterpret_cast<MMAL_PORT_USERDATA_T *>(&callback_data);

            if (this->m_State.common_settings.verbose)
                fprintf(stderr, "Enabling encoder output port\n");

            // Enable the encoder output port and tell it its callback function
            status = mmal_port_enable(m_pEncoderOutputPort, encoder_buffer_callback);

            // Send all the buffers to the encoder output port
            num = mmal_queue_length(this->m_State.encoder_pool->queue);

            for (q = 0; q < num; q++)
            {
                MMAL_BUFFER_HEADER_T *buffer = mmal_queue_get(this->m_State.encoder_pool->queue);

                if (!buffer)
                    vcos_log_error("Unable to get a required buffer %d from pool queue", q);

                if (mmal_port_send_buffer(m_pEncoderOutputPort, buffer) != MMAL_SUCCESS)
                    vcos_log_error("Unable to send a buffer to encoder output port (%d)", q);
            }

            if (this->m_State.burstCaptureMode)
            {
                mmal_port_parameter_set_boolean(this->m_State.camera_component->control, MMAL_PARAMETER_CAMERA_BURST_CAPTURE, 1);
            }

            if (this->m_State.camera_parameters.enable_annotate)
            {
                if ((this->m_State.camera_parameters.enable_annotate & ANNOTATE_APP_TEXT) && this->m_State.common_settings.gps)
                {
                    char *text = raspi_gps_location_string();
                    raspicamcontrol_set_annotate(this->m_State.camera_component, this->m_State.camera_parameters.enable_annotate,
                                                 text,
                                                 this->m_State.camera_parameters.annotate_text_size,
                                                 this->m_State.camera_parameters.annotate_text_colour,
                                                 this->m_State.camera_parameters.annotate_bg_colour,
                                                 this->m_State.camera_parameters.annotate_justify,
                                                 this->m_State.camera_parameters.annotate_x,
                                                 this->m_State.camera_parameters.annotate_y);
                    free(text);
                }
                else
                    raspicamcontrol_set_annotate(this->m_State.camera_component, this->m_State.camera_parameters.enable_annotate,
                                                 this->m_State.camera_parameters.annotate_string,
                                                 this->m_State.camera_parameters.annotate_text_size,
                                                 this->m_State.camera_parameters.annotate_text_colour,
                                                 this->m_State.camera_parameters.annotate_bg_colour,
                                                 this->m_State.camera_parameters.annotate_justify,
                                                 this->m_State.camera_parameters.annotate_x,
                                                 this->m_State.camera_parameters.annotate_y);
            }

            if (this->m_State.common_settings.verbose)
                fprintf(stderr, "Starting capture %d\n", frame);

            if (mmal_port_parameter_set_boolean(m_pCameraStillPort, MMAL_PARAMETER_CAPTURE, 1) != MMAL_SUCCESS)
            {
                vcos_log_error("%s: Failed to start capture", __func__);
            }
            else
            {
                // Wait for capture to complete
                // For some reason using vcos_semaphore_wait_timeout sometimes returns immediately with bad parameter error
                // even though it appears to be all correct, so reverting to untimed one until figure out why its erratic
                vcos_semaphore_wait(&callback_data.complete_semaphore);
                if (this->m_State.common_settings.verbose)
                    fprintf(stderr, "Finished capture %d\n", frame);
            }

            // Ensure we don't die if get callback with no open file
            callback_data.file_handle = NULL;

            if (output_file) {
                if (output_file != stdout)
                {
                    rename_file(&this->m_State, output_file, final_filename, use_filename, frame);
                }
                else
                {
                    fflush(output_file);
                }
            }
            // Disable encoder output port
            status = mmal_port_disable(m_pEncoderOutputPort);
        }

        if (use_filename)
        {
            free(use_filename);
            use_filename = NULL;
        }
        if (final_filename)
        {
            free(final_filename);
            final_filename = NULL;
        }
    } // end for (frame)

    vcos_semaphore_delete(&callback_data.complete_semaphore);
    return true;
}

bool RaspiStill::SetWidth(int nWidth){
    this->m_State.common_settings.width = nWidth;
    return true;
}
bool RaspiStill::SetHeight(int nHeight){
    this->m_State.common_settings.height = nHeight;
    return true;
}

bool RaspiStill::SetOutputFile(std::string sFile){
    char *s = new char [sFile.length() + 10]; // leaving space for timelapse generated changes to filename 
    strncpy(s, sFile.c_str(), sFile.length() + 1);
    this->m_State.common_settings.filename = s; 
    return true;
}
int RaspiStill::GetWidth(){
    return this->m_State.common_settings.width;    
}
int RaspiStill::GetHeight(){
    return this->m_State.common_settings.height;    
}
bool RaspiStill::SetJPEGQuality(int nQuality){
    this->m_State.quality = nQuality;
    if (this->m_State.quality > 100) this->m_State.quality = 100;
    return true;
}

bool RaspiStill::SetRequestRAW(bool bRequestRaw){
    this->m_State.wantRAW = 1;
    return true;
}

bool RaspiStill::SetFrameStart(int nFrameStart){
    this->m_State.frameStart = nFrameStart;
    return true;
}

bool RaspiStill::SetUseDateTime(bool bUseDateTime){
    this->m_State.datetime = bUseDateTime ? 1 : 0;
    return true;
}

bool RaspiStill::SetUseTimeStamp(bool bUseTimeStamp){
    this->m_State.timestamp = bUseTimeStamp ? 1 : 0;
    return true;
}

bool RaspiStill::SetTimeout(int nTimeout){
    this->m_State.timeout = nTimeout;
    return true;
}

bool RaspiStill::SetUseThumbnail(bool bUseThumbnail, int nThumbnailWidth, int nThumbnailHeight, int nThumbnailQuality = 50){
    if (bUseThumbnail)
    {
        this->m_State.thumbnailConfig.enable = 1;
        this->m_State.thumbnailConfig.width = nThumbnailWidth;
        this->m_State.thumbnailConfig.height = nThumbnailHeight;
        this->m_State.thumbnailConfig.quality = nThumbnailQuality;
    }
    else
    {
        this->m_State.thumbnailConfig.enable = 0;
    }
    return true;
}

bool RaspiStill::SetEncoding(std::string sEncoding){
    for (int i=0; i < encoding_xref_size; i++) {
        if(encoding_xref[i].format.compare(sEncoding) == 0)
        {
            this->m_State.encoding = encoding_xref[i].encoding;
            return true;
        }
    }
    return false;
}

bool RaspiStill::SetSharpness(int nSharpness){
    this->m_State.camera_parameters.sharpness = nSharpness;
    return true;
}

bool RaspiStill::SetContrast(int nContrast){
    this->m_State.camera_parameters.contrast = nContrast;
    return true;
}     

bool RaspiStill::SetBrightness(int nBrightness){
    if (nBrightness < 0 || nBrightness > 100) return false;
    this->m_State.camera_parameters.brightness = nBrightness;
    return true;
}
bool RaspiStill::SetSaturation(int nSaturation){
    if (nSaturation < -100 || nSaturation > 100) return false;
    this->m_State.camera_parameters.saturation = nSaturation;
    return true;
}    
bool RaspiStill::SetISO(int nISO){
    this->m_State.camera_parameters.ISO = nISO;
    return true;
}    
bool RaspiStill::SetVideoStabilization(bool bStabilize){
    this->m_State.camera_parameters.videoStabilisation = bStabilize? 1 : 0;
    return true;
}    
bool RaspiStill::SetEVCompensation(int nCompensation){
    if (nCompensation < -10 || nCompensation > 10) return false;
    this->m_State.camera_parameters.exposureCompensation = nCompensation;
    return true;
}    
bool RaspiStill::SetExposureMode(MMAL_PARAM_EXPOSUREMODE_T eExposureMode){
    this->m_State.camera_parameters.exposureMode = eExposureMode;
    return true;
}    
bool RaspiStill::SetFlickerMode(MMAL_PARAM_FLICKERAVOID_T eFlickerMode){
    this->m_State.camera_parameters.flickerAvoidMode = eFlickerMode;
    return true;
}    
bool RaspiStill::SetAWBMode(MMAL_PARAM_AWBMODE_T eAWBMode){
    this->m_State.camera_parameters.awbMode = eAWBMode;
    return true;
}    
bool RaspiStill::SetImageFX(MMAL_PARAM_IMAGEFX_T eImageFX){
    this->m_State.camera_parameters.imageEffect = eImageFX;
    return true;
}
bool RaspiStill::SetColourFX(bool bEnable, int nU, int nV){
    if (bEnable) {
        this->m_State.camera_parameters.colourEffects.enable = 1;
        this->m_State.camera_parameters.colourEffects.u = nU;
        this->m_State.camera_parameters.colourEffects.v = nV;
    } else {
        this->m_State.camera_parameters.colourEffects.enable = 0;
    }
    return true;
}
bool RaspiStill::SetExposureMeteringMode(MMAL_PARAM_EXPOSUREMETERINGMODE_T eMeterMode){
    this->m_State.camera_parameters.exposureMeterMode = eMeterMode;
    return true;
}
bool RaspiStill::SetRotation(int nRotation){
    this->m_State.camera_parameters.rotation = nRotation;
    return true;
} 
bool RaspiStill::SetHFlip(bool bHFlip){
    this->m_State.camera_parameters.hflip = bHFlip ? 1 : 0;
    return true;
}
bool RaspiStill::SetVFlip(bool bVFlip){
    this->m_State.camera_parameters.vflip = bVFlip ? 1 : 0;
    return true;
}
bool RaspiStill::SetROI(int nX, int nY, int nW, int nH){
      if (nX > 1.0 || nY > 1.0 || nW > 1.0 || nH > 1.0)
      {
         return false;
      }

      // Make sure we stay within bounds
      if (nX + nW > 1.0)
         nW = 1 - nX;

      if (nY + nH > 1.0)
         nH = 1 - nY;

      this->m_State.camera_parameters.roi.x = nX;
      this->m_State.camera_parameters.roi.y = nY;
      this->m_State.camera_parameters.roi.w = nW;
      this->m_State.camera_parameters.roi.h = nH;
      return true;
}
bool RaspiStill::SetShutterSpeed(int nSpeed){
    this->m_State.camera_parameters.shutter_speed = nSpeed;
    return true;
}
bool RaspiStill::SetAWBGains(int nR, int nB){
    this->m_State.camera_parameters.awb_gains_b = nB;
    this->m_State.camera_parameters.awb_gains_r = nR;
    return true;
}
bool RaspiStill::SetDRCLevelMode(MMAL_PARAMETER_DRC_STRENGTH_T eDRCLevel){
    this->m_State.camera_parameters.drc_level = eDRCLevel;
    return true;
}
bool RaspiStill::SetStatsPass(bool bStatsPass){
    this->m_State.camera_parameters.stats_pass = bStatsPass ? 1 : 0;
    return true;
}