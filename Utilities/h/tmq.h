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

/* $Id: tmq.h,v 1.20 1998/01/08 02:36:26 mark Exp $ */

#ifndef SR_TMQ_H
#define SR_TMQ_H

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct _TimerEvent {
    SR_UINT32       when;	/* value of GetTimeNow() when timer fires */
    SR_UINT32       period;	/* period of timer, 0 -> oneshot, centi-secs */
    SR_UINT32       id;
    void           *info;
    struct _TimerEvent *prev;
    struct _TimerEvent *next;
    char            fired;
}               TimerEvent;

typedef struct _TimeOutDescriptor TimeOutDescriptor;

typedef void (*TodCallbackFunction) (TimeOutDescriptor *);

struct _TimeOutDescriptor {
    SR_INT32             TimeOutType;
    SR_INT32             TimerId;
    TodCallbackFunction  CallBack;
    SR_INT32             UserData1;
    void                *UserData2;
};

#define Info_Pdu_Timer ((void *)-1)

typedef struct TimerQueue_s {
    TimerEvent *head;
    TimerEvent *tail;
    SR_UINT32 start; /* starting point to search for unused timer id */
    TimerEvent *tm;  /* pre-allocated timer event for CanSetTimeout() */
    void *apinfo;    /* application-specific info, eg EvQ in master/subagent */
} TimerQueue;

TimerQueue *InitTimerQueue
    SR_PROTOTYPE((void *apinfo));

SR_UINT32 TvpToTimeOfDay
    SR_PROTOTYPE((const struct timeval * tp));

int TimeQEmpty 
    SR_PROTOTYPE((const TimerQueue *TmQ));

int TimersInQ
    SR_PROTOTYPE((const TimerQueue *TmQ));

void TimeQInsertBefore
    SR_PROTOTYPE((TimerQueue *TmQ,
		  TimerEvent * newtm,
                  TimerEvent * beforetm));

void TimeQEnqueue
    SR_PROTOTYPE((TimerQueue *TmQ,
		  TimerEvent * tm));

TimerEvent     *TimeQDequeue
    SR_PROTOTYPE((TimerQueue *TmQ,
		  TimerEvent * tm));

TimerEvent     *TimeQFirst
    SR_PROTOTYPE((TimerQueue *TmQ));

int CheckTimers
    SR_PROTOTYPE((TimerQueue *TmQ));

int CanSetTimeout
    SR_PROTOTYPE((void));

void DontNeedTimeout
    SR_PROTOTYPE((void));

int SetPeriodicTimeout
    SR_PROTOTYPE((SR_UINT32 when,
                  SR_UINT32 period,
                  void *info));

int SetOneshotTimeout
    SR_PROTOTYPE((SR_UINT32 when,
                  void *info));

int CancelTimeout
    SR_PROTOTYPE((int id));

int DeferTimeout
    SR_PROTOTYPE((int id,
                  SR_UINT32 when,
                  int absolute));

SR_UINT32 GetTimeoutInfo
    SR_PROTOTYPE((SR_UINT32 id,
                  SR_UINT32 *when,
                  SR_UINT32 *period,
                  void **info,
                  char *fired));

void AdjustTimeouts
    SR_PROTOTYPE((SR_INT32 delta));

int PostTimerHook
    SR_PROTOTYPE((TimerQueue *TmQ,
		  SR_UINT32 id,
                  void *info));

int CancelTimerHook
    SR_PROTOTYPE((TimerQueue *TmQ,
		  SR_UINT32 id,
                  int anymore));

void DestroyTimerQueue
    SR_PROTOTYPE((TimerQueue *TmQ));

TimerQueue *GetTimerQueue
    SR_PROTOTYPE((void));


int TmCanSetTimeout
  SR_PROTOTYPE((TimerQueue *TmQ));

void TmDontNeedTimeout
  SR_PROTOTYPE((TimerQueue *TmQ));

int TmSetPeriodicTimeout
  SR_PROTOTYPE((TimerQueue *TmQ,
		SR_UINT32   when,
		SR_UINT32   period,
		void           *info));

int TmSetOneshotTimeout
  SR_PROTOTYPE((TimerQueue *TmQ,
		SR_UINT32       when,
		void           *info));

int TmCancelTimeout
  SR_PROTOTYPE((TimerQueue *TmQ,
		int         id));

int TmDeferTimeout
  SR_PROTOTYPE((TimerQueue *TmQ,
		int         id,
		SR_UINT32   when,
		int         absolute));


SR_UINT32 TmGetTimeoutInfo
  SR_PROTOTYPE((TimerQueue *TmQ,
		SR_UINT32 id,
		SR_UINT32 *when,
		SR_UINT32 *period,
		void      **info,
		char      *fired));


void TmAdjustTimeouts
  SR_PROTOTYPE((TimerQueue *TmQ,
		SR_INT32 delta));

SR_UINT32 TimerCurTime
  SR_PROTOTYPE((void));

#ifdef  __cplusplus
}
#endif

#endif				/* SR_TMQ_H */
