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

/* $Id: auth_lib.h,v 1.5 1998/01/08 02:42:04 mark Exp $ */

#ifndef SR_SRI_AUTH_LIB_H
#define SR_SRI_AUTH_LIB_H

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct _AuthHeader {
    OctetString    *packlet;
    int             wrapper_size;

    SR_UINT32       version;
#ifdef SR_SNMPv1_WRAPPER
    OctetString    *community;
#endif                          /* SR_SNMPv1_WRAPPER */

#ifdef SR_SNMPv2_PACKET
    /* Wrapper information */
    SR_UINT32       mms;
    SR_UINT32       reportableFlag;
    SR_UINT32       spi;

    /* Flag indicating whether the structure contains cached information */
    SR_UINT32       cached;

    /* Input to AuthPriv service for building packets */
    /* This stuff should be filled in before the call to BuildAuthentication */
    /*OctetString    *authSnmpID;*/ /* Use duplicate member below */
    /*OctetString    *userName;*/ /* Use identityName member below */
/* FIX_THIS change to snmpBoots, snmpTime */
    SR_INT32       authSnmpBoots;
    SR_INT32       authSnmpTime;
#ifndef SR_UNSECURABLE
    OctetString    *authKey;
#endif /* SR_UNSECURABLE */

    OctetString    *transportLabel;

    /* ContextInfo information */
    OctetString    *contextSnmpID;
    OctetString    *contextName;

    /* Output from AuthPriv service when parsing packets */
    OctetString    *authSnmpID;
    OctetString    *identityName;
    OctetString    *groupName;
#endif                          /* SR_SNMPv2_PACKET */
} AuthHeader;

#ifdef SR_SNMPv2_PACKET

#ifndef SR_UNSECURABLE
struct _AuthHeader *MakeAuthentication(
    SR_UINT32 spi,
    struct _OctetString *community,
    SR_UINT32 mms,
    struct _OctetString *authSnmpID,
    struct _OctetString *identityName,
    struct _OctetString *contextSnmpID,
    struct _OctetString *contextName,
    SR_INT32 snmpBoots,
    SR_INT32 snmpTime,
    struct _OctetString *authKey,
    SR_UINT32 cache);
#else /* SR_UNSECURABLE */
struct _AuthHeader *MakeAuthentication(
    SR_UINT32 spi,
    struct _OctetString *community,
    SR_UINT32 mms,
    struct _OctetString *authSnmpID,
    struct _OctetString *identityName,
    struct _OctetString *contextSnmpID,
    struct _OctetString *contextName,
    SR_INT32 snmpBoots,
    SR_INT32 snmpTime,
    SR_UINT32 cache);
#endif /* SR_UNSECURABLE */

#else /* SR_SNMPv2_PACKET */

struct _AuthHeader *MakeAuthentication(
    struct _OctetString *community);

#endif /* SR_SNMPv2_PACKET */

int BuildAuthentication(
    struct _AuthHeader *auth_ptr,
    struct _Pdu *pdu_ptr);

void FreeAuthentication(
    struct _AuthHeader *auth_ptr);

struct _AuthHeader *ParseAuthentication(
    const unsigned char *packet_ptr,
    SR_INT32 length,
    int *error_code);

struct _Pdu *ParsePdu(
    const struct _AuthHeader *auth_ptr);

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif				/* SR_SRI_AUTH_LIB_H */
