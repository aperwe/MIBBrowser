
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

/* $Id: inst_lib.h,v 1.12 1998/01/08 02:33:40 mark Exp $ */

#ifndef SR_INST_LIB_H
#define SR_INST_LIB_H

#ifdef  __cplusplus
extern "C" {
#endif

/* Function  prototypes for the functions defined in inst_lib.c */


int InstToInt
    SR_PROTOTYPE((const struct _OID *inst,
		  int index,
		  SR_INT32 *val,
		  int searchType,
		  int *carry));

int InstToUInt
    SR_PROTOTYPE((const struct _OID *inst,
		  int index,
		  SR_UINT32 *val,
		  int searchType,
		  int *carry));

int InstToIP
    SR_PROTOTYPE((const struct _OID *inst,
		  int index,
		  SR_UINT32 *ipAddr,
		  int searchType,
		  int *carry));

int InstToFixedOctetString
    SR_PROTOTYPE((const struct _OID *inst,
		  int index,
		  struct _OctetString **os,
		  int searchType,
		  int *carry,
		  int size));

int InstToNetworkAddress
    SR_PROTOTYPE((const struct _OID *inst,
		  int index,
		  struct _OctetString **os,
		  int searchType,
		  int *carry));

int InstToImpliedOctetString
    SR_PROTOTYPE((const struct _OID *inst,
		  int index,
		  struct _OctetString **os,
		  int searchType,
		  int *carry));

int InstToVariableOctetString
    SR_PROTOTYPE((const struct _OID *inst,
		  int index,
		  struct _OctetString **os,
		  int searchType,
		  int *carry));

int InstToImpliedOID
    SR_PROTOTYPE((const OID *inst,
		  int index,
		  OID **os,
		  int searchType,
		  int *carry));

int InstToVariableOID
    SR_PROTOTYPE((const OID *inst,
		  int index,
		  OID **os,
		  int searchType,
		  int *carry));

int GetVariableIndexLength
    SR_PROTOTYPE((const struct _OID *inst,
		  int index));

int GetImpliedIndexLength
    SR_PROTOTYPE((const struct _OID *inst,
		  int index));

int CmpOIDInst
    SR_PROTOTYPE((const struct _OID *ptr1,
		  const struct _OID *ptr2,
		  int index));

OctetString *IPToOctetString
    SR_PROTOTYPE((SR_UINT32 IPAddr));

SR_UINT32 OctetStringToIP
    SR_PROTOTYPE((const OctetString *os));

SR_UINT32 NetworkAddressToIP
    SR_PROTOTYPE((const OctetString *os));

SR_UINT32 OctetStringToUlong
    SR_PROTOTYPE((const OctetString *os));

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif	/* SR_INST_LIB_H */
