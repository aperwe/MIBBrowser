/************************************************************
*
*					AccessIntegrator
*
*	Copyright (C)  Siemens AG 2002  All Rights reserved.
*
************************************************************/

/**
*	@file		snmpio.c
*	@brief		<TODO>
*	@author		Artur Perwenis
*	@date		10-Jan-2002
*
*	@history
*	@item		10-Jan-2002		Artur Perwenis		Imported.
*	@item		10-Jan-2002		Artur Perwenis		Added header include for 'htons' support.
*	@item		10-Jan-2002		Artur Perwenis		Added header "sri/sigfunc.h"
*	@item		10-Jan-2002		Artur Perwenis		Commented out a call to SetSigFunction (), which is unsupported on Win32 systems.
*/

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

#if (! ( defined(lint) ) && defined(SR_RCSID))
static const char rcsid[] = "$Id: snmpio.c,v 1.40 1998/01/06 18:48:30 mark Exp $";
#endif	/* (! ( defined(lint) ) && defined(SR_RCSID)) */

/*
 * snmpio.c
 *
 *   snmpio.c contains routines which are commonly called by SNMP
 *   applications for network input and output.
 */

#include "sr_conf.h"

#include <stdio.h>

#include <string.h>

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif  /* HAVE_MEMORY_H */

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif  /* HAVE_MALLOC_H */

#include <stdlib.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */

#include <sys/types.h>         /*  needed for recvfrom and soclose */


#include <winsock.h>
 
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif /* HAVE_SYS_SOCKET_H */

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif /* HAVE_NETINET_IN_H */

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif /* HAVE_ARPA_INET_H */

#include "sr_proto.h"
#include "sr_type.h"

#include "sr_snmp.h"
#include "sr_fdset.h"

/* this include ONLY standard include files */
#include "snmpio.h"

//Artur Perwenis addition. 2002.01.10
//#include "mibtools/mosy/manifest.h"
#include "sri/sigfunc.h"
/*
 * Before you're tempted to put include files like netdb.h and
 * netinet/in.h here, don't.  some systems (ultrix, for example)
 * don't check for multiple inclusion of these and they are included in
 * include/snmpio.* which is where you should put them.
 */

#include "diag.h"
SR_FILENAME



#include "sr_time.h"


struct hostent *hp;



int TimeOut
    SR_PROTOTYPE((void));

char imagename[128]; /* place to hold name of image */
int fd;
int received_state;
SR_INT32 packet_len;
unsigned char packet[SR_PACKET_LEN];
int seconds;



/*
 * SendRequest: This is now a macro in ../include/snmpio.h
 */

/*
 * SrSendRequest:
 */
int
SrSendRequest(
    int sock,
    const unsigned char *packet,
    const SR_INT32 packet_length)
{
    unsigned char   *out_pkt;

    /* allocate the packet */
    out_pkt = (unsigned char *) malloc((size_t)SR_DEFAULT_MAX_MSG_SIZE);
    if (out_pkt == NULL) {
	DPRINTF((APWARN, "%s:  couldn't get send buffer\n", imagename));
	return(FALSE);
    }

    /*
     * copy the data into the buffer
     *
     * we have to cast packet_length to be an size_t to avoid trouble
     * with long/int sizes on some machines
     */
    memcpy(out_pkt, packet, (size_t) packet_length);

    /* for debug PrintPacket(packet, packet_length); */
    if (sendto(sock, (char *)out_pkt, (int) packet_length, 0,
             (struct sockaddr *)&Sin, sizeof(Sin)) < 0) {
	DPRINTF((APWARN, "%s:  send.\n", imagename));
	free((char *) out_pkt);
	return (FALSE);
    }



    /* free the packet buffer */
    free((char *) out_pkt);
    return (TRUE);
}


/*
 * GetResponse:
 */
int
GetResponse()
{



#if (defined(HAVE_RECVFROM) || defined(SR_WINSOCK))
    int fromlen = sizeof(from);
#endif	/* (defined(HAVE_RECVFROM) || defined(SR_WINSOCK)) */


#if defined(SR_WINSOCK)
    fd_set readmask;
    struct timeval timeout;
#endif	/* defined(SR_WINSOCK) */


#ifdef SR_WINSOCK
    unsigned long argp = 1;
    int cc;
#endif /* SR_WINSOCK */

    received_state = SR_WAITING;

#if (defined(HAVE_RECVFROM) && ! ( defined(SR_WINSOCK) ))
#ifdef HAVE_ALARM
    alarm(seconds);
#endif	/* HAVE_ALARM */
    packet_len = (SR_INT32) recvfrom(fd, (char *) packet, sizeof(packet), 0,
				 (struct sockaddr *)&from, &fromlen);
#ifdef HAVE_ALARM
    alarm(0);
#endif	/* HAVE_ALARM */
    if (packet_len <= 0) {
	return (SR_TIMEOUT);
    }
    return (SR_RECEIVED);
#endif	/* (defined(HAVE_RECVFROM) && ! ( defined(SR_WINSOCK) )) */

#if defined(SR_WINSOCK)
    FD_ZERO(&readmask);
    SR_FD_SET(fd, &readmask);
    timeout.tv_sec = seconds;
    timeout.tv_usec = 0;
#ifdef SR_WINSOCK
    /* set the socket to non-blocking */
    ioctlsocket(fd, FIONBIO, &argp); 
#endif	/* SR_WINSOCK */
    cc = select((fd + 1),
                (SELECT_BITMASK_TYPE)&readmask,
                (SELECT_BITMASK_TYPE)0,
                (SELECT_BITMASK_TYPE)0,
                &timeout);
    if (cc > 0) {
        packet_len = (SR_INT32) recvfrom(fd, (char *) packet, sizeof(packet), 0,
				 (struct sockaddr *)&from, &fromlen);
        if (packet_len > 0) {
	    return (SR_RECEIVED);
	}
    }
    return (SR_TIMEOUT);
#endif	/* defined(SR_WINSOCK) */



}

/*
 * CloseUp: 
 */
void
CloseUp()
{

#ifdef HAVE_CLOSE
    close(fd);
#endif /* HAVE_CLOSE */

#ifdef SR_WINSOCK
    closesocket(fd);
#endif /* WINSOCK */



}


/*
 * TimeOut: Handle timing out.
 */
int
TimeOut()
{
    received_state = SR_TIMEOUT;


#ifdef SYSV
//    SetSigFunction(SIGALRM, (SigFunc) TimeOut);
#endif				/* SYSV */

    return (0);
}

/*
 * MakeReqId: 
 */
SR_INT32
MakeReqId()
{
    time_t        ltime;

    time(&ltime);
    return (SR_INT32) (ltime & 0x7fff);
}



