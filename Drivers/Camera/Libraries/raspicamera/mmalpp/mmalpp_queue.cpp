#include "mmalpp_queue.h"

MMALPP_BEGIN

/// ctors.
Queue::Queue()
    : queue_(mmalpp_impl_::create_queue_())
{}

Queue::Queue(MMAL_QUEUE_T* queue)
    : queue_(queue)
{}

/**
 * Check if Queue exists.
 */
bool
Queue::is_null() const
{ return queue_ == nullptr; }

/**
 * Release the Queue.
 */
void
Queue::release()
{ mmalpp_impl_::release_queue_(queue_); }

/**
 * Get the number of Buffers in the Queue.
 */
std::size_t
Queue::size() const
{ return mmalpp_impl_::get_queue_lenght_(queue_); }

/**
 * Put a Buffer into a queue.
 */
void
Queue::put(const Buffer& buffer)
{ mmalpp_impl_::put_in_queue_(queue_, buffer.get()); }

/**
 * Put back a Buffer into a queue.
 */
void
Queue::put_back(const Buffer& buffer)
{ mmalpp_impl_::put_back_in_queue_(queue_, buffer.get()); }

/**
 * Get a Buffer from the queue.
 */
Buffer
Queue::get_buffer(int timeout_ms)
{ return mmalpp_impl_::get_buffer_from_queue_(queue_, timeout_ms); }


MMALPP_END