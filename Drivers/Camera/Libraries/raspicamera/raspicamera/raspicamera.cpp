#include <iostream>
#include <fstream>
#include <chrono>

#include "raspicamera.h"

uint32_t RoundUp(int NumToRound, int Multiplier)
{
    if(Multiplier == 0 || Multiplier == 1){
        return NumToRound;
    }
    uint32_t IsPositive = (int)(NumToRound >= 0);
    return ((NumToRound + IsPositive * (Multiplier - 1)) / Multiplier) * Multiplier;
}

RaspiCamera::RaspiCamera(){
    bcm_host_init();
}

RaspiCamera::RaspiCamera(uint32_t Width, uint32_t Height){
    this->Settings.Width = Width;
    this->Settings.Height = Height;
    bcm_host_init();
}

RaspiCamera::~RaspiCamera(){
    if (this->_pNullsink) this->_pNullsink->disconnect();
    if (this->_pEncoder) this->_pEncoder->disconnect();
    if (this->_pCamera) this->_pCamera->disconnect();

    if (this->_pNullsink) this->_pNullsink->close();
    if (this->_pEncoder) this->_pEncoder->close();
    if (this->_pCamera) this->_pCamera->close();

    delete this->_pNullsink;
    delete this->_pEncoder;
    delete this->_pCamera;
}

void RaspiCamera::Initialize(){
    _InitCamera();
    _InitEncoder();
}

void RaspiCamera::Capture(std::vector<uint8_t>* ImgData){

    /// Define the callback that the encoder will call for each buffer
    this->_pEncoder->output(0).enable(this->_DefaultOutputCallback);

    /// Create a pool based on buffer_size and buffer_num value of the
    /// port which is associated to.
    this->_pEncoder->output(0).create_pool(this->_pEncoder->output(0).buffer_num_recommended(),
                                this->_pEncoder->output(0).buffer_size_recommended());

    /// Enable the encoder.
    this->_pEncoder->enable();

    /// Set userdata to the port so that you can get it from the callback.
    /// Of course you can pass a struct or a class or any kind of data you want.
    PortData UserData;
    UserData.pData = ImgData;
    VCOS_STATUS_T vcos_status = vcos_semaphore_create(&UserData.CompletedSem, "amc_camera_sem", 0);
    vcos_assert(vcos_status == VCOS_SUCCESS);
    this->_pEncoder->output(0).set_userdata(UserData);

    MMAL_PARAMETER_EXPOSUREMODE_T exp_mode =
    {
        {
            MMAL_PARAMETER_EXPOSURE_MODE,
            sizeof ( exp_mode )
        },
        MMAL_PARAM_EXPOSUREMODE_AUTO
    };

    /// Set some exposure mode parameter.
    this->_pCamera->control().parameter().set_header(&exp_mode.hdr);

    /// Send the pool created above to the port.
    this->_pEncoder->output(0).send_all_buffers();

    /// Start capture just one frame
    this->_pCamera->output(2).parameter().set_boolean(MMAL_PARAMETER_CAPTURE, true);
    auto start = std::chrono::high_resolution_clock::now();

    std::cout << "Started" << std::endl;

    vcos_semaphore_wait(&UserData.CompletedSem);
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_taken = finish - start;

    std::cout << "Done in: " << time_taken.count() << "s" << std::endl;

    this->_pEncoder->output(0).disable();
    this->_pEncoder->output(0).release_pool();
    this->_pEncoder->disable();
}

void RaspiCamera::_InitCamera(){
    this->_pCamera = new mmalpp::Component("vc.ril.camera");

    // Set sensor mode
    this->_pCamera->control().parameter().set_int32(MMAL_PARAMETER_CAMERA_CUSTOM_SENSOR_CONFIG, this->Settings.SensorMode);

    // Control Port setup
    this->_pCamera->control().enable(this->_DefaultControlCallback);

    MMAL_PARAMETER_CAMERA_CONFIG_T CamConfig = this->_GetCameraConfig();
    this->_pCamera->control().parameter().set_header(&CamConfig.hdr);

    // Set Buffer size and num
    this->_pCamera->output(MMAL_CAMERA_CAPTURE_PORT).set_default_buffer();

    // Set Still Port format
    MMAL_ES_FORMAT_T * format = this->_pCamera->output(MMAL_CAMERA_CAPTURE_PORT).format();
    format->encoding = MMAL_ENCODING_OPAQUE;
    format->es->video.width = RoundUp(this->Settings.Width, 32);
    format->es->video.height = RoundUp(this->Settings.Height, 16);
    format->es->video.crop.x = 0;
    format->es->video.crop.y = 0;
    format->es->video.crop.width = this->Settings.Width;
    format->es->video.crop.height = this->Settings.Height;
    format->es->video.frame_rate.num = 0;
    format->es->video.frame_rate.den = 1;

    /// Commit changes
    this->_pCamera->output(MMAL_CAMERA_CAPTURE_PORT).commit();

    /// Enable component
    this->_pCamera->enable();

    /// Pipe preview to Null sink for now
    this->_pNullsink = new mmalpp::Component("vc.null_sink"); 

    /// Connect OUTPUT port 0 (preview port) to a null_sink component.
    this->_pCamera->output(MMAL_CAMERA_PREVIEW_PORT).connect_to(this->_pNullsink->input(0),  MMAL_CONNECTION_FLAG_TUNNELLING
                                                                                    | MMAL_CONNECTION_FLAG_ALLOCATION_ON_INPUT);

    /// Enable the connection created above.
    this->_pCamera->output(MMAL_CAMERA_PREVIEW_PORT).connection().enable();

    /// Enable null_sink component.
    this->_pNullsink->enable();
}

