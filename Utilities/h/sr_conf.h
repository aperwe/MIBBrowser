/*
 *
 * Copyright (C) 1992-1998 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 252.227-7013;
 *  subparagraphs (c)(4) and (d) of the Commercial Computer
 *  Software-Restricted Rights Clause, FAR 52.227-19; and in similar
 *  clauses in the NASA FAR Supplement and other corresponding
 *  governmental regulations.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

/* $Id: sr_conf.h,v 1.56.2.5.2.2 1998/05/21 17:15:21 vaughn Exp $ */

/*
 * This file in turn includes OS-specific .h configuration files as well
 * as the feature-based config file.
 *
 * This file should be included FIRST in every file (even before system
 * .h files) with a "#include "sr_conf.h"" directive.
 */

#ifndef SR_SR_CONF_H
#define SR_SR_CONF_H

#ifdef  __cplusplus
extern "C" {
#endif

#define SR_MAJOR_VERSION      15
#define SR_MINOR_VERSION      1
#define SR_BRANCH_VERSION     1
#define SR_PATCH_VERSION      1
#define SR_MAJOR_VERSION_STR  "15"
#define SR_MINOR_VERSION_STR  "1"
#define SR_BRANCH_VERSION_STR "1"
#define SR_PATCH_VERSION_STR  "1"
#define SR_VERSION            "15.1.1.1"

#ifdef SR_SNMPv1_ADMIN
#ifdef SR_SNMPv1_PACKET_MIB
#undef SR_SNMPv1_PACKET_MIB
#endif /* SR_SNMPv1_PACKET_MIB */
#endif /* SR_SNMPv1_ADMIN */

#ifdef SR_SNMP_ADMIN_MIB
#ifdef SR_SNMPv2_ADMIN
#define SR_SNMPv2_ADMIN_MIB
#endif /* SR_SNMPv2_ADMIN_MIB */
#ifdef SR_SNMPv3_ADMIN
#define SR_SNMPv3_ADMIN_MIB
#endif /* SR_SNMPv3_ADMIN_MIB */
#endif /* SR_SNMP_ADMIN_MIB */

#ifdef SR_NOTIFY_BASIC_WITH_FILTERS_COMPLIANCE
#define SR_NOTIFY_FILTERING
#endif /* SR_NOTIFY_BASIC_WITH_FILTERS_COMPLIANCE */
#ifdef SR_NOTIFY_FULL_COMPLIANCE
#define SR_NOTIFY_FILTERING
#endif /* SR_NOTIFY_FULL_COMPLIANCE */



#ifdef SCO5
#include "osconf/sco5.h"
#endif /* SCO5 */






#ifdef SR_WIN32
#ifndef SR_WINCE
#include "osconf/win32.h"
#else /* SR_WINCE */
#include "osconf/wince.h"
#endif /* SR_WINCE */
#endif /* SR_WIN32 */







#ifdef UNIXWARE
#include "osconf/unixware.h"
#endif	/* UNIXWARE */

#ifdef AIX
#include "osconf/aix.h"
#endif /* AIX */



#ifdef HPUX
#ifdef HPUX10
#include "osconf/hpux1001.h"
#else
#include "osconf/hpux903.h"
#endif	/* HPUX10 */
#endif /* HPUX */





#ifdef SR_OSF
#ifndef SR_OSF4
#include "osconf/osf.h"
#else	/* SR_OSF4 */
#include "osconf/osf4.h"
#endif	/* SR_OSF4 */
#endif /* SR_OSF */

#ifdef SOLARIS
#include "osconf/sunos53.h"
#endif /* SOLARIS */

#ifdef SUNOS
#include "osconf/sunos413.h"
#endif	/* SUNOS */

#ifdef SINIX_MIPS
#include "osconf/sinix542mips.h"
#endif	/* SUNOS */









/*
 * For now, if SELECT_BITMASK_TYPE is undefined, define it to fd_set*.
 * In the future, this may appear in each OS-specific *.h file.
 */
#ifndef SELECT_BITMASK_TYPE
#define SELECT_BITMASK_TYPE fd_set *
#endif /* SELECT_BITMASK_TYPE */

/*
 * SR_AGT_CONF_FILE_PREFIX: the directory (without the ending /) holding the
 * agent configuration files
 */
#ifndef SR_AGT_CONF_FILE_PREFIX
#ifndef SR_SIEMENS_NIXDORF
#define SR_AGT_CONF_FILE_PREFIX "/etc/srconf/agt"
#else	/* SR_SIEMENS_NIXDORF */
#define SR_AGT_CONF_FILE_PREFIX "/etc/snmp/agt"
#endif	/* SR_SIEMENS_NIXDORF */
#endif	/* SR_AGT_CONF_FILE_PREFIX */

/*
 * SR_MGR_CONF_FILE_PREFIX: the directory (without the trailing /)
 * holding the management configuration files
 */
#ifndef SR_MGR_CONF_FILE_PREFIX
#ifndef SR_SIEMENS_NIXDORF
#define SR_MGR_CONF_FILE_PREFIX "/etc/srconf/mgr"
#else	/* SR_SIEMENS_NIXDORF */
#define SR_MGR_CONF_FILE_PREFIX "/etc/snmp/mgr"
#endif	/* SR_SIEMENS_NIXDORF */
#endif	/* SR_MGR_CONF_FILE_PREFIX */

/*
 * POLL_INTERFACES:  whether to include code to periodically poll the
 * interface configuration
 */
#ifndef POLL_INTERFACES

#if defined(SUNOS)
#define POLL_INTERFACES
#endif	/* defined(SUNOS) */

#endif	/* POLL_INTERFACES */

/*
 * POLL_INTERVAL:  the number of seconds between polling of interfaces
 */
#ifndef POLL_INTERVAL
#define POLL_INTERVAL 5
#endif	/* POLL_INTERVAL */

/*
 * UNIX:  define this if your system is pretty UNIX-ish, including such
 * things as signal(), BSD sockets, and so forth
 */
#ifndef UNIX	/* in case it's on make command line */
#endif	/* UNIX */

#include "sr_featr.h"

#ifndef SR_CLEAR_MALLOC
#define SR_CLEAR(x)
#define SR_CLEAR_N(x,n)
#else /* SR_CLEAR_MALLOC */
#define SR_CLEAR(x) (memset((char *)(x), 0, sizeof(*(x))))
#define SR_CLEAR_N(x,n) (memset((char *)(x), 0, sizeof(*(x)) * (n)))
#endif /* SR_CLEAR_MALLOC */

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif	/* SR_SR_CONF_H */
