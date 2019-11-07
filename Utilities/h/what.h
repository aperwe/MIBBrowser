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

#ifdef SR_WHAT
#ifndef SR_SEEN_WHAT_H
#define SR_SEEN_WHAT_H

#ifdef __cplusplus
extern  {
#endif  /* __cplusplus */
#ifdef AIX
static const char what_AIX[] = "@(#)-DAIX";
#endif /* AIX */












#ifdef EMANATE
static const char what_EMANATE[] = "@(#)-DEMANATE";
#endif /* EMANATE */

#ifdef EMANATE_LINES
static const char what_EMANATE_LINES[] = "@(#)-DEMANATE_LINES";
#endif /* EMANATE_LINES */





#ifdef GETGROUPS_T
static const char what_GETGROUPS_T[] = "@(#)-DGETGROUPS_T";
#endif /* GETGROUPS_T */

#ifdef HAVE_ALARM
static const char what_HAVE_ALARM[] = "@(#)-DHAVE_ALARM";
#endif /* HAVE_ALARM */

#ifdef HAVE_ALTZONE
static const char what_HAVE_ALTZONE[] = "@(#)-DHAVE_ALTZONE";
#endif /* HAVE_ALTZONE */

#ifdef HAVE_ARPA_INET_H
static const char what_HAVE_ARPA_INET_H[] = "@(#)-DHAVE_ARPA_INET_H";
#endif /* HAVE_ARPA_INET_H */

#ifdef HAVE_A_OUT_H
static const char what_HAVE_A_OUT_H[] = "@(#)-DHAVE_A_OUT_H";
#endif /* HAVE_A_OUT_H */



#ifdef HAVE_CLOSE
static const char what_HAVE_CLOSE[] = "@(#)-DHAVE_CLOSE";
#endif /* HAVE_CLOSE */


static const char what_HAVE_CTYPE_H[] = "@(#)-DHAVE_CTYPE_H";

#ifdef HAVE_DIRENT_H
static const char what_HAVE_DIRENT_H[] = "@(#)-DHAVE_DIRENT_H";
#endif /* HAVE_DIRENT_H */

#ifdef HAVE_DLFCN_H
static const char what_HAVE_DLFCN_H[] = "@(#)-DHAVE_DLFCN_H";
#endif /* HAVE_DLFCN_H */

#ifdef HAVE_DL_H
static const char what_HAVE_DL_H[] = "@(#)-DHAVE_DL_H";
#endif /* HAVE_DL_H */

#ifdef HAVE_EIGHT_BYTE_LONGS
static const char what_HAVE_EIGHT_BYTE_LONGS[] = "@(#)-DHAVE_EIGHT_BYTE_LONGS";
#endif /* HAVE_EIGHT_BYTE_LONGS */

static const char what_HAVE_ERRNO_H[] = "@(#)-DHAVE_ERRNO_H";

static const char what_HAVE_FCNTL_H[] = "@(#)-DHAVE_FCNTL_H";

#ifdef HAVE_FTIME
static const char what_HAVE_FTIME[] = "@(#)-DHAVE_FTIME";
#endif /* HAVE_FTIME */

static const char what_HAVE_GETCWD[] = "@(#)-DHAVE_GETCWD";

static const char what_HAVE_GETENV[] = "@(#)-DHAVE_GETENV";

static const char what_HAVE_GETHOSTBYNAME[] = "@(#)-DHAVE_GETHOSTBYNAME";

static const char what_HAVE_GETHOSTNAME[] = "@(#)-DHAVE_GETHOSTNAME";

#ifdef HAVE_GETMNTENT
static const char what_HAVE_GETMNTENT[] = "@(#)-DHAVE_GETMNTENT";
#endif /* HAVE_GETMNTENT */

#ifdef HAVE_GETPAGESIZE
static const char what_HAVE_GETPAGESIZE[] = "@(#)-DHAVE_GETPAGESIZE";
#endif /* HAVE_GETPAGESIZE */

#ifdef HAVE_GETSERVBYNAME
static const char what_HAVE_GETSERVBYNAME[] = "@(#)-DHAVE_GETSERVBYNAME";
#endif /* HAVE_GETSERVBYNAME */

#ifdef HAVE_GETTIMEOFDAY
static const char what_HAVE_GETTIMEOFDAY[] = "@(#)-DHAVE_GETTIMEOFDAY";
#endif /* HAVE_GETTIMEOFDAY */



static const char what_HAVE_INET_ADDR[] = "@(#)-DHAVE_INET_ADDR";

static const char what_HAVE_INET_NTOA[] = "@(#)-DHAVE_INET_NTOA";


#ifdef HAVE_KVM_H
static const char what_HAVE_KVM_H[] = "@(#)-DHAVE_KVM_H";
#endif /* HAVE_KVM_H */

static const char what_HAVE_LIMITS_H[] = "@(#)-DHAVE_LIMITS_H";

#ifdef HAVE_MALLOC_H
static const char what_HAVE_MALLOC_H[] = "@(#)-DHAVE_MALLOC_H";
#endif /* HAVE_MALLOC_H */

#ifdef HAVE_MEMMOVE
static const char what_HAVE_MEMMOVE[] = "@(#)-DHAVE_MEMMOVE";
#endif /* HAVE_MEMMOVE */

#ifdef HAVE_MEMORY_H
static const char what_HAVE_MEMORY_H[] = "@(#)-DHAVE_MEMORY_H";
#endif /* HAVE_MEMORY_H */

static const char what_HAVE_MKDIR[] = "@(#)-DHAVE_MKDIR";

#ifdef HAVE_MMAP
static const char what_HAVE_MMAP[] = "@(#)-DHAVE_MMAP";
#endif /* HAVE_MMAP */

#ifdef HAVE_NETDB_H
static const char what_HAVE_NETDB_H[] = "@(#)-DHAVE_NETDB_H";
#endif /* HAVE_NETDB_H */

#ifdef HAVE_NETINET_IN_H
static const char what_HAVE_NETINET_IN_H[] = "@(#)-DHAVE_NETINET_IN_H";
#endif /* HAVE_NETINET_IN_H */

#ifdef HAVE_NETINET_IN_SYSTM_H
static const char what_HAVE_NETINET_IN_SYSTM_H[] = "@(#)-DHAVE_NETINET_IN_SYSTM_H";
#endif /* HAVE_NETINET_IN_SYSTM_H */

#ifdef HAVE_NETINET_IP_H
static const char what_HAVE_NETINET_IP_H[] = "@(#)-DHAVE_NETINET_IP_H";
#endif /* HAVE_NETINET_IP_H */

#ifdef HAVE_NETINET_IP_ICMP_H
static const char what_HAVE_NETINET_IP_ICMP_H[] = "@(#)-DHAVE_NETINET_IP_ICMP_H";
#endif /* HAVE_NETINET_IP_ICMP_H */

#ifdef HAVE_NETNS_IDP_H
static const char what_HAVE_NETNS_IDP_H[] = "@(#)-DHAVE_NETNS_IDP_H";
#endif /* HAVE_NETNS_IDP_H */

#ifdef HAVE_NETNS_NS_H
static const char what_HAVE_NETNS_NS_H[] = "@(#)-DHAVE_NETNS_NS_H";
#endif /* HAVE_NETNS_NS_H */

#ifdef HAVE_NET_IF_ARP_H
static const char what_HAVE_NET_IF_ARP_H[] = "@(#)-DHAVE_NET_IF_ARP_H";
#endif /* HAVE_NET_IF_ARP_H */

#ifdef HAVE_NET_IF_H
static const char what_HAVE_NET_IF_H[] = "@(#)-DHAVE_NET_IF_H";
#endif /* HAVE_NET_IF_H */


#ifdef HAVE_NET_ROUTE_H
static const char what_HAVE_NET_ROUTE_H[] = "@(#)-DHAVE_NET_ROUTE_H";
#endif /* HAVE_NET_ROUTE_H */

#ifdef HAVE_NLIST_H
static const char what_HAVE_NLIST_H[] = "@(#)-DHAVE_NLIST_H";
#endif /* HAVE_NLIST_H */


#ifdef HAVE_PATHS_H
static const char what_HAVE_PATHS_H[] = "@(#)-DHAVE_PATHS_H";
#endif /* HAVE_PATHS_H */






static const char what_HAVE_PUTENV[] = "@(#)-DHAVE_PUTENV";

#ifdef HAVE_RECVFROM
static const char what_HAVE_RECVFROM[] = "@(#)-DHAVE_RECVFROM";
#endif /* HAVE_RECVFROM */

#ifdef HAVE_RENAME
static const char what_HAVE_RENAME[] = "@(#)-DHAVE_RENAME";
#endif /* HAVE_RENAME */


static const char what_HAVE_SELECT[] = "@(#)-DHAVE_SELECT";

#ifdef HAVE_SENDTO
static const char what_HAVE_SENDTO[] = "@(#)-DHAVE_SENDTO";
#endif /* HAVE_SENDTO */

static const char what_HAVE_SETJMP_H[] = "@(#)-DHAVE_SETJMP_H";

#ifdef HAVE_SGTTY_H
static const char what_HAVE_SGTTY_H[] = "@(#)-DHAVE_SGTTY_H";
#endif /* HAVE_SGTTY_H */

#ifdef HAVE_SIGADDSET
static const char what_HAVE_SIGADDSET[] = "@(#)-DHAVE_SIGADDSET";
#endif /* HAVE_SIGADDSET */

#ifdef HAVE_SIGDELSET
static const char what_HAVE_SIGDELSET[] = "@(#)-DHAVE_SIGDELSET";
#endif /* HAVE_SIGDELSET */

#ifdef HAVE_SIGEMPTYSET
static const char what_HAVE_SIGEMPTYSET[] = "@(#)-DHAVE_SIGEMPTYSET";
#endif /* HAVE_SIGEMPTYSET */

#ifdef HAVE_SIGINTERRUPT
static const char what_HAVE_SIGINTERRUPT[] = "@(#)-DHAVE_SIGINTERRUPT";
#endif /* HAVE_SIGINTERRUPT */

#ifdef HAVE_SIGISMEMBER
static const char what_HAVE_SIGISMEMBER[] = "@(#)-DHAVE_SIGISMEMBER";
#endif /* HAVE_SIGISMEMBER */

#ifdef HAVE_SIGNAL
static const char what_HAVE_SIGNAL[] = "@(#)-DHAVE_SIGNAL";
#endif /* HAVE_SIGNAL */

#ifdef HAVE_SIGNAL_H
static const char what_HAVE_SIGNAL_H[] = "@(#)-DHAVE_SIGNAL_H";
#endif /* HAVE_SIGNAL_H */

#ifdef HAVE_SIGPROCMASK
static const char what_HAVE_SIGPROCMASK[] = "@(#)-DHAVE_SIGPROCMASK";
#endif /* HAVE_SIGPROCMASK */

#ifdef HAVE_SLEEP
static const char what_HAVE_SLEEP[] = "@(#)-DHAVE_SLEEP";
#endif /* HAVE_SLEEP */

static const char what_HAVE_SOCKET[] = "@(#)-DHAVE_SOCKET";


#ifdef HAVE_STAB_H
static const char what_HAVE_STAB_H[] = "@(#)-DHAVE_STAB_H";
#endif /* HAVE_STAB_H */

#ifdef HAVE_STDARG_H
static const char what_HAVE_STDARG_H[] = "@(#)-DHAVE_STDARG_H";
#endif /* HAVE_STDARG_H */

static const char what_HAVE_STDDEF_H[] = "@(#)-DHAVE_STDDEF_H";

static const char what_HAVE_STDLIB_H[] = "@(#)-DHAVE_STDLIB_H";

#ifdef HAVE_STRCASECMP
static const char what_HAVE_STRCASECMP[] = "@(#)-DHAVE_STRCASECMP";
#endif /* HAVE_STRCASECMP */

static const char what_HAVE_STRCSPN[] = "@(#)-DHAVE_STRCSPN";

static const char what_HAVE_STRDUP[] = "@(#)-DHAVE_STRDUP";

#ifdef HAVE_STRERROR
static const char what_HAVE_STRERROR[] = "@(#)-DHAVE_STRERROR";
#endif /* HAVE_STRERROR */


static const char what_HAVE_STRING_H[] = "@(#)-DHAVE_STRING_H";

static const char what_HAVE_STRSPN[] = "@(#)-DHAVE_STRSPN";

static const char what_HAVE_STRSTR[] = "@(#)-DHAVE_STRSTR";

static const char what_HAVE_STRTOL[] = "@(#)-DHAVE_STRTOL";

#ifdef HAVE_ST_BLKSIZE
static const char what_HAVE_ST_BLKSIZE[] = "@(#)-DHAVE_ST_BLKSIZE";
#endif /* HAVE_ST_BLKSIZE */

#ifdef HAVE_ST_BLOCKS
static const char what_HAVE_ST_BLOCKS[] = "@(#)-DHAVE_ST_BLOCKS";
#endif /* HAVE_ST_BLOCKS */

#ifdef HAVE_ST_RDEV
static const char what_HAVE_ST_RDEV[] = "@(#)-DHAVE_ST_RDEV";
#endif /* HAVE_ST_RDEV */

#ifdef HAVE_SYNCH_H
static const char what_HAVE_SYNCH_H[] = "@(#)-DHAVE_SYNCH_H";
#endif /* HAVE_SYNCH_H */

#ifdef HAVE_SYS_ERRLIST
static const char what_HAVE_SYS_ERRLIST[] = "@(#)-DHAVE_SYS_ERRLIST";
#endif /* HAVE_SYS_ERRLIST */

#ifdef HAVE_SYS_FCNTL_H
static const char what_HAVE_SYS_FCNTL_H[] = "@(#)-DHAVE_SYS_FCNTL_H";
#endif /* HAVE_SYS_FCNTL_H */

#ifdef HAVE_SYS_FILE_H
static const char what_HAVE_SYS_FILE_H[] = "@(#)-DHAVE_SYS_FILE_H";
#endif /* HAVE_SYS_FILE_H */

#ifdef HAVE_SYS_FILIO_H
static const char what_HAVE_SYS_FILIO_H[] = "@(#)-DHAVE_SYS_FILIO_H";
#endif /* HAVE_SYS_FILIO_H */

#ifdef HAVE_SYS_FSID_H
static const char what_HAVE_SYS_FSID_H[] = "@(#)-DHAVE_SYS_FSID_H";
#endif /* HAVE_SYS_FSID_H */

#ifdef HAVE_SYS_FSTYP_H
static const char what_HAVE_SYS_FSTYP_H[] = "@(#)-DHAVE_SYS_FSTYP_H";
#endif /* HAVE_SYS_FSTYP_H */

#ifdef HAVE_SYS_IOCTL_H
static const char what_HAVE_SYS_IOCTL_H[] = "@(#)-DHAVE_SYS_IOCTL_H";
#endif /* HAVE_SYS_IOCTL_H */

#ifdef HAVE_SYS_MNTTAB_H
static const char what_HAVE_SYS_MNTTAB_H[] = "@(#)-DHAVE_SYS_MNTTAB_H";
#endif /* HAVE_SYS_MNTTAB_H */

#ifdef HAVE_SYS_NERR
static const char what_HAVE_SYS_NERR[] = "@(#)-DHAVE_SYS_NERR";
#endif /* HAVE_SYS_NERR */

#ifdef HAVE_SYS_PARAM_H
static const char what_HAVE_SYS_PARAM_H[] = "@(#)-DHAVE_SYS_PARAM_H";
#endif /* HAVE_SYS_PARAM_H */

#ifdef HAVE_SYS_PROCESSOR_H
static const char what_HAVE_SYS_PROCESSOR_H[] = "@(#)-DHAVE_SYS_PROCESSOR_H";
#endif /* HAVE_SYS_PROCESSOR_H */

#ifdef HAVE_SYS_PROCFS_H
static const char what_HAVE_SYS_PROCFS_H[] = "@(#)-DHAVE_SYS_PROCFS_H";
#endif /* HAVE_SYS_PROCFS_H */

#ifdef HAVE_SYS_PROC_H
static const char what_HAVE_SYS_PROC_H[] = "@(#)-DHAVE_SYS_PROC_H";
#endif /* HAVE_SYS_PROC_H */

#ifdef HAVE_SYS_RESOURCE_H
static const char what_HAVE_SYS_RESOURCE_H[] = "@(#)-DHAVE_SYS_RESOURCE_H";
#endif /* HAVE_SYS_RESOURCE_H */

#ifdef HAVE_SYS_SELECT_H
static const char what_HAVE_SYS_SELECT_H[] = "@(#)-DHAVE_SYS_SELECT_H";
#endif /* HAVE_SYS_SELECT_H */


#ifdef HAVE_SYS_SOCKET_H
static const char what_HAVE_SYS_SOCKET_H[] = "@(#)-DHAVE_SYS_SOCKET_H";
#endif /* HAVE_SYS_SOCKET_H */

#ifdef HAVE_SYS_STATVFS_H
static const char what_HAVE_SYS_STATVFS_H[] = "@(#)-DHAVE_SYS_STATVFS_H";
#endif /* HAVE_SYS_STATVFS_H */

static const char what_HAVE_SYS_STAT_H[] = "@(#)-DHAVE_SYS_STAT_H";

#ifdef HAVE_SYS_SWAP_H
static const char what_HAVE_SYS_SWAP_H[] = "@(#)-DHAVE_SYS_SWAP_H";
#endif /* HAVE_SYS_SWAP_H */

#ifdef HAVE_SYS_SYSINFO_H
static const char what_HAVE_SYS_SYSINFO_H[] = "@(#)-DHAVE_SYS_SYSINFO_H";
#endif /* HAVE_SYS_SYSINFO_H */

static const char what_HAVE_SYS_TIMEB_H[] = "@(#)-DHAVE_SYS_TIMEB_H";

#ifdef HAVE_SYS_TIMERS_H
static const char what_HAVE_SYS_TIMERS_H[] = "@(#)-DHAVE_SYS_TIMERS_H";
#endif /* HAVE_SYS_TIMERS_H */

#ifdef HAVE_SYS_TIMES_H
static const char what_HAVE_SYS_TIMES_H[] = "@(#)-DHAVE_SYS_TIMES_H";
#endif /* HAVE_SYS_TIMES_H */

#ifdef HAVE_SYS_TIME_H
static const char what_HAVE_SYS_TIME_H[] = "@(#)-DHAVE_SYS_TIME_H";
#endif /* HAVE_SYS_TIME_H */

static const char what_HAVE_SYS_TYPES_H[] = "@(#)-DHAVE_SYS_TYPES_H";

#ifdef HAVE_SYS_UIO_H
static const char what_HAVE_SYS_UIO_H[] = "@(#)-DHAVE_SYS_UIO_H";
#endif /* HAVE_SYS_UIO_H */

#ifdef HAVE_SYS_UN_H
static const char what_HAVE_SYS_UN_H[] = "@(#)-DHAVE_SYS_UN_H";
#endif /* HAVE_SYS_UN_H */

#ifdef HAVE_SYS_VAR_H
static const char what_HAVE_SYS_VAR_H[] = "@(#)-DHAVE_SYS_VAR_H";
#endif /* HAVE_SYS_VAR_H */

#ifdef HAVE_SYS_VFS_H
static const char what_HAVE_SYS_VFS_H[] = "@(#)-DHAVE_SYS_VFS_H";
#endif /* HAVE_SYS_VFS_H */

#ifdef HAVE_SYS_WAIT_H
static const char what_HAVE_SYS_WAIT_H[] = "@(#)-DHAVE_SYS_WAIT_H";
#endif /* HAVE_SYS_WAIT_H */




#ifdef HAVE_TERMIO_H
static const char what_HAVE_TERMIO_H[] = "@(#)-DHAVE_TERMIO_H";
#endif /* HAVE_TERMIO_H */

#ifdef HAVE_THREAD_H
static const char what_HAVE_THREAD_H[] = "@(#)-DHAVE_THREAD_H";
#endif /* HAVE_THREAD_H */

#ifdef HAVE_TIMERS_H
static const char what_HAVE_TIMERS_H[] = "@(#)-DHAVE_TIMERS_H";
#endif /* HAVE_TIMERS_H */

static const char what_HAVE_TIME_H[] = "@(#)-DHAVE_TIME_H";

#ifdef HAVE_TM_GMTOFF
static const char what_HAVE_TM_GMTOFF[] = "@(#)-DHAVE_TM_GMTOFF";
#endif /* HAVE_TM_GMTOFF */


#ifdef HAVE_TM_ZONE
static const char what_HAVE_TM_ZONE[] = "@(#)-DHAVE_TM_ZONE";
#endif /* HAVE_TM_ZONE */



#ifdef HAVE_TZNAME
static const char what_HAVE_TZNAME[] = "@(#)-DHAVE_TZNAME";
#endif /* HAVE_TZNAME */

#ifdef HAVE_UNISTD_H
static const char what_HAVE_UNISTD_H[] = "@(#)-DHAVE_UNISTD_H";
#endif /* HAVE_UNISTD_H */

#ifdef HAVE_USLEEP
static const char what_HAVE_USLEEP[] = "@(#)-DHAVE_USLEEP";
#endif /* HAVE_USLEEP */


#ifdef HAVE_UTIME_NULL
static const char what_HAVE_UTIME_NULL[] = "@(#)-DHAVE_UTIME_NULL";
#endif /* HAVE_UTIME_NULL */

#ifdef HAVE_UTMPX_H
static const char what_HAVE_UTMPX_H[] = "@(#)-DHAVE_UTMPX_H";
#endif /* HAVE_UTMPX_H */

#ifdef HAVE_UTMP_H
static const char what_HAVE_UTMP_H[] = "@(#)-DHAVE_UTMP_H";
#endif /* HAVE_UTMP_H */

#ifdef HAVE_VALLOC
static const char what_HAVE_VALLOC[] = "@(#)-DHAVE_VALLOC";
#endif /* HAVE_VALLOC */

#ifdef HAVE_VARARGS_H
static const char what_HAVE_VARARGS_H[] = "@(#)-DHAVE_VARARGS_H";
#endif /* HAVE_VARARGS_H */

#ifdef HAVE_VFORK_H
static const char what_HAVE_VFORK_H[] = "@(#)-DHAVE_VFORK_H";
#endif /* HAVE_VFORK_H */

#ifdef HAVE_VM_PAGE_H
static const char what_HAVE_VM_PAGE_H[] = "@(#)-DHAVE_VM_PAGE_H";
#endif /* HAVE_VM_PAGE_H */

static const char what_HAVE_VPRINTF[] = "@(#)-DHAVE_VPRINTF";

static const char what_HAVE_VSPRINTF[] = "@(#)-DHAVE_VSPRINTF";

#ifdef HAVE_WAIT3
static const char what_HAVE_WAIT3[] = "@(#)-DHAVE_WAIT3";
#endif /* HAVE_WAIT3 */

#ifdef HAVE_WINSOCK_H
static const char what_HAVE_WINSOCK_H[] = "@(#)-DHAVE_WINSOCK_H";
#endif /* HAVE_WINSOCK_H */

#ifdef HPUX
static const char what_HPUX[] = "@(#)-DHPUX";
#endif /* HPUX */

#ifdef HPUX10
static const char what_HPUX10[] = "@(#)-DHPUX10";
#endif /* HPUX10 */











#ifdef NEWTIME
static const char what_NEWTIME[] = "@(#)-DNEWTIME";
#endif /* NEWTIME */











static const char what_RETSIGTYPE[] = "@(#)-DRETSIGTYPE";

#ifdef RTLD_LAZY
static const char what_RTLD_LAZY[] = "@(#)-DRTLD_LAZY";
#endif /* RTLD_LAZY */

#ifdef RTLD_NOW
static const char what_RTLD_NOW[] = "@(#)-DRTLD_NOW";
#endif /* RTLD_NOW */

#ifdef SCO
static const char what_SCO[] = "@(#)-DSCO";
#endif /* SCO */


#ifdef SCO5
static const char what_SCO5[] = "@(#)-DSCO5";
#endif /* SCO5 */

#ifdef SINIX
static const char what_SINIX[] = "@(#)-DSINIX";
#endif /* SINIX */

#ifdef SINIX_MIPS
static const char what_SINIX_MIPS[] = "@(#)-DSINIX_MIPS";
#endif /* SINIX_MIPS */

static const char what_SIZEOF_CHAR[] = "@(#)-DSIZEOF_CHAR";

#ifdef SIZEOF_CHAR_P
static const char what_SIZEOF_CHAR_P[] = "@(#)-DSIZEOF_CHAR_P";
#endif /* SIZEOF_CHAR_P */

static const char what_SIZEOF_INT[] = "@(#)-DSIZEOF_INT";

#ifdef SIZEOF_INT_P
static const char what_SIZEOF_INT_P[] = "@(#)-DSIZEOF_INT_P";
#endif /* SIZEOF_INT_P */

#ifdef SIZEOF_LONG
static const char what_SIZEOF_LONG[] = "@(#)-DSIZEOF_LONG";
#endif /* SIZEOF_LONG */

#ifdef SOLARIS
static const char what_SOLARIS[] = "@(#)-DSOLARIS";
#endif /* SOLARIS */








#ifdef SR_BSD
static const char what_SR_BSD[] = "@(#)-DSR_BSD";
#endif /* SR_BSD */













static const char what_SR_IP[] = "@(#)-DSR_IP";













#ifdef SR_NEED_MASTER_SYMS
static const char what_SR_NEED_MASTER_SYMS[] = "@(#)-DSR_NEED_MASTER_SYMS";
#endif /* SR_NEED_MASTER_SYMS */








#ifdef SR_OSF
static const char what_SR_OSF[] = "@(#)-DSR_OSF";
#endif /* SR_OSF */

#ifdef SR_OSF4
static const char what_SR_OSF4[] = "@(#)-DSR_OSF4";
#endif /* SR_OSF4 */









#ifdef SR_SADK
static const char what_SR_SADK[] = "@(#)-DSR_SADK";
#endif /* SR_SADK */

#ifdef SR_SIEMENS_NIXDORF
static const char what_SR_SIEMENS_NIXDORF[] = "@(#)-DSR_SIEMENS_NIXDORF";
#endif /* SR_SIEMENS_NIXDORF */


#ifdef SR_SNMPv1
static const char what_SR_SNMPv1[] = "@(#)-DSR_SNMPv1";
#endif /* SR_SNMPv1 */

#ifdef SR_SNMPv2
static const char what_SR_SNMPv2[] = "@(#)-DSR_SNMPv2";
#endif /* SR_SNMPv2 */

#ifdef SR_SNMPv2c
static const char what_SR_SNMPv2c[] = "@(#)-DSR_SNMPv2c";
#endif /* SR_SNMPv2c */

#ifdef SR_SNMPv3
static const char what_SR_SNMPv3[] = "@(#)-DSR_SNMPv3";
#endif /* SR_SNMPv3 */


#ifdef SR_SUPPORT_LOCAL_SUBS
static const char what_SR_SUPPORT_LOCAL_SUBS[] = "@(#)-DSR_SUPPORT_LOCAL_SUBS";
#endif /* SR_SUPPORT_LOCAL_SUBS */

#ifdef SR_SUPPORT_SHLIB_SUBS
static const char what_SR_SUPPORT_SHLIB_SUBS[] = "@(#)-DSR_SUPPORT_SHLIB_SUBS";
#endif /* SR_SUPPORT_SHLIB_SUBS */

#ifdef SR_TCP_IPC
static const char what_SR_TCP_IPC[] = "@(#)-DSR_TCP_IPC";
#endif /* SR_TCP_IPC */


#ifdef SR_UDS_IPC
static const char what_SR_UDS_IPC[] = "@(#)-DSR_UDS_IPC";
#endif /* SR_UDS_IPC */

#ifdef SR_UNIXWR21
static const char what_SR_UNIXWR21[] = "@(#)-DSR_UNIXWR21";
#endif /* SR_UNIXWR21 */

#ifdef SR_USE_FCNTL
static const char what_SR_USE_FCNTL[] = "@(#)-DSR_USE_FCNTL";
#endif /* SR_USE_FCNTL */



#ifdef SR_USE_RTLD_LAZY
static const char what_SR_USE_RTLD_LAZY[] = "@(#)-DSR_USE_RTLD_LAZY";
#endif /* SR_USE_RTLD_LAZY */

#ifdef SR_USE_RTLD_NOW
static const char what_SR_USE_RTLD_NOW[] = "@(#)-DSR_USE_RTLD_NOW";
#endif /* SR_USE_RTLD_NOW */











#ifdef SR_WIN32
static const char what_SR_WIN32[] = "@(#)-DSR_WIN32";
#endif /* SR_WIN32 */

#ifdef SR_WINDOWS
static const char what_SR_WINDOWS[] = "@(#)-DSR_WINDOWS";
#endif /* SR_WINDOWS */

#ifdef SR_WINDOWS_CONSOLE
static const char what_SR_WINDOWS_CONSOLE[] = "@(#)-DSR_WINDOWS_CONSOLE";
#endif /* SR_WINDOWS_CONSOLE */


#ifdef SR_WINSOCK
static const char what_SR_WINSOCK[] = "@(#)-DSR_WINSOCK";
#endif /* SR_WINSOCK */


#ifdef STDC_HEADERS
static const char what_STDC_HEADERS[] = "@(#)-DSTDC_HEADERS";
#endif /* STDC_HEADERS */

#ifdef SUNOS
static const char what_SUNOS[] = "@(#)-DSUNOS";
#endif /* SUNOS */

#ifdef SUNOS40
static const char what_SUNOS40[] = "@(#)-DSUNOS40";
#endif /* SUNOS40 */

#ifdef SUNOS41
static const char what_SUNOS41[] = "@(#)-DSUNOS41";
#endif /* SUNOS41 */

#ifdef SYSV
static const char what_SYSV[] = "@(#)-DSYSV";
#endif /* SYSV */

#ifdef TIME_WITH_SYS_TIME
static const char what_TIME_WITH_SYS_TIME[] = "@(#)-DTIME_WITH_SYS_TIME";
#endif /* TIME_WITH_SYS_TIME */




#ifdef UNIX
static const char what_UNIX[] = "@(#)-DUNIX";
#endif /* UNIX */


#ifdef UNIXWARE
static const char what_UNIXWARE[] = "@(#)-DUNIXWARE";
#endif /* UNIXWARE */

#ifdef VR4
static const char what_VR4[] = "@(#)-DVR4";
#endif /* VR4 */

#ifdef WANT_SETUPENVIRON
static const char what_WANT_SETUPENVIRON[] = "@(#)-DWANT_SETUPENVIRON";
#endif /* WANT_SETUPENVIRON */

#ifdef WORDS_BIGENDIAN
static const char what_WORDS_BIGENDIAN[] = "@(#)-DWORDS_BIGENDIAN";
#endif /* WORDS_BIGENDIAN */

#ifdef _ALL_SOURCE
static const char what__ALL_SOURCE[] = "@(#)-D_ALL_SOURCE";
#endif /* _ALL_SOURCE */

#ifdef const
static const char what_const[] = "@(#)-Dconst";
#endif /* const */


#ifdef vfork
static const char what_vfork[] = "@(#)-Dvfork";
#endif /* vfork */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* SR_SEEN_WHAT_H */
/* DO NOT PUT ANYTHING AFTER THIS ENDIF */

#endif /* SR_WHAT */

