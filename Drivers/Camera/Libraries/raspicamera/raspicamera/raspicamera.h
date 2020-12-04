#include <stdio.h>
#include <vector>
#include <vector>
#include "interface/vcos/vcos.h"
#include "bcm_host.h"

#include "mmalpp.h"

// Standard port setting for the camera component
#define MMAL_CAMERA_PREVIEW_PORT 0
#define MMAL_CAMERA_VIDEO_PORT 1
#define MMAL_CAMERA_CAPTURE_PORT 2

struct PortData {
    VCOS_SEMAPHORE_T CompletedSem;
    std::vector<uint8_t>* pData;
};

struct CameraSettings {
    uint32_t CameraNum;
    uint32_t Width;
    uint32_t Height;
    MMAL_FOURCC_T OutputEncoding;
    uint32_t SensorMode;
    uint32_t JPGQuality;

    CameraSettings(): CameraNum(0),
                Width(640),
                Height(480),
                OutputEncoding(MMAL_ENCODING_JPEG),
                SensorMode(0),
                JPGQuality(100) {}
};

class RaspiCamera {
    public:
        CameraSettings Settings;

        RaspiCamera();
        RaspiCamera(uint32_t Width, uint32_t Height);
        ~RaspiCamera();
        void Initialize();
        void Capture(std::vector<u_char>* ImgData);

    private:
        mmalpp::Component* _pCamera;
        mmalpp::Component* _pEncoder;
        mmalpp::Component* _pNullsink;

        mmalpp::Port<mmalpp::OUTPUT>* _pOutputPort;

        void _InitCamera();
        void _InitEncoder();

        MMAL_PARAMETER_CAMERA_CONFIG_T _GetCameraConfig() const;

        static void _DefaultOutputCallback(mmalpp::Generic_port& Port, mmalpp::Buffer Buffer);
        static void _DefaultControlCallback(mmalpp::Generic_port& Port, mmalpp::Buffer Buffer);
};