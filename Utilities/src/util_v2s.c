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

#ifndef lint
#ifdef  SR_RCSID
static const char rcsid[] = "$Id: util_v2s.c,v 1.37.4.1 1998/03/19 14:22:27 partain Exp $";
#endif  /* SR_RCSID */
#endif /* lint */


/*
 * this file contains code to deal with usec-specific initialization and
 * communication
 */

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#include <sys/types.h>

#include <string.h>

#include <ctype.h>

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif  /* HAVE_MALLOC_H */

#include <stddef.h>

#include "sr_snmp.h"
#include "sr_ad.h"
#include "diag.h"
SR_FILENAME

#include "sr_boots.h"
#include "sr_user.h"
#include "sr_cfg.h"
#ifndef SR_UNSECURABLE
#include "pw2key.h"
#endif /* SR_UNSECURABLE */
#include "sr_utils.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "snmpio.h"
#include "prnt_lib.h"
#include "oid_lib.h"

#include "sr_trans.h"
#include "sr_msg.h"
#include "snmpv2s/v2type.h"
#include "v2_msg.h"

/* globals in snmpio.c */
extern int fd;
extern SR_INT32 packet_len;
extern unsigned char packet[SR_PACKET_LEN];

#ifdef SR_SNMPv2_PACKET
static int util_LookupUserData(
    SnmpV2Lcd *snmp_v2_lcd,
    OctetString *authSnmpID,
    OctetString *userName,
    UserData *userData);

extern OctetString *util_local_snmpID;

#include "snmpv2s/i_adms.h"
v2AdminSnmpScalars_t util_v2AdminSnmpScalarsData;
v2AdminStats_t util_v2AdminStatsData;

#include "snmpv2s/i_usec.h"
usecUser_t util_usecUserData;
usecStats_t util_usecStatsData;
usecScalars_t util_usecScalarsData;
SnmpV2Table util_userNameTable;
SnmpV2Table util_dummy_userNameTable;
#ifdef SR_CONFIG_FP
static const PARSER_RECORD_TYPE util_userNameEntryRecord = {
    PARSER_TABLE,
    "userNameEntry ",
    userNameEntryConverters,
    userNameEntryTypeTable,
    &util_userNameTable,
    NULL,
    offsetof(userNameEntry_t, userStatus),
    offsetof(userNameEntry_t, userMemoryType),
    -1,
    (char *)userNameEntryRecordFormatString
};
static const PARSER_RECORD_TYPE util_snmpBootsRecord = {
    PARSER_SCALAR, "snmpBoots ", snmpBootsConverters, snmpBootsType,
    NULL, &util_usecScalarsData.snmpBoots, -1, -1, -1, NULL
};

extern const PARSER_RECORD_TYPE snmpBootsAndTimeRecord;
#endif /* SR_CONFIG_FP */

#ifdef SR_SNMPv1_WRAPPER
#include "snmpv2s/i_comm.h"
srCommunityMIB_t util_srCommunityMIBData;
SnmpV2Table util_srCommunityTable;
#ifdef SR_CONFIG_FP
static const PARSER_RECORD_TYPE util_srCommunityEntryRecord = {
    PARSER_TABLE,
    "communityEntry ",
    srCommunityEntryConverters,
    srCommunityEntryTypeTable,
    &util_srCommunityTable,
    NULL,
    offsetof(srCommunityEntry_t, srCommunityStatus),
    offsetof(srCommunityEntry_t, srCommunityMemoryType),
    -1,
    (char *)srCommunityEntryRecordFormatString
};
#endif /* SR_CONFIG_FP */
#endif /* SR_SNMPv1_WRAPPER */
#endif /* SR_SNMPv2_PACKET */

/*
 * The following is a static cluConfigEntry structure used for maintenance
 * operations.  This is generally used to determine the snmpID of a device
 * when there is no transportSnmpIDEntry available.
 */
