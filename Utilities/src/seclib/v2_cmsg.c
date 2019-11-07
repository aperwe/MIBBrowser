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
 *
 */

#if (! ( defined(lint) ) && defined(SR_RCSID))
static const char rcsid[] = "$Id: v2_cmsg.c,v 1.4.4.1 1998/03/19 14:20:44 partain Exp $";
#endif	/* (! ( defined(lint) ) && defined(SR_RCSID)) */

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif	/* HAVE_MALLOC_H */

#include <string.h>

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif	/* HAVE_MEMORY_H */

#include "sr_snmp.h"
#include "sr_trans.h"
#include "snmpv2s/v2type.h"
#include "lookup.h"
#include "v2table.h"
#include "sr_msg.h"
#include "v2_msg.h"
#include "sr_user.h"
#include "userauth.h"

#include "diag.h"
SR_FILENAME

SnmpMessage *
SrCreateV2SnmpMessage(
    SR_UINT32       spi,
    OctetString    *authSnmpID,
    OctetString    *identityName,
    OctetString    *contextSnmpID,
    OctetString    *contextName,
#ifndef SR_UNSECURABLE
    OctetString    *authKey,
#endif /* SR_UNSECURABLE */
    SR_UINT32       cache)
{
    FNAME("SrCreateV2SnmpMessage")
    SnmpMessage *snmp_msg;

    snmp_msg = (SnmpMessage *)malloc(sizeof(SnmpMessage));
    if (snmp_msg == NULL) {
        DPRINTF((APWARN, "%s:  snmp_msg malloc\n", Fname));
        goto fail;
    }
    SR_CLEAR(snmp_msg);

    snmp_msg->version = SR_SNMPv2_VERSION;
    snmp_msg->u.v2.mms = 0;
    snmp_msg->u.v2.spi = spi;
    snmp_msg->u.v2.authSnmpBoots = 0;
    snmp_msg->u.v2.authSnmpTime = 0;
    if (cache) {
        snmp_msg->u.v2.authSnmpID = CloneOctetString(authSnmpID);
        if (snmp_msg->u.v2.authSnmpID == NULL) {
            DPRINTF((APWARN, "%s, authSnmpID malloc\n", Fname));
            goto fail;
        }
        snmp_msg->u.v2.identityName = CloneOctetString(identityName);
        if (snmp_msg->u.v2.identityName == NULL) {
            DPRINTF((APWARN, "%s, identityName malloc\n", Fname));
            goto fail;
        }
        snmp_msg->u.v2.contextSnmpID = CloneOctetString(contextSnmpID);
        if (snmp_msg->u.v2.contextSnmpID == NULL) {
            DPRINTF((APWARN, "%s, contextSnmpID malloc\n", Fname));
            goto fail;
        }
        if (contextName != NULL) {
            snmp_msg->u.v2.contextName = CloneOctetString(contextName);
        } else {
            snmp_msg->u.v2.contextName = MakeOctetStringFromText("default");
        }
        if (snmp_msg->u.v2.contextName == NULL) {
            DPRINTF((APWARN, "%s, contextName malloc\n", Fname));
            goto fail;
        }
#ifndef SR_UNSECURABLE
        if (authKey != NULL) {
            snmp_msg->u.v2.authKey = CloneOctetString(authKey);
            if (snmp_msg->u.v2.authKey == NULL) {
                DPRINTF((APWARN, "%s, authKey malloc\n", Fname));
                goto fail;
            }
        }
#endif /* SR_UNSECURABLE */
    } else {
        snmp_msg->u.v2.authSnmpID = authSnmpID;
        snmp_msg->u.v2.identityName = identityName;
        snmp_msg->u.v2.contextSnmpID = contextSnmpID;
        snmp_msg->u.v2.contextName = contextName;
#ifndef SR_UNSECURABLE
        snmp_msg->u.v2.authKey = authKey;
#endif /* SR_UNSECURABLE */
    }
    snmp_msg->cached = cache;
    return snmp_msg;

  fail:
    if (snmp_msg != NULL) {
        SrFreeSnmpMessage(snmp_msg);
        free(snmp_msg);
    }
    return NULL;
}
