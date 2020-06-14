/*
 * Copyright (c) 2010-2014, Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TARGET_AURIX_H
#define TARGET_AURIX_H

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <endian.h>
#include "gptp_os_types.h"

#include <errno.h>
#include <stddef.h>
#include <net/ptp_time.h>


#define __ASSERT  // __ASSERT(cond, "" __VA_ARGS__)


// Use from <arpa/inet.h>
#if 0
#define ntohs(x)
#define ntohl(x)
#define htons(x)
#define htonl(x)
#endif
#define ntohll(x)    	be64toh(x)
#define htonll(x) 		htobe64(x)

// Use from <sys/socket.h>
#if 0
#define PF_UNSPEC      	0          /**< Unspecified protocol family.  */
#define AF_UNSPEC       PF_UNSPEC   /**< Unspecified address family.   */
#endif

/** Protocol numbers from IANA/BSD */
// Use from <netinet/in.h>
#if 0
enum {
   IPPROTO_IP = 0,            /**< IP protocol (pseudo-val for setsockopt() */
   IPPROTO_ICMP = 1,          /**< ICMP protocol   */
   IPPROTO_TCP = 6,           /**< TCP protocol    */
   IPPROTO_UDP = 17,          /**< UDP protocol    */
   IPPROTO_IPV6 = 41,         /**< IPv6 protocol   */
   IPPROTO_ICMPV6 = 58,       /**< ICMPv6 protocol */
   IPPROTO_RAW = 255,         /**< RAW IP packets  */
};
#endif



/**
 * @brief Check for macro definition in compiler-visible expressions
 *
 * This trick was pioneered in Linux as the config_enabled() macro.
 * The madness has the effect of taking a macro value that may be
 * defined to "1" (e.g. CONFIG_MYFEATURE), or may not be defined at
 * all and turning it into a literal expression that can be used at
 * "runtime".  That is, it works similarly to
 * "defined(CONFIG_MYFEATURE)" does except that it is an expansion
 * that can exist in a standard expression and be seen by the compiler
 * and optimizer.  Thus much ifdef usage can be replaced with cleaner
 * expressions like:
 *
 *     if (IS_ENABLED(CONFIG_MYFEATURE))
 *             myfeature_enable();
 *
 * INTERNAL
 * First pass just to expand any existing macros, we need the macro
 * value to be e.g. a literal "1" at expansion time in the next macro,
 * not "(1)", etc...  Standard recursive expansion does not work.
 */
#define IS_ENABLED(config_macro) Z_IS_ENABLED1(config_macro)

/* Now stick on a "_XXXX" prefix, it will now be "_XXXX1" if config_macro
 * is "1", or just "_XXXX" if it's undefined.
 *   ENABLED:   Z_IS_ENABLED2(_XXXX1)
 *   DISABLED   Z_IS_ENABLED2(_XXXX)
 */
#define Z_IS_ENABLED1(config_macro) Z_IS_ENABLED2(_XXXX##config_macro)

/* Here's the core trick, we map "_XXXX1" to "_YYYY," (i.e. a string
 * with a trailing comma), so it has the effect of making this a
 * two-argument tuple to the preprocessor only in the case where the
 * value is defined to "1"
 *   ENABLED:    _YYYY,    <--- note comma!
 *   DISABLED:   _XXXX
 */
#define _XXXX1 _YYYY,

/* Then we append an extra argument to fool the gcc preprocessor into
 * accepting it as a varargs macro.
 *                         arg1   arg2  arg3
 *   ENABLED:   Z_IS_ENABLED3(_YYYY,    1,    0)
 *   DISABLED   Z_IS_ENABLED3(_XXXX 1,  0)
 */
#define Z_IS_ENABLED2(one_or_two_args) Z_IS_ENABLED3(one_or_two_args true, false)

/* And our second argument is thus now cooked to be 1 in the case
 * where the value is defined to 1, and 0 if not:
 */
#define Z_IS_ENABLED3(ignore_this, val, ...) val



/* Some helper defines for traffic class support */
#if defined(CONFIG_NET_TC_TX_COUNT) && defined(CONFIG_NET_TC_RX_COUNT)
#define NET_TC_TX_COUNT CONFIG_NET_TC_TX_COUNT
#define NET_TC_RX_COUNT CONFIG_NET_TC_RX_COUNT

#if NET_TC_TX_COUNT > NET_TC_RX_COUNT
#define NET_TC_COUNT NET_TC_TX_COUNT
#else
#define NET_TC_COUNT NET_TC_RX_COUNT
#endif
#else /* CONFIG_NET_TC_TX_COUNT && CONFIG_NET_TC_RX_COUNT */
#define NET_TC_TX_COUNT 1
#define NET_TC_RX_COUNT 1
#define NET_TC_COUNT 1
#endif /* CONFIG_NET_TC_TX_COUNT && CONFIG_NET_TC_RX_COUNT */


#endif /* TARGET_AURIX_H */
