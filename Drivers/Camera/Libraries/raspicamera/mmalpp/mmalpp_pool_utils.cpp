#include <utils/mmalpp_pool_utils.h>

MMALPP_BEGIN

namespace mmalpp_impl_ {

/**
 * Resize a pool of MMAL_BUFFER_HEADER_T. This allows modifying either the number of
 * allocated buffers, the payload size or both at the same time.
 */
void
pool_resize_(MMAL_POOL_T* pool_, std::size_t headers_, uint32_t size_)
{ if (MMAL_STATUS_T status = mmal_pool_resize(
                pool_, headers_, size_); status)
        e_check__(status, "cannot resize the pool"); }

/**
 * Destroy a pool of MMAL_BUFFER_HEADER_T.
 * This will also deallocate all of the memory which was allocated when creating or
 * resizing the pool.
 */
void
pool_release_(MMAL_POOL_T* pool_)
{ mmal_pool_destroy(pool_); }

/**
 * Create a pool of MMAL_BUFFER_HEADER_T.
 * After allocation, all allocated buffer headers will have been added to the queue.
 * It is valid to create a pool with no buffer headers, or with zero size payload buffers.
 * The resize() function can be used to increase or decrease the number of buffer
 * headers, or the size of the payload buffers, after creation of the pool.
 */
MMAL_POOL_T*
create_pool_(std::size_t headers_, uint32_t size_)
{ return mmal_pool_create(headers_, size_); }

};

MMALPP_END

