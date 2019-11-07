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

#include "sr_conf.h"

#include <stdio.h>

#include <ctype.h>

#include <stdlib.h>

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif	/* HAVE_MALLOC_H */

#include <string.h>

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif	/* HAVE_MEMORY_H */

#include <stddef.h>

#include "sr_snmp.h"
#include "sr_trans.h"
#include "global.h"
#ifndef SR_UNSECURABLE
#include "md5.h"
#endif /* SR_UNSECURABLE */
#include "sr_boots.h"
#include "diag.h"
SR_FILENAME

#include "lookup.h"
#include "v2table.h"
#include "scan.h"

#include "sr_user.h"

#include "sr_ad.h"

#include "snmpv2s/v2type.h"
#include "sr_msg.h"
#include "v2_msg.h"
#include "userauth.h"
#include "sr_vta.h"

static void SrLookupSnmpBootsAndTime(
    SnmpV2Lcd *snmp_v2_lcd,
    OctetString *snmpID,
    SR_INT32 *snmpBoots,
    SR_INT32 *snmpTime);

/*
 * BuildUserAuthPrivMsg
 *
 * This is the hook function called by BuildAuthentication to produce an
 * SnmpV2AuthMessage.  The arguments are:
 *
 *    scoped_pdu - contains the serialized plain-text ScopedPDU
 *    auth_ptr   - should contain the authSnmpID and identityName to
 *                 be used in building the packet.  This structure may
 *                 also contain cached information about this identity.
 *                 If it does not contain cached information, then this
 *                 info must be retrieved from the LCD.
 *    padding    - Specifies the padding required at the beginning of the
 *                 SnmpV2AuthMessage required to make space for the
 *                 additional fields in the final SnmpV2Message.  This
 *                 is used to reduce the number of OctetString structures
 *                 which must be allocated.
 *
 * The resulting SnmpV2AuthMessage (along with padding) is placed in the
 * packlet field of the auth_ptr argument.
 *
 * The return value is -1 on failure, 0 on success. (maybe should change)
 */
