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

#include <stdlib.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif  /* HAVE_MALLOC_H */

#include <errno.h>

#include <sys/types.h>



#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif  /* HAVE_MEMORY_H */

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif  /* HAVE_NETINET_IN_H */


#include <string.h>

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif  /* HAVE_SYS_SOCKET_H */


#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif  /* HAVE_NETDB_H */

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif /* HAVE_ARPA_INET_H */

#include "sr_type.h"
#include "sr_time.h"

#include <windows.h>
#include <winsock.h>





#include "sr_snmp.h"
#include "diag.h"
SR_FILENAME

#include "sr_cfg.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "sr_trans.h"


#include "sr_ip.h"


#ifdef SOLARIS
/* gethostname() is missing from the Solaris 2.5.1 system include files. */
int gethostname
    SR_PROTOTYPE((char *name, int namelen));
#endif /* SOLARIS */

static SR_UINT32       LocalIPAddress = (SR_UINT32)0;

#define SA_CAST(x) (struct sockaddr *)x



/*
 * GetLocalIPAddress:
 *
 *   This routine will attempt to determine the local IP address of a device,
 *   and return this value as an unsigned 32-bit integer.  Returned in host
 *   byte order.
 */
SR_UINT32
GetLocalIPAddress()
{
    FNAME("GetLocalIPAddress")
    char hostname[64];
    struct hostent *hp;

    if (LocalIPAddress != (SR_UINT32)0) {
        return LocalIPAddress;
    }

    if (!gethostname(hostname, 64)) {
        DPRINTF((APTRACE, "%s: hostname is %s\n", Fname, hostname));
        LocalIPAddress = inet_addr(hostname);
        if (LocalIPAddress == (SR_UINT32) -1) {
            hp = gethostbyname(hostname);
            if (hp != NULL) {
                memcpy((char *)&LocalIPAddress,
                      (char *)hp->h_addr,
                      (int)sizeof(SR_INT32));
            }
        }
        LocalIPAddress = ntohl(LocalIPAddress);
    }

    if (LocalIPAddress == (SR_UINT32) -1) {
        DPRINTF((APERROR, "%s: Could not determine agent IP address.  This can cause\n", Fname)); 
        DPRINTF((APERROR,"    management station failure.  Is host system misconfigured?\n"));
    }
    return LocalIPAddress;
}

SR_INT32
ConvToken_IP_TAddr(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    OctetString **local = (OctetString **)value;
    SR_INT32 s1, s2, s3, s4, port;
    short unsigned int s_port;
    int cc;

    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            cc = sscanf(*token, INT32_FORMAT"."INT32_FORMAT"."INT32_FORMAT"."INT32_FORMAT":"INT32_FORMAT,
                        &s1, &s2, &s3, &s4, &port);
            if (cc < 4) {
                return -1;
            }
            if (cc == 4) {
                port = (SR_INT32)0;
            }
            if ((s1 > 255) ||
                (s1 < 0)   ||
                (s2 > 255) ||
                (s2 < 0)   ||
                (s3 > 255) ||
                (s3 < 0)   ||
                (s4 > 255) ||
                (s4 < 0)   ||
                (port < 0) ||
                (port > 65535)) {
                return -1;
            }
            *local = MakeOctetString(NULL, 6);
            if (*local == NULL) {
                return -1;
            }
            s_port = (short unsigned int)port;
            (*local)->octet_ptr[0] = (unsigned char)s1;
            (*local)->octet_ptr[1] = (unsigned char)s2;
            (*local)->octet_ptr[2] = (unsigned char)s3;
            (*local)->octet_ptr[3] = (unsigned char)s4;
            (*local)->octet_ptr[4] = (unsigned char)((int) (s_port & 0xff00) >> 8);
            (*local)->octet_ptr[5] = (unsigned char)(s_port & 0xff);
            return 0;
        case PARSER_CONVERT_TO_TOKEN:
            *token = (char *)malloc(32);
            if (*token == NULL) {
                return -1;
            }
            if ((*local)->length != 6) {
                strcpy(*token, "0.0.0.0:0");
                return 0;
            } else {
                s1 = (SR_INT32)(*local)->octet_ptr[0];
                s2 = (SR_INT32)(*local)->octet_ptr[1];
                s3 = (SR_INT32)(*local)->octet_ptr[2];
                s4 = (SR_INT32)(*local)->octet_ptr[3];
                s_port = (((short unsigned int)(*local)->octet_ptr[4]) << 8) +
                         ((short unsigned int)(*local)->octet_ptr[5]);
                port = (SR_UINT32)s_port;
                sprintf(*token, "%d.%d.%d.%d:%d", (int)s1, (int)s2, (int)s3, (int)s4, (int)port);
                return 0;
            }
        default:
            return -1;
    }
}

void
CopyIpTransportInfo(dst, src)
    TransportInfo *dst;
    const TransportInfo *src;
{
    dst->type = src->type;
    dst->flags = src->flags;
    dst->fd = src->fd;
    dst->t_ipAddr = src->t_ipAddr;
    dst->t_ipPort = src->t_ipPort;
}

