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

/* $Id: frmt_lib.h,v 1.8 1998/01/08 02:33:27 mark Exp $ */

#ifndef SR_FRMT_LIB_H
#define SR_FRMT_LIB_H

#ifdef  __cplusplus
extern "C" {
#endif

void SPrintOctetString
    SR_PROTOTYPE((const struct _OctetString *oct_ptr,
		  int wrap,
		  char *));

int SPrintVarBind
    SR_PROTOTYPE((const struct _VarBind *vb_ptr,
		  char *));

int SPrintVarBindValue
    SR_PROTOTYPE((const struct _VarBind *vb_ptr,
		  char *,
		  const char *));

int SPrintAscii
    SR_PROTOTYPE((const struct _OctetString *os_ptr,
		  char *));

#ifdef  __cplusplus
}
#endif

#endif				/* !defined SR_FRMT_LIB_H */