int
SrBuildUserAuthPrivMsg(
    SnmpLcd *snmp_lcd,
    SnmpV2Lcd *snmp_v2_lcd,
    SnmpMessage *snmp_msg,
    Pdu *pdu_ptr,
    SR_UINT32 padding)
{
    FNAME("SrBuildUserAuthPrivMsg")
    SR_UINT32     priv_len = 0;
    SR_UINT32 len;
    SR_UINT32     ainfo_len, ainfo_lenlen, ainfo_total_len;
    SR_UINT32     amsg_len, amsg_lenlen, amsg_total_len;
    unsigned char *auth_msg = NULL;
    unsigned char *wptr = NULL;
    unsigned char *digest_ptr = NULL;
#ifndef SR_UNSECURABLE
    int need_keys = 0;
    UserData userData;
    MD5_CTX       MD;
    unsigned char md5_digest[MD5_DIGEST_SIZE];
#endif /* SR_UNSECURABLE */

    /* Determine reportableFlag */
    snmp_msg->u.v2.reportableFlag = 0;
    if ((pdu_ptr->type == GET_REQUEST_TYPE) ||
        (pdu_ptr->type == GET_NEXT_REQUEST_TYPE) ||
        (pdu_ptr->type == GET_BULK_REQUEST_TYPE) ||
        (pdu_ptr->type == SET_REQUEST_TYPE) ||
        (pdu_ptr->type == INFORM_REQUEST_TYPE)) {
        snmp_msg->u.v2.reportableFlag = 1;
    }

    len = FindLenOctetString(snmp_msg->u.v2.contextSnmpID) +
          FindLenOctetString(snmp_msg->u.v2.contextName) +
          pdu_ptr->packlet->length;

#ifndef SR_UNSECURABLE
    if (snmp_msg->u.v2.spi >= SR_SPI_USEC_AUTH) {
        if (snmp_msg->u.v2.authKey == NULL) {
            need_keys = 1;
        }
    }
    if (need_keys) {
        if (snmp_msg->u.v2.authKey != NULL) {
            FreeOctetString(snmp_msg->u.v2.authKey);
        }
        snmp_msg->u.v2.authKey = NULL;
        if (SrLookupUserData(snmp_v2_lcd,
                             snmp_msg->u.v2.authSnmpID,
                             snmp_msg->u.v2.identityName,
                             &userData)) {
            DPRINTF((APACCESS, "%s: unknown authSnmpID/userName\n", Fname));
            goto fail;
        }
        snmp_msg->u.v2.authKey = CloneOctetString(userData.auth_secret);
        if (snmp_msg->u.v2.authKey == NULL) {
            DPRINTF((APACCESS, "%s: no auth key\n", Fname));
            goto fail;
        }
    }
#endif /* SR_UNSECURABLE */

#ifndef SR_UNSECURABLE

    /* Extract boots and time values from LCD */
    SrLookupSnmpBootsAndTime(snmp_v2_lcd,
                             snmp_msg->u.v2.authSnmpID,
                             &snmp_msg->u.v2.authSnmpBoots,
                             &snmp_msg->u.v2.authSnmpTime);

    /* Get length of authInfo */
    if (snmp_msg->u.v2.spi == SR_SPI_USEC_AUTH) {
        ainfo_len = FindLenOctetString(snmp_msg->u.v2.authSnmpID) +
                    FindLenOctetString(snmp_msg->u.v2.identityName) +
                    FindLenInt(snmp_msg->u.v2.authSnmpBoots) +
                    FindLenInt(snmp_msg->u.v2.authSnmpTime) +
                    MD5_DIGEST_SIZE + 2;
    } else
#endif /* SR_UNSECURABLE */
    {
        ainfo_len = FindLenOctetString(snmp_msg->u.v2.authSnmpID) +
                    FindLenOctetString(snmp_msg->u.v2.identityName) +
                    FindLenInt(snmp_msg->u.v2.authSnmpBoots) +
                    FindLenInt(snmp_msg->u.v2.authSnmpTime) +
                    2;
    }
    ainfo_lenlen = DoLenLen(ainfo_len);
    ainfo_total_len = 1 + ainfo_lenlen + ainfo_len;

    /* Get length of SnmpV2AuthMessage */
    amsg_len = ainfo_total_len + len;
    amsg_lenlen = DoLenLen(amsg_len);
    amsg_total_len = 1 + amsg_lenlen + amsg_len;

    /* Allocate auth_ptr->packlet */

    snmp_msg->packlet = MakeOctetString(NULL, amsg_total_len + padding);
    if (snmp_msg->packlet == NULL) {
        goto fail;
    }

    /* Set auth_msg to point to appropriate part of auth_ptr->packlet */
    auth_msg = snmp_msg->packlet->octet_ptr + padding;

    /* Clear extra bytes for des padding at end of packlet */

    /* Initialize working pointer */
    wptr = auth_msg;

    /* Add authInfo fields to the buffer */
    *wptr++ = V2_AUTH_MSG_TYPE;
    AddLen(&wptr, amsg_lenlen, amsg_len);
    *wptr++ = USEC_AUTH_INFO_TYPE;
    AddLen(&wptr, ainfo_lenlen, ainfo_len);
    AddOctetString(&wptr, OCTET_PRIM_TYPE, snmp_msg->u.v2.authSnmpID);
    AddOctetString(&wptr, OCTET_PRIM_TYPE, snmp_msg->u.v2.identityName);
    AddInt(&wptr, INTEGER_TYPE, snmp_msg->u.v2.authSnmpBoots);
    AddInt(&wptr, INTEGER_TYPE, snmp_msg->u.v2.authSnmpTime);
#ifndef SR_UNSECURABLE
    if (snmp_msg->u.v2.spi == SR_SPI_USEC_AUTH) {
        digest_ptr = wptr + 2;
        AddOctetString(&wptr, OCTET_PRIM_TYPE, snmp_msg->u.v2.authKey);
    } else
#endif /* SR_UNSECURABLE */
    {
        *wptr++ = OCTET_PRIM_TYPE;
        *wptr++ = 0;
    }

    /* Add ScopedPDU */
    AddOctetString(&wptr, OCTET_PRIM_TYPE, snmp_msg->u.v2.contextSnmpID);
    AddOctetString(&wptr, OCTET_PRIM_TYPE, snmp_msg->u.v2.contextName);
    memcpy(wptr, pdu_ptr->packlet->octet_ptr, (int)pdu_ptr->packlet->length);
    if (priv_len > pdu_ptr->packlet->length) {
        memset(wptr + pdu_ptr->packlet->length, 0,
               priv_len - pdu_ptr->packlet->length);
    }

#ifndef SR_UNSECURABLE

    if (snmp_msg->u.v2.spi == SR_SPI_USEC_AUTH) {
        MD5Init(&MD);
        MD5Update(&MD, auth_msg, (unsigned int)amsg_total_len);
        MD5Final(md5_digest, &MD);
    
        /* Then fill in the md5_digest with the computed digest */
        memcpy(digest_ptr, md5_digest, MD5_DIGEST_SIZE);
    }
#endif /* SR_UNSECURABLE */

    return 0;

  fail:
    return -1;
}

