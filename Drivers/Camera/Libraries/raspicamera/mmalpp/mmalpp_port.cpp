#include "mmalpp_port.h"
#include "mmalpp_connection.h"

MMALPP_BEGIN

/// ctor.
Parameter::Parameter(MMAL_PORT_T* port)
                    : port_(port)
{}

void
Parameter::set_header(MMAL_PARAMETER_HEADER_T* hdr)
{ mmalpp_impl_::set_parameters_to_port_(port_, hdr); }

void
Parameter::set_boolean(uint32_t id, bool value)
{ mmalpp_impl_::set_boolean_to_port_(port_, id, value); }

void
Parameter::set_int64(uint32_t id, int64_t value)
{ mmalpp_impl_::set_int64_to_port_(port_, id, value); }

void
Parameter::set_uint64(uint32_t id, uint64_t value)
{ mmalpp_impl_::set_uint64_to_port_(port_, id, value); }

void
Parameter::set_int32(uint32_t id, int32_t value)
{ mmalpp_impl_::set_int32_to_port_(port_, id, value); }

void
Parameter::set_uint32(uint32_t id, uint32_t value)
{ mmalpp_impl_::set_uint32_to_port_(port_, id, value); }

void
Parameter::set_rational(uint32_t id, int32_t num, int32_t den)
{ mmalpp_impl_::set_rational_to_port_(port_, id, num, den); }

void
Parameter::set_string(uint32_t id, const std::string& value)
{ mmalpp_impl_::set_string_to_port_(port_, id, value); }

// ctor.
Generic_port::Generic_port(MMAL_PORT_T* port,
                MMAL_POOL_T* pool)
    :   port_(port),
        pool_(pool)
{}

/**
 * Check is this Port exists.
 */
bool
Generic_port::is_null() const
{ return port_ == nullptr; }

/**
 * Commit changes to the port's format.
 */
void
Generic_port::commit()
{ mmalpp_impl_::commit_format_(port_); }

/**
 * Copy format from another port.
 */
void
Generic_port::copy_from(const Generic_port& port)
{ mmalpp_impl_::copy_format_(port.format(), format()); }

/**
 * Check if this Port is enabled.
 */
bool
Generic_port::is_enabled() const
{ return port_->is_enabled; }

/**
 * Get Port's type.
 */
MMAL_PORT_TYPE_T
Generic_port::type() const
{ return port_->type; }

/**
 * Get index of this port in its type list.
 */
uint16_t
Generic_port::index() const
{ return port_->index; }

/**
 * Get capabilities of the port.
 */
uint32_t
Generic_port::capabilities() const
{ return port_->capabilities; }

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
Generic_port::flush() const
{ mmalpp_impl_::flush_port_(get()); }

/**
 * Get buffers number of the port.
 */
std::size_t
Generic_port::buffer_num() const
{ return port_->buffer_num; }

/**
 * Get buffers number of the port.
 */
void
Generic_port::buffer_num(unsigned int new_num)
{ port_->buffer_num = new_num; }

/**
 * Get buffer size of the port.
 */
std::size_t
Generic_port::buffer_size() const
{ return port_->buffer_size; }

/**
 * Set buffer size of the port.
 */
void
Generic_port::buffer_size(unsigned int new_size)
{ port_->buffer_size = new_size; }

/**
 * Get minimum buffers number of the port.
 */
std::size_t
Generic_port::buffer_num_min() const
{ return port_->buffer_num_min; }

/**
 * Get minimum buffers size of the port.
 */
std::size_t
Generic_port::buffer_size_min() const
{ return port_->buffer_size_min; }

/**
 * Get the recommended buffers number of the port.
 */
std::size_t
Generic_port::buffer_num_recommended() const
{ return port_->buffer_num_recommended; }

/**
 * Get recommended buffer size of the port.
 */
std::size_t
Generic_port::buffer_size_recommended() const
{ return port_->buffer_size_recommended; }

/**
 * Set buffer_num and buffer_size to recommended value. If recommended values are 0,
 * they will be set to minimum values.
 */
