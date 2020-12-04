#include "mmalpp_pool.h"

MMALPP_BEGIN

/// ctors.
Pool::Pool(MMAL_POOL_T* pool)
    : pool_(pool)
{}

Pool::Pool(std::size_t headers, uint32_t size)
    : pool_(mmalpp_impl_::create_pool_(headers, size))
{}

/**
 * Release all Buffers in the Pool.
 */
void
Pool::release()
{ mmalpp_impl_::pool_release_(pool_); pool_ = nullptr; }

/**
 * Check if Pool exists.
 */
bool
Pool::is_null() const
{ return pool_ == nullptr; }

/**
 * Get a Buffer from a queue. If a timeout is greater than 0 it will wait
 * up to timeout, then will abort if nothing is returned.
 * If timeout is 0 it will get a Buffer without waiting. If timeout is less than 0
 * the function will block until a Buffer will be available.
 */
Buffer
Pool::get_buffer(int timeout_ms)
{ return mmalpp_impl_::get_buffer_from_queue_(pool_->queue, timeout_ms); }

/**
 * Get the Queue associated with the Pool.
 */
Queue
Pool::queue() const
{ return {pool_->queue}; }

/**
 * Get the number of Buffers in the Pool.
 */
std::size_t
Pool::size() const
{ return pool_->headers_num; }

/**
 * Access to the Pool and get the n-th Buffer in it.
 * Attention: this function doesn't check bounds.
 */
Buffer
Pool::operator[](uint32_t n)
{ return pool_->header[n]; }

/**
 * Access to the Pool and get the n-th Buffer in it.
 * Attention: this function doesn't check bounds.
 */
const Buffer
Pool::operator[](uint32_t n) const
{ return pool_->header[n]; }

/**
 * Resize the Pool.
 */
void
Pool::resize(std::size_t headers, uint32_t size)
{ mmalpp_impl_::pool_resize_(pool_, headers, size); }

/**
 * Get the MMAL_POOL_T pointer.
 */
MMAL_POOL_T*
Pool::get() const
{ return pool_; }


MMALPP_END

