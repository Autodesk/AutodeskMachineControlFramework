/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef UA_CONFIG_H_
#define UA_CONFIG_H_

/**
 * open62541 Version
 * ----------------- */
#define UA_OPEN62541_VER_MAJOR 1
#define UA_OPEN62541_VER_MINOR 3
#define UA_OPEN62541_VER_PATCH 0
#define UA_OPEN62541_VER_LABEL "-rc1-78-g6c2ede52" /* Release candidate label, etc. */
#define UA_OPEN62541_VER_COMMIT "v1.3-rc1-78-g6c2ede52"
#define UA_OPEN62541_VERSION "v1.3.0-rc1-78-g6c2ede52"

/**
 * Feature Options
 * ---------------
 * Changing the feature options has no effect on a pre-compiled library. */

#define UA_LOGLEVEL 300
#ifndef UA_ENABLE_AMALGAMATION
/* #undef UA_ENABLE_AMALGAMATION */
#endif
#define UA_ENABLE_METHODCALLS
#define UA_ENABLE_NODEMANAGEMENT
#define UA_ENABLE_SUBSCRIPTIONS
/* #undef UA_ENABLE_PUBSUB */
/* #undef UA_ENABLE_PUBSUB_ENCRYPTION */
/* #undef UA_ENABLE_PUBSUB_FILE_CONFIG */
/* #undef UA_ENABLE_PUBSUB_ETH_UADP */
/* #undef UA_ENABLE_PUBSUB_DELTAFRAMES */
/* #undef UA_ENABLE_PUBSUB_INFORMATIONMODEL */
/* #undef UA_ENABLE_PUBSUB_INFORMATIONMODEL_METHODS */
#define UA_ENABLE_DA
/* #undef UA_ENABLE_HISTORIZING */
#define UA_ENABLE_PARSING
/* #undef UA_ENABLE_EXPERIMENTAL_HISTORIZING */
#define UA_ENABLE_SUBSCRIPTIONS_EVENTS
/* #undef UA_ENABLE_JSON_ENCODING */
/* #undef UA_ENABLE_PUBSUB_MQTT */
/* #undef UA_ENABLE_MQTT_TLS */
/* #undef UA_ENABLE_MQTT_TLS_OPENSSL */
/* #undef UA_ENABLE_ENCRYPTION_MBEDTLS */
/* #undef UA_ENABLE_TPM2_SECURITY */
/* #undef UA_ENABLE_ENCRYPTION_OPENSSL */
/* #undef UA_ENABLE_ENCRYPTION_LIBRESSL */
#if defined(UA_ENABLE_ENCRYPTION_MBEDTLS) || defined(UA_ENABLE_ENCRYPTION_OPENSSL) || defined(UA_ENABLE_ENCRYPTION_LIBRESSL)
#define UA_ENABLE_ENCRYPTION
#endif
/* #undef UA_ENABLE_SUBSCRIPTIONS_ALARMS_CONDITIONS */

/* Multithreading */
/* #undef UA_ENABLE_IMMUTABLE_NODES */
#define UA_MULTITHREADING 0

/* Advanced Options */
#define UA_ENABLE_STATUSCODE_DESCRIPTIONS
#define UA_ENABLE_TYPEDESCRIPTION
#define UA_ENABLE_NODESET_COMPILER_DESCRIPTIONS
/* #undef UA_ENABLE_DETERMINISTIC_RNG */
/* #undef UA_ENABLE_DISCOVERY */
/* #undef UA_ENABLE_DISCOVERY_MULTICAST */
/* #undef UA_ENABLE_WEBSOCKET_SERVER */
/* #undef UA_ENABLE_QUERY */
/* #undef UA_ENABLE_MALLOC_SINGLETON */
#define UA_ENABLE_DISCOVERY_SEMAPHORE
/* #undef UA_ENABLE_UNIT_TEST_FAILURE_HOOKS */
/* #undef UA_ENABLE_VALGRIND_INTERACTIVE */
#define UA_VALGRIND_INTERACTIVE_INTERVAL 1000
#define UA_GENERATED_NAMESPACE_ZERO
#define UA_GENERATED_NAMESPACE_ZERO_FULL
/* #undef UA_ENABLE_PUBSUB_MONITORING */
/* #undef UA_ENABLE_PUBSUB_BUFMALLOC */

/* #undef UA_PACK_DEBIAN */

/* Options for Debugging */
/* #undef UA_DEBUG */
/* #undef UA_DEBUG_DUMP_PKGS */
/* #undef UA_DEBUG_FILE_LINE_INFO */
/**
 * Function Export
 * ---------------
 * On Win32: Define ``UA_DYNAMIC_LINKING`` and ``UA_DYNAMIC_LINKING_EXPORT`` in
 * order to export symbols for a DLL. Define ``UA_DYNAMIC_LINKING`` only to
 * import symbols from a DLL.*/
#define UA_DYNAMIC_LINKING

/* Shortcuts for extern "C" declarations */
#if !defined(_UA_BEGIN_DECLS)
# ifdef __cplusplus
#  define _UA_BEGIN_DECLS extern "C" {
# else
#  define _UA_BEGIN_DECLS
# endif
#endif
#if !defined(_UA_END_DECLS)
# ifdef __cplusplus
#  define _UA_END_DECLS }
# else
#  define _UA_END_DECLS
# endif
#endif

/* Select default architecture if non is selected through CMake or compiler define */
#if 1  && !defined(UA_ARCHITECTURE_ECOS) && !defined(UA_ARCHITECTURE_FREERTOSLWIP) && !defined(UA_ARCHITECTURE_POSIX) && !defined(UA_ARCHITECTURE_VXWORKS) && !defined(UA_ARCHITECTURE_WEC7) && !defined(UA_ARCHITECTURE_WIN32)
# ifdef _WIN32
#  define UA_ARCHITECTURE_WIN32
# else
#  define UA_ARCHITECTURE_POSIX
# endif
#endif

/**
 * POSIX Feature Flags
 * -------------------
 * These feature flags have to be set before including the first POSIX
 * header. */
#ifdef UA_ARCHITECTURE_POSIX
# if !defined(_XOPEN_SOURCE)
#  define _XOPEN_SOURCE 600
# endif
# ifndef _DEFAULT_SOURCE
#  define _DEFAULT_SOURCE
# endif
/* On older systems we need to define _BSD_SOURCE.
 * _DEFAULT_SOURCE is an alias for that. */
# ifndef _BSD_SOURCE
#  define _BSD_SOURCE
# endif
#endif

// specific architectures can undef this
#define UA_HAS_GETIFADDR

#include "win32/ua_architecture.h"

#endif /* UA_CONFIG_H_ */
