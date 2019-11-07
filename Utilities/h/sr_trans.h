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

/* $Id: sr_trans.h,v 1.18 1998/01/08 02:36:12 mark Exp $ */

#ifndef SR_SR_TRANS_H
#define SR_SR_TRANS_H

#ifdef  __cplusplus
extern "C" {
#endif

extern const OID      *rfc1157DomainOID;
extern const OID      *snmpUDPDomainOID;

#define SR_UNDEFINED_TRANSPORT  0

typedef struct transportInfo TransportInfo;


#define	SR_UDP_TRANSPORT	1
#define	SR_IP_TRANSPORT		SR_UDP_TRANSPORT


typedef struct ipTransportInfo IpTransportInfo;

struct ipTransportInfo {
    SR_UINT32 addr;       /* In network byte order */
    unsigned short port;  /* In network byte order */
};


/*
 * This structure is used to contain generalized transport information.
 * The type member indicates the type of transport represented, and the
 * union contains the specific transport information.
 */
struct transportInfo {
    SR_INT32 type;			/* type of transport */
    SR_INT32 flags;                     /* OS specific flags */
    SR_INT32 fd;
    union {
        IpTransportInfo ip;
    } ti;
};

/* Access Macros */
#define	t_ipAddr	ti.ip.addr
#define	t_ipPort	ti.ip.port


void CopyTransportInfo
    SR_PROTOTYPE((TransportInfo *dst,
                  const TransportInfo *src));

void CopyTransportAddr
    SR_PROTOTYPE((TransportInfo *dst,
                  const TransportInfo *src));

int CmpTransportInfo
    SR_PROTOTYPE((const TransportInfo *ti1,
                  const TransportInfo *ti2));

int CmpTransportInfoWithMask
    SR_PROTOTYPE((const TransportInfo *ti1,
		  const TransportInfo *ti2,
		  const TransportInfo *mask));
 
int TransportInfoToOIDAndOctetString
    SR_PROTOTYPE((const TransportInfo *ti,
                  OID **domain,
                  OctetString **address));

int OIDAndOctetStringToTransportInfo
    SR_PROTOTYPE((TransportInfo *ti,
                  OID *domain,
                  const OctetString *address));

int SendToTransport
    SR_PROTOTYPE((const char *data,
                  int len,
                  const TransportInfo *ti));

int ReceiveFromTransport
    SR_PROTOTYPE((char *data,
                  int len,
                  TransportInfo *ti));

char *FormatTransportString
    SR_PROTOTYPE((char *buf,
                  int len,
                  const TransportInfo *ti));

int OpenTransport
    SR_PROTOTYPE((TransportInfo *ti));

int BindTransport
    SR_PROTOTYPE((TransportInfo *ti,
                  int tries));

void CloseTransport
    SR_PROTOTYPE((TransportInfo *ti));

#ifdef  __cplusplus
}
#endif

#endif	/* SR_SR_TRANS_H */
