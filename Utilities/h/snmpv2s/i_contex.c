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
static const char rcsid[] = "$Id: i_contex.c,v 1.3.4.1 1998/03/19 14:21:16 partain Exp $";
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

/* global data describing the v2ContextTable entries */
Index_t v2ContextEntryIndex[] = {
    { offsetof(v2ContextEntry_t, v2ContextSnmpID),  T_octet},
    { offsetof(v2ContextEntry_t, v2ContextName),  T_octet}
};

const SnmpType v2ContextEntryTypeTable[] = {
    { OCTET_PRIM_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(v2ContextEntry_t, v2ContextSnmpID), 0 },
    { OCTET_PRIM_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(v2ContextEntry_t, v2ContextName), 1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(v2ContextEntry_t, v2ContextLocalEntity), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(v2ContextEntry_t, v2ContextLocalTime), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(v2ContextEntry_t, v2ContextMemoryType), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(v2ContextEntry_t, v2ContextStatus), -1 },
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(v2ContextEntry_t, RowStatusTimerId), -1 },
    /* Additional entries for user defines go here */
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(v2ContextEntry_t, userSkip), -1}, /* userSkip */
    { COUNTER_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(v2ContextEntry_t, referenceCount), -1}, /* referenceCount */
    { -1, -1, (unsigned short) -1, -1 }
};

#ifdef SR_CONFIG_FP
const PARSER_CONVERTER v2ContextEntryConverters[] = {
    { ConvToken_snmpID, NULL },
    { ConvToken_textOctetString, NULL },
    { ConvToken_textOctetString, NULL },
    { ConvToken_integer, NULL },
    { SR_INTEGER_TRANSLATION, (char *)&storageType_TranslationTable },
    { ConvToken_integer, "1" },
    { ConvToken_integer, "-1" },
    { ConvToken_integer, "0" },
    { ConvToken_unsigned, "1" },
    { NULL, NULL }
};
const char v2ContextEntryRecordFormatString[] =
    "#Entry type: v2ContextEntry\n"
    "#Format:  v2ContextSnmpID  (octetString)\n"
    "#         v2ContextName  (text)\n"
    "#         v2ContextLocalEntity  (text)\n"
    "#         v2ContextLocalTime  (integer)\n"
    "#         v2ContextMemoryType  (nonVolatile, permanent, readOnly)";
#endif /* SR_CONFIG_FP */

/* initialize support for v2ContextEntry objects */
int
i_v2ContextEntry_initialize(
    OctetString *snmpID,
    SnmpV2Table *v2ContextTable)
{
    int index;
    v2ContextEntry_t *vce;

    /* initialize index descriptor */
    v2ContextTable->tp = (void **)malloc(sizeof(v2ContextEntry_t *));
    if (v2ContextTable->tp == NULL) {
        return 0;
    }
    v2ContextTable->tip = v2ContextEntryIndex;
    v2ContextTable->nitems = 0;
    v2ContextTable->nindices = sizeof(v2ContextEntryIndex) / sizeof(Index_t);
    v2ContextTable->rowsize = sizeof(v2ContextEntry_t);

    /* Add the default context */
    v2ContextTable->tip[0].value.octet_val = snmpID;
    v2ContextTable->tip[1].value.octet_val = MakeOctetStringFromText("default");
    if (v2ContextTable->tip[1].value.octet_val == NULL) {
        return 0;
    }
    index = NewTableEntry(v2ContextTable);
    FreeOctetString(v2ContextTable->tip[1].value.octet_val);
    if (index == -1) {
        return 0;
    }
    vce = (v2ContextEntry_t *)v2ContextTable->tp[index];
    vce->v2ContextLocalEntity = MakeOctetString(NULL, 0);
    vce->v2ContextLocalTime = 1;
    vce->v2ContextMemoryType = READONLY;
    vce->v2ContextStatus = RS_ACTIVE;
    vce->userSkip = 2;
    vce->referenceCount = -1;


    return 1;
}

/* terminate support for v2ContextEntry objects */
int
i_v2ContextEntry_terminate(
    SnmpV2Table *v2ContextTable)
{
    /* free allocated memory */
    while(v2ContextTable->nitems > 0) {
        FreeEntries(v2ContextEntryTypeTable, v2ContextTable->tp[0]);
        free(v2ContextTable->tp[0]);
        RemoveTableEntry(v2ContextTable, 0);
    }
    
    free(v2ContextTable->tp);
    return 1;
}
