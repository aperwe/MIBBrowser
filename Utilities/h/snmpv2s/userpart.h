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

/* $Id: userpart.h,v 1.3 1998/01/07 22:04:04 mark Exp $ */

#ifndef SR_SNMPV2S_USERPART_H
#define SR_SNMPV2S_USERPART_H

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef SR_UNSECURABLE
#define U_userNameEntry \
    OctetString *auth_secret; \
    OctetString *new_auth_secret;
#endif /* SR_UNSECURABLE */

#define U_v2ContextEntry \
    SR_INT32 userSkip; \
    SR_UINT32 referenceCount;

#define U_acEntry \
    SR_INT32 userSkip;

#define U_viewTreeEntry \
    SR_INT32 userSkip;


#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif	/* SR_SNMPV2S_USERPART_H */
