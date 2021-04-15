#ifndef MMALPP_QUEUE_H
#define MMALPP_QUEUE_H

#include <interface/mmal/mmal_types.h>
#include <interface/mmal/mmal_queue.h>

#include "mmalpp_buffer.h"
#include "utils/mmalpp_queue_utils.h"
#include "macros.h"

MMALPP_BEGIN

class Queue {
public:

    /// ctors.
    Queue();

    Queue(MMAL_QUEUE_T* queue);

    /**
     * Check if Queue exists.
     */
    bool
    is_null() const;

    /**
     * Release the Queue.
     */
    void
    release();

    /**
     * Get the number of Buffers in the Queue.
     */
    std::size_t
    size() const;

    /**
     * Put a Buffer into a queue.
     */
    void
    put(const Buffer& buffer);

    /**
     * Put back a Buffer into a queue.
     */
    void
    put_back(const Buffer& buffer);

    /**
     * Get a Buffer from the queue.
     */
    Buffer
    get_buffer(int timeout_ms = 0);

private:
    MMAL_QUEUE_T* queue_;

};

MMALPP_END

#endif // MMALPP_QUEUE_H
