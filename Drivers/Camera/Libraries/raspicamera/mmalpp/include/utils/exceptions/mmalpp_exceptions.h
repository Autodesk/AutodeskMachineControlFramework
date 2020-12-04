#ifndef MMALPP_EXCEPTIONS_H
#define MMALPP_EXCEPTIONS_H

#include <stdexcept>

#include <interface/mmal/mmal_types.h>
#include "../../macros.h"

MMALPP_BEGIN

namespace mmalpp_impl_ {

/**
 * @brief If error code is one of them it throw an exception.
 */
void e_check__(const MMAL_STATUS_T e_code_, const std::string& msg_);

};

MMALPP_END

#endif // MMALPP_EXCEPTIONS_H
