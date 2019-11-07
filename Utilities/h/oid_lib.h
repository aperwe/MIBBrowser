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

/* $Id: oid_lib.h,v 1.13 1998/01/08 02:34:05 mark Exp $ */

#ifndef SR_OID_LIB_H
#define SR_OID_LIB_H

#ifdef  __cplusplus
extern "C" {
#endif

char *GetType
    SR_PROTOTYPE((const char *string,
		  SR_INT32 value));

char *GetBitsType
    SR_PROTOTYPE((const char *string,
		  const OctetString *value));

int ParseSubIdHex
    SR_PROTOTYPE((const unsigned char **temp_ptr,
		  SR_UINT32 *value));

int ParseSubIdOctal
    SR_PROTOTYPE((const unsigned char **temp_ptr,
		  SR_UINT32 *value));

int ParseSubIdDecimal
    SR_PROTOTYPE((const unsigned char **temp_ptr,
		  SR_UINT32 *value));

short MakeDotFromOID
    SR_PROTOTYPE((const struct _OID *oid_ptr,
		  char *buffer));

short GetStringFromSubIds
    SR_PROTOTYPE((char *string_buffer,
		  SR_UINT32 *sid,
		  int sid_counter));

#ifndef WANT_2L_FUNCS
int DecChar2L
    SR_PROTOTYPE((const char *str,
		  SR_UINT32 *value));

int HexChar2L
    SR_PROTOTYPE((const char *str,
		  SR_UINT32 *value));

int OctChar2L
    SR_PROTOTYPE((const char *str,
		  SR_UINT32 *value));

int Str2L
    SR_PROTOTYPE((const char *str,
		  int base,
		  SR_UINT32 *value));
#endif /* WANT_2L_FUNCS */

int CmpNOID
    SR_PROTOTYPE((const struct _OID *oida,
		  const struct _OID *oidb,
		  SR_INT32 length));

int Inst2IP
    SR_PROTOTYPE((const SR_UINT32 *inst,
		  int instLength,
		  SR_UINT32 *ipAddr,
		  int searchType));

int CmpOIDClass
    SR_PROTOTYPE((const struct _OID *ptr1,
		  const struct _OID *ptr2));

int CmpOID
    SR_PROTOTYPE((const struct _OID *ptr1,
		  const struct _OID *ptr2));

int CmpOIDWithLen
    SR_PROTOTYPE((const struct _OID *oid1,
		  const struct _OID *oid2));

int CheckOID
    SR_PROTOTYPE((const struct _OID *oid1_ptr,
		  const struct _OID *oid2_ptr));

#ifdef  __cplusplus
}
#endif

#endif				/* !defined SR_OID_LIB_H_ */
