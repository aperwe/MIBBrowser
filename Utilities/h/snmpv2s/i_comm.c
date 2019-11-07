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
static const char rcsid[] = "$Id: i_comm.c,v 1.4.4.1 1998/03/19 14:21:16 partain Exp $";
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

#include "sr_type.h"
#include "sr_time.h"

#include "sr_snmp.h"
#include "sr_trans.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "snmpv2.h"
#include "v2type.h"

/* global data describing the srCommunityTable entries */
Index_t srCommunityEntryIndex[] = {
    { offsetof(srCommunityEntry_t, srCommunityAuthSnmpID),  T_octet},
    { offsetof(srCommunityEntry_t, srCommunityName),  T_octet}
};

const SnmpType srCommunityEntryTypeTable[] = {
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(srCommunityEntry_t, srCommunityAuthSnmpID), 0 },
    { OCTET_PRIM_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(srCommunityEntry_t, srCommunityName), 1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(srCommunityEntry_t, srCommunityGroupName), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(srCommunityEntry_t, srCommunityContextSnmpID), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(srCommunityEntry_t, srCommunityContextName), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(srCommunityEntry_t, srCommunityTransportLabel), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(srCommunityEntry_t, srCommunityMemoryType), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(srCommunityEntry_t, srCommunityStatus), -1 },
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(srCommunityEntry_t, RowStatusTimerId), -1 },
    /* Additional entries for user defines go here */
    { -1, -1, (unsigned short) -1, -1 }
};

#ifdef SR_CONFIG_FP
const PARSER_CONVERTER srCommunityEntryConverters[] = {
    { ConvToken_snmpID, NULL },
    { ConvToken_textOctetString, NULL },
    { ConvToken_textOctetString, NULL },
    { ConvToken_snmpID, NULL },
    { ConvToken_textOctetString, NULL },
    { ConvToken_textOctetString, NULL },
    { SR_INTEGER_TRANSLATION, (char *)&storageType_TranslationTable },
    { ConvToken_integer, "1" },
    { ConvToken_integer, "-1" },
    { NULL, NULL }
};
const char srCommunityEntryRecordFormatString[] =
    "#Entry type: srCommunityEntry\n"
    "#Format:  srCommunityAuthSnmpID  (octetString)\n"
    "#         srCommunityName  (text)\n"
    "#         srCommunityGroupName  (text)\n"
    "#         srCommunityContextSnmpID  (octetString)\n"
    "#         srCommunityContextName  (text)\n"
    "#         srCommunityTransportLabel  (text)\n"
    "#         srCommunityMemoryType  (nonVolatile, permanent, readOnly)";
#endif /* SR_CONFIG_FP */

extern void srCommunityEntry_free
    SR_PROTOTYPE((srCommunityEntry_t *data));

/* initialize support for srCommunityMIB objects */
int
i_srCommunityMIB_initialize(
    srCommunityMIB_t *srCommunityMIBData)
{
   srCommunityMIBData->srCommunitySpinLock = 0;
    return 1;
}

/* terminate support for srCommunityMIB objects */
int
i_srCommunityMIB_terminate(
    srCommunityMIB_t *srCommunityMIBData)
{
    return 1;
}

/* initialize support for srCommunityEntry objects */
int
i_srCommunityEntry_initialize(
    SnmpV2Table *srCommunityTable)
{
    /* initialize index descriptor */
    srCommunityTable->tp = (void **)malloc (sizeof(srCommunityEntry_t *));
    if (srCommunityTable->tp == NULL) {
        return 0;
    }
    srCommunityTable->tip = srCommunityEntryIndex;
    srCommunityTable->nitems = 0;
    srCommunityTable->nindices = sizeof(srCommunityEntryIndex) / sizeof(Index_t);
    srCommunityTable->rowsize = sizeof(srCommunityEntry_t);

    return 1;
}

/* terminate support for srCommunityEntry objects */
int
i_srCommunityEntry_terminate(
    SnmpV2Table *srCommunityTable)
{
    /* free allocated memory */
    while(srCommunityTable->nitems > 0) {
        FreeEntries(srCommunityEntryTypeTable, srCommunityTable->tp[0]);
        free(srCommunityTable->tp[0]);
        RemoveTableEntry(srCommunityTable, 0);
    }
    
    free(srCommunityTable->tp);
    return 1;
}
