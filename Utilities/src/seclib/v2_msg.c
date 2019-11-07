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
static const char rcsid[] = "$Id: v2_msg.c,v 1.7.4.1 1998/03/19 14:20:45 partain Exp $";
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

void
SrParseV2SnmpMessage(
    SnmpMessage *snmp_msg,
    SnmpLcd *sl_ptr,
    void *v_sl_ptr,
    TransportInfo *src_ti,
    TransportInfo *dst_ti,
    const unsigned char *msg,
    const int msg_len)
{
    FNAME("SrParseV2SnmpMessage")
    short type;
    SR_UINT32         secflags;
    OctetString       auth_msg;
    SnmpV2Lcd        *snmp_v2_lcd = (SnmpV2Lcd *)v_sl_ptr;

    /************/
    /* Step (2) */
    /************/
    snmp_msg->u.v2.mms = ParseUInt(&snmp_msg->wptr, snmp_msg->end_ptr, &type);
    if (type == -1) {
        DPRINTF((APPACKET, "%s: mms:\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        return;
    }

    /************/
    /* Step (3) */
    /************/
    secflags = ParseUInt(&snmp_msg->wptr, snmp_msg->end_ptr, &type);
    if (type == -1) {
        DPRINTF((APPACKET, "%s: secflags", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        return;
    }
    snmp_msg->u.v2.reportableFlag = secflags & 1;
    snmp_msg->u.v2.spi = secflags >> 1;
    switch (snmp_msg->u.v2.spi) {
        case SR_SPI_USEC_NOAUTH:
#ifndef SR_UNSECURABLE
        case SR_SPI_USEC_AUTH:
#endif /* SR_UNSECURABLE */
        case SR_SPI_MAINT:
            break;
        default:
            snmp_msg->error_code = UNKNOWN_SPI_ERROR;
            if (snmp_msg->u.v2.reportableFlag) {
                goto maint_report;
            }
            return;
    }

    /************/
    /* Step (4) */
    /************/
    /* Call the auth_priv service */
    auth_msg.octet_ptr = (unsigned char *) snmp_msg->wptr;
    auth_msg.length = snmp_msg->end_ptr - snmp_msg->wptr;
    SrParseUserAuthPrivMsg(sl_ptr, snmp_v2_lcd, src_ti, &auth_msg, snmp_msg);
    snmp_msg->wptr = auth_msg.octet_ptr;
    if (snmp_msg->error_code) {
        goto done;
    }

  done:
    /* Determine maximum size of a response pdu that can be sent back */
    snmp_msg->wrapper_size = snmp_msg->wptr - msg;
    snmp_msg->max_vbl_size = snmp_msg->u.v2.mms -
                             snmp_msg->wrapper_size -
                             SR_PDU_WRAPPER_SIZE;

    /* Copy remaining data (the PDU portion) into snmp_msg->packlet */
    snmp_msg->packlet =
        MakeOctetString(snmp_msg->wptr,
                        (SR_INT32) (snmp_msg->end_ptr - snmp_msg->wptr));
    return;

  maint_report:
    if (snmp_msg->u.v2.authSnmpID) {
        FreeOctetString(snmp_msg->u.v2.authSnmpID);
    }
    if (snmp_msg->u.v2.identityName) {
        FreeOctetString(snmp_msg->u.v2.identityName);
    }
    if (snmp_msg->u.v2.contextSnmpID) {
        FreeOctetString(snmp_msg->u.v2.contextSnmpID);
    }
    if (snmp_msg->u.v2.contextName) {
        FreeOctetString(snmp_msg->u.v2.contextName);
    }
    snmp_msg->u.v2.spi = SR_SPI_MAINT;
    snmp_msg->u.v2.authSnmpID =
        CloneOctetString(snmp_v2_lcd->v2AdminSnmpScalarsData->snmpID);
    snmp_msg->u.v2.identityName = MakeOctetStringFromText("report");
    snmp_msg->u.v2.contextSnmpID =
        CloneOctetString(snmp_v2_lcd->v2AdminSnmpScalarsData->snmpID);
    snmp_msg->u.v2.contextName = MakeOctetStringFromText("default");
    return;
}

int
SrBuildV2SnmpMessage(
    SnmpMessage *snmp_msg,
    Pdu *pdu_ptr,
    void *v_sl_ptr,
    SnmpLcd *sl_ptr)
{
    FNAME("SrBuildV2SnmpMessage")
    SR_UINT32         secflags;
    OctetString      *auth_msg = NULL;
    SR_INT32          lenlen;
    SR_INT32          datalen;
    unsigned char    *wptr;
    SnmpV2Lcd        *snmp_v2_lcd = (SnmpV2Lcd *)v_sl_ptr;

    snmp_msg->u.v2.mms =
        snmp_v2_lcd->v2AdminSnmpScalarsData->snmpMaxMessageSize;

    switch (snmp_msg->u.v2.spi) {
        case SR_SPI_MAINT:
        case SR_SPI_USEC_NOAUTH:
#ifndef SR_UNSECURABLE
        case SR_SPI_USEC_AUTH:
#endif /* SR_UNSECURABLE */
            break;

        default:
            DPRINTF((APWARN, "%s, bad sPI: %d\n", Fname, snmp_msg->u.v2.spi));
            goto fail;
    }

    if (SrBuildUserAuthPrivMsg(sl_ptr, snmp_v2_lcd, snmp_msg, pdu_ptr, 0)) {
        goto fail;
    }

    secflags = (snmp_msg->u.v2.spi << 1) | snmp_msg->u.v2.reportableFlag;
    datalen = snmp_msg->packlet->length +
              FindLenUInt(snmp_msg->version) +
              FindLenUInt(snmp_msg->u.v2.mms) +
              FindLenUInt(secflags);
    lenlen = DoLenLen(datalen);

    auth_msg = snmp_msg->packlet;
    snmp_msg->packlet = MakeOctetString(NULL, 1 + lenlen + datalen);
    if (snmp_msg->packlet == NULL) {
       DPRINTF((APPACKET, "%s: snmp_msg->packlet\n", Fname));
       goto fail;
    }
    wptr = snmp_msg->packlet->octet_ptr;
    *wptr++ = SEQUENCE_TYPE;
    AddLen(&wptr, lenlen, datalen);
    AddUInt(&wptr, INTEGER_TYPE, snmp_msg->version);
    AddUInt(&wptr, INTEGER_TYPE, snmp_msg->u.v2.mms);
    AddUInt(&wptr, INTEGER_TYPE, secflags);
    memcpy(wptr, auth_msg->octet_ptr, (int)auth_msg->length);
    FreeOctetString(auth_msg);
    return 0;

  fail:
    if (auth_msg != NULL) {
        FreeOctetString(auth_msg);
    }
    return -1;
}
