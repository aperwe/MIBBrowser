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
static const char rcsid[] = "$Id: gettime.c,v 1.35.4.2 1998/04/16 20:32:11 reid Exp $";
#endif	/* (! ( defined(lint) ) && defined(SR_RCSID)) */

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>	/* for malloc */

#include <string.h>

#include <sys/types.h>



#include "sr_type.h"
#ifdef SR_WINDOWS
#include <windows.h>
#endif /* SR_WINDOWS */


#include "sr_time.h"

#include "sr_snmp.h"



#ifdef SR_WINDOWS
#include <windows.h>
#endif /* SR_WINDOWS */


#include "tmq.h" /* For TvpToTimeOfDay() prototype */

/*
 * basetime will be measured in 100ths of seconds.
 * Don't export it, since it has no non-local meaning anyway
 * (it wraps every 1.36 years).
 */
static SR_UINT32 basetime;
/*
 * agtbasetime will be measured in 100ths of seconds.
 * Don't export it, since it has no non-local meaning anyway
 * (it wraps every 1.36 years).  It is used in AgtGetTimeNow()
 * which gets the amount of time the agent has been up.
 * This is useful where an invariant time base is needed.
 */
static SR_UINT32 agtbasetime;
/*
 * bootTOD is used to keep track of when this entity was
 * booted.  This is necessary when one needs to print
 * a date/time based on sysUptime (as in the log book functions);
 */
static SrTOD bootTOD;
/*
 * maBootTimeOffset is used by subagents to compute date/time
 * based on a sysUptime (which is based on the master agent boot time.
 * This will always be zero on the authoritive (master/monolithic) program.
 */
static SR_UINT32 maBootTimeOffset = 0;

#undef SR_USE_GETTIMEOFDAY
#undef SR_USE_CTICKS
#undef SR_USE_FTIME
#undef SR_USE_TICKGET
#undef SR_USE_TM_GET

#ifdef HAVE_GETTIMEOFDAY
#define SR_USE_GETTIMEOFDAY
#else /* HAVE_GETTIMEOFDAY */

#ifdef HAVE_FTIME
#define SR_USE_FTIME
#endif       /* HAVE_FTIME */





#endif	/* HAVE_GETTIMEOFDAY */
/*
 *  Local prototypes & statics
 */
static SR_UINT32 GetAbsoluteTime(void);
static int dayspermonth[] = { -1,31,-1,31,30,31,30,31,31,30,31,30,31 };



#ifdef SR_WINCE
static SR_UINT32
GetAbsoluteTime()
{
    return (SR_UINT32) (GetTickCount() / 100);
}
#endif /* SR_WINCE */

SR_UINT32
GetTimeNow()
{
    return (GetAbsoluteTime()) - basetime;
}

/*
 *  Return amount of time agent has been up.  This is distinct
 *  from GetTimeNow (which returns sysUptime), for applications
 *  (such as host resources) that need a invariant timing base.
 */

SR_UINT32
GetSubagentTimeNow(void)
{
    return GetAbsoluteTime() - agtbasetime;
}


void
AdjustTimeNow(target)
    SR_UINT32 target;
{
    SR_UINT32 current;
    SR_UINT32 oldbasetime = basetime;

    current = GetTimeNow();
    basetime -= target - current;
/*
 *  keep offset for sysUpToTOD routine
 */
    maBootTimeOffset += basetime - oldbasetime;
    return;
}


#ifdef SR_USE_GETTIMEOFDAY
SR_UINT32
TvpToTimeOfDay(tp)
    const struct timeval *tp;
{
    return (((SR_UINT32) (tp->tv_sec)) * 100 +
	    ((SR_UINT32) (tp->tv_usec / 10000))) - basetime;
}
#endif /* SR_USE_GETTIMEOFDAY */

/* ----------------------------------------------------------------------
 *  leapyear(y) - return 1 if y is a leapyear, else 0.
 *  moved out of psos ifdefs, as later code now uses it.
 */
static int 
leapyear(int y) 
{
    if((((y%4) == 0) && !((y % 100) == 0)) ||
       ((y % 400) == 0)) {
        return 1;
    }
    return 0;
}


