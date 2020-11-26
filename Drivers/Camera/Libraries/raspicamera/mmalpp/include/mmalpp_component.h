#ifndef MMALPP_COMPONENT_H
#define MMALPP_COMPONENT_H

#include <string>
#include <vector>

#include "mmalpp_port.h"
#include "mmalpp_connection.h"
#include "utils/mmalpp_component_utils.h"
#include "mmalpp_support.h"
#include "macros.h"

MMALPP_BEGIN

class Component {

public:

    /// ctor.
    Component(const std::string& name);

    /**
     * Close component. Disable all ports, release all pools associated
     * with the ports and destroy the component.
     */
    void
    close();

    /**
     * Disconnect this component from others. This method must be call
     * for each component before call the close method.
     */
    void
    disconnect();

    /**
     * Check if it exists.
     */
    bool
    is_null() const;

    /**
     * Enable component
     */
    void
    enable();

    /**
     * Disable component
     */
    void
    disable();

    /**
     * Check if it is enabled.
     */
    bool
    is_enable() const;

    /**
     * Get input port number.
     */
    uint32_t
    inputs() const;

    /**
     * Get output port number.
     */
    uint32_t
    outputs() const;

    /**
     * Get n-th output port.
     */
    Port<OUTPUT>&
    output(uint16_t num);

    /**
     * Get n-th input port.
     */
    Port<INPUT>&
    input(uint16_t num);

    /**
     * Get control port.
     */
    Port<CONTROL>&
    control();

private:

    MMAL_COMPONENT_T* component_;

    std::vector<Port<INPUT>> input_;
    std::vector<Port<OUTPUT>> output_;
    Port<CONTROL> control_;

};

MMALPP_END

#endif // MMALPP_COMPONENT_H
