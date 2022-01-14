/* This work is licensed under a Creative Commons CCZero 1.0 Universal License.
 * See http://creativecommons.org/publicdomain/zero/1.0/ for more information.
 *
 *    Copyright 2016-2017 (c) Fraunhofer IOSB (Author: Julius Pfrommer)
 *    Copyright 2017 (c) Stefan Profanter, fortiss GmbH
 */

#ifndef UA_ACCESSCONTROL_DEFAULT_H_
#define UA_ACCESSCONTROL_DEFAULT_H_

#include <open62541/plugin/accesscontrol.h>
#include <open62541/server.h>

_UA_BEGIN_DECLS

typedef struct {
    UA_String username;
    UA_String password;
} UA_UsernamePasswordLogin;

/* Default access control. The log-in can be anonymous or username-password. A
 * logged-in user has all access rights.
 *
 * The certificate verification plugin lifecycle is moved to the access control
 * system. So it is cleared up eventually together with the AccessControl. */
UA_EXPORT UA_StatusCode
UA_AccessControl_default(UA_ServerConfig *config,
                         UA_Boolean allowAnonymous,
                         UA_CertificateVerification *verifyX509,
                         const UA_ByteString *userTokenPolicyUri,
                         size_t usernamePasswordLoginSize,
                         const UA_UsernamePasswordLogin *usernamePasswordLogin);

_UA_END_DECLS

#endif /* UA_ACCESSCONTROL_DEFAULT_H_ */
