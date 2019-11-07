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


/* $Id: v2init.h,v 1.3 1998/01/07 22:04:11 mark Exp $ */

#ifndef SR_V2INIT_H
#define SR_V2INIT_H


#ifdef __cplusplus
extern "C" {
#endif

extern int k_srCommunityMIB_initialize(
    char *contextName_text,
    srCommunityMIB_t *srCommunityMIBData);

extern int k_srCommunityEntry_initialize(
    char *contextName_text,
    SnmpV2Table *srCommunityTable);

extern int k_adms_initialize(
    char *contextName_text,
    v2AdminSnmpScalars_t *v2AdminSnmpScalarsData,
    v2AdminStats_t *v2AdminStatsData);

extern int k_v2ContextEntry_initialize(
    SnmpV2Table *v2ContextTable);

extern int k_v2AdminViewTree_initialize(
    v2AdminViewTree_t *v2AdminViewTreeData);

extern int k_viewTreeEntry_initialize(
    SnmpV2Table *viewTreeTable);

extern int k_v2AdminAccessControl_initialize(
    v2AdminAccessControl_t *v2aacd);

int k_acEntry_initialize(SnmpV2Table *at);

extern int k_v2AdminTransport_initialize(
    v2AdminTransport_t *v2AdminTransportData);

extern int k_transportEntry_initialize(
    SnmpV2Table *transportTable);

extern int k_v2AdminNotify_initialize(
    v2AdminNotify_t *v2AdminNotifyData);

extern int k_notifyEntry_initialize(
    SnmpV2Table *notifyTable);

extern int k_proxyForwardingEntry_initialize(
    SnmpV2Table *proxyForwardingTable);

extern int k_usecScalars_initialize(
    char *contextName_text,
    usecScalars_t *usecScalarsData);

extern int k_usecStats_initialize(
    char *contextName_text,
    usecStats_t *usecStatsData);

extern int k_usecUser_initialize(
    char *contextName_text,
    usecUser_t *usecUserData);

extern int k_userNameEntry_initialize(
    char *contextName_text,
    SnmpV2Table *userNameTable);

#ifdef __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif /* SR_V2INIT_H */
