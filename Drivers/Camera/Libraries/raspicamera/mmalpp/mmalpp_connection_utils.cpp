#include <utils/mmalpp_connection_utils.h>

MMALPP_BEGIN

namespace mmalpp_impl_ {

/**
 * Create a connection between two ports.
 * The connection shall include a pool of buffer headers suitable for the current format of
 * the output port. The format of the input port shall have been set to the same as that of
 * the input port.
 * Note that connections are reference counted and creating a connection automatically
 * acquires a reference to it.
 */
MMAL_CONNECTION_T*
create_connection_(MMAL_PORT_T* src_, MMAL_PORT_T* dst_, uint32_t flags_)
{
    MMAL_CONNECTION_T* connection_;
    if (MMAL_STATUS_T status = mmal_connection_create(
                &connection_, src_, dst_, flags_); status)
        e_check__(status, "cannot create connection between: "
                  + std::string(src_->name) + " and "
                  + std::string(dst_->name));
    return connection_;
}

/**
 * Enable a connection.
 * The format of the two ports must have been committed before calling this function,
 * although note that on creation, the connection automatically copies and commits the
 * output port's format to the input port.
 */
void
enable_connection_(MMAL_CONNECTION_T* connection_)
{
    if (MMAL_STATUS_T status = mmal_connection_enable(connection_); status)
        e_check__(status, "cannot enable connection between: "
                  + std::string(connection_->out->name) + " and "
                  + std::string(connection_->in->name));
}

/**
 * Disable a connection.
 */
void
disable_connection_(MMAL_CONNECTION_T* connection_)
{
    if (MMAL_STATUS_T status = mmal_connection_disable(connection_); status)
        e_check__(status, "cannot disable connection between: "
                  + std::string(connection_->out->name) + " and "
                  + std::string(connection_->in->name));
}

/**
 * Release a reference on a connection
 * Release an acquired reference on a connection. Triggers the destruction of the connection when
 * the last reference is being released.
 */
void
destroy_connection_(MMAL_CONNECTION_T* connection_)
{
    if (MMAL_STATUS_T status = mmal_connection_release(connection_); status)
        e_check__(status, "cannot release connection between: "
                  + std::string(connection_->out->name) + " and "
                  + std::string(connection_->in->name));
}

};

MMALPP_END