unsigned char maint_snmpID_octet_ptr[] =
  { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
OctetString maint_snmpID = { maint_snmpID_octet_ptr, 12 };
unsigned char maint_contextName_octet_ptr[] =
  { 0x64, 0x65, 0x66, 0x61, 0x75, 0x6c, 0x74 };
OctetString maint_contextName = { maint_contextName_octet_ptr, 7 };
#ifndef SR_UNSECURABLE
OctetString maint_authKey = { NULL, 0 };
#endif /* SR_UNSECURABLE */
userNameEntry_t maint_cluce = {
  &maint_snmpID,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  (SR_INT32)0,
  (SR_INT32)0,
  (long)0,
#ifndef SR_UNSECURABLE
  &maint_authKey,
  NULL,
#endif /* SR_UNSECURABLE */
  "\0\0"
};

extern OctetString *defaultContextSnmpID;
extern OctetString *defaultContext;

extern SnmpLcd snmp_lcd;

SnmpV2Lcd util_snmp_v2_lcd;


#ifndef SR_UNSECURABLE
int
PromptForUserPasswords(snmpID, userName)
    OctetString *snmpID;
    OctetString *userName;
{
    int index;
    userNameEntry_t *cce;
    char pw1[128];
    char *pw1_env;

    strcpy(pw1, "");

    pw1_env = getenv("SR_UTIL_AUTH_PASSWORD");
    if (pw1_env != NULL) {
        strcpy(pw1, pw1_env);
    } else {
        get_password("Enter Authentication password : ", pw1, sizeof(pw1));
    }

    util_userNameTable.tip[0].value.octet_val = snmpID;
    util_userNameTable.tip[1].value.octet_val = userName;
    index = NewTableEntry(&util_userNameTable);
    if (index == -1) {
        return -1;
    }
    cce = util_userNameTable.tp[index];
    if (strlen(pw1) == 0) {
        cce->auth_secret = MakeOctetString(NULL, 0);
    } else {
        cce->auth_secret = SrTextPasswordToKey(SR_PW2KEY_ALGORITHM_MD5, pw1);
    }

    return index;
}
#else /* SR_UNSECURABLE */
int
CreateV2NoAuthUser(snmpID, userName)
    OctetString *snmpID;
    OctetString *userName;
{
    int index;

    util_userNameTable.tip[0].value.octet_val = snmpID;
    util_userNameTable.tip[1].value.octet_val = userName;
    index = NewTableEntry(&util_userNameTable);
    if (index == -1) {
        return -1;
    }

    return index;
}
#endif /* SR_UNSECURABLE */

userNameEntry_t *cluce;
OctetString *v2_handle = NULL;

SR_UINT32 outOfLifetime_sids[] =   { 1, 3, 6, 1, 6, 3, 1135, 2, 1, 0 };
SR_UINT32 unknownUserName_sids[] = { 1, 3, 6, 1, 6, 3, 1135, 2, 2, 0 };
OID outOfLifetime_OID = { 10, outOfLifetime_sids };
OID unknownUserName_OID = { 10, unknownUserName_sids };
OID *outOfLifetime = &outOfLifetime_OID;
OID *unknownUserName = &unknownUserName_OID;
Pdu *fake_pdu;

int
InitializeUser(input, communication_type)
    char *input;
    int communication_type;
{
    int index;
    OID *oid;
    VarBind *vb;

    /*
     * For now, the v2_handle will be treated as the userName
     */
    v2_handle = MakeOctetStringFromText(input);
    if (v2_handle == NULL) {
        return 0;
    }

    if (communication_type == SNMP_COMMUNICATIONS) {
        cluce = &maint_cluce;
        maint_cluce.userName = v2_handle;
    }
    if (communication_type == TRAP_SEND_COMMUNICATIONS) {
/* Need to search util_userNameTable for localSnmpID/v2_handle, if there, use the
   keys, otherwise, immediately prompt for the keys. */
        util_userNameTable.tip[0].value.octet_val = util_local_snmpID;
        util_userNameTable.tip[1].value.octet_val = v2_handle;
        index = SearchTable(&util_userNameTable, EXACT);
        if (index == -1) {
            DPRINTF((APCONFIG, "No local config info for this user.\n"));
#ifndef SR_UNSECURABLE
            index = PromptForUserPasswords(util_local_snmpID, v2_handle);
#else /* SR_UNSECURABLE */
            index = CreateV2NoAuthUser(util_local_snmpID, v2_handle);
#endif /* SR_UNSECURABLE */
            if (index == -1) {
                return 0;
            }
        }
        cluce = (userNameEntry_t *)util_userNameTable.tp[index];
    }
    if (communication_type == TRAP_RCV_COMMUNICATIONS) {
        cluce = NULL;
    }

    oid = MakeOIDFromDot("1.3.6.1.2.1.1.3");
    if (oid == NULL) { 
        return 0;
    }
    vb = MakeVarBindWithNull(oid, NULL);
    if (vb == NULL) { 
        return 0;
    }
    fake_pdu = MakePdu(GET_NEXT_REQUEST_TYPE, 0L,0L,0L,NULL,NULL,0L,0L,0L);
    if (fake_pdu == NULL) { 
        return 0;
    }

    return 1;
}

Pdu *
PerformUserRequest(out_pdu_ptr, status)
    Pdu *out_pdu_ptr;
    int *status;
{
    SnmpMessage *snmp_msg = NULL;
    Pdu *in_pdu_ptr = NULL;
    SR_INT32 curtime;
    SR_INT32 sPI;
    int index;
    int use_fake_pdu = 0;

    *status = 0;

    /*
     * Attempt to send the pdu and process the response, using the
     * parameters initialized by InitializeUser(), and possibly updated
     * by previous calls to PerformUserRequest().  If a Report PDU is
     * received as the response, these parameters will be updated
     * appropriately, and the request will be re-built and re-sent.
     * This occurs by jumping back to the build_again label.
     */

  build_again:
    if (in_pdu_ptr != NULL) {
        FreePdu(in_pdu_ptr);
        in_pdu_ptr = NULL;
    }
    if (snmp_msg != NULL) {
        SrFreeSnmpMessage(snmp_msg);
        snmp_msg = NULL;
    }
    /* Build the snmp_msg */
    if ( (cluce == &maint_cluce) && (communication_type != TRAP_SEND_COMMUNICATIONS) ) {
        sPI = SR_SPI_USEC_NOAUTH;
        use_fake_pdu = 1;
    } else {
#ifndef SR_UNSECURABLE
        sPI = SR_SPI_USEC_AUTH;
        if (cluce->auth_secret == NULL) {
            sPI = SR_SPI_USEC_NOAUTH;
        } else if (cluce->auth_secret->length == 0) {
            sPI = SR_SPI_USEC_NOAUTH;
	}
#else /* !SR_UNSECURABLE */
        sPI = SR_SPI_USEC_NOAUTH;
#endif /* !SR_UNSECURABLE */
    }
    snmp_msg =
        SrCreateV2SnmpMessage(sPI,
                           (communication_type == TRAP_SEND_COMMUNICATIONS) ?
                               ((out_pdu_ptr->type == INFORM_REQUEST_TYPE) ?
                                     cluce->userAuthSnmpID : util_local_snmpID) :
                                     cluce->userAuthSnmpID,
                           cluce->userName,
                           (defaultContextSnmpID == NULL) ?
                               cluce->userAuthSnmpID : defaultContextSnmpID,
                           (defaultContext == NULL) ?
                               v2_defaultContext : defaultContext,
#ifndef SR_UNSECURABLE
                           cluce->auth_secret,
#endif /* SR_UNSECURABLE */
                           TRUE);
    if (snmp_msg == NULL) {
        *status = SR_ERROR;
        goto done;
    }
    if (use_fake_pdu) {
        fake_pdu->u.normpdu.request_id = out_pdu_ptr->u.normpdu.request_id;
        if (BuildPdu(fake_pdu) == -1) {
          DPRINTF((APERROR, "BuildPdu failed.  Giving up.\n"));
          *status = SR_ERROR;
          goto done;
        }
    }
    if (SrBuildSnmpMessage(snmp_msg,
                           use_fake_pdu ? fake_pdu : out_pdu_ptr,
                           &snmp_lcd)) {
        *status = SR_ERROR;
        goto done;
    }
    use_fake_pdu = 0;

    /* Send the request */
    if (SrSendRequest(fd,
                      snmp_msg->packlet->octet_ptr,
                      snmp_msg->packlet->length) != TRUE) {
        *status = SR_ERROR;
        goto done;
    }

#ifdef SR_SNMPv1_PACKET
    if ((out_pdu_ptr->type == TRAP_TYPE) ||
        (out_pdu_ptr->type == SNMPv2_TRAP_TYPE)) { 
        goto done;
    }
#else /* SR_SNMPv1_PACKET */
    if (out_pdu_ptr->type == SNMPv2_TRAP_TYPE) { 
        goto done;
    }
#endif /* SR_SNMPv1_PACKET */

    /* Wait for response */
  wait_again:
    *status = GetResponse();
    if (*status == SR_ERROR) {
        DPRINTF((APERROR, "%s:  receive error.\n", util_name));
        goto done;
    }
    if (*status == SR_TIMEOUT) {
        DPRINTF((APERROR, "%s:  no response.\n", util_name));
        goto done;
    }

    SrFreeSnmpMessage(snmp_msg);
    snmp_msg = NULL;

    snmp_msg = SrParseSnmpMessage(&snmp_lcd,
                                  NULL,
                                  NULL,
                                  packet,
                                  packet_len);
    if (snmp_msg == NULL) {
        DPRINTF((APERROR, "%s:  error parsing packet.\n",
                 util_name));
        *status = SR_ERROR;
        goto done;
    }

    /* Parse the PDU */
    in_pdu_ptr = SrParsePdu(snmp_msg->packlet->octet_ptr,
                            snmp_msg->packlet->length);
    if (in_pdu_ptr == NULL) {
        DPRINTF((APERROR, "%s:  error parsing pdu packlet.\n", util_name));
        *status = SR_ERROR;
        goto done;
    }

    /* Check if this is a report PDU, and process appropriately */
    if (in_pdu_ptr->type == REPORT_TYPE) {

        /* If out-of-lifetime, record the new times and try again */
        if (CmpOID(outOfLifetime, in_pdu_ptr->var_bind_list->name) == 0) {
            if (in_pdu_ptr->u.normpdu.request_id != out_pdu_ptr->u.normpdu.request_id) {
                DPRINTF((APALWAYS, "request id mismatch.\n"));
                FreePdu(in_pdu_ptr);
                in_pdu_ptr = NULL;
                goto wait_again;
            }
            DPRINTF((APTRACE, "REPORT received for out-of-lifetime.\n"));
            curtime = GetTimeNow() / 100;
            SetBootsAndTimeEntry(cluce->userAuthSnmpID,
                                 snmp_msg->u.v2.authSnmpBoots,
                                 snmp_msg->u.v2.authSnmpTime - curtime,
                                 snmp_msg->u.v2.authSnmpTime - curtime);
            out_pdu_ptr->u.normpdu.request_id = ++global_req_id;
            if (BuildPdu(out_pdu_ptr) == -1) {
              DPRINTF((APERROR, "BuildPdu failed.  Giving up.\n"));
              *status = SR_ERROR;
              goto done;
            }
            goto build_again;
        }

        /* If an unknown user, record the authSnmpID and try again */
        if (CmpOID(unknownUserName, in_pdu_ptr->var_bind_list->name) == 0) {
#ifndef SR_UNSECURABLE
#endif /* SR_UNSECURABLE */
                if (in_pdu_ptr->u.normpdu.request_id !=
                    out_pdu_ptr->u.normpdu.request_id) {
                    DPRINTF((APALWAYS, "request id mismatch.\n"));
                    FreePdu(in_pdu_ptr);
                    in_pdu_ptr = NULL;
                    goto wait_again;
                }
#ifndef SR_UNSECURABLE
#endif /* SR_UNSECURABLE */
            DPRINTF((APTRACE, "REPORT received for unknown user name.\n"));
            if (snmp_msg->u.v2.authSnmpID == NULL) {
                DPRINTF((APERROR, "bad authSnmpID in REPORT.\n"));
                FreePdu(in_pdu_ptr);
                in_pdu_ptr = NULL;
                *status = SR_ERROR;
                goto done;
            }
            if (snmp_msg->u.v2.authSnmpID->length != 12) {
                DPRINTF((APERROR, "bad authSnmpID in REPORT.\n"));
                FreePdu(in_pdu_ptr);
                in_pdu_ptr = NULL;
                *status = SR_ERROR;
                goto done;
            }
            if (!CmpOctetStrings(snmp_msg->u.v2.authSnmpID, cluce->userAuthSnmpID)) {
                DPRINTF((APERROR, "Username %s is unknown to this agent.\n",
                         util_handle));
                FreePdu(in_pdu_ptr);
                in_pdu_ptr = NULL;
                *status = SR_ERROR;
                goto done;
            }
            util_userNameTable.tip[0].value.octet_val = snmp_msg->u.v2.authSnmpID;
            util_userNameTable.tip[1].value.octet_val = v2_handle;
            index = SearchTable(&util_userNameTable, EXACT);
            if (index == -1) {
                DPRINTF((APCONFIG, "No local config info for this agent.\n"));
#ifndef SR_UNSECURABLE
                index = PromptForUserPasswords(snmp_msg->u.v2.authSnmpID, v2_handle);
#else /* SR_UNSECURABLE */
                index = CreateV2NoAuthUser(snmp_msg->u.v2.authSnmpID,v2_handle);
#endif /* SR_UNSECURABLE */
                if (index == -1) {
                    FreePdu(in_pdu_ptr);
                    in_pdu_ptr = NULL;
                    *status = SR_ERROR;
                    goto done;
                }
            }
            cluce = (userNameEntry_t *)util_userNameTable.tp[index];
            out_pdu_ptr->u.normpdu.request_id = ++global_req_id;
            if (BuildPdu(out_pdu_ptr) == -1) {
              DPRINTF((APERROR, "BuildPdu failed.  Giving up.\n"));
              *status = SR_ERROR;
              goto done;
            }
            goto build_again;
        }

        /* Otherwise we cannot recover */
        DPRINTF((APERROR, "REPORT received, cannot recover:\n"));
        PrintVarBindList(in_pdu_ptr->var_bind_list);
        FreePdu(in_pdu_ptr);
        in_pdu_ptr = NULL;
        *status = SR_ERROR;
        goto done;
    }

    /* Make sure request id matches */
    if (in_pdu_ptr->u.normpdu.request_id != out_pdu_ptr->u.normpdu.request_id) {
        DPRINTF((APALWAYS, "request id mismatch.\n"));
        FreePdu(in_pdu_ptr);
        in_pdu_ptr = NULL;
        goto wait_again;
    }

#ifndef SR_UNSECURABLE
    if (sPI == SR_SPI_USEC_AUTH) {
        curtime = GetTimeNow() / 100;
        SetBootsAndTimeEntry(cluce->userAuthSnmpID,
                             snmp_msg->u.v2.authSnmpBoots,
                             snmp_msg->u.v2.authSnmpTime - curtime,
                             snmp_msg->u.v2.authSnmpTime - curtime);
    }
#endif /* SR_UNSECURABLE */


    /* If a response PDU was received, just drop through and return the pdu */

  done:
    if (snmp_msg != NULL) {
        SrFreeSnmpMessage(snmp_msg);
        snmp_msg = NULL;
    }
    return in_pdu_ptr;
}

static int
util_LookupUserData(
    SnmpV2Lcd *snmp_v2_lcd,
    OctetString *authSnmpID,
    OctetString *userName,
    UserData    *userData)
{
    int index;
    if ((communication_type == SNMP_COMMUNICATIONS) ||
        (communication_type == TRAP_SEND_COMMUNICATIONS)) {
        if (cluce == &maint_cluce) {
            userData->userGroupName = NULL;
            userData->userTransportLabel = NULL;
#ifndef SR_UNSECURABLE
            userData->auth_secret = NULL;
#endif /* SR_UNSECURABLE */
        } else {
            userData->userGroupName = NULL;
            userData->userTransportLabel = NULL;
#ifndef SR_UNSECURABLE
            userData->auth_secret = cluce->auth_secret;
#endif /* SR_UNSECURABLE */
        }
        return 0;
    }
    if (communication_type == TRAP_RCV_COMMUNICATIONS) {
        util_userNameTable.tip[0].value.octet_val = authSnmpID;
        util_userNameTable.tip[1].value.octet_val = userName;
        index = SearchTable(&util_userNameTable, EXACT);
        if (index == -1) {
            return -1;
        }
        cluce = (userNameEntry_t *)util_userNameTable.tp[index];
        userData->userGroupName = NULL;
        userData->userTransportLabel = NULL;
#ifndef SR_UNSECURABLE
        userData->auth_secret = cluce->auth_secret;
#endif /* SR_UNSECURABLE */
        return 0;
    }
    return -1;
}
