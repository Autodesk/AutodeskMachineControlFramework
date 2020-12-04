#include "mmalpp_connection.h"

MMALPP_BEGIN


/// ctor.
Connection::Connection(Port<OUTPUT>* source,
            Port<INPUT>* target,
            uint32_t flags)
    : connection_(mmalpp_impl_::create_connection_(source->get(),
                                                    target->get(),
                                                    flags)),
        source_(source),
        target_(target)
{ target->connection_ = this; }

/**
 * Check if it exists.
 */
bool
Connection::is_null()
{ return connection_ == nullptr; }

/**
 * Check if it is enabled.
 */
bool
Connection::is_enabled()
{ return connection_->is_enabled; }

/**
 * Get a reference to the source port. (Read-only)
 */
const Port<OUTPUT>&
Connection::source() const
{ return *source_; }

/**
 * Get a reference to the target port. (Read-only)
 */
const Port<INPUT>&
Connection::target() const
{ return *target_; }

/**
 * Enable the connection.
 */
void
Connection::enable()
{ mmalpp_impl_::enable_connection_(connection_); }

/**
 * Disable the connection.
 */
void
Connection::disable()
{ mmalpp_impl_::disable_connection_(connection_); }

/**
 * Destroy the connection.
 */
void
Connection::release()
{
    mmalpp_impl_::destroy_connection_(connection_);
    target_->connection_ = nullptr;
}

/**
 * Get the MMAL_CONNECTION_T pointer.
 */
MMAL_CONNECTION_T*
Connection::get()
{ return connection_; }

void ConnectionDeleter::operator()(Connection* c){
    delete c;
}

MMALPP_END