#ifndef SR_WINCE
OctetString *
MakeDateAndTime(tm)
    const struct tm *tm;
{
    unsigned char buff[8];

    if (tm == NULL) {
        return NULL;
    } else {
        buff[0] = (char) ((tm->tm_year + 1900) >> 8);
        buff[1] = (char) ((tm->tm_year + 1900));
        buff[2] = (char) tm->tm_mon + 1;
        buff[3] = (char) tm->tm_mday;
        buff[4] = (char) tm->tm_hour;
        buff[5] = (char) tm->tm_min;
        buff[6] = (char) tm->tm_sec;
        buff[7] = 0;
    }
    return MakeOctetString(buff, 8);
}
#endif /* SR_WINCE */

/* ----------------------------------------------------------------------
 * 
 *  GetTOD - Get current time of day.
 *
 *  Arguments:
 *  i  SrTOD *tod
 *     Ptr to SrTOD structure.  The current time of day is filled in here.
 *     Note that if the current tod is not available, 
 *         tod->secs = 0
 *         tod->nsecs = sysUptime
 *     This is intended for use on systems where there is no time of
 *     day concept.
 *  o  int (function value)
 *     0: all ok.
 *     -1: some system call error.   Check errno.
 *     -2: not implemented or implementable on this OS.
 *
 *  Commentary:
 *     GetAbsoluteTime call kept for use, as it is used in GetTimeNow()
 *     which is --very-- heavily called.  This avoids an additional
 *     scaling operation to preserve data that is lost anyway 
 *     (most of the fractional digits are truncated in GetTimeNow).
 *     
 */
int 
GetTOD(SrTOD *tod) 
{

#ifdef SR_USE_FTIME
    struct _timeb t;

    _ftime(&t);
    tod->secs = t.time;
    tod->nsecs = t.millitm * (1000 * 1000);
#endif /* SR_USE_FTIME */

#ifdef SR_USE_CTICKS
/*
 *  Note careful operation order in nsecs computation
 */
    tod->secs = cticks / TPS;
    tod->nsecs = (cticks % TPS) * ((SR_INT32) 1000000000 / TPS);
#endif /* SR_USE_CTICKS */

#ifdef SR_USE_TICKGET
    SR_UINT32       ticks;
    SR_UINT32       clk_rate;

    ticks = (SR_UINT32) tickGet();
    clk_rate = (SR_UINT32) sysClkRateGet();
    tod->secs = ticks / clk_rate;
    tod->nsecs = (ticks % clk_rate) * ((SR_INT32) 1000000000 / clk_rate);
#endif /* SR_USE_TICKGET */

#ifdef SR_USE_TM_GET
    unsigned long date, time, ticks;

    tm_get(&date, &time, &ticks);

    ticks = (ticks * 100) / KC_TICKS2SEC;

    tod->secs = ConvertToCalendarTime(date >> 16, (date & 0xff00) >> 8,
                           date & 0xff,
                           time >> 16, (time & 0xff00) >> 8, time & 0xff);
    tod->secs += ticks / KC_TICKS2SEC;
    tod->nsecs = ((ticks % KC_TICKS2SEC) * 
        ((SR_INT32) 1000000000) / KC_TICKS2SEC);
#endif /* SR_USE_TM_GET */

#ifdef SR_USE_GETTIMEOFDAY
    struct timeval  tv;
    struct timezone tz;

    if (gettimeofday(&tv, &tz) == -1) {
        return -1;
    }
    tod->secs = tv.tv_sec;
    tod->nsecs = tv.tv_usec * 1000;
#endif /* SR_USE_GETTIMEOFDAY */

#ifdef SR_USE_TIMER_ENVIRON
    time_t secs;
    unsigned int ticks;
    int ticks_per_sec;

    _os_getime(&secs, &ticks);
    ticks_per_sec = ticks >> 16;
    ticks &= 0xFFFF;

    tod->secs = secs + ticks / ticks_per_sec;
    tod->nsecs = (ticks % ticks_per_sec ) * 
        ((SR_INT32) 1000000000) / tics_per_sec);
#endif /* SR_USE_TIMER_ENVIRON */
    return 0;
}

