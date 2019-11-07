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
static const char rcsid[] = "$Id: i_notify.c,v 1.4.4.1 1998/03/19 14:21:17 partain Exp $";
#endif	/* (! ( defined(lint) ) && defined(SR_RCSID)) */

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#include <sys/types.h>

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif  /* HAVE_MEMORY_H */

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

/* note: the following prototypes may be included automatically now.
         check for duplicates  -cws   */
void notifyEntry_free
     SR_PROTOTYPE((notifyEntry_t *data));

int notifyEntryReadyToActivate
     SR_PROTOTYPE((notifyEntry_t *data));


/* global data describing the notifyTable entries */
Index_t notifyEntryIndex[] = {
    { offsetof(notifyEntry_t, notifyIndex),  T_uint}
};

const SnmpType notifyEntryTypeTable[] = {
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(notifyEntry_t, notifyIndex), 0 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(notifyEntry_t, notifySPI), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(notifyEntry_t, notifyIdentityName), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(notifyEntry_t, notifyTransportLabel), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(notifyEntry_t, notifyContextName), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(notifyEntry_t, notifyViewName), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(notifyEntry_t, notifyMemoryType), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(notifyEntry_t, notifyStatus), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(notifyInformParametersEntry_t, notifyConfirm), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(notifyInformParametersEntry_t, notifyAuthSnmpID), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(notifyInformParametersEntry_t, notifyTimeout), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(notifyInformParametersEntry_t, notifyMaxRetry), -1 },
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(notifyEntry_t, RowStatusTimerId), -1 },
    /* Additional entries for user defines go here */
    { -1, -1, (unsigned short) -1, -1 }
};

#ifdef SR_CONFIG_FP
const PARSER_CONVERTER notifyEntryConverters[] = {
    { ConvToken_integer, NULL },
    { SR_INTEGER_TRANSLATION, (char *)&spi_TranslationTable },
    { ConvToken_textOctetString, NULL },
    { ConvToken_textOctetString, NULL },
    { ConvToken_textOctetString, NULL },
    { ConvToken_textOctetString, NULL },
    { SR_INTEGER_TRANSLATION, (char *)&storageType_TranslationTable },
    { ConvToken_integer, "1" },
    { ConvToken_integer, "0" },
    { ConvToken_octetString, "-" },
    { ConvToken_integer, "0" },
    { ConvToken_integer, "0" },
    { ConvToken_integer, "-1" },
    { NULL, NULL }
};
const char notifyEntryRecordFormatString[] =
    "#Entry type: notifyEntry\n"
    "#Format:  notifyIndex  (integer)\n"
    "#         notifySPI  (snmpv1, snmpv2c, usecNoAuth, usecAuth, usecPriv)\n"
    "#         notifyIdentityName  (text)\n"
    "#         notifyTransportLabel  (text)\n"
    "#         notifyContextName  (text)\n"
    "#         notifyViewName  (text)\n"
    "#         notifyMemoryType  (nonVolatile, permanent, readOnly)";
#endif /* SR_CONFIG_FP */

/* initialize support for v2AdminNotify objects */
int
i_v2AdminNotify_initialize(
    v2AdminNotify_t *v2AdminNotifyData)
{
   v2AdminNotifyData->notifySpinLock = 0;
    return 1;
}

/* terminate support for v2AdminNotify objects */
int
i_v2AdminNotify_terminate(
    v2AdminNotify_t *v2AdminNotifyData)
{
    return 1;
}

/* initialize support for notifyEntry objects */
int
i_notifyEntry_initialize(
    SnmpV2Table *notifyTable)
{
    /* initialize index descriptor */
    notifyTable->tp = (void **)malloc(sizeof(notifyEntry_t *));
    if (notifyTable->tp == NULL) {
        return 0;
    }
    notifyTable->tip = notifyEntryIndex;
    notifyTable->nitems = 0;
    notifyTable->nindices = sizeof(notifyEntryIndex) / sizeof(Index_t);
    notifyTable->rowsize = sizeof(notifyEntry_t);

    return 1;
}

/* terminate support for notifyEntry objects */
int
i_notifyEntry_terminate(
    SnmpV2Table *notifyTable)
{
    /* free allocated memory */
    while(notifyTable->nitems > 0) {
        FreeEntries(notifyEntryTypeTable, notifyTable->tp[0]);
        free(notifyTable->tp[0]);
        RemoveTableEntry(notifyTable, 0);
    }
    
    free(notifyTable->tp);
    return 1;
}
