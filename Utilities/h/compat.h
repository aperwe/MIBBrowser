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

/* $Id: compat.h,v 1.15.4.1 1998/04/08 21:48:53 moulton Exp $ */

#ifndef SR_COMPAT_H
#define SR_COMPAT_H

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

#ifndef HAVE_STRCASECMP
int strcasecmp
    SR_PROTOTYPE((const char *s1,
                  const char *s2));
int strncasecmp
    SR_PROTOTYPE((const char *s1,
		  const char *s2,
		  int n));
#endif  /* HAVE_STRCASECMP */


#if ! ( defined(HAVE_USLEEP) )

#if ! ( defined(SCO) )
void usleep SR_PROTOTYPE((unsigned int m));
#endif	/* ! ( defined(SCO) ) */

#endif	/* ! ( defined(HAVE_USLEEP) ) */

#ifndef HAVE_MEMMOVE
void *memmove
   SR_PROTOTYPE((void *s1,
		 const void *s2,
		 size_t n));
#endif	/* HAVE_MEMMOVE */




/* 
 * POSIX specifies that these macros should be defined in <sys/stat.h>
 * If they are not defined we define them to the usual UNIX values.
 */

#ifndef S_IRWXU
#define	S_IRWXU	00700
#endif /* S_IRWXU */
#ifndef S_IRUSR
#define	S_IRUSR	00400
#endif /* S_IRUSR */
#ifndef S_IWUSR
#define	S_IWUSR	00200
#endif /* S_IWUSR */
#ifndef S_IXUSR
#define	S_IXUSR	00100
#endif /* S_IXUSR */
#ifndef S_IRWXG
#define	S_IRWXG	00070
#endif /* S_IRWXG */
#ifndef S_IRGRP
#define	S_IRGRP	00040
#endif /* S_IRGRP */
#ifndef S_IWGRP
#define	S_IWGRP	00020
#endif /* S_IWGRP */
#ifndef S_IXGRP
#define	S_IXGRP	00010
#endif /* S_IXGRP */
#ifndef S_IRWXO
#define	S_IRWXO	00007
#endif /* S_IRWXO */
#ifndef S_IROTH
#define	S_IROTH	00004
#endif /* S_IROTH */
#ifndef S_IWOTH
#define	S_IWOTH	00002
#endif /* S_IWOTH */
#ifndef S_IXOTH
#define	S_IXOTH	00001
#endif /* S_IXOTH */

#ifndef S_ISDIR
#define	S_ISDIR(mode)	(((mode)&0xF000) == 0x4000)
#endif /* S_ISDIR */
#ifndef S_ISCHR
#define	S_ISCHR(mode)	(((mode)&0xF000) == 0x2000)
#endif /* S_ISCHR */
#ifndef S_ISBLK
#define	S_ISBLK(mode)	(((mode)&0xF000) == 0x6000)
#endif /* S_ISBLK */
#ifndef S_ISREG
#define	S_ISREG(mode)	(((mode)&0xF000) == 0x8000)
#endif /* S_ISREG */
#ifndef S_ISFIFO
#define	S_ISFIFO(mode)	(((mode)&0xF000) == 0x1000)
#endif /* S_ISFIFO */


#ifdef SUNOS
#include <malloc.h>
/* The SunOS 4.x realloc() is not ANSI compliant */
#define realloc sr_realloc
malloc_t sr_realloc(malloc_t ptr, size_t size);
#endif /* SUNOS */

#ifdef SR_WIN32
/* Winsock select() has some restrictions on empty FD_SETS that
   don't exist on UNIX, so we can't use it quite as it is. */

#define select(n, r, w, e, t) sr_select(n, r, w, e, t, __FILE__)

#ifndef _WINSOCKAPI_
struct timeval;
typedef struct fd_set fd_set;
#endif /* _WINSOCKAPI_ */
int sr_select(int nfds, fd_set *readfds, fd_set *writefds, 
	      fd_set *exceptfds, const struct timeval *timeout,
	      const char *caller_filename);
#endif   /* SR_WIN32 */

#ifdef  __cplusplus
}
#endif	/* __cplusplus */

/* Do not put anything after this #endif */
#endif	/* SR_COMPAT_H */
