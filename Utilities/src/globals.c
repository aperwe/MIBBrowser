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

#if (! ( defined(lint) ) && defined(SR_RCSID))
static const char rcsid[] = "$Id: globals.c,v 1.8.4.1 1998/03/19 14:22:23 partain Exp $";
#endif	/* (! ( defined(lint) ) && defined(SR_RCSID)) */

#include "sr_conf.h"
#include "sr_type.h"
#include "sr_snmp.h"
#include "util_def.h"

/* Number of times to retry a request */
int retries;
int tretries;

/*
 * The following variables are used to maintain information about what
 * SNMP version is being used, details about authentication info for
 * the version in use, and other info used for sending/receiving SNMP
 * packets.
 */
int communication_type;
int util_version;
char *util_name;
char *util_dest;
char *util_handle;

#define FNBUFSIZ 256
char            fn_snmpinfo_dat[FNBUFSIZ];
char            fn_mgr_file[FNBUFSIZ];

SR_INT32 global_req_id;
