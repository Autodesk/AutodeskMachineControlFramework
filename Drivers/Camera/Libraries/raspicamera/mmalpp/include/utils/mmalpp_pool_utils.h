#ifndef MMALPP_POOL_UTILS_H
#define MMALPP_POOL_UTILS_H

#include <interface/mmal/mmal_types.h>
#include <interface/mmal/mmal_pool.h>
#include <interface/mmal/mmal_port.h>
#include <interface/mmal/util/mmal_util.h>

#include "exceptions/mmalpp_exceptions.h"
#include "../macros.h"

MMALPP_BEGIN

namespace mmalpp_impl_ {

/**
 * Resize a pool of MMAL_BUFFER_HEADER_T. This allows modifying either the number of
 * allocated buffers, the payload size or both at the same time.
 */
void
pool_resize_(MMAL_POOL_T* pool_, std::size_t headers_, uint32_t size_);

/**
 * Destroy a pool of MMAL_BUFFER_HEADER_T.
 * This will also deallocate all of the memory which was allocated when creating or
 * resizing the pool.
 */
void
pool_release_(MMAL_POOL_T* pool_);

/**
 * Create a pool of MMAL_BUFFER_HEADER_T.
 * After allocation, all allocated buffer headers will have been added to the queue.
 * It is valid to create a pool with no buffer headers, or with zero size payload buffers.
 * The resize() function can be used to increase or decrease the number of buffer
 * headers, or the size of the payload buffers, after creation of the pool.
 */
MMAL_POOL_T*
create_pool_(std::size_t headers_, uint32_t size_);

};

MMALPP_END

#endif // MMALPP_POOL_UTILS_H