void
CopyIpTransportAddr(dst, src)
    TransportInfo *dst;
    const TransportInfo *src;
{
    dst->t_ipAddr = src->t_ipAddr;
    dst->t_ipPort = src->t_ipPort;
}

int
CmpIpTransportInfo(ti1, ti2)
    const TransportInfo *ti1;
    const TransportInfo *ti2;
{
    if (ti1->t_ipAddr != ti2->t_ipAddr) {
        return -1;
    }
    if (ti1->t_ipPort != ti2->t_ipPort) {
        return -1;
    }
    return 0;
}

int
CmpIpTransportInfoWithMask(ti1, ti2, mask)
    const TransportInfo *ti1;
    const TransportInfo *ti2;
    const TransportInfo *mask;
 {
     if ((ti1->t_ipAddr & mask->t_ipAddr) != 
	 (ti2->t_ipAddr & mask->t_ipAddr)) {
         return -1;
     }
     if ((ti1->t_ipPort & mask->t_ipPort) !=
	 (ti2->t_ipPort & mask->t_ipPort)) {
         return -1;
     }
     return 0;
}

OctetString *
IpTransportInfoToOctetString(ti)
    const TransportInfo *ti;
{
    OctetString *os;

    os = MakeOctetString(NULL, 6);
    if (os == NULL) {
        return NULL;
    }
    memcpy(os->octet_ptr, &ti->t_ipAddr, 4);
    memcpy(os->octet_ptr + 4, &ti->t_ipPort, 2);
    return os;
}

int
OctetStringToIpTransportInfo(ti, os)
    TransportInfo *ti;
    const OctetString *os;
{
    if (os == NULL) {
        return -1;
    }
    if (os->length != 6) {
        return -1;
    }
    ti->type = SR_IP_TRANSPORT;
    memcpy(&ti->t_ipAddr, os->octet_ptr, 4);
    memcpy(&ti->t_ipPort, os->octet_ptr + 4, 2);
    return 0;
}



int
SendToIpTransport(data, len, ti)
    const char *data;
    int len;
    const TransportInfo *ti;
{
    struct sockaddr_in addr;

#ifdef SR_CLEAR_AUTOS
    memset(&addr, 0, sizeof(addr));
#endif	/* SR_CLEAR_AUTOS */
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ti->t_ipAddr;
    addr.sin_port = ti->t_ipPort;


    return sendto(ti->fd, (char *)data, len, 0, SA_CAST(&addr), sizeof(addr));
}

int
ReceiveFromIpTransport(data, len, ti)
    char *data;
    int len;
    TransportInfo *ti;
{
    int status;
    struct sockaddr_in from;
    int             fromlen;

    fromlen = sizeof(from);
    status = recvfrom(ti->fd, data, len, 0, SA_CAST(&from), &fromlen);

    ti->t_ipAddr = from.sin_addr.s_addr;
    ti->t_ipPort = from.sin_port;


    return status;
}

char *
FormatIpTransportString(buf, len, ti)
    char *buf;
    int len;
    const TransportInfo *ti;
{
    SR_UINT32 addr = ntohl(ti->t_ipAddr);
    unsigned long port = (unsigned long)ntohs(ti->t_ipPort);

    if (len < 22) {
        strcpy(buf, "");
    } else {
        sprintf(buf, "%lu.%lu.%lu.%lu:%lu",
                (unsigned long)((addr & 0xff000000) >> 24),
                (unsigned long)((addr & 0xff0000) >> 16),
                (unsigned long)((addr & 0xff00) >> 8),
                (unsigned long)(addr & 0xff),
                port);
    }
    return buf;
}


int
OpenIpTransport(ti)
    TransportInfo *ti;
{
    ti->fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (ti->fd < 0) {
        DPRINTF((APERROR, "Could not open socket for IP\n"));
        return -1;
    }
    return 0;
}

int
BindIpTransport(ti, tries)
    TransportInfo *ti;
    int tries;
{
    struct sockaddr_in addr;
    int status;
    const int optval = 1;

#ifdef SR_CLEAR_AUTOS
    memset(&addr, 0, sizeof(addr));
#endif	/* SR_CLEAR_AUTOS */
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = ti->t_ipAddr;
    addr.sin_port = ti->t_ipPort;

    if (setsockopt(ti->fd, SOL_SOCKET, SO_REUSEADDR, 
		   (void *)&optval, sizeof(optval)) != 0) {
        DPRINTF((APWARN,
		 "Failed to set address re-use option for socket\n"));
    }

    while (tries != 0) {
        status = bind(ti->fd, SA_CAST(&addr), sizeof(addr));
        if (status >= 0) {
            return 0;
        }
        if (tries > 0) {
            tries--;
        }
        DPRINTF((APTRACE, "Trying to bind to socket\n"));
    }
    DPRINTF((APTRACE, "Cannot bind to socket\n"));
    return -1;
}

void
CloseIpTransport(ti)
    TransportInfo *ti;
{
#ifdef SR_WIN32
    closesocket(ti->fd);
#else /* SR_WIN32 */
    close(ti->fd);
#endif /* SR_WIN32 */
}