/*
 * ParseUserAuthPrivMsg
 *
 * This is the hook function called by ParseAuthentication to extract
 * the authSnmpID, identityName, groupName, and ScopedPDU from an
 * SnmpV2AuthMessage.  The arguments are:
 *
 *    auth_msg   - contains a pointer to the SnmpV2AuthMessage portion
 *                 of the SnmpV2Message contained in the packlet field
 *                 of the auth_ptr argument.
 *    auth_ptr   - contains the spi, reportable flag, and incoming
 *                 SnmpV2Message.
 *
 *    This function will parse out the contextSnmpID and contextName.
 *    This is technically supposed to be done in the upper layer, but
 *    must be done here to find the start of the Pdu so it can be decrypted.
 *    The Pdu will be decrypted (if necessary) in place within the
 *    SnmpV2AuthMessage.  The auth_msg argument will be adjusted to point
 *    to the Pdu upon return.  The other return values (authSnmpID,
 *    identityName, groupName, contextSnmpID, and contextName) will be
 *    placed in the auth_ptr structure.  In addition, if the cached flag
 *    field of auth_ptr is set, information about the user contained in
 *    the SnmpV2AuthMessage will be stored in the auth_ptr structure.
 *
 * The return value is -1 on failure, 0 on success.
 */
void
SrParseUserAuthPrivMsg(
    SnmpLcd *snmp_lcd,
    SnmpV2Lcd *snmp_v2_lcd,
    TransportInfo *srcTI,
    OctetString *auth_msg,
    SnmpMessage *snmp_msg)
{
    FNAME("SrParseUserAuthPrivMsg")
#ifndef SR_UNSECURABLE
    unsigned char md5_digest[MD5_DIGEST_SIZE];
    MD5_CTX       MD;
#endif /* SR_UNSECURABLE */
    OctetString *digest = NULL;
    unsigned char *digest_ptr = NULL;
    const unsigned char *wptr, *end_ptr, *new_end_ptr, *save_end_ptr;
    UserData userData;
#ifndef SR_UNSECURABLE
#endif /* SR_UNSECURABLE */
    short type;
    SR_INT32 seq_length;
    SR_INT32 snmpBoots, snmpTime;
    SR_INT32 transportMMS;

    wptr = auth_msg->octet_ptr;
    end_ptr = wptr + auth_msg->length;

    /************/
    /* Step (1) */
    /************/

    /* Extract and check SnmpV2AuthMessage tag and length */
    type = V2_AUTH_MSG_TYPE;
    seq_length = ParseImplicitSequence(&wptr, end_ptr, &type);
    if (seq_length == -1) {
        DPRINTF((APPACKET, "%s: bad SnmpV2AuthMessage tag\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        if (snmp_msg->u.v2.reportableFlag) {
            goto maint_report;
        }
        goto fail;
    }
    new_end_ptr = wptr + seq_length;
    if (new_end_ptr < end_ptr) {
        end_ptr = new_end_ptr;
    } else if (new_end_ptr > end_ptr) {
        DPRINTF((APPACKET, "%s: bad SnmpV2AuthMessage BER length:\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        if (snmp_msg->u.v2.reportableFlag) {
            goto maint_report;
        }
        goto fail;
    }

    /* Save the pointer to the end of the packet so it can be restored later */
    save_end_ptr = end_ptr;

    /* Extract and check authInfo tag and length */
    type = USEC_AUTH_INFO_TYPE;
    seq_length = ParseImplicitSequence(&wptr, end_ptr, &type);
    if (seq_length == -1) {
        DPRINTF((APPACKET, "%s: bad authInfo tag\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        if (snmp_msg->u.v2.reportableFlag) {
            goto maint_report;
        }
        goto fail;
    }
    new_end_ptr = wptr + seq_length;
    if (new_end_ptr > end_ptr) {
        DPRINTF((APPACKET, "%s: bad authInfo BER length:\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        if (snmp_msg->u.v2.reportableFlag) {
            goto maint_report;
        }
        goto fail;
    }

    /************/
    /* Step (2) */
    /************/

    /* Extract authSnmpID */
    snmp_msg->u.v2.authSnmpID = ParseOctetString(&wptr, end_ptr, &type);
    if (snmp_msg->u.v2.authSnmpID == NULL) {
        DPRINTF((APPACKET, "%s: bad authSnmpID\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        if (snmp_msg->u.v2.reportableFlag) {
            goto maint_report;
        }
        goto fail;
    }

    /* Extract userName */
    snmp_msg->u.v2.identityName = ParseOctetString(&wptr, end_ptr, &type);
    if (snmp_msg->u.v2.identityName == NULL) {
        DPRINTF((APPACKET, "%s: bad userName\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        if (snmp_msg->u.v2.reportableFlag) {
            goto maint_report;
        }
        goto fail;
    }

    /* Extract authSnmpBoots */
    snmp_msg->u.v2.authSnmpBoots = ParseInt(&wptr, end_ptr, &type);
    if (type == -1) {
        DPRINTF((APPACKET, "%s: bad authSnmpBoots\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        if (snmp_msg->u.v2.reportableFlag) {
            goto maint_report;
        }
        goto fail;
    }

    /* Extract authSnmpTime */
    snmp_msg->u.v2.authSnmpTime = ParseInt(&wptr, end_ptr, &type);
    if (type == -1) {
        DPRINTF((APPACKET, "%s: bad authSnmpTime\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        if (snmp_msg->u.v2.reportableFlag) {
            goto maint_report;
        }
        goto fail;
    }

    /* Extract digest */
    digest = ParseOctetString(&wptr, end_ptr, &type);
    if (digest == NULL) {
        DPRINTF((APPACKET, "%s: bad digest\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        if (snmp_msg->u.v2.reportableFlag) {
            goto maint_report;
        }
        goto fail;
    }
    digest_ptr = (unsigned char *) wptr - digest->length;

    /* We are done parsing the authInfo portion.  Now restore end_ptr to
     * point at the end of the packet */
    end_ptr = save_end_ptr;

    /* If the sPI is maintenance(3), then we can proceed directly to parse
     * out the context information */
    if (snmp_msg->u.v2.spi == SR_SPI_MAINT) {
        snmp_msg->u.v2.groupName = MakeOctetStringFromText("maint");
        goto parse_context;
    }

#ifndef SR_UNSECURABLE
    if (snmp_msg->u.v2.spi == SR_SPI_USEC_AUTH) {
        if (digest->length != MD5_DIGEST_SIZE) {
            DPRINTF((APPACKET, "%s: bad digest\n", Fname));
            snmp_msg->error_code = WRONG_DIGEST_VALUE_ERROR;
            if (snmp_msg->u.v2.reportableFlag) {
                goto maint_report_with_req_id;
            }
            goto fail;
        }
    }
#endif /* SR_UNSECURABLE */

    /************/
    /* Step (3) */
    /************/

    /* Lookup usec data */
    
    if (SrLookupUserData(snmp_v2_lcd,
                         snmp_msg->u.v2.authSnmpID,
                         snmp_msg->u.v2.identityName,
                         &userData)) {
        DPRINTF((APACCESS, "%s: unknown authSnmpID/userName\n", Fname));
        snmp_msg->error_code = UNKNOWN_USER_NAME_ERROR;
        if (snmp_msg->u.v2.reportableFlag) {
            goto maint_report_with_req_id;
        }
        goto fail;
    }

    /* Copy additional info into the auth_ptr structure */
    snmp_msg->u.v2.groupName = CloneOctetString(userData.userGroupName);
    if (userData.userTransportLabel != NULL) {
        snmp_msg->u.v2.transportLabel =
            CloneOctetString(userData.userTransportLabel);
    } else {
        snmp_msg->u.v2.transportLabel = NULL;
    }
#ifndef SR_UNSECURABLE
    if (userData.auth_secret != NULL) {
        snmp_msg->u.v2.authKey = CloneOctetString(userData.auth_secret);
    } else {
        snmp_msg->u.v2.authKey = NULL;
    }
#endif /* SR_UNSECURABLE */

    /************/
    /* Step (4) */
    /************/

    /* nothing to do here */

    /************/
    /* Step (5) */
    /************/

    /* validate transport address */
    transportMMS = SrValidateTransportAddress(snmp_lcd->vta_data,
                                              snmp_lcd->vta_function,
                                              srcTI,
                                              snmp_msg->u.v2.transportLabel);
    if (transportMMS == 0) {
        DPRINTF((APACCESS, "%s: invalid source address\n", Fname));
        snmp_msg->error_code = UNKNOWN_USER_NAME_ERROR;
        if (snmp_msg->u.v2.reportableFlag) {
            goto maint_report_with_req_id;
        }
        goto fail;
    }
    snmp_msg->u.v2.mms = MIN(snmp_msg->u.v2.mms, transportMMS);

#ifndef SR_UNSECURABLE
    /************/
    /* Step (6) */
    /************/

    if (snmp_msg->u.v2.spi == SR_SPI_USEC_AUTH) {
        /* Copy authKey into digest portion of SnmpV2AuthMessage */
	if (userData.auth_secret == NULL) {
            if (snmp_msg->u.v2.reportableFlag) {
                goto maint_report_with_req_id;
            }
            goto fail;
	}
        memcpy(digest_ptr,
               userData.auth_secret->octet_ptr,
               MD5_DIGEST_SIZE);
        MD5Init(&MD);
        MD5Update(&MD, auth_msg->octet_ptr, (unsigned int)auth_msg->length);
        MD5Final(md5_digest, &MD);
    
        /* Compare the computed digest with the extracted digest */
        if (memcmp(digest->octet_ptr, md5_digest, MD5_DIGEST_SIZE)) {
            DPRINTF((APACCESS, "%s: authentication failure\n", Fname));
            snmp_msg->error_code = WRONG_DIGEST_VALUE_ERROR;
            if (snmp_msg->u.v2.reportableFlag) {
                goto maint_report_with_req_id;
            }
            goto fail;
        }

        SrLookupSnmpBootsAndTime(snmp_v2_lcd,
                                 snmp_msg->u.v2.authSnmpID,
                                 &snmpBoots,
                                 &snmpTime);
        if (snmp_msg->u.v2.authSnmpBoots < snmpBoots) {
            DPRINTF((APACCESS, "%s: not in lifetime failure\n", Fname));
            snmp_msg->error_code = NOT_IN_TIME_WINDOW_ERROR;
            if (snmp_msg->u.v2.reportableFlag) {
                goto auth_report;
            }
            goto fail;
        }
        if ((snmp_msg->u.v2.authSnmpBoots == snmpBoots) &&
            (snmp_msg->u.v2.authSnmpTime < (snmpTime - SR_USEC_LIFETIME_WINDOW))) {
            DPRINTF((APACCESS, "%s: not in lifetime failure\n", Fname));
            snmp_msg->error_code = NOT_IN_TIME_WINDOW_ERROR;
            if (snmp_msg->u.v2.reportableFlag) {
                goto auth_report;
            }
            goto fail;
        }
    }
#endif /* SR_UNSECURABLE */

    /*********************/
    /* Intermediate Step */
    /*********************/

  parse_context:
    /* Need to parse out the contextSnmpID and contextName before the
     * PDU can be decrypted. */

    /* Extract contextSnmpID */
    snmp_msg->u.v2.contextSnmpID = ParseOctetString(&wptr, end_ptr, &type);
    if (snmp_msg->u.v2.contextSnmpID == NULL) {
        DPRINTF((APPACKET, "%s: bad contextSnmpID\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        if (snmp_msg->u.v2.reportableFlag) {
            goto maint_report;
        }
        goto fail;
    }

    /* Extract contextName */
    snmp_msg->u.v2.contextName = ParseOctetString(&wptr, end_ptr, &type);
    if (snmp_msg->u.v2.contextName == NULL) {
        DPRINTF((APPACKET, "%s: bad contextName\n", Fname));
        snmp_msg->error_code = ASN_PARSE_ERROR;
        if (snmp_msg->u.v2.reportableFlag) {
            goto maint_report;
        }
        goto fail;
    }


    /************/
    /* Step (8) */
    /************/

    /* Update auth_msg to point at the Pdu (for return values) */
    auth_msg->octet_ptr = (unsigned char *) wptr;

    /* return values */
    if (digest != NULL) {
        FreeOctetString(digest);
        digest = NULL;
    }
    return;

  fail:
    if (digest != NULL) {
        FreeOctetString(digest);
        digest = NULL;
    }
    return;

  maint_report_with_req_id:
    /* Skip over contextSnmpID and contextName and point at Pdu */
    snmp_msg->u.v2.contextSnmpID = ParseOctetString(&wptr, end_ptr, &type);
    if (snmp_msg->u.v2.contextSnmpID == NULL) {
        goto maint_report;
    }
    snmp_msg->u.v2.contextName = ParseOctetString(&wptr, end_ptr, &type);
    if (snmp_msg->u.v2.contextName == NULL) {
        goto maint_report;
    }
    auth_msg->octet_ptr = (unsigned char *) wptr;

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
    if (digest != NULL) {
        FreeOctetString(digest);
        digest = NULL;
    }
    return;

#ifndef SR_UNSECURABLE
  auth_report:
    /* Skip over contextSnmpID and contextName and point at Pdu */
    snmp_msg->u.v2.contextSnmpID = ParseOctetString(&wptr, end_ptr, &type);
    if (snmp_msg->u.v2.contextSnmpID != NULL) {
        snmp_msg->u.v2.contextName = ParseOctetString(&wptr, end_ptr, &type);
        if (snmp_msg->u.v2.contextName != NULL) {
            auth_msg->length -= wptr - (const unsigned char *)auth_msg->octet_ptr;
            auth_msg->octet_ptr = (unsigned char *) wptr;
        }
    }

    snmp_msg->u.v2.spi = SR_SPI_USEC_AUTH;
    if (snmp_msg->u.v2.contextSnmpID) {
        FreeOctetString(snmp_msg->u.v2.contextSnmpID);
    }
    if (snmp_msg->u.v2.contextName) {
        FreeOctetString(snmp_msg->u.v2.contextName);
    }
    snmp_msg->u.v2.contextSnmpID =
        CloneOctetString(snmp_v2_lcd->v2AdminSnmpScalarsData->snmpID);
    snmp_msg->u.v2.contextName = MakeOctetStringFromText("default");
    if (digest != NULL) {
        FreeOctetString(digest);
        digest = NULL;
    }
    return;
#endif /* SR_UNSECURABLE */
}

int
SrLookupUserData(
    SnmpV2Lcd *snmp_v2_lcd,
    OctetString *snmpID,
    OctetString *userName,
    UserData *userData)
{
    int index;
    userNameEntry_t *userNameEntry;
    SnmpV2Table *unt = snmp_v2_lcd->userNameTable;

    unt->tip[0].value.octet_val = snmpID;
    unt->tip[1].value.octet_val = userName;
    index = SearchTable(unt, EXACT);
    if (index == -1) {
        goto override;
    }
    userNameEntry = (userNameEntry_t *) unt->tp[index];
    if (userNameEntry->userStatus != RS_ACTIVE) {
        goto override;
    }
    if (userNameEntry->userGroupName != NULL) {
        userData->userGroupName = userNameEntry->userGroupName;
    } else {
        userData->userGroupName = userNameEntry->userName;
    }
    userData->userTransportLabel = userNameEntry->userTransportLabel;
#ifndef SR_UNSECURABLE
    userData->auth_secret = userNameEntry->auth_secret;
#endif /* SR_UNSECURABLE */
    return 0;

  override:
    if (snmp_v2_lcd->override_userNameTable == NULL) {
        return -1;
    }
    return (*snmp_v2_lcd->override_userNameTable)(snmp_v2_lcd,
                                                  snmpID,
                                                  userName,
                                                  userData);
}

static void
SrLookupSnmpBootsAndTime(
    SnmpV2Lcd *snmp_v2_lcd,
    OctetString *snmpID,
    SR_INT32 *snmpBoots,
    SR_INT32 *snmpTime)
{
    SR_INT32 lastTime;

    *snmpBoots = 0L;
    *snmpTime = 0L;

    if (!CmpOctetStrings(snmpID, snmp_v2_lcd->v2AdminSnmpScalarsData->snmpID)) {
        *snmpBoots = snmp_v2_lcd->usecScalarsData->snmpBoots;
        *snmpTime = GetTimeNow() / 100;
    } else {
        LookupBootsAndTimeEntry(snmpID, snmpBoots, snmpTime, &lastTime);
    }
}