void
Generic_port::set_default_buffer()
{
    (port_->buffer_num_recommended == 0) ?
                port_->buffer_num = port_->buffer_num_min
            : port_->buffer_num = port_->buffer_num_recommended;
    (port_->buffer_size_recommended == 0) ?
                port_->buffer_size = port_->buffer_size_min
            : port_->buffer_size = port_->buffer_size_recommended;
}

/**
 * Send a Buffer to this port.
 */
void
Generic_port::send_buffer(const Buffer& buffer) const
{ mmalpp_impl_::port_send_buffer(get(), buffer.get()); }

/**
 * Get Parameter instance to set port's parameter.
 */
Parameter
Generic_port::parameter() const
{ return port_; }

/**
 * Get a MMAL_PORT_T* pointer.
 */
MMAL_PORT_T*
Generic_port::get() const
{ return port_; }

/**
 * Get port's format.
 */
MMAL_ES_FORMAT_T*
Generic_port::format() const
{ return port_->format; }

// /**
//  * Set userdata to the port.
//  */
// template<typename U>
// void
// Generic_port::set_userdata(U& u)
// { p_data_ptr__.userdata__ = reinterpret_cast<MMAL_PORT_USERDATA_T*>(&u); }

// /**
//  * Get userdata from the port already casted to U type.
//  */
// template<typename U>
// U&
// Generic_port::get_userdata_as()
// { return *reinterpret_cast<U*>(p_data_ptr__.userdata__); }

/**
 * Disable this port.
 */
void
Generic_port::disable() const
{ mmalpp_impl_::disable_port_(port_); }

// /**
//  * Enable this port and set a callback.
//  */
// template<typename F_>
// void
// Generic_port::enable(F_&& f)
// {
//     p_data_ptr__.instance__ = this;
//     p_data_ptr__.callback__ = f;
//     port_->userdata = reinterpret_cast<MMAL_PORT_USERDATA_T*>(&p_data_ptr__);

//     mmalpp_impl_::enable_port_(port_,
//                                 [] (MMAL_PORT_T* port__, MMAL_BUFFER_HEADER_T* buffer__) {
//         try {

//             P_data_ptr_* ptr_ = reinterpret_cast<P_data_ptr_*>(port__->userdata);
//             ptr_->callback__(*ptr_->instance__, buffer__);

//         } catch (std::exception&)
//         {}
//     });
// }

/**
 * Create a Pool and associate it with this port.
 */
void
Generic_port::create_pool(std::size_t headers, uint32_t size)
{ pool_ = mmalpp_impl_::port_pool_create_(port_, headers, size); }

/**
 * Get the Pool associated with this port.
 */
Pool
Generic_port::pool()
{ return pool_; }

/**
 * Send all Buffer on the associated Pool to this port.
 */
void
Generic_port::send_all_buffers()
{
    for(std::size_t i = 0; i < pool_->headers_num; ++i)
        send_buffer(mmalpp_impl_::get_buffer_from_queue_(pool_->queue));
}

/**
 * Destroy the Pool associated with this Port.
 */
void
Generic_port::release_pool()
{ mmalpp_impl_::port_pool_release_(port_, pool_); }


/// Specialization of Port<INPUT>
Port<INPUT>::Port(MMAL_PORT_T* port)
            :   Generic_port(port),
                connection_(nullptr) {}

/// Specialization of Port<OUTPUT>
Port<OUTPUT>::Port(MMAL_PORT_T* port)
                    :   Generic_port(port),
                        connection_(nullptr) {}

/**
 * Connect this OUTPUT Port to an INPUT Port. When this connection has been created,
 * it will set a pointer to this connection in the INPUT Port class. When this connection
 * will be released that pointer will be set to nullptr.
 */
void
Port<OUTPUT>::connect_to(Port<INPUT>& target, uint32_t flags)
{ connection_ = std::make_unique<Connection>(this, &target, flags); }

/*!5tortion

    * Get the Connection.
    */
Connection&
Port<OUTPUT>::connection()
{ return *connection_.get(); }

/**
 * Get pointer to connection
 */
Connection*
Port<OUTPUT>::connection_ptr()
{return connection_.get(); }


MMALPP_END