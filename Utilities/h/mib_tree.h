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

/* $Id: mib_tree.h,v 1.15 1998/01/08 02:33:53 mark Exp $ */

#ifndef SR_MIB_TREE_H
#define SR_MIB_TREE_H

#ifdef  __cplusplus
extern "C" {
#endif




/* Symbols for selecting collision behavior when loading/merging MIBs */
#define SR_COL_REPLACE 0
#define SR_COL_DISCARD 1

/* Symbols for controlling MIB tree traversal behaviour */
#define BEHAVIOUR_REJECT_PARENT      0
#define BEHAVIOUR_ACCEPT_LEAF_PARENT 1
#define BEHAVIOUR_ACCEPT_ANY_PARENT  2

/*
 * These TYPE definitions are only used in this structure and are used to
 * distinguish types that normally are treated as other types.  The
 * normal types for these are as follows:
 *      DISPLAY_STRING_TYPE  OCTET_PRIM_TYPE
 *      PHYS_ADDRESS_TYPE    OCTET_PRIM_TYPE
 *      NETWORK_ADDRESS_TYPE OCTET_PRIM_TYPE
 *      SERVICES_TYPE        INTEGER_TYPE
 */
#define DISPLAY_STRING_TYPE  0x80
#define PHYS_ADDRESS_TYPE    0x81
#define NETWORK_ADDRESS_TYPE 0x82
#define SERVICES_TYPE        0x83
/*
 * In addition, the type field may contain one of these.
 */
#define NON_LEAF_TYPE        0x84
#define AGGREGATE_TYPE       0x85
/*
 * These definitions are for the access of variables.
 */
#define READ_ONLY_ACCESS             0x01
#define READ_WRITE_ACCESS            0x02
#define READ_CREATE_ACCESS           0x03
#define NOT_ACCESSIBLE_ACCESS        0x04
#define ACCESSIBLE_FOR_NOTIFY_ACCESS 0x04 /* will change to 0x05 */

int CalculateMIBHash
    SR_PROTOTYPE((const char *str, int len));

OID_TREE_ELE *NewOID_TREE_ELE
    SR_PROTOTYPE((void));

void ReleaseOID_TREE_ELE
    SR_PROTOTYPE((OID_TREE_ELE *ote));

MIB_TREE *CreateMIBTree
    SR_PROTOTYPE((const char *name,
                  const OID *prefix));

void AttachMIBTree
    SR_PROTOTYPE((MIB_TREE *mib_tree));

void DetachMIBTree
    SR_PROTOTYPE((MIB_TREE *mib_tree));

int ReadMIBFileEntry
    SR_PROTOTYPE((ConfigFile *cfgp,
                  OID_TREE_ELE **otepp));

int ReadMIBFile
    SR_PROTOTYPE((const char *filename,
                  MIB_TREE *mib_tree,
                  int collision));

int MergeMIBTrees
    SR_PROTOTYPE((MIB_TREE *mib_tree_dst,
                  MIB_TREE *mib_tree_src,
                  int collision));

void FreeMIBTree
    SR_PROTOTYPE((MIB_TREE *mib_tree));

void FreeOidTreeNodeStorage
    SR_PROTOTYPE((void));

OID_TREE_ELE *AddOTEToMibTree
    SR_PROTOTYPE((MIB_TREE *mib_tree,
                  OID_TREE_ELE *ote,
                  int collision));

void RemoveOTEFromMibTree
    SR_PROTOTYPE((MIB_TREE *mib_tree,
                  OID_TREE_ELE *ote));

int
MergeMIBFromFile
    SR_PROTOTYPE((const char *filename));

/* Prototypes for mib_lib.c */
OID_TREE_ELE *SR_GetMIBNodeFromOID
    SR_PROTOTYPE((const OID *oid_ptr,
                  int behaviour));

OID_TREE_ELE *SR_GetMIBNodeFromDot
    SR_PROTOTYPE((const char *name,
                  int behaviour));

OID_TREE_ELE *GetMIBNodeFromOID
    SR_PROTOTYPE((const OID *oid_ptr));

OID_TREE_ELE *GetMIBNodeFromDot
    SR_PROTOTYPE((const char *name));

OID_TREE_ELE *LP_GetMIBNodeFromOID
    SR_PROTOTYPE((const OID *oid_ptr));

OID_TREE_ELE *LP_GetMIBNodeFromDot
    SR_PROTOTYPE((const char *name));

OID_TREE_ELE *AP_GetMIBNodeFromOID
    SR_PROTOTYPE((const OID *oid_ptr));

OID_TREE_ELE *AP_GetMIBNodeFromDot
    SR_PROTOTYPE((const char *name));

void FreeOTE
    SR_PROTOTYPE((OID_TREE_ELE *ote));

OID_TREE_ELE *select_behaviour
    SR_PROTOTYPE((OID_TREE_ELE *,
                  int));

OID_TREE_ELE *OIDTreeDive
    SR_PROTOTYPE((OID_TREE_ELE *oid_tree_ptr, 
		  SR_UINT32 *sid,
		  int *sid_counter_ptr));

short TypeStringToShort
    SR_PROTOTYPE((const char *str));

const char *TypeShortToString
    SR_PROTOTYPE((int shrt));

short AccessStringToShort
    SR_PROTOTYPE((const char *str));

const char *AccessShortToString
    SR_PROTOTYPE((int shrt));

#ifdef  __cplusplus
}
#endif

#endif                                /* !defined SR_MIB_TREE_H */
