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

#include "sr_conf.h"

#include <stdio.h>

#include <windows.h>
#include <winsock.h>

#include <stdlib.h>

#include <errno.h>

#include <sys/types.h>



#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif  /* HAVE_MEMORY_H */


#include <string.h>

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif  /* HAVE_MALLOC_H */

#ifdef SR_WINDOWS
#include <windows.h>
#endif /* SR_WINDOWS */

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif /* HAVE_NETINET_IN_H */


#include "sr_snmp.h"
#include "sr_trans.h"
#include "snmpid.h"
#include "diag.h"
SR_FILENAME

#include "sr_ip.h"    /* for GetLocalIPAddress() */



/*
 * SrGetSnmpID:  Determines an engine's snmpID or snmpEngineID value.
 */
OctetString *
SrGetSnmpID(
    SR_UINT32 algorithm,
    void *data)
{
    FNAME("SrGetSnmpID")
    OctetString *snmpID;

    snmpID = MakeOctetString(NULL, 12);
    if (snmpID == NULL) {
        DPRINTF((APERROR, "%s:  CANNOT ALLOCATE snmpID/snmpEngineID\n", Fname));
        return NULL;
    }

    /* IANA_ENTERPRISE_NUMBER defined in include/features.h */
    *(SR_UINT32 *)(snmpID->octet_ptr) = htonl(IANA_ENTERPRISE_NUMBER);

    switch (algorithm) {
        case SR_SNMPID_ALGORITHM_SIMPLE:
            snmpID->octet_ptr[4] = (unsigned char)algorithm;

            if (data == SR_SNMPID_ALGORITHM_SIMPLE_AGT) {
                snmpID->octet_ptr[5] = (unsigned char)0;
            } else
            if (data == SR_SNMPID_ALGORITHM_SIMPLE_MGR) {
                snmpID->octet_ptr[5] = (unsigned char)1;
            } else {
                DPRINTF((APERROR, "%s:  BAD ALGORITHM DATA\n", Fname));
            }

            *(unsigned short *)(snmpID->octet_ptr + 6) =
                htons((unsigned short)GetSNMPPort());

            *(SR_UINT32 *)(snmpID->octet_ptr + 8) =
                htonl(GetLocalIPAddress());

            break;
        default:
            DPRINTF((APERROR, "%s:  BAD ALGORITHM\n", Fname));
            break;
    }

    return snmpID;
}
