
#ifndef RASPISTILL_CPP_H
#define RASPISTILL_CPP_H

#include <string>
#include <vector>

extern "C" {
    #include "RaspiStill.h"
    #include "interface/mmal/mmal_port.h"
}

/* Light API for still camera derived from C executable raspistill
*/
class RaspiStill
{
    private:
        MMAL_STATUS_T status;
        PORT_USERDATA callback_data;
        // These are just for easy access
        MMAL_PORT_T *m_pCameraPreviewPort = NULL;
        MMAL_PORT_T *m_pCameraVideoPort = NULL;
        MMAL_PORT_T *m_pCameraStillPort = NULL;
        MMAL_PORT_T *m_pPreviewInputPort = NULL;
        MMAL_PORT_T *m_pEncoderInputPort = NULL;
        MMAL_PORT_T *m_pEncoderOutputPort = NULL;
        RASPICAM_CAMERA_PARAMETERS *m_pCameraParams = NULL;

    public:
        // Holds the current state of the class
        RASPISTILL_STATE m_State;

        // Constructor
        RaspiStill();
        // Destructor
        ~RaspiStill();

        bool Initialize();
        bool CaptureToFile(std::string sFile);
        bool Capture(std::vector<uint8_t>& data);
        bool Capture(unsigned char* pPreallocatedData, size_t nPreallocatedLength, size_t *pFinalLength = NULL);

        // Setting common settings
        // Note that these need to be called before Initialize
        bool SetWidth(int nWidth);
        bool SetHeight(int nHeight);
        bool SetOutputFile(std::string sFile);
        int GetWidth();
        int GetHeight();

        // Setting various camera settings
        // Note that these need to be called before Initialize
        bool SetJPEGQuality(int nQuality);
        bool SetRequestRAW(bool bRequestRaw);
        bool SetFrameStart(int nFrameStart);
        bool SetUseDateTime(bool bUseDateTime);
        bool SetUseTimeStamp(bool bUseTimeStamp);
        bool SetTimeout(int nTimeout);
        bool SetUseThumbnail(bool bUseThumbnail, int nThumbnailWidth, int nThumbnailHeight, int nThumbnailQuality);
        bool SetEncoding(std::string sEncoding);
        bool SetSharpness(int nSharpness); // -100 to 100
        bool SetContrast(int nContrast); // -100 to 100
        bool SetBrightness(int nBrightness); // 0 to 100
        bool SetSaturation(int nSaturation); // -100 to 100
        bool SetISO(int nISO);
        bool SetVideoStabilization(bool bStalibilize);
        bool SetEVCompensation(int nCompensation); // -10 to +10
        bool SetExposureMode(MMAL_PARAM_EXPOSUREMODE_T eExposureMode);
        bool SetFlickerMode(MMAL_PARAM_FLICKERAVOID_T eFlickerMode);
        bool SetAWBMode(MMAL_PARAM_AWBMODE_T eAWBMode);
        bool SetImageFX(MMAL_PARAM_IMAGEFX_T eImageFX);
        bool SetColourFX(bool bEnable, int nU, int nV);
        bool SetExposureMeteringMode(MMAL_PARAM_EXPOSUREMETERINGMODE_T eMeterMode);
        bool SetRotation(int nRotation); // in degrees (0 - 359)
        bool SetHFlip(bool bHFlip);
        bool SetVFlip(bool bVFlip);
        bool SetROI(int nX, int nY, int nW, int nH); // in fraction (0 < X,Y,W,H < 1)
        bool SetShutterSpeed(int nSpeed);
        bool SetAWBGains(int nR, int nB);
        bool SetDRCLevelMode(MMAL_PARAMETER_DRC_STRENGTH_T eDRCLevel);
        bool SetStatsPass(bool bStatsPass);
      //   bool SetAnnotate(bool bAnnotate);
      //   bool SetAnnotateExtras(bool bAnnotateExtras);
};

const static struct
{
   std::string format;
   MMAL_FOURCC_T encoding;
} encoding_xref[] =
{
   {"jpg", MMAL_ENCODING_JPEG},
   {"bmp", MMAL_ENCODING_BMP},
   {"gif", MMAL_ENCODING_GIF},
   {"png", MMAL_ENCODING_PNG},
   {"ppm", MMAL_ENCODING_PPM},
   {"tga", MMAL_ENCODING_TGA}
};

const static int encoding_xref_size = sizeof(encoding_xref) / sizeof(encoding_xref[0]);

const static struct
{
   std::string description;
   int nextFrameMethod;
} next_frame_description[] =
{
   {"Single capture",         FRAME_NEXT_SINGLE},
   {"Capture on timelapse",   FRAME_NEXT_TIMELAPSE},
   {"Capture on keypress",    FRAME_NEXT_KEYPRESS},
   {"Run forever",            FRAME_NEXT_FOREVER},
   {"Capture on GPIO",        FRAME_NEXT_GPIO},
   {"Capture on signal",      FRAME_NEXT_SIGNAL},
};

const static int next_frame_description_size = sizeof(next_frame_description) / sizeof(next_frame_description[0]);

#endif