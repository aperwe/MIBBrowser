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
static const char rcsid[] = "$Id: v_adms.c,v 1.3.4.1 1998/03/19 14:21:21 partain Exp $";
#endif	/* (! ( defined(lint) ) && defined(SR_RCSID)) */

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif	/* HAVE_MALLOC_H */

#include "sr_snmp.h"
#include "sr_proto.h"
#include "comunity.h"
#include "v2clssc.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "makevb.h"
#include "inst_lib.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"

#include "mibout.h"

/*---------------------------------------------------------------------
 * Retrieve data from the v2AdminSnmpScalars family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
v2AdminSnmpScalars_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;

#ifdef v2AdminSnmpScalars_DISABLE_CACHE
    v2AdminSnmpScalars_t *data;
#else /* v2AdminSnmpScalars_DISABLE_CACHE */
    static v2AdminSnmpScalars_t *data;
    static int             last_serialNum;
    static OID             *last_incoming;
    /* if the cache is not valid */
    if ( (serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
         (CmpOIDInst(incoming, last_incoming, (int)object->oid.length) != 0) ||
         (!VALID(object->nominator, data->valid)) ) {
     
#endif /* v2AdminSnmpScalars_DISABLE_CACHE */
    /*
     * Check the object instance.
     *
     * An EXACT search requires that the instance be of length 1 and
     * the single instance element be 0.
     *
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    switch (searchType) {
      case EXACT:
        if (instLength == 1 && incoming->oid_ptr[incoming->length - 1] == 0) {
          arg = object->nominator;
        }
        break;

      case NEXT:
        if (instLength <= 0) {
          arg = object->nominator;
        }
        break;

    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_v2AdminSnmpScalars_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

#ifndef v2AdminSnmpScalars_DISABLE_CACHE
     if (arg == -1) {
         data = NULL;
     }
     else {
         last_serialNum = serialNum;
         if (last_incoming != NULL) {
            FreeOID(last_incoming);
         }
         last_incoming = CloneOID(incoming);
     }
  }
  else {
     arg = object->nominator;
  }
#endif /* v2AdminSnmpScalars_DISABLE_CACHE */

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_snmpID
    case I_snmpID:
       dp = (void *) (CloneOctetString(data->snmpID));
       break;
#endif /* I_snmpID */

#ifdef I_snmpMaxMessageSize
    case I_snmpMaxMessageSize:
       dp = (void *) (&data->snmpMaxMessageSize);
       break;
#endif /* I_snmpMaxMessageSize */

#ifdef I_maxIdentityNameLength
    case I_maxIdentityNameLength:
       dp = (void *) (&data->maxIdentityNameLength);
       break;
#endif /* I_maxIdentityNameLength */

#ifdef I_maxGroupNameLength
    case I_maxGroupNameLength:
       dp = (void *) (&data->maxGroupNameLength);
       break;
#endif /* I_maxGroupNameLength */

#ifdef I_maxV2ContextNameLength
    case I_maxV2ContextNameLength:
       dp = (void *) (&data->maxV2ContextNameLength);
       break;
#endif /* I_maxV2ContextNameLength */

#ifdef I_maxViewTreeNameLength
    case I_maxViewTreeNameLength:
       dp = (void *) (&data->maxViewTreeNameLength);
       break;
#endif /* I_maxViewTreeNameLength */

#ifdef I_maxTransportLabelLength
    case I_maxTransportLabelLength:
       dp = (void *) (&data->maxTransportLabelLength);
       break;
#endif /* I_maxTransportLabelLength */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the v2AdminStats family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
v2AdminStats_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;

#ifdef v2AdminStats_DISABLE_CACHE
    v2AdminStats_t  *data;
#else /* v2AdminStats_DISABLE_CACHE */
    static v2AdminStats_t  *data;
    static int             last_serialNum;
    static OID             *last_incoming;
    /* if the cache is not valid */
    if ( (serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
         (CmpOIDInst(incoming, last_incoming, (int)object->oid.length) != 0) ||
         (!VALID(object->nominator, data->valid)) ) {
     
#endif /* v2AdminStats_DISABLE_CACHE */
    /*
     * Check the object instance.
     *
     * An EXACT search requires that the instance be of length 1 and
     * the single instance element be 0.
     *
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    switch (searchType) {
      case EXACT:
        if (instLength == 1 && incoming->oid_ptr[incoming->length - 1] == 0) {
          arg = object->nominator;
        }
        break;

      case NEXT:
        if (instLength <= 0) {
          arg = object->nominator;
        }
        break;

    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_v2AdminStats_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

#ifndef v2AdminStats_DISABLE_CACHE
     if (arg == -1) {
         data = NULL;
     }
     else {
         last_serialNum = serialNum;
         if (last_incoming != NULL) {
            FreeOID(last_incoming);
         }
         last_incoming = CloneOID(incoming);
     }
  }
  else {
     arg = object->nominator;
  }
#endif /* v2AdminStats_DISABLE_CACHE */

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_v2AdminStatsUnknownSPIs
    case I_v2AdminStatsUnknownSPIs:
       dp = (void *) (&data->v2AdminStatsUnknownSPIs);
       break;
#endif /* I_v2AdminStatsUnknownSPIs */

#ifdef I_v2AdminStatsUnknownContexts
    case I_v2AdminStatsUnknownContexts:
       dp = (void *) (&data->v2AdminStatsUnknownContexts);
       break;
#endif /* I_v2AdminStatsUnknownContexts */

#ifdef I_v2AdminStatsUnavailableContexts
    case I_v2AdminStatsUnavailableContexts:
       dp = (void *) (&data->v2AdminStatsUnavailableContexts);
       break;
#endif /* I_v2AdminStatsUnavailableContexts */

#ifdef I_v2AdminStatsCacheMisses
    case I_v2AdminStatsCacheMisses:
       dp = (void *) (&data->v2AdminStatsCacheMisses);
       break;
#endif /* I_v2AdminStatsCacheMisses */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}
