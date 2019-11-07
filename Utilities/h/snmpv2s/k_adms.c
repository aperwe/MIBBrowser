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
static const char rcsid[] = "$Id: k_adms.c,v 1.6.4.1 1998/03/19 14:21:18 partain Exp $";
#endif	/* (! ( defined(lint) ) && defined(SR_RCSID)) */

#include "sr_conf.h"

#include <stdio.h>

#include <string.h>

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif	/* HAVE_MEMORY_H */

#include "sr_snmp.h"
#include "sr_proto.h"
#include "comunity.h"
#include "v2clssc.h"
#include "sr_trans.h"
#include "context.h"
#include "mapctx.h"
#include "method.h"
#include "lookup.h"
#include "v2table.h"
#include "sr_cfg.h"
#include "scan.h"
#include "mibout.h"

contextMappingList_t contextMappingList_v2AdminSnmpScalarsData = { NULL, 0, NULL };
contextMappingList_t contextMappingList_v2AdminStatsData = { NULL, 0, NULL };

static v2AdminSnmpScalars_t *access_v2AdminSnmpScalarsData;
static v2AdminStats_t *access_v2AdminStatsData;

int
k_adms_initialize(
    char *contextName_text,
    v2AdminSnmpScalars_t *v2AdminSnmpScalarsData,
    v2AdminStats_t *v2AdminStatsData)
{
   SET_ALL_VALID(v2AdminSnmpScalarsData->valid);
   if (contextName_text == NULL) {
       AddContextMappingText(&contextMappingList_v2AdminSnmpScalarsData,
                             NULL,
                             v2AdminSnmpScalarsData);
       AddContextMappingText(&contextMappingList_v2AdminSnmpScalarsData,
                             "default",
                             v2AdminSnmpScalarsData);
   } else {
       AddContextMappingText(&contextMappingList_v2AdminSnmpScalarsData,
                             contextName_text,
                             v2AdminSnmpScalarsData);
   }

   SET_ALL_VALID(v2AdminStatsData->valid);
   if (contextName_text == NULL) {
      AddContextMappingText(&contextMappingList_v2AdminStatsData,
                            NULL,
                            v2AdminStatsData);
      AddContextMappingText(&contextMappingList_v2AdminStatsData,
                            "default",
                            v2AdminStatsData);
   } else {
      AddContextMappingText(&contextMappingList_v2AdminStatsData,
                            contextName_text,
                            v2AdminStatsData);
   }

   return 1;
}

int
k_adms_terminate(void)
{
   return 1;
}

#ifdef U_v2AdminSnmpScalars
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_v2AdminSnmpScalarsFreeUserpartData (data)
    v2AdminSnmpScalars_t *data;
{
    /* nothing to free by default */
}
#endif /* U_v2AdminSnmpScalars */

v2AdminSnmpScalars_t *
k_v2AdminSnmpScalars_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
    static int prev_serialNum = -1;
    static int prev_serialNum_initialized = 0;

    if (!prev_serialNum_initialized) {
        if (serialNum != prev_serialNum) {
            MapContext(&contextMappingList_v2AdminSnmpScalarsData,
                       contextInfo,
                       (void *)&access_v2AdminSnmpScalarsData);
        }
    } else {
        MapContext(&contextMappingList_v2AdminSnmpScalarsData,
                   contextInfo,
                   (void *)&access_v2AdminSnmpScalarsData);
        prev_serialNum_initialized = 1;
    }
    prev_serialNum = serialNum;

   return(access_v2AdminSnmpScalarsData);
}

#ifdef U_v2AdminStats
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_v2AdminStatsFreeUserpartData (data)
    v2AdminStats_t *data;
{
    /* nothing to free by default */
}
#endif /* U_v2AdminStats */

v2AdminStats_t *
k_v2AdminStats_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
    static int prev_serialNum = -1;
    static int prev_serialNum_initialized = 0;

    if (!prev_serialNum_initialized) {
        if (serialNum != prev_serialNum) {
            MapContext(&contextMappingList_v2AdminStatsData,
                       contextInfo,
                       (void *)&access_v2AdminStatsData);
        }
    } else {
        MapContext(&contextMappingList_v2AdminStatsData,
                   contextInfo,
                   (void *)&access_v2AdminStatsData);
        prev_serialNum_initialized = 1;
    }
    prev_serialNum = serialNum;

   return(access_v2AdminStatsData);
}
