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

/* $Id: diag.h,v 1.46 1998/02/10 17:42:51 levi Exp $ */

#ifndef SR_DIAG_H
#define SR_DIAG_H

#include <stdio.h>

#ifdef  __cplusplus
extern "C" {
#endif

#if (defined(SR_DEBUG) || defined(WANT_SR_FILENAME))
#define SR_FILENAME static const char sr_filename[] = __FILE__;
#else	/* (defined(SR_DEBUG) || defined(WANT_SR_FILENAME)) */
#define SR_FILENAME
#endif	/* (defined(SR_DEBUG) || defined(WANT_SR_FILENAME)) */

typedef struct _LogEntry {
    SR_INT32    LogLevel;
    SR_UINT32   timestamp;
    char       *progname;
    char       *filename;
    SR_INT32    linenum;
    char       *message;
    void       *UserData;
} LogEntry;


#ifdef SR_DEBUG
typedef void (*LogFunc)
	SR_PROTOTYPE((LogEntry *log));

#ifndef SR_LOGBOOK_BUF_SIZE
#define SR_LOGBOOK_BUF_SIZE 512
#endif	/* SR_LOGBOOK_BUF_SIZE */

typedef struct LogBook_s {
    SR_INT32    LogLevel;
    char        progname_buf[80];
    char        message_buf[SR_LOGBOOK_BUF_SIZE];
    LogEntry   *log_global;
    LogFunc     log_func;
    void       *log_data;
} LogBook;


#define SRLOG_MAX_FORMAT   1  /* highest legal value for format field */

typedef struct _LogAppData {
    SR_INT32    operFlags;
    SR_INT32    format;       /* 0=origianl format, SRLOG_MAX_FORMAT=newest */
    FILE       *logfile_fp;
} LogAppData;


#define LOG( X )  dlfLog(dLog X, __LINE__, sr_filename)

extern  void dlfLog
	SR_PROTOTYPE((LogEntry *log, int line, const char *file));
#endif /* SR_DEBUG */

#if (defined(HAVE_STDARG_H) && defined(STDC_HEADERS))
#ifdef SR_DEBUG
extern  LogEntry *dLog
	SR_PROTOTYPE((LogBook *log_book, SR_INT32 log_level, ...));
#endif /* SR_DEBUG */
extern  int dprintf
	SR_PROTOTYPE((SR_INT32 log_level,...));
#else	/* (defined(HAVE_STDARG_H) && defined(STDC_HEADERS)) */
#ifdef SR_DEBUG
extern  LogEntry *dLog
#ifndef _DEBUG_C_
	SR_PROTOTYPE((int va_alist, ...));
#else /* _DEBUG_C_ */
	SR_PROTOTYPE((char *va_alist, ...));
#endif /* _DEBUG_C_ */
#endif /* SR_DEBUG */
extern  int dprintf
#ifndef _DEBUG_C_
	SR_PROTOTYPE((int va_alist, ...));
#else /* _DEBUG_C_ */
	SR_PROTOTYPE((char *va_alist, ...));
#endif /* _DEBUG_C_ */
#endif	/* (defined(HAVE_STDARG_H) && defined(STDC_HEADERS)) */
extern  void dlfprint
        SR_PROTOTYPE((int line,const char *file));



#ifdef SR_DEBUG
LogFunc BkSetLogFunction
	SR_PROTOTYPE((LogBook *log_book, LogFunc new_func));

LogFunc SetLogFunction
	SR_PROTOTYPE((LogFunc new_func));

LogFunc BkGetLogFunction
	SR_PROTOTYPE((LogBook *log_book));

LogFunc GetLogFunction
	SR_PROTOTYPE((void));


/* LogFunc functions */

void	dsender
	SR_PROTOTYPE((LogBook *log_book, LogEntry *log));

void	FreeLogMessage
	SR_PROTOTYPE((LogEntry *log));

void	PrintLogMessage
	SR_PROTOTYPE((LogEntry *log));

void	PrintShortLogMessage
	SR_PROTOTYPE((LogEntry *log));

#ifdef EMANATE
void    PrintShortLogMessageAndSendToSubagents
	SR_PROTOTYPE((LogEntry *log));

void	SendLogMessageToSubagents
	SR_PROTOTYPE((LogEntry *log));
#endif /* EMANATE */


/* miscellaneous functions */

LogBook *GetLogBook
	SR_PROTOTYPE((void));

LogBook *OpenLog
	SR_PROTOTYPE((void));

void    CloseLog
	SR_PROTOTYPE((LogBook *log_book));

LogEntry *MakeLogMessage
	SR_PROTOTYPE((SR_INT32    LogLevel,
		      SR_UINT32   timestamp,
		      const char *progname,
		      const char *filename,
		      SR_INT32    linenum,
		      const char *message,
		      void       *UserData));

LogEntry *CopyLogMessage
	SR_PROTOTYPE((LogEntry *log));

void	BkSetLogProgramName
	SR_PROTOTYPE((LogBook *log_book, char *s));

void	SetProgname
	SR_PROTOTYPE((char *s));

void	TestLogMessages
	SR_PROTOTYPE((void));

char   *GetLogFullPath
	SR_PROTOTYPE((const char *file_name));



#define FNAME(x) static const char Fname[] = x;
#define NULL_CHECK(p,d,l) if ((p) == NULL) { DPRINTF(d); goto l; }


#define DPRINTF( X )  dprintf X, dlfprint(__LINE__, sr_filename)

SR_INT32 BkSetLogLevel
	 SR_PROTOTYPE((LogBook *log_book, SR_INT32 log_level));

SR_INT32 SetLogLevel
	 SR_PROTOTYPE((SR_INT32 log_level));

SR_INT32 BkGetLogLevel
	 SR_PROTOTYPE((LogBook *log_book));

SR_INT32 GetLogLevel
	 SR_PROTOTYPE((void));

void   *BkSetLogUserData
	SR_PROTOTYPE((LogBook *log_book, void *new_data));

void   *SetLogUserData
	SR_PROTOTYPE((void *new_data));

void   *BkGetLogUserData
	SR_PROTOTYPE((LogBook *log_book));

void   *GetLogUserData
	SR_PROTOTYPE((void));

int     init_extensions
	SR_PROTOTYPE((void));


#define SRLOG_FILE         0x00000001  /* print log messages to a log file? */
#define SRLOG_TRACEFILE    0x00000002  /* allow trace messages in log file? */
#define SRLOG_APPEND       0x00000004  /* append to existing log file? */
#define SRLOG_STDOUT       0x00000008  /* print log messages to stdout? */
#define SRLOG_STDERR       0x00000010  /* print log messages to stderr? */
#ifdef EMANATE
#ifdef HPUX
#define SRLOG_HP_MTOS      0x08000000  /* delay MA->SA exchange of messages */
#define SRLOG_HP_STOM      0x10000000  /* delay SA->MA exchange of messages */
#else /* HPUX */
#define SRLOG_RESERVED_D   0x08000000
#define SRLOG_RESERVED_C   0x10000000
#endif /* HPUX */
#define SRLOG_MTOS         0x20000000  /* send log messages to subagents? */
#define SRLOG_STOM         0x40000000  /* process messages from subagents? */
#else /* EMANATE */
#define SRLOG_RESERVED_D   0x08000000
#define SRLOG_RESERVED_C   0x10000000
#define SRLOG_RESERVED_B   0x20000000
#define SRLOG_RESERVED_A   0x40000000
#endif /* EMANATE */
#define SRLOG_EXTEND       0x80000000  /* reserved for extensions */

#else				/* SR_DEBUG */


#define DPRINTF( X )
#define FNAME(x)
#define NULL_CHECK(p,d,l) if ((p) == NULL) goto l

#endif				/* SR_DEBUG */


/*
 * If you add any additional log levels, be sure to update
 * the function TestLogMessages() in debug.c!
 */

#define APWARN   0x1000
#define APERROR  0x2000
#define APTHREAD 0x4000   /* same as TASKTRACE */
#define APTIMER  0x8000   /* same as TIMERTRACE */


#define APTRACE		0x0080	/* Generic Trace */


#define APCONFIG        0x00010000  /* Trace config file i/o */
#define APPACKET        0x00020000  /* Trace SNMP packet parse/builds */
#define APTRAP          0x00040000  /* Trace trap/inform messages */
#define APACCESS        0x00080000  /* Trace access control processing */
#if defined(EMANATE)
#define APEMANATE       0x00100000  /* EMANATE master/subagent tracing */
#endif	/* defined(EMANATE) */
#define APVERBOSE       0x00200000  /* Extra debugging tracing */
#define APUSER          0x00400000  /* User debug trace - customers */
                    /*  0x00800000     Reserved */
                    /*  0x10000000     Reserved */
                    /*  0x20000000     Reserved */
                    /*  0x40000000     Reserved */
                    /*  0x80000000     Reserved */

#define APALL		0x7FFF7FFF /* OR of all the above DO NOT PASS TO DPRINTF */

#define APALWAYS	0x00000000 /* always print this message PASS TO DPRINTF */


#ifdef SR_DEBUG
char * sys_errname
    SR_PROTOTYPE((int i));
#endif	/* SR_DEBUG */

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif				/* ! SR_DIAG_H */
