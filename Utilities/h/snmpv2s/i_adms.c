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
 */

#if (! ( defined(lint) ) && defined(SR_RCSID))
static const char rcsid[] = "$Id: i_adms.c,v 1.4.4.1 1998/03/19 14:21:16 partain Exp $";
#endif	/* (! ( defined(lint) ) && defined(SR_RCSID)) */

#include "sr_conf.h"

#include <stdio.h>

#include <string.h>

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif	/* HAVE_MEMORY_H */

#include "sr_snmp.h"
#include "sr_trans.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "v2type.h"

int
i_adms_initialize(
    OctetString *snmpID,
    v2AdminSnmpScalars_t *v2AdminSnmpScalarsData,
    v2AdminStats_t *v2AdminStatsData)
{
   v2AdminSnmpScalarsData->snmpID = snmpID;
   v2AdminSnmpScalarsData->snmpMaxMessageSize = SR_DEFAULT_MAX_MSG_SIZE;
   v2AdminSnmpScalarsData->maxIdentityNameLength = 255;
   v2AdminSnmpScalarsData->maxGroupNameLength = 255;
   v2AdminSnmpScalarsData->maxV2ContextNameLength = 255;
   v2AdminSnmpScalarsData->maxViewTreeNameLength = 255;
   v2AdminSnmpScalarsData->maxTransportLabelLength = 255;

   v2AdminStatsData->v2AdminStatsUnknownSPIs = 0;
   v2AdminStatsData->v2AdminStatsUnknownContexts = 0;
   v2AdminStatsData->v2AdminStatsUnavailableContexts = 0;
   v2AdminStatsData->v2AdminStatsCacheMisses = 0;

   return 1;
}

int
i_adms_terminate(
    v2AdminSnmpScalars_t *v2AdminSnmpScalarsData,
    v2AdminStats_t *v2AdminStatsData)
{
   return 1;
}
