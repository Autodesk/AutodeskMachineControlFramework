#include <utils/exceptions/mmalpp_exceptions.h>

MMALPP_BEGIN

namespace mmalpp_impl_ {

/**
 * @brief If error code is one of them it throw an exception.
 */
void e_check__(const MMAL_STATUS_T e_code_, const std::string& msg_)
{
    switch (e_code_)
    {
    case MMAL_ENOMEM: throw std::runtime_error("out of memory: " + msg_);
    case MMAL_ENOSPC: throw std::runtime_error("out of resources (other than memory): " + msg_);
    case MMAL_EINVAL: throw std::invalid_argument("argument not valid: " + msg_);
    case MMAL_ENOSYS: throw std::logic_error("function not implemented");
    case MMAL_ENOENT: throw std::runtime_error("no such file or directory");
    case MMAL_ENXIO: throw std::runtime_error("no such device or address");
    case MMAL_EIO: throw std::runtime_error("I/O error");
    case MMAL_ESPIPE: throw std::runtime_error("illegal seek");
    case MMAL_ECORRUPT: throw std::runtime_error("data corrupted");
    case MMAL_ENOTREADY: throw std::runtime_error("component not ready: " + msg_);
    case MMAL_ECONFIG: throw std::runtime_error("component not configured " + msg_);
    case MMAL_EISCONN: throw std::runtime_error("port is already connected");
    case MMAL_ENOTCONN: throw std::runtime_error("port is disconnected");
    case MMAL_EAGAIN: throw std::runtime_error("resource temporarily unavailable");
    case MMAL_EFAULT: throw std::overflow_error("bad address");
//    case MMAL_STATUS_MAX
    default: break;
    }
}

};

MMALPP_END
