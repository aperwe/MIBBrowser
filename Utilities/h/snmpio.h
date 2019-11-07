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

/*
snmpio.h

     snmpio.h contains those data elements common to the routines
     which are commonly called by SNMP applications for net input
     and output
*/

/* $Id: snmpio.h,v 1.17 1998/01/08 02:34:46 mark Exp $ */

#ifndef SR_SNMPIO_H
#define SR_SNMPIO_H

#ifdef  __cplusplus
extern "C" {
#endif

#include <winsock.h>

#include "snmpio.bsd"		/* include file for use berkeley sockets */



#define SR_WAITING 0
#define SR_TIMEOUT 1
#define SR_RECEIVED 2
#define SR_ERROR  -1

#define SR_PACKET_LEN SR_DEFAULT_MAX_MSG_SIZE

#define SendRequest(s,a) \
    SrSendRequest((s), (a)->packlet->octet_ptr, (a)->packlet->length);

int SrSendRequest
    SR_PROTOTYPE((int sock,
		  const unsigned char *packet,
                  const SR_INT32 packet_length));

int GetResponse
    SR_PROTOTYPE((void));

void CloseUp
    SR_PROTOTYPE((void));

int TimeOut
    SR_PROTOTYPE((void));

SR_INT32 MakeReqId
    SR_PROTOTYPE((void));

SR_UINT32 GetLocalIPAddress
    SR_PROTOTYPE((void));


#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif /* SR_SNMPIO_H */
