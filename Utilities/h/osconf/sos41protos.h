/*
 *
 * Copyright (C) 1996-1998 by SNMP Research, Incorporated.
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

/* $Id: sos41protos.h,v 1.12 1998/02/03 09:25:47 partain Exp $ */

#ifndef SR_SOS4PROTOS_H
#define SR_SOS4PROTOS_H

#ifdef  __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "sr_time.h"
#include "sr_proto.h"

extern int accept 
    SR_PROTOTYPE((int s,
                  void *addr,
                  int *addrlen));

extern unsigned int alarm 
    SR_PROTOTYPE((unsigned int sec));

extern int bind 
    SR_PROTOTYPE((int s,
                  const void *addr,
                  int addrlen));

extern void bzero
    SR_PROTOTYPE((char *s,
		  int len));

extern int close 
    SR_PROTOTYPE((int filedes));

extern int connect 
    SR_PROTOTYPE((int s,
                  const void *addr,
                 int addrlen));

extern int fclose 
    SR_PROTOTYPE((FILE *stream));

extern int fflush 
    SR_PROTOTYPE((FILE *strean ));

extern pid_t fork 
    SR_PROTOTYPE((void));

extern int fprintf 
    SR_PROTOTYPE((FILE *stream,
                  const char *format,
                  ...));
extern int fputs 
    SR_PROTOTYPE((const char *s,
                  FILE *stream));

extern int fscanf 
    SR_PROTOTYPE((FILE *stream,
                  const char *format,
                  ...));

extern int getdtablesize 
    SR_PROTOTYPE((void));

extern int gethostname 
    SR_PROTOTYPE((char *hostname,
                  size_t size));

extern int getopt 
    SR_PROTOTYPE((int argc,
                  char * const argv[],
                  const char *optstring));

extern int getpeername
    SR_PROTOTYPE((int s,
                 struct sockaddr *name,
                 int *namelen));

extern pid_t getpid 
    SR_PROTOTYPE((void));


extern int getsockname
    SR_PROTOTYPE((int s,
                 struct sockaddr *name,
                 int *namelen));

extern int gettimeofday
    SR_PROTOTYPE((struct timeval *t,
		   struct timezone *z));

extern char *inet_ntoa
    SR_PROTOTYPE((struct in_addr in));

extern int siginterrupt
    SR_PROTOTYPE((int sig, int flag));

extern u_long inet_addr
    SR_PROTOTYPE((char *cp));

extern int sendto
    SR_PROTOTYPE((int s,
		   char *m,
		   int l,
		   int f,
		   struct sockaddr *t,
		   int tl));

extern int recvfrom
    SR_PROTOTYPE((int s,
		   char *b,
		   int l,
		   int f,
		   struct sockaddr *fr,
		   int *fl));

extern time_t time
    SR_PROTOTYPE((time_t *t));

extern int toupper
    SR_PROTOTYPE((int i));

extern int ioctl 
    SR_PROTOTYPE((int fd,
                  int request,
                  ...));

extern int listen 
    SR_PROTOTYPE((int s,
                  int backlog));

extern int lockf 
    SR_PROTOTYPE((int fd,
                  int function,
                  off_t size));

extern off_t lseek 
    SR_PROTOTYPE((int fd,
                  off_t offset,
                  int whence));

extern int pipe 
    SR_PROTOTYPE((int fd[2]));

extern int printf 
    SR_PROTOTYPE((const char* format,
                  ...));

extern long random 
    SR_PROTOTYPE((void));

extern int recv 
    SR_PROTOTYPE((int s,
                  void *buf,
                  int len,
                  int flags));

extern int rename 
    SR_PROTOTYPE((const char *src,
                  const char *dst));

extern int select 
    SR_PROTOTYPE(( size_t nfds,
                  fd_set *rdfd,
                  fd_set *wrfd,
                  fd_set *exfd,
		  struct timeval *timeout )); 

extern int send 
    SR_PROTOTYPE((int s,
                  const void *buf,
                  int len,
                  int flags));

extern int setlinebuf 
    SR_PROTOTYPE((FILE *stream));

extern int setsockopt 
    SR_PROTOTYPE((int s,
                  int level,
                  int optname,
                  const void *optval,
                  int optlen));

extern int setvbuf 
    SR_PROTOTYPE((FILE *stream,
                  char *buf,
                  int type,
                  size_t size));

extern int srandom 
    SR_PROTOTYPE((int seed));

extern unsigned int sleep 
    SR_PROTOTYPE((unsigned int seconds));

extern int socket 
    SR_PROTOTYPE((int s,
                  int type,
                  int procotol));

extern int strcasecmp
    SR_PROTOTYPE((const char *s1,
                 const char *s2));

extern int strncasecmp
    SR_PROTOTYPE((const char *s1,
                 const char *s2,
                 int n));

extern long strtol 
    SR_PROTOTYPE((const char *str,
                  char **ptr,
                  int base));

extern int system 
    SR_PROTOTYPE((const char *command));

extern int tolower 
    SR_PROTOTYPE((int c));

extern void usleep 
    SR_PROTOTYPE((unsigned useconds));

extern int vsprintf
    SR_PROTOTYPE((char *s,
		   const char *format,
		   char *arg));


#ifndef __USE_FIXED_PROTOTYPES__
/* when you used -D__USE_FIXED_PROTOTYPES__ these are in include files */
extern int vprintf 
    SR_PROTOTYPE((const char *format,
                  ...));

extern int vfprintf 
    SR_PROTOTYPE((FILE *stream,
                  const char *format,
                  ...));

extern int _flsbuf 
    SR_PROTOTYPE((unsigned char c,
                  FILE *stream));
extern int sscanf 
    SR_PROTOTYPE((char *s,
                  const char *format,
                  ...));
size_t strftime
    SR_PROTOTYPE((const char *s,
                 size_t maxsize,
                 const char *format,
                 const struct tm *timeptr));
extern int scanf 
    SR_PROTOTYPE(( char* format,
                  ...));
extern int fwrite 
    SR_PROTOTYPE((const char *ptr,
                  size_t size,
                  size_t nitems,
                  FILE *stream));
extern int fread 
    SR_PROTOTYPE((char *ptr,
                  size_t size,
                  size_t nitems,
                  FILE *stream));
#endif	/* __USE_FIXED_PROTOTYPES__ */

extern int _filbuf 
    SR_PROTOTYPE((FILE *stream));

#ifdef lint	
extern void exit 
    SR_PROTOTYPE((int status)); /* this is right.  stdlib.h is wrong */

extern int perror 
    SR_PROTOTYPE((const char *s));

#else
/* exit() in stdlib.h claims it returns int.  it lies. */
extern void perror 
    SR_PROTOTYPE((const char *s ));

#endif /* lint */

/* Define if you have the atoi function */
#define HAVE_ATOI 1

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif	/* SR_SOS4PROTOS_H */
