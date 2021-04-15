#include "mmalpp_component.h"

MMALPP_BEGIN

/// ctor.
Component::Component(const std::string& name)
    : component_(mmalpp_impl_::create_component_(name.c_str())),
        control_(component_->control)
{
    mmalpp_impl_::setup_ports_(input_, component_->input, component_->input_num);
    mmalpp_impl_::setup_ports_(output_, component_->output, component_->output_num);
}

/**
 * Close component. Disable all ports, release all pools associated
 * with the ports and destroy the component.
 */
void
Component::close()
{
    mmalpp_impl_::close_(output_);
    mmalpp_impl_::close_(input_);
    mmalpp_impl_::close_single_(control_);
    mmalpp_impl_::release_component_(component_);
}

/**
 * Disconnect this component from others. This method must be call
 * for each component before call the close method.
 */
void
Component::disconnect()
{ mmalpp_impl_::disconnect_ports_(output_); }

/**
 * Check if it exists.
 */
bool
Component::is_null() const
{ return component_ == nullptr; }

/**
 * Enable component
 */
void
Component::enable()
{ mmalpp_impl_::enable_component_(component_); }

/**
 * Disable component
 */
void
Component::disable()
{ mmalpp_impl_::disable_component_(component_); }

/**
 * Check if it is enabled.
 */
bool
Component::is_enable() const
{ return component_->is_enabled; }

/**
 * Get input port number.
 */
uint32_t
Component::inputs() const
{ return component_->input_num; }

/**
 * Get output port number.
 */
uint32_t
Component::outputs() const
{ return component_->output_num; }

/**
 * Get n-th output port.
 */
Port<OUTPUT>&
Component::output(uint16_t num)
{ return output_[num]; }

/**
 * Get n-th input port.
 */
Port<INPUT>&
Component::input(uint16_t num)
{ return input_[num]; }

/**
 * Get control port.
 */
Port<CONTROL>&
Component::control()
{ return control_; }

MMALPP_END
