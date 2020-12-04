#ifndef MMALPP_SUPPORT_H
#define MMALPP_SUPPORT_H

#include <interface/mmal/mmal_port.h>

#include "macros.h"

MMALPP_BEGIN

namespace mmalpp_impl_ {

/**
 * Fill a container (vector) with ports.
 */
template <typename V_>
void
setup_ports_(V_& v_,
             MMAL_PORT_T** ports_,
             uint32_t n_)
{
    v_.reserve(n_);
    for(std::size_t i = 0; i < n_; ++i)
        v_.emplace_back(ports_[i]);
}

/**
 * Close a port.
 */
template <typename P_>
void
close_single_(P_& port_)
{
    if (!port_.is_null() && port_.is_enabled()) {
        port_.disable();
        port_.flush();
        if (!port_.pool().is_null())
            port_.pool().release();
    }

}

/**
 * Close all ports in a container.
 */
template <typename P_>
void
close_(P_& ports_)
{
    for (auto& p : ports_)
        close_single_(p);
}

/**
 * Disconnect all ports in a container.
 */
template <typename P_>
void
disconnect_ports_(P_& ports_)
{
    for (auto& p : ports_)
        if (!p.is_null() && p.is_enabled())
            if (p.connection_ptr() && !p.connection().is_null()) {
                if (p.connection().is_enabled())
                    p.connection().disable();
                p.connection().release();
            }
}

};

MMALPP_END

#endif // MMALPP_SUPPORT_H