/* ----------------------------------------------------------------------
 * 
 *  PrintTOD - Print current time of day.
 *
 *  Arguments:
 *  i  const SrTOD *tod
 *     Ptr to SrTOD structure. (include/sri/gettime.h)
 *     .  if time->secs == 0, it is assumed that time->nsecs has a 
 *        centiseconds, and is printed as ddd hh:mm:ss.ff
 *     .  if time == NULL, the current time is assumed.
 *     .  The time format is yyyy/mm/dd hh:mm:ss.ff
 *  i  char *buf
 *     Buffer in which to store time representation.  Must be at
 *     least 23 bytes long.  If null, a buffer is malloced, which
 *     the caller must then mfree.
 *  i  const int buflen
 *     Length of buf.
 *  o  char * (function value)
 *     Pointer to either buf or buffer malloced.  
 *     If NULL, malloc  failed, or buflen too small.
 */
char *
PrintTOD(const SrTOD *tod, char *buf, const int buflen)
{
    SrTOD curtod;		/* if tod argument NULL */
    long f;			/* fractional seconds */
    unsigned long s;		/* seconds */
    long m;			/* minutes */
    long h;			/* hours */
    long d;			/* day */
    long mo;			/* month */
    long y;			/* year */

    if (tod == NULL) {
        GetTOD(&curtod);
        tod = &curtod;
    }
/*
 *  If tod->secs == 0, then tod->nsecs represents a sysuptime.
 */
    if (tod->secs == 0) {
       s = tod->nsecs / 100;
       f = tod->nsecs % 100;
    }  else {
       s = tod->secs;
       f = tod->nsecs / (1000 * 1000 * 10);		/* get centiseconds */
    }
/*
 *  Verify buffer size
 */
    if (buf != NULL) {
       if ((tod->secs != 0 && buflen < 24) || buflen < 16) {
          return NULL;
       }
    } else if (buf == NULL) {
        buf = (char *) malloc(24);
        if (buf == NULL) {
            return NULL;
        }
    }
/*
 *  Split off fields.
 */
    m = s / 60;
    s %= 60;
    h = m / 60;
    m %= 60;
    d = h / 24;
    h %= 24;
/*
 *  If this is a sysuptime, print ddd hh:mm:ss.ff
 */
    if (tod->secs == 0) {
        sprintf(buf, "%3ld %2.2ld:%2.2ld:%2.2ld.%2.2ld", d, h, m, s, f);
        return buf;
    }
/*
 *  Otherwise, figure out the rest of the fields.  Note that we
 *  one to the day count, since Midnight Jan 1, 1970, is day zero.
 */
    else {
        int isleap = 0, dpm = 0;
        d++;
        for (y = 1970; d > 0; y++) {
            isleap = leapyear(y);
            d -= (365 + isleap);
        }
        d += (365 + isleap);
        y--;

        for (mo = 1; d > 0 && mo < 12; mo++) {
           if (mo == 2) {
               dpm = 28 + isleap;
           } else {
               dpm = dayspermonth[mo];
           }
           d -= dpm;
        }
        if (d <= 0) {
           mo--;
           d += dpm;
        }
        sprintf(buf, "%4ld/%2.2ld/%2.2ld %2.2ld:%2.2ld:%2.2ld.%2.2ld", 
            y, mo, d, h, m, s, f);
        return buf;
    }
}


/* ----------------------------------------------------------------------
 * 
 *  sysUpToTOD - Get time of day from sysUptime.
 *  This routine is cogniscent of altered basetimes (due to 
 *  sysUptime synchronization with master agent) so it can be
 *  called from master- or sub-agent context.
 *
 *  Arguments:
 *  i const SR_UINT32 uptime;
 *    sysUptime (from GetTimeNow()) or master agent).
 *  io SrTOD *tod
 *    Time of day returned here
 */
void 
sysUpToTOD(const SR_UINT32 uptime, SrTOD *tod)
{
    SR_UINT32 adjbase;

    adjbase = uptime + maBootTimeOffset;
    
    tod->nsecs = bootTOD.nsecs + (adjbase % 100) * (1000 * 1000 * 10);
    tod->secs = bootTOD.secs + adjbase / 100;
    if (tod->nsecs >= 1000 * 1000 * 1000) {
       tod->secs += tod->nsecs / (1000 * 1000 * 1000);
       tod->nsecs = tod->nsecs % (1000 * 1000 * 1000);
    }
    return;
}
