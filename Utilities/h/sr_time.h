/*
 *
 * Copyright (C) 1996-1998 by SNMP Research, Incorporated.
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

/* $Id: sr_time.h,v 1.12 1998/01/08 02:36:10 mark Exp $ */

/*
 * This file simply gathers all of the necessary include files for using
 * time functions.
 */

#ifndef SR_SR_TIME_H
#define SR_SR_TIME_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "sr_conf.h"

#include <sys/types.h>

#ifdef TIME_WITH_SYS_TIME
#include <sys/time.h>
#include <time.h>
#else
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#include <time.h>
#endif	/* HAVE_SYS_TIME_H */
#endif	/* TIME_WITH_SYS_TIME */

#ifdef HAVE_SYS_TIMES_H
#include <sys/times.h>
#endif	/* HAVE_SYS_TIMES_H */

#ifdef HAVE_FTIME
#include <sys/timeb.h>
#endif	/* HAVE_FTIME */



#ifdef  __cplusplus
}
#endif

#endif	/* SR_SR_TIME_H */