void RaspiCamera::_InitEncoder(){
    this->_pEncoder = new mmalpp::Component("vc.ril.image_encode");

    /// Connect 2nd OUTPUT port (still port) to the encoder component.
    this->_pCamera->output(MMAL_CAMERA_CAPTURE_PORT).connect_to(this->_pEncoder->input(0),
              MMAL_CONNECTION_FLAG_TUNNELLING | MMAL_CONNECTION_FLAG_ALLOCATION_ON_INPUT);

    /// Enable connection
    this->_pCamera->output(MMAL_CAMERA_CAPTURE_PORT).connection().enable();

    /// Copy format from input port
    this->_pEncoder->output(0).copy_from(this->_pEncoder->input(0));

    /// Set the encoder format to something (JPEG for example)
    this->_pEncoder->output(0).format()->encoding = this->Settings.OutputEncoding;
    if (this->Settings.OutputEncoding == MMAL_ENCODING_JPEG)
        this->_pEncoder->output(0).parameter().set_uint32(MMAL_PARAMETER_JPEG_Q_FACTOR, this->Settings.JPGQuality);

    // Set buffer sizes
    this->_pEncoder->output(0).buffer_size(this->_pEncoder->output(0).buffer_size_recommended());
    this->_pEncoder->output(0).buffer_num(this->_pEncoder->output(0).buffer_num_recommended());

    /// Commit encoder's format changes
    this->_pEncoder->output(0).commit();
}


MMAL_PARAMETER_CAMERA_CONFIG_T RaspiCamera::_GetCameraConfig() const
{
    MMAL_PARAMETER_CAMERA_CONFIG_T CamConfig = {
        {MMAL_PARAMETER_CAMERA_CONFIG, sizeof ( CamConfig ) },
        this->Settings.Width, // max_stills_w
        this->Settings.Height, // max_stills_h
        0, // stills_yuv422
        1, // one_shot_stills
        640, // max_preview_video_w
        480, // max_preview_video_h
        3, // num_preview_video_frames
        0, // stills_capture_circular_buffer_height
        0, // fast_preview_resume
        MMAL_PARAM_TIMESTAMP_MODE_ZERO // use_stc_timestamp
    };

    return CamConfig;
}


void RaspiCamera::_DefaultOutputCallback(mmalpp::Generic_port& Port, mmalpp::Buffer Buffer)
{
    bool complete = false;
    PortData& userdata = Port.get_userdata_as<PortData>();
    std::vector<uint8_t>* p_data = userdata.pData;

    if (p_data && !Buffer.is_null())
    {
        uint32_t buff_size = Buffer.size();
        for (uint32_t i = 0; i < buff_size; i++){
            p_data->push_back(Buffer[i]);
        }
    }

    /// Check if we've received the whole frame
    if (Buffer.flags() & (MMAL_BUFFER_HEADER_FLAG_EOS | MMAL_BUFFER_HEADER_FLAG_FRAME_END))
        complete = true;

    Buffer.release();

    /// Return a new buffer to port since we used one
    if (Port.is_enabled()){
        mmalpp::Buffer new_buffer = Port.pool().queue().get_buffer();
        if (new_buffer.is_null()){
            std::cout << "Error!: Could not get new buffer" << std::endl;
        }
        else {
            Port.send_buffer(new_buffer);
        }
    }

    if (complete)
        vcos_semaphore_post(&(userdata.CompletedSem));
}

void RaspiCamera::_DefaultControlCallback(mmalpp::Generic_port& Port, mmalpp::Buffer Buffer)
{
    if (Port.is_enabled())
        Buffer.release();
}