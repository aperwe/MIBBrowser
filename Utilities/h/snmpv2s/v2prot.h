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


/*
 * Arguments used to create this file:
 * -o v2 -test_and_incr -parser -row_status -userpart -storage_type 
 * -ansi 
 */

/* $Id: v2prot.h,v 1.5 1998/01/07 22:04:21 mark Exp $ */

/*
 * File Description:
 */

/*
 * automatically generated by the mib compiler -- do not edit
 */

#ifndef SR_V2PROT_H
#define SR_V2PROT_H


#ifdef __cplusplus
extern "C" {
#endif

#include "sr_proto.h"
#include "snmpv2s/userpart.h"


/*
 * The srCommunityMIB family. 
 */


extern VarBind * srCommunityMIB_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern void srCommunityMIB_free
    SR_PROTOTYPE((srCommunityMIB_t *data));

extern int srCommunityMIB_test
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern int srCommunityMIB_set
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern void srCommunityMIB_free
    SR_PROTOTYPE((srCommunityMIB_t *data));

extern srCommunityMIB_t *k_srCommunityMIB_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator));

extern int k_srCommunityMIB_test
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *dp,
                  ContextInfo *contextInfo));

extern int k_srCommunityMIB_set
    SR_PROTOTYPE((srCommunityMIB_t *data,
                  ContextInfo *contextInfo,
                  int function));

extern int k_srCommunityMIB_ready
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp));

extern int srCommunityMIB_undo
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern srCommunityMIB_t *Clone_srCommunityMIB
    SR_PROTOTYPE((srCommunityMIB_t *data));



/*
 * The srCommunityEntry family. 
 */


extern VarBind * srCommunityEntry_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern int srCommunityEntry_test
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern int srCommunityEntry_set
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern srCommunityEntry_t *k_srCommunityEntry_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  OctetString * srCommunityAuthSnmpID,
                  OctetString * srCommunityName));

extern int k_srCommunityEntry_test
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *dp,
                  ContextInfo *contextInfo));

extern int k_srCommunityEntry_set
    SR_PROTOTYPE((srCommunityEntry_t *data,
                  ContextInfo *contextInfo,
                  int function));

extern int k_srCommunityEntry_ready
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp));

extern int srCommunityEntry_undo
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern srCommunityEntry_t *Clone_srCommunityEntry
    SR_PROTOTYPE((srCommunityEntry_t *data));

extern int k_srCommunityEntry_set_defaults
    SR_PROTOTYPE((doList_t *dp));



/*
 * The v2AdminSnmpScalars family. 
 */

extern VarBind * v2AdminSnmpScalars_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern v2AdminSnmpScalars_t *k_v2AdminSnmpScalars_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator));



/*
 * The v2AdminStats family. 
 */


extern VarBind * v2AdminStats_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern v2AdminStats_t *k_v2AdminStats_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator));



/*
 * The v2ContextEntry family. 
 */

extern VarBind * v2ContextEntry_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern int v2ContextEntry_test
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern int v2ContextEntry_set
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern v2ContextEntry_t *k_v2ContextEntry_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  OctetString * v2ContextSnmpID,
                  OctetString * v2ContextName));

extern int k_v2ContextEntry_test
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *dp,
                  ContextInfo *contextInfo));

extern int k_v2ContextEntry_set
    SR_PROTOTYPE((v2ContextEntry_t *data,
                  ContextInfo *contextInfo,
                  int function));

extern int k_v2ContextEntry_ready
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp));

extern int v2ContextEntry_undo
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern v2ContextEntry_t *Clone_v2ContextEntry
    SR_PROTOTYPE((v2ContextEntry_t *data));

extern int k_v2ContextEntry_set_defaults
    SR_PROTOTYPE((doList_t *dp));



/*
 * The v2AdminViewTree family. 
 */

extern VarBind * v2AdminViewTree_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern void v2AdminViewTree_free
    SR_PROTOTYPE((v2AdminViewTree_t *data));

extern int v2AdminViewTree_test
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern int v2AdminViewTree_set
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern void v2AdminViewTree_free
    SR_PROTOTYPE((v2AdminViewTree_t *data));

extern v2AdminViewTree_t *k_v2AdminViewTree_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator));

extern int k_v2AdminViewTree_test
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *dp,
                  ContextInfo *contextInfo));

extern int k_v2AdminViewTree_set
    SR_PROTOTYPE((v2AdminViewTree_t *data,
                  ContextInfo *contextInfo,
                  int function));

extern int k_v2AdminViewTree_ready
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp));

extern int v2AdminViewTree_undo
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern v2AdminViewTree_t *Clone_v2AdminViewTree
    SR_PROTOTYPE((v2AdminViewTree_t *data));



/*
 * The viewTreeEntry family. 
 */

extern VarBind * viewTreeEntry_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern int viewTreeEntry_test
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern int viewTreeEntry_set
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern viewTreeEntry_t *k_viewTreeEntry_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  OctetString * viewTreeName,
                  OID * viewTreeSubTree));

extern int k_viewTreeEntry_test
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *dp,
                  ContextInfo *contextInfo));

extern int k_viewTreeEntry_set
    SR_PROTOTYPE((viewTreeEntry_t *data,
                  ContextInfo *contextInfo,
                  int function));

extern int k_viewTreeEntry_ready
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp));

extern int viewTreeEntry_undo
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern viewTreeEntry_t *Clone_viewTreeEntry
    SR_PROTOTYPE((viewTreeEntry_t *data));

extern int k_viewTreeEntry_set_defaults
    SR_PROTOTYPE((doList_t *dp));



/*
 * The v2AdminAccessControl family. 
 */

extern VarBind * v2AdminAccessControl_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern void v2AdminAccessControl_free
    SR_PROTOTYPE((v2AdminAccessControl_t *data));

extern int v2AdminAccessControl_test
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern int v2AdminAccessControl_set
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern void v2AdminAccessControl_free
    SR_PROTOTYPE((v2AdminAccessControl_t *data));

extern v2AdminAccessControl_t *k_v2AdminAccessControl_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator));

extern int k_v2AdminAccessControl_test
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *dp,
                  ContextInfo *contextInfo));

extern int k_v2AdminAccessControl_set
    SR_PROTOTYPE((v2AdminAccessControl_t *data,
                  ContextInfo *contextInfo,
                  int function));

extern int k_v2AdminAccessControl_ready
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp));

extern int v2AdminAccessControl_undo
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern v2AdminAccessControl_t *Clone_v2AdminAccessControl
    SR_PROTOTYPE((v2AdminAccessControl_t *data));



/*
 * The acEntry family. 
 */

extern VarBind * acEntry_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern int acEntry_test
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern int acEntry_set
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern acEntry_t *k_acEntry_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  SR_INT32 acSPI,
                  OctetString * acGroupName,
                  OctetString * acContextName));

extern int k_acEntry_test
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *dp,
                  ContextInfo *contextInfo));

extern int k_acEntry_set
    SR_PROTOTYPE((acEntry_t *data,
                  ContextInfo *contextInfo,
                  int function));

extern int k_acEntry_ready
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp));

extern int acEntry_undo
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern acEntry_t *Clone_acEntry
    SR_PROTOTYPE((acEntry_t *data));

extern int k_acEntry_set_defaults
    SR_PROTOTYPE((doList_t *dp));



/*
 * The v2AdminTransport family. 
 */

extern VarBind * v2AdminTransport_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern void v2AdminTransport_free
    SR_PROTOTYPE((v2AdminTransport_t *data));

extern int v2AdminTransport_test
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern int v2AdminTransport_set
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern void v2AdminTransport_free
    SR_PROTOTYPE((v2AdminTransport_t *data));

extern v2AdminTransport_t *k_v2AdminTransport_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator));

extern int k_v2AdminTransport_test
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *dp,
                  ContextInfo *contextInfo));

extern int k_v2AdminTransport_set
    SR_PROTOTYPE((v2AdminTransport_t *data,
                  ContextInfo *contextInfo,
                  int function));

extern int k_v2AdminTransport_ready
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp));

extern int v2AdminTransport_undo
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern v2AdminTransport_t *Clone_v2AdminTransport
    SR_PROTOTYPE((v2AdminTransport_t *data));



/*
 * The transportEntry family. 
 */

extern VarBind * transportEntry_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern int transportEntry_test
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern int transportEntry_set
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern transportEntry_t *k_transportEntry_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  OctetString * transportLabel,
                  SR_INT32 transportSubindex));

extern int k_transportEntry_test
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *dp,
                  ContextInfo *contextInfo));

extern int k_transportEntry_set
    SR_PROTOTYPE((transportEntry_t *data,
                  ContextInfo *contextInfo,
                  int function));

extern int k_transportEntry_ready
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp));

extern int transportEntry_undo
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern transportEntry_t *Clone_transportEntry
    SR_PROTOTYPE((transportEntry_t *data));

extern int k_transportEntry_set_defaults
    SR_PROTOTYPE((doList_t *dp));



/*
 * The v2AdminNotify family. 
 */

extern VarBind * v2AdminNotify_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern void v2AdminNotify_free
    SR_PROTOTYPE((v2AdminNotify_t *data));

extern int v2AdminNotify_test
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern int v2AdminNotify_set
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern void v2AdminNotify_free
    SR_PROTOTYPE((v2AdminNotify_t *data));

extern v2AdminNotify_t *k_v2AdminNotify_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator));

extern int k_v2AdminNotify_test
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *dp,
                  ContextInfo *contextInfo));

extern int k_v2AdminNotify_set
    SR_PROTOTYPE((v2AdminNotify_t *data,
                  ContextInfo *contextInfo,
                  int function));

extern int k_v2AdminNotify_ready
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp));

extern int v2AdminNotify_undo
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern v2AdminNotify_t *Clone_v2AdminNotify
    SR_PROTOTYPE((v2AdminNotify_t *data));



/*
 * The notifyEntry family. 
 */


#define notifyInformParametersEntry_get notifyEntry_get
#define notifyInformParametersEntry_test notifyEntry_test
#define notifyInformParametersEntry_set notifyEntry_set

extern VarBind * notifyEntry_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern int notifyEntry_test
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern int notifyEntry_set
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern notifyEntry_t *k_notifyEntry_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  SR_INT32 notifyIndex));

extern int k_notifyEntry_test
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *dp,
                  ContextInfo *contextInfo));

extern int k_notifyEntry_set
    SR_PROTOTYPE((notifyEntry_t *data,
                  ContextInfo *contextInfo,
                  int function));

extern int k_notifyEntry_ready
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp));

extern int notifyEntry_undo
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern notifyEntry_t *Clone_notifyEntry
    SR_PROTOTYPE((notifyEntry_t *data));

extern int k_notifyEntry_set_defaults
    SR_PROTOTYPE((doList_t *dp));



/*
 * The proxyForwardingEntry family. 
 */

extern VarBind * proxyForwardingEntry_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern int proxyForwardingEntry_test
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern int proxyForwardingEntry_set
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern proxyForwardingEntry_t *k_proxyForwardingEntry_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  SR_INT32 proxyIndex));

extern int k_proxyForwardingEntry_test
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *dp,
                  ContextInfo *contextInfo));

extern int k_proxyForwardingEntry_set
    SR_PROTOTYPE((proxyForwardingEntry_t *data,
                  ContextInfo *contextInfo,
                  int function));

extern int k_proxyForwardingEntry_ready
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp));

extern int proxyForwardingEntry_undo
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern proxyForwardingEntry_t *Clone_proxyForwardingEntry
    SR_PROTOTYPE((proxyForwardingEntry_t *data));

extern int k_proxyForwardingEntry_set_defaults
    SR_PROTOTYPE((doList_t *dp));



/*
 * The usecScalars family. 
 */

extern VarBind * usecScalars_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern usecScalars_t *k_usecScalars_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator));



/*
 * The usecStats family. 
 */

extern VarBind * usecStats_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern usecStats_t *k_usecStats_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator));



/*
 * The usecUser family. 
 */

extern VarBind * usecUser_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern void usecUser_free
    SR_PROTOTYPE((usecUser_t *data));

extern int usecUser_test
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern int usecUser_set
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern void usecUser_free
    SR_PROTOTYPE((usecUser_t *data));

extern usecUser_t *k_usecUser_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator));

extern int k_usecUser_test
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *dp,
                  ContextInfo *contextInfo));

extern int k_usecUser_set
    SR_PROTOTYPE((usecUser_t *data,
                  ContextInfo *contextInfo,
                  int function));

extern int k_usecUser_ready
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp));

extern int usecUser_undo
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern usecUser_t *Clone_usecUser
    SR_PROTOTYPE((usecUser_t *data));



/*
 * The userNameEntry family. 
 */

extern VarBind * userNameEntry_get
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  int searchType,
                  ContextInfo *contextInfo,
                  int serialNum));

extern int userNameEntry_test
    SR_PROTOTYPE((OID *incoming,
                  ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern int userNameEntry_set
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern userNameEntry_t *k_userNameEntry_get
    SR_PROTOTYPE((int serialNum,
                  ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  OctetString * userAuthSnmpID,
                  OctetString * userName));

extern int k_userNameEntry_test
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *dp,
                  ContextInfo *contextInfo));

extern int k_userNameEntry_set
    SR_PROTOTYPE((userNameEntry_t *data,
                  ContextInfo *contextInfo,
                  int function));

extern int k_userNameEntry_ready
    SR_PROTOTYPE((ObjectInfo *object,
                  ObjectSyntax *value,
                  doList_t *doHead,
                  doList_t *dp));

extern int userNameEntry_undo
    SR_PROTOTYPE((doList_t *doHead,
                  doList_t *doCur,
                  ContextInfo *contextInfo));

extern userNameEntry_t *Clone_userNameEntry
    SR_PROTOTYPE((userNameEntry_t *data));

extern int k_userNameEntry_set_defaults
    SR_PROTOTYPE((doList_t *dp));



#ifdef __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif /* SR_V2PROT_H */
