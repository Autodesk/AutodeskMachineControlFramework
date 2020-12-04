#ifndef MMALPP_COMPONENT_UTILS_H
#define MMALPP_COMPONENT_UTILS_H

#include <interface/mmal/mmal_types.h>
#include <interface/mmal/mmal_component.h>

#include "exceptions/mmalpp_exceptions.h"
#include "../macros.h"

MMALPP_BEGIN

namespace mmalpp_impl_ {

/**
 * Create an instance of a component.
 * The newly created component will expose ports to the client. All the exposed ports are
 * disabled by default.
 * Note that components are reference counted and creating a component automatically
 * acquires a reference to it.
 */
MMAL_COMPONENT_T*
create_component_(const char* name_);

/**
 * Release a reference on a component.
 * Release an acquired reference on a component. Triggers the destruction of the component when
 * the last reference is being released.
 */
void
release_component_(MMAL_COMPONENT_T* component_);

/**
 * Enable processing on a component.
 */
void
enable_component_(MMAL_COMPONENT_T* component_);

/**
 * Disable processing on a component.
 */
void
disable_component_(MMAL_COMPONENT_T* component_);

};

MMALPP_END

#endif // MMALPP_COMPONENT_UTILS_H
