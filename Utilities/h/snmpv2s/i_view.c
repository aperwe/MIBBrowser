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
static const char rcsid[] = "$Id: i_view.c,v 1.3.4.1 1998/03/19 14:21:18 partain Exp $";
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

/* global data describing the viewTreeTable entries */
Index_t viewTreeEntryIndex[] = {
    { offsetof(viewTreeEntry_t, viewTreeName),  T_var_octet},
    { offsetof(viewTreeEntry_t, viewTreeSubTree),  T_oid}
};

const SnmpType viewTreeEntryTypeTable[] = {
    { OCTET_PRIM_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(viewTreeEntry_t, viewTreeName), 0 },
    { OBJECT_ID_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(viewTreeEntry_t, viewTreeSubTree), 1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(viewTreeEntry_t, viewTreeMask), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(viewTreeEntry_t, viewTreeType), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(viewTreeEntry_t, viewTreeMemoryType), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(viewTreeEntry_t, viewTreeStatus), -1 },
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(viewTreeEntry_t, RowStatusTimerId), -1 },
    /* Additional entries for user defines go here */
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(viewTreeEntry_t, userSkip), -1 },    /* userSkip */
    { -1, -1, (unsigned short) -1, -1 }
};
#ifdef SR_CONFIG_FP
const PARSER_CONVERTER viewTreeEntryConverters[] = {
    { ConvToken_textOctetString, NULL },
    { ConvToken_OID, NULL },
    { ConvToken_octetString, NULL },
    { SR_INTEGER_TRANSLATION, (char *)&viewType_TranslationTable },
    { SR_INTEGER_TRANSLATION, (char *)&storageType_TranslationTable },
    { ConvToken_integer, "1" },
    { ConvToken_integer, "-1" },
    { ConvToken_integer, "0" },
    { NULL, NULL }
};
const char viewTreeEntryRecordFormatString[] =
    "#Entry type: viewTreeEntry\n"
    "#Format:  viewTreeName  (text)\n"
    "#         viewTreeSubTree  (OID)\n"
    "#         viewTreeMask  (octetString)\n"
    "#         viewTreeType  (included, excluded)\n"
    "#         viewTreeMemoryType  (nonVolatile, permanent, readOnly)";
#endif /* SR_CONFIG_FP */

/* initialize support for v2AdminViewTree objects */
int
i_v2AdminViewTree_initialize(
    v2AdminViewTree_t *v2AdminViewTreeData)
{
    v2AdminViewTreeData->viewTreeSpinLock = 0;
    return 1;
}

/* terminate support for v2AdminViewTree objects */
int
i_v2AdminViewTree_terminate(
    v2AdminViewTree_t *v2AdminViewTreeData)
{
    return 1;
}

/* initialize support for viewTreeEntry objects */
int
i_viewTreeEntry_initialize(
    SnmpV2Table *viewTreeTable)
{
    /* initialize index descriptor */
    viewTreeTable->tp = (void **)malloc(sizeof(viewTreeEntry_t *));
    if (viewTreeTable->tp == NULL) {
        return 0;
    }
    viewTreeTable->tip = viewTreeEntryIndex;
    viewTreeTable->nitems = 0;
    viewTreeTable->nindices = sizeof(viewTreeEntryIndex) / sizeof(Index_t);
    viewTreeTable->rowsize = sizeof(viewTreeEntry_t);

    return 1;
}

/* terminate support for viewTreeEntry objects */
int
i_viewTreeEntry_terminate(
    SnmpV2Table *viewTreeTable)
{
    /* free allocated memory */
    while(viewTreeTable->nitems > 0) {
        FreeEntries(viewTreeEntryTypeTable, viewTreeTable->tp[0]);
        free(viewTreeTable->tp[0]);
        RemoveTableEntry(viewTreeTable, 0);
    }
    
    free(viewTreeTable->tp);
    return 1;
}
