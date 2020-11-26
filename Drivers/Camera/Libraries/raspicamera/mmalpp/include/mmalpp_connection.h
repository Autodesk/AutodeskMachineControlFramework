#ifndef MMALPP_CONNECTION_H
#define MMALPP_CONNECTION_H

#include <interface/mmal/mmal_types.h>
#include <interface/mmal/mmal_port.h>
#include <interface/mmal/mmal_pool.h>
#include <interface/mmal/mmal_queue.h>
#include <interface/mmal/util/mmal_connection.h>

#include "mmalpp_port.h"
#include "mmalpp_fwd_decl.h"
#include "utils/mmalpp_connection_utils.h"
#include "macros.h"

MMALPP_BEGIN

class Connection {

public:

    /// ctor.
    Connection(Port<OUTPUT>* source,
               Port<INPUT>* target,
               uint32_t flags = 0);

    /**
     * Check if it exists.
     */
    bool
    is_null();

    /**
     * Check if it is enabled.
     */
    bool
    is_enabled();

    /**
     * Get a reference to the source port. (Read-only)
     */
    const Port<OUTPUT>&
    source() const;

    /**
     * Get a reference to the target port. (Read-only)
     */
    const Port<INPUT>&
    target() const;

    /**
     * Enable the connection.
     */
    void
    enable();

    /**
     * Disable the connection.
     */
    void
    disable();

    /**
     * Destroy the connection.
     */
    void
    release();

    /**
     * Get the MMAL_CONNECTION_T pointer.
     */
    MMAL_CONNECTION_T*
    get();

private:
    MMAL_CONNECTION_T* connection_;
    Port<OUTPUT>* source_;
    Port<INPUT>* target_;

};

MMALPP_END

#endif // MMALPP_CONNECTION_H
