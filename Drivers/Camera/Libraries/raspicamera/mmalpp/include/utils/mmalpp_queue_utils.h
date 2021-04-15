#ifndef MMALPP_QUEUE_UTILS_H
#define MMALPP_QUEUE_UTILS_H

#include <interface/mmal/mmal_types.h>
#include <interface/mmal/mmal_queue.h>

#include "exceptions/mmalpp_exceptions.h"
#include "../macros.h"

MMALPP_BEGIN

namespace mmalpp_impl_ {

/**
 * Create a queue of MMAL_BUFFER_HEADER_T
 */
MMAL_QUEUE_T*
create_queue_();

/**
 * Get the number of MMAL_BUFFER_HEADER_T currently in a queue.
 */
unsigned int
get_queue_lenght_(MMAL_QUEUE_T* queue_);

/**
 * Destroy a queue of MMAL_BUFFER_HEADER_T
 */
void
release_queue_(MMAL_QUEUE_T* queue_);

/**
 * Get a MMAL_BUFFER_HEADER_T from a queue
 */
MMAL_BUFFER_HEADER_T*
get_buffer_from_queue_no_time_(MMAL_QUEUE_T* queue_);

/**
 * Wait for a MMAL_BUFFER_HEADER_T from a queue, up to a given timeout.
 * This is the same as a wait, except that it will abort in case of timeout.
 */
MMAL_BUFFER_HEADER_T*
wait_ms_from_queue_(MMAL_QUEUE_T* queue_, std::size_t interval_);

/**
 * Wait for a MMAL_BUFFER_HEADER_T from a queue. This is the same as a get
 * except that this will block until a buffer header is available.
 */
MMAL_BUFFER_HEADER_T*
wait_from_queue_(MMAL_QUEUE_T* queue_);

/**
 * Put a MMAL_BUFFER_HEADER_T into a queue.
 */
void
put_in_queue_(MMAL_QUEUE_T* queue_, MMAL_BUFFER_HEADER_T* buffer_);

/**
 * Put a MMAL_BUFFER_HEADER_T back at the start of a queue.
 * This is used when a buffer header was removed from the queue but not
 * fully processed and needs to be put back where it was originally taken.
 */
void
put_back_in_queue_(MMAL_QUEUE_T* queue_, MMAL_BUFFER_HEADER_T* buffer_);

/**
 * Get a Buffer from a queue. If a timeout is greater than 0 it will wait
 * up to timeout, then will abort if nothing is returned.
 * If timeout is 0 it will get a Buffer without waiting. If timeout is less than 0
 * the function will block until a Buffer will be available.
 */
MMAL_BUFFER_HEADER_T*
get_buffer_from_queue_(MMAL_QUEUE_T* queue_, int timeout_ms_ = 0);


};

MMALPP_END

#endif // MMALPP_QUEUE_UTILS_H
