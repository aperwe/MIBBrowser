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

/* $Id: sr_ad.h,v 1.3 1998/01/08 02:35:18 mark Exp $ */

#ifndef SR_AD_H
#define SR_AD_H

#ifdef  __cplusplus
extern "C" {
#endif

/* tags for functions */
#define SR_AD_LOOKUP_USER_DATA                   1
#define SR_AD_VALIDATE_TRANSPORT_ADDRESS         2
#define SR_AD_DO_REPORT                          3
#define SR_AD_LOOKUP_SNMP_BOOTS_AND_TIME         4

/* tags for global data */
#define SR_AD_LOCAL_SNMP_ID                      5001


void *sr_set_ad
    SR_PROTOTYPE((SR_UINT32 tag,
                  void *data));

void *sr_rem_ad
    SR_PROTOTYPE((SR_UINT32 tag));

void *sr_get_ad
    SR_PROTOTYPE((SR_UINT32 tag));

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif				/* SR_AD_H */
