#include <utils/mmalpp_port_utils.h>

MMALPP_BEGIN

namespace mmalpp_impl_ {

/**
 * Enable processing on a port. If this port is connected to another one,
 * the given callback must be NULL, while for a disconnected port,
 * the callback must be non-NULL.
 */
void
enable_port_(MMAL_PORT_T* port_,
             MMAL_PORT_BH_CB_T cb_)
{ if (MMAL_STATUS_T status = mmal_port_enable(port_, cb_); status)
        e_check__(status, "cannot enable port: "
                  + std::string(port_->name)); }

/**
 * Disable processing on a port. Disabling a port will stop all processing
 * on this port and return all (non-processed) buffer headers to the client.
 * If this is a connected output port, the input port to which it is connected shall
 * also be disabled. Any buffer pool shall be released.
 */
void
disable_port_(MMAL_PORT_T* port_)
{ if (MMAL_STATUS_T status = mmal_port_disable(port_); status)
        e_check__(status, "cannot disable port: "
                  + std::string(port_->name)); }

/**
 * Send a buffer header to a port.
 */
void
port_send_buffer(MMAL_PORT_T* port_, MMAL_BUFFER_HEADER_T* buffer_)
{ if (MMAL_STATUS_T status = mmal_port_send_buffer(
                port_, buffer_); status)
        e_check__(status, "cannot send buffer to port: "
                  + std::string(port_->name)); }

/**
 * Commit format changes on a port.
 */
void
commit_format_(MMAL_PORT_T* port_)
{ if (MMAL_STATUS_T status = mmal_port_format_commit(
                port_); status)
        e_check__(status, "cannot commit format on port "
                  + std::string(port_->name)); }

/**
 * Shallow copy a format structure. It is worth noting that the extradata buffer
 * will not be copied in the new format.
 */
void
copy_format_(MMAL_ES_FORMAT_T* src_, MMAL_ES_FORMAT_T* dst_)
{ mmal_format_copy(dst_, src_); }

/**
 * Ask a port to release all the buffer headers it currently has.
 * Flushing a port will ask the port to send all the buffer headers it currently has
 * to the client. Flushing is an asynchronous request and the flush call will
 * return before all the buffer headers are returned to the client.
 * It is up to the client to keep a count on the buffer headers to know when the
 * flush operation has completed.
 * It is also important to note that flushing will also reset the state of the port
 * and any processing which was buffered by the port will be lost.
 */
void
flush_port_(MMAL_PORT_T* port_)
{ if (MMAL_STATUS_T status = mmal_port_flush(port_); status)
        e_check__(status, "cannot flush port "
                  + std::string(port_->name)); }

/**
 * Set a parameter on a port.
 */
void
set_parameters_to_port_(MMAL_PORT_T* port_,
                        MMAL_PARAMETER_HEADER_T* param_)
{ if (MMAL_STATUS_T status = mmal_port_parameter_set(
                port_, param_); status)
        e_check__(status, "cannot set parameter to the port: "
                  + std::string(port_->name)); }

/**
 * Set a parameter on a port.
 */
void
set_boolean_to_port_(MMAL_PORT_T* port_,
                     uint32_t id_,
                     int32_t value_)
{ if (MMAL_STATUS_T status = mmal_port_parameter_set_boolean(
                port_, id_, value_); status)
        e_check__(status, "cannot set boolean parameter to the port: "
                  + std::string(port_->name)); }

/**
 * Set a parameter on a port.
 */
void
set_uint64_to_port_(MMAL_PORT_T* port_,
                    uint32_t id_,
                    uint64_t value_)
{ if (MMAL_STATUS_T status = mmal_port_parameter_set_uint64(
                port_, id_, value_); status)
        e_check__(status, "cannot set uint64_t parameter to the port: "
                  + std::string(port_->name)); }

/**
 * Set a parameter on a port.
 */
void
set_int64_to_port_(MMAL_PORT_T* port_,
                   uint32_t id_,
                   int64_t value_)
{ if (MMAL_STATUS_T status = mmal_port_parameter_set_int64(
                port_, id_, value_); status)
        e_check__(status, "cannot set int64_t parameter to the port: "
                  + std::string(port_->name)); }

/**
 * Set a parameter on a port.
 */
void
set_uint32_to_port_(MMAL_PORT_T* port_,
                    uint32_t id_,
                    uint32_t value_)
{ if (MMAL_STATUS_T status = mmal_port_parameter_set_uint32(
                port_, id_, value_); status)
        e_check__(status, "cannot set uint32_t parameter to the port: "
                  + std::string(port_->name)); }

/**
 * Set a parameter on a port.
 */
void
set_int32_to_port_(MMAL_PORT_T* port_,
                   uint32_t id_,
                   int32_t value_)
{ if (MMAL_STATUS_T status = mmal_port_parameter_set_int32(
                port_, id_, value_); status)
        e_check__(status, "cannot set int32_t parameter to the port: "
                  + std::string(port_->name)); }

/**
 * Set a parameter on a port.
 */
void
set_rational_to_port_(MMAL_PORT_T* port_,
                      uint32_t id_,
                      int32_t num_,
                      int32_t den_)
{ if (MMAL_STATUS_T status = mmal_port_parameter_set_rational(
                port_, id_, MMAL_RATIONAL_T{num_, den_}); status)
        e_check__(status, "cannot set MMAL_RATIONAL_T "
                          "({int32_t, int32_t}) parameter to the port: "
                  + std::string(port_->name)); }

/**
 * Set a parameter on a port.
 */
void
set_string_to_port_(MMAL_PORT_T* port_,
                    uint32_t id_,
                    const std::string& value_)
{ if (MMAL_STATUS_T status = mmal_port_parameter_set_string(
                port_, id_, value_.c_str()); status)
        e_check__(status, "cannot set string parameter to the port: "
                  + std::string(port_->name)); }

/**
 * Create a pool of MMAL_BUFFER_HEADER_T associated with a specific port.
 * This allows a client to allocate memory for the payload buffers based on the preferences
 * of a port. This for instance will allow the port to allocate memory which can be shared
 * between the host processor and videocore.
 * After allocation, all allocated buffer headers will have been added to the queue.
 * It is valid to create a pool with no buffer headers, or with zero size payload buffers.
 * The mmal_pool_resize() function can be used to increase or decrease the number of buffer
 * headers, or the size of the payload buffers, after creation of the pool.
 */
MMAL_POOL_T*
port_pool_create_(MMAL_PORT_T* port_,
                  std::size_t headers_,
                  uint32_t size_)
{ return mmal_port_pool_create(port_, headers_, size_); }

/**
 * Destroy a pool of MMAL_BUFFER_HEADER_T associated with a specific port.
 * This will also deallocate all of the memory which was allocated when creating or
 * resizing the pool.
 */
void
port_pool_release_(MMAL_PORT_T* port_,
                   MMAL_POOL_T* pool_)
{ mmal_port_pool_destroy(port_, pool_); }

/**
 * Send a Buffer to a specific port.
 */
void
send_buffer_(MMAL_PORT_T* port_,
             MMAL_BUFFER_HEADER_T* buffer_)
{ if (MMAL_STATUS_T status = mmal_port_send_buffer(
                port_, buffer_); status)
        e_check__(status, "cannot send buffer to the port "
                  + std::string(port_->name)); }

};

MMALPP_END

