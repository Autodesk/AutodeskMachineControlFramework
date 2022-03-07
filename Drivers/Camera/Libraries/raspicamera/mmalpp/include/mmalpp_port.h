#ifndef MMALPP_PORT_H
#define MMALPP_PORT_H

#include <functional>
#include <memory>

#include <interface/mmal/mmal_types.h>
#include <interface/mmal/mmal_port.h>
#include <interface/mmal/mmal_pool.h>
#include <interface/mmal/mmal_queue.h>
#include <interface/mmal/util/mmal_connection.h>

#include "utils/mmalpp_port_utils.h"
#include "mmalpp_buffer.h"
#include "mmalpp_pool.h"
#include "mmalpp_types.h"
#include "mmalpp_fwd_decl.h"
#include "macros.h"

MMALPP_BEGIN

/// This class is used to set parameter of ports.
class Parameter {
public:

    /// ctor.
    Parameter(MMAL_PORT_T* port);

    void
    set_header(MMAL_PARAMETER_HEADER_T* hdr);

    void
    set_boolean(uint32_t id, bool value);

    void
    set_int64(uint32_t id, int64_t value);

    void
    set_uint64(uint32_t id, uint64_t value);

    void
    set_int32(uint32_t id, int32_t value);

    void
    set_uint32(uint32_t id, uint32_t value);

    void
    set_rational(uint32_t id, int32_t num, int32_t den);

    void
    set_string(uint32_t id, const std::string& value);

private:
    MMAL_PORT_T* port_;

};

/// Base port class
class Generic_port {

public:

    // ctor.
    Generic_port (MMAL_PORT_T* port = nullptr,
                  MMAL_POOL_T* pool = nullptr);

    /**
     * Check is this Port exists.
     */
    bool
    is_null() const;

    /**
     * Commit changes to the port's format.
     */
    void
    commit();

    /**
     * Copy format from another port.
     */
    void
    copy_from(const Generic_port& port);

    /**
     * Check if this Port is enabled.
     */
    bool
    is_enabled() const;

    /**
     * Get Port's type.
     */
    MMAL_PORT_TYPE_T
    type() const;

    /**
     * Get index of this port in its type list.
     */
    uint16_t
    index() const;

    /**
     * Get capabilities of the port.
     */
    uint32_t
    capabilities() const;

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
    flush() const;

    /**
     * Get buffers number of the port.
     */
    std::size_t
    buffer_num() const;

    /**
     * Get buffers number of the port.
     */
    void
    buffer_num(unsigned int new_num);

    /**
     * Get buffer size of the port.
     */
    std::size_t
    buffer_size() const;

    /**
     * Set buffer size of the port.
     */
    void
    buffer_size(unsigned int new_size);

    /**
     * Get minimum buffers number of the port.
     */
    std::size_t
    buffer_num_min() const;

    /**
     * Get minimum buffers size of the port.
     */
    std::size_t
    buffer_size_min() const;

    /**
     * Get the recommended buffers number of the port.
     */
    std::size_t
    buffer_num_recommended() const;

    /**
     * Get recommended buffer size of the port.
     */
    std::size_t
    buffer_size_recommended() const;

    /**
     * Set buffer_num and buffer_size to recommended value. If recommended values are 0,
     * they will be set to minimum values.
     */
    void
    set_default_buffer();

    /**
     * Send a Buffer to this port.
     */
    void
    send_buffer(const Buffer& buffer) const;

    /**
     * Get Parameter instance to set port's parameter.
     */
    Parameter
    parameter() const;

    /**
     * Get a MMAL_PORT_T* pointer.
     */
    MMAL_PORT_T*
    get() const;

    /**
     * Get port's format.
     */
    MMAL_ES_FORMAT_T*
    format() const;

    /**
     * Set userdata to the port.
     */
    template<typename U>
    void
    set_userdata(U& u)
    { p_data_ptr__.userdata__ = reinterpret_cast<MMAL_PORT_USERDATA_T*>(&u); }

    /**
     * Get userdata from the port already casted to U type.
     */
    template<typename U>
    U&
    get_userdata_as()
    { return *reinterpret_cast<U*>(p_data_ptr__.userdata__); }

    /**
     * Disable this port.
     */
    void
    disable() const;

    /**
     * Enable this port and set a callback.
     */
    template<typename F_>
    void
    enable(F_&& f)
    {
        p_data_ptr__.instance__ = this;
        p_data_ptr__.callback__ = f;
        port_->userdata = reinterpret_cast<MMAL_PORT_USERDATA_T*>(&p_data_ptr__);

        mmalpp_impl_::enable_port_(port_,
                                    [] (MMAL_PORT_T* port__, MMAL_BUFFER_HEADER_T* buffer__) {
            try {

                P_data_ptr_* ptr_ = reinterpret_cast<P_data_ptr_*>(port__->userdata);
                ptr_->callback__(*ptr_->instance__, buffer__);

            } catch (std::exception&)
            {}
        });
    }

    /**
     * Create a Pool and associate it with this port.
     */
    void
    create_pool(std::size_t headers = 0, uint32_t size = 0);

    /**
     * Get the Pool associated with this port.
     */
    Pool
    pool();

    /**
     * Send all Buffer on the associated Pool to this port.
     */
    void
    send_all_buffers();

    /**
     * Destroy the Pool associated with this Port.
     */
    void
    release_pool();

protected:
    MMAL_PORT_T* port_;
    MMAL_POOL_T* pool_;

    /// Private data
    struct P_data_ptr_
    {
        using cb_type_ = std::function<void(Generic_port&, Buffer)>;

        Generic_port* instance__;
        cb_type_ callback__;
        MMAL_PORT_USERDATA_T* userdata__;

    } p_data_ptr__;

};

/// Port class
template <PORT_TYPE T_>
class Port : public Generic_port {
public:

    /// ctor.
    Port(MMAL_PORT_T* port)
    : Generic_port(port)
    {}

    Connection* connection_;
};

/// Specialization of Port<INPUT>
template <>
class Port<INPUT> : public Generic_port {
public:

    /// ctor.
    Port(MMAL_PORT_T* port);

    /// Connection pointer. It stores the same Connection object of the OUTPUT Port
    /// which is connected to.
    Connection* connection_;

};

/// Specialization of Port<OUTPUT>
template <>
class Port<OUTPUT> : public Generic_port {
public:

    ///ctor.
    Port(MMAL_PORT_T* port);

    /**
     * Connect this OUTPUT Port to an INPUT Port. When this connection has been created,
     * it will set a pointer to this connection in the INPUT Port class. When this connection
     * will be released that pointer will be set to nullptr.
     */
    void
    connect_to(Port<INPUT>& target, uint32_t flags = 0);

    /**
     * Get the Connection.
     */
    Connection&
    connection();

    /**
     * Get pointer to connection
     */
    Connection*
    connection_ptr();

private:
    /// Connection pointer. This is the same Connection object of the INPUT Port
    /// which is connected to.
    std::unique_ptr<Connection> connection_;

};

MMALPP_END

#endif // MMALPP_PORT_H
