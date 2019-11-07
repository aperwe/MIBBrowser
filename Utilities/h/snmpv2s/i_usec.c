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
static const char rcsid[] = "$Id: i_usec.c,v 1.3.4.1 1998/03/19 14:21:17 partain Exp $";
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

const SnmpType snmpBootsType[] = {
    { INTEGER_TYPE, SR_READ_WRITE, (unsigned short) -1, -1 },
    { -1, -1, (unsigned short) -1, -1 }
};
#ifdef SR_CONFIG_FP
const PARSER_CONVERTER snmpBootsConverters[] = {
    { ConvToken_integer, NULL },
    { NULL, NULL }
};
#endif /* SR_CONFIG_FP */

/* global data describing the usecUser family */

/* global data describing the userNameTable entries */
Index_t userNameEntryIndex[] = {
    { offsetof(userNameEntry_t, userAuthSnmpID),  T_octet},
    { offsetof(userNameEntry_t, userName),  T_octet}
};

const SnmpType userNameEntryTypeTable[] = {
    { OCTET_PRIM_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(userNameEntry_t, userAuthSnmpID), 0 },
    { OCTET_PRIM_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(userNameEntry_t, userName), 1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(userNameEntry_t, userGroupName), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(userNameEntry_t, userAuthChange), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(userNameEntry_t, userPrivChange), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(userNameEntry_t, userNovel), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(userNameEntry_t, userTransportLabel), -1 },
    { OBJECT_ID_TYPE, SR_READ_CREATE,
      offsetof(userNameEntry_t, userCloneFrom), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(userNameEntry_t, userMemoryType), -1 },
    { INTEGER_TYPE, SR_READ_CREATE,
      offsetof(userNameEntry_t, userStatus), -1 },
    { INTEGER_TYPE, SR_NOT_ACCESSIBLE,
      offsetof(userNameEntry_t, RowStatusTimerId), -1 },
    /* Additional entries for user defines go here */
#ifndef SR_UNSECURABLE
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(userNameEntry_t, auth_secret), -1 },
    { OCTET_PRIM_TYPE, SR_READ_CREATE,
      offsetof(userNameEntry_t, new_auth_secret), -1 },
#endif /* SR_UNSECURABLE */
    { -1, -1, (unsigned short) -1, -1 }
};

#ifdef SR_CONFIG_FP
const PARSER_CONVERTER userNameEntryConverters[] = {
    { ConvToken_snmpID, NULL },
    { ConvToken_textOctetString, NULL },
    { ConvToken_textOctetString, NULL },
    { ConvToken_textOctetString, "-" },
    { ConvToken_textOctetString, "-" },
    { ConvToken_textOctetString, "-" },
    { ConvToken_textOctetString, NULL },
    { ConvToken_OID, "0.0" },
    { SR_INTEGER_TRANSLATION, (char *)&storageType_TranslationTable },
    { ConvToken_integer, "1" },
    { ConvToken_integer, "-1" },
#ifndef SR_UNSECURABLE
    { ConvToken_authPrivPassword, NULL },
    { ConvToken_authPrivPassword, "-" },
#endif /* SR_UNSECURABLE */
    { NULL, NULL }
};
const char userNameEntryRecordFormatString[] =
    "#Entry type: userNameEntry\n"
    "#Format:  userAuthSnmpID  (octetString)\n"
    "#         userName  (text)\n"
    "#         userGroupName  (text)\n"
    "#         userTransportLabel  (text)\n"
    "#         userMemoryType  (nonVolatile, permanent, readOnly)"
#ifndef SR_UNSECURABLE
    "\n"
    "#         AuthKey  (octetString)"
#endif /* SR_UNSECURABLE */
    ;
#endif /* SR_CONFIG_FP */

int
i_usecScalars_initialize(
    usecScalars_t *usecScalarsData)
{
    usecScalarsData->snmpBoots = 0;
    usecScalarsData->snmpTime = 0;
    return 1;
}

int
i_usecScalars_terminate(
    usecScalars_t *usecScalarsData)
{
    return 1;
}

int
i_usecStats_initialize(
    usecStats_t *usecStatsData)
{
    usecStatsData->usecStatsNotInTimeWindows = 0;
    usecStatsData->usecStatsUnknownUserNames = 0;
    usecStatsData->usecStatsWrongDigestValues = 0;
    usecStatsData->usecStatsBadPrivacys = 0;
    return 1;
}

int
i_usecStats_terminate(
    usecStats_t *usecStatsData)
{
    return 1;
}

int
i_usecUser_initialize(
    usecUser_t *usecUserData)
{
    usecUserData->userSpinLock = 0;
    usecUserData->maxUserNameLength = 255;
    return 1;
}

int
i_usecUser_terminate(
    usecUser_t *usecUserData)
{
    return 1;
}

int
i_userNameEntry_initialize(
    SnmpV2Table *userNameTable)
{
    userNameTable->tp = (void **)malloc(sizeof(userNameEntry_t *));
    if (userNameTable->tp == NULL) {
        return 0;
    }
    userNameTable->tip = userNameEntryIndex;
    userNameTable->nitems = 0;
    userNameTable->nindices = sizeof(userNameEntryIndex) / sizeof(Index_t);
    userNameTable->rowsize = sizeof(userNameEntry_t);

    return 1;
}

int
i_userNameEntry_terminate(
    SnmpV2Table *userNameTable)
{
    /* free allocated memory */
    while(userNameTable->nitems > 0) {
        FreeEntries(userNameEntryTypeTable, userNameTable->tp[0]);
        free(userNameTable->tp[0]);
        RemoveTableEntry(userNameTable, 0);
    }
    
    free(userNameTable->tp);
    return 1;
}
