#ifndef SR_SRI_GETTIME_H
#define SR_SRI_GETTIME_H


#ifdef  __cplusplus
extern "C" {
#endif

/*
 *  Data structure returned by GetTOD and PrintTOD
 */

typedef struct srTOD {
    SR_UINT32 secs;		/* Full seconds */
    SR_UINT32 nsecs;		/* Fractional (nano) seconds */
} SrTOD;


SR_UINT32 GetTimeNow
    SR_PROTOTYPE((void));

SR_UINT32 GetSubagentTimeNow(void);

void AdjustTimeNow
    SR_PROTOTYPE((SR_UINT32 target));

OctetString *MakeDateAndTime
    SR_PROTOTYPE((const struct tm *tm));

int GetTOD(SrTOD *tod);

char *PrintTOD(const SrTOD *tod, char *buf, const int buflen);

void sysUpToTOD(const SR_UINT32 uptime, SrTOD *tod);


#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif				/* SR_SRI_GETTIME_H */
