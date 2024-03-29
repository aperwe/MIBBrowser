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

/* $Id: v2_msg.h,v 1.5 1998/01/20 16:52:52 levi Exp $ */

#ifndef SR_V2_MSG_H
#define SR_V2_MSG_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "sr_user.h"

typedef struct SR_SnmpV2Lcd SnmpV2Lcd;

typedef int (*OverrideSrLookupUserData)(
    SnmpV2Lcd *snmp_v2_lcd,
    OctetString *snmpID,
    OctetString *userName,
    UserData *userData);

struct SR_SnmpV2Lcd {
    v2AdminSnmpScalars_t *v2AdminSnmpScalarsData;
    v2AdminStats_t *v2AdminStatsData;
    usecScalars_t *usecScalarsData;
    usecStats_t *usecStatsData;
    usecUser_t *usecUserData;
    SnmpV2Table *userNameTable;
    OverrideSrLookupUserData override_userNameTable;
};

void SrParseV2SnmpMessage(
    SnmpMessage *snmp_msg,
    SnmpLcd *sl_ptr,
    void *v_sl_ptr,
    TransportInfo *src_ti,
    TransportInfo *dst_ti,
    const unsigned char *msg,
    const int msg_len);

int SrBuildV2SnmpMessage(
    SnmpMessage *snmp_msg,
    Pdu *pdu_ptr,
    void *v_sl_ptr,
    SnmpLcd *sl_ptr);

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif				/* SR_V2_MSG_H */
