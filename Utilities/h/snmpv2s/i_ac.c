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
static const char rcsid[] = "$Id: i_ac.c,v 1.3.4.1 1998/03/19 14:21:16 partain Exp $";
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
#endif	/* HAVE_MALLOC_H */

#include <stddef.h>

#include "sr_snmp.h"
#include "sr_trans.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "snmpv2.h"
#include "v2type.h"

/* global data describing the acTable entries */
Index_t acEntryIndex[] = {
    { offsetof(acEntry_t, acSPI),  T_uint},
    { offsetof(acEntry_t, acGroupName),  T_var_octet},
    { offsetof(acEntry_t, acContextName),  T_octet}
};

const SnmpType acEntryTypeTable[] = {
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(acEntry_t, acSPI), 0 },
    { OCTET_PRIM_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(acEntry_t, acGroupName), 1 },
    { OCTET_PRIM_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(acEntry_t, acContextName), 2 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(acEntry_t, acContextNameMask), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(acEntry_t, acPrivs), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(acEntry_t, acReadViewName), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(acEntry_t, acWriteViewName), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(acEntry_t, acMemoryType), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(acEntry_t, acStatus), -1 },
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(acEntry_t, RowStatusTimerId), -1 },
    /* Additional entries for user defines go here */
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(acEntry_t, userSkip), -1 },
    { -1, -1, (unsigned short) -1, -1 }
};

#ifdef SR_CONFIG_FP
const PARSER_CONVERTER acEntryConverters[] = {
    { SR_INTEGER_TRANSLATION, (char *)&spi_TranslationTable },
    { ConvToken_textOctetString, NULL },
    { ConvToken_textOctetString, NULL },
    { ConvToken_octetString, NULL },
    { SR_INTEGER_TRANSLATION, (char *)&privs_TranslationTable },
    { ConvToken_textOctetString, NULL },
    { ConvToken_textOctetString, NULL },
    { SR_INTEGER_TRANSLATION, (char *)&storageType_TranslationTable },
    { ConvToken_integer, "1" },
    { ConvToken_integer, "-1" },
    { ConvToken_integer, "0" },
    { NULL, NULL }
};
const char acEntryRecordFormatString[] =
    "#Entry type: acEntry\n"
    "#Format:  acSPI  (snmpv1, snmpv2c, usecNoAuth, usecAuth, usecPriv)\n"
    "#         acGroupName  (text)\n"
    "#         acContextName  (text)\n"
    "#         acContextNameMask  (octetString)\n"
    "#         acPrivs  (nothing, readOnly, readWrite)\n"
    "#         acReadViewName  (text)\n"
    "#         acWriteViewName  (text)\n"
    "#         acMemoryType  (nonVolatile, permanent, readOnly)";
#endif /* SR_CONFIG_FP */

/* initialize support for v2AdminAccessControl objects */
int
i_v2AdminAccessControl_initialize(
    v2AdminAccessControl_t *v2AdminAccessControlData)
{
    v2AdminAccessControlData->acSpinLock = 0;
    return 1;
}

/* terminate support for v2AdminAccessControl objects */
int
i_v2AdminAccessControl_terminate(
    v2AdminAccessControl_t *v2AdminAccessControlData)
{
    return 1;
}

/* initialize support for acEntry objects */
int
i_acEntry_initialize(
    SnmpV2Table *acTable)
{
    /* initialize index descriptor */
    if ((acTable->tp = (void **) malloc (sizeof(acEntry_t *))) == NULL) {
        return 0;
    }
    acTable->tip = acEntryIndex;
    acTable->nitems = 0;
    acTable->nindices = sizeof(acEntryIndex) / sizeof(Index_t);
    acTable->rowsize = sizeof(acEntry_t);

    return 1;
}

/* terminate support for acEntry objects */
int
i_acEntry_terminate(
    SnmpV2Table *acTable)
{
    /* free allocated memory */
    while(acTable->nitems > 0) {
        FreeEntries(acEntryTypeTable, acTable->tp[0]);
        free(acTable->tp[0]);
        RemoveTableEntry(acTable, 0);
    }

    free(acTable->tp);
    return 1;
}
