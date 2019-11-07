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
static const char rcsid[] = "$Id: i_trans.c,v 1.3.4.1 1998/03/19 14:21:17 partain Exp $";
#endif	/* (! ( defined(lint) ) && defined(SR_RCSID)) */

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#include <sys/types.h>

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif	/* HAVE_MEMORY_H */

#include <string.h>

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif  /* HAVE_MALLOC_H */

#include <stddef.h>

#include "sr_snmp.h"
#include "sr_trans.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "snmpv2.h"
#include "v2type.h"
#include "tdomain.h"

#ifdef SETS
extern void transportEntry_free
    SR_PROTOTYPE((transportEntry_t *data));
#endif	/* SETS */


v2AdminTransport_t v2AdminTransportData;

/* global data describing the transportTable entries */
Index_t transportEntryIndex[] = {
    { offsetof(transportEntry_t, transportLabel),  T_var_octet},
    { offsetof(transportEntry_t, transportSubindex),  T_uint}
};

const SnmpType transportEntryTypeTable[] = {
    { OCTET_PRIM_TYPE, SR_NOT_ACCESSIBLE, offsetof(transportEntry_t, transportLabel), 0 },
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE, offsetof(transportEntry_t, transportSubindex), 1 },
    { OBJECT_ID_TYPE, SR_READ_CREATE, offsetof(transportEntry_t, transportDomain), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE, offsetof(transportEntry_t, transportAddress), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE, offsetof(transportEntry_t, transportReceiveMask), -1 },
    { INTEGER_TYPE, SR_READ_CREATE, offsetof(transportEntry_t, transportMMS), -1 },
    { INTEGER_TYPE, SR_READ_CREATE, offsetof(transportEntry_t, transportMemoryType), -1 },
    { INTEGER_TYPE, SR_READ_CREATE, offsetof(transportEntry_t, transportStatus), -1 },
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE, offsetof(transportEntry_t, RowStatusTimerId), -1 },
    /* Additional entries for user defines go here */
    { -1, -1, (unsigned short) -1, -1 }
};

#ifdef SR_CONFIG_FP
const PARSER_CONVERTER transportEntryConverters[] = {
    { ConvToken_textOctetString, NULL },
    { ConvToken_integer, NULL },
    { ConvToken_TDomain, NULL },
    { ConvToken_TAddr, NULL },
    { ConvToken_TMask, NULL },
    { ConvToken_integer, NULL },
    { SR_INTEGER_TRANSLATION, (char *)&storageType_TranslationTable },
    { ConvToken_integer, "1" },
    { ConvToken_integer, "-1" },
    { NULL, NULL }
};
const char transportEntryRecordFormatString[] =
    "#Entry type: transportEntry\n"
    "#Format:  transportLabel  (text)\n"
    "#         transportSubIndex  (integer)\n"
    "#         transportDomain  (snmpUDPDomain, snmpIPXDomain, etc.)\n"
    "#         transportAddress  (transport address, i.e. 192.147.142.254:0)\n"
    "#         transportReceiveMask  (transport mask, i.e. 255.255.255.252:0)\n"
    "#         transportMMS  (integer)\n"
    "#         transportMemoryType  (nonVolatile, permanent, readOnly)";
#endif /* SR_CONFIG_FP */

/* initialize support for v2AdminTransport objects */
int
i_v2AdminTransport_initialize(
    v2AdminTransport_t *v2AdminTransportData)
{
    v2AdminTransportData->transportSpinLock = 0;
    return 1;
}

/* terminate support for v2AdminTransport objects */
int
i_v2AdminTransport_terminate(
    v2AdminTransport_t *v2AdminTransportData)
{
    return 1;
}

/* initialize support for transportEntry objects */
int
i_transportEntry_initialize(
    SnmpV2Table *transportTable)
{
    /* initialize index descriptor */
    transportTable->tp = (void **)malloc(sizeof(transportEntry_t *));
    if (transportTable->tp == NULL) {
        return 0;
    }
    transportTable->tip = transportEntryIndex;
    transportTable->nitems = 0;
    transportTable->nindices = sizeof(transportEntryIndex) / sizeof(Index_t);
    transportTable->rowsize = sizeof(transportEntry_t);

    return 1;
}

/* terminate support for transportEntry objects */
int
i_transportEntry_terminate(
    SnmpV2Table *transportTable)
{
    /* free allocated memory */
    while(transportTable->nitems > 0) {
        FreeEntries(transportEntryTypeTable, transportTable->tp[0]);
        free(transportTable->tp[0]);
        RemoveTableEntry(transportTable, 0);
    }
    
    free(transportTable->tp);
    return 1;
}
