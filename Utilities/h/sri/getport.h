
#ifndef SR_SRI_GETPORT_H
#define SR_SRI_GETPORT_H

#ifdef  __cplusplus
extern "C" {
#endif

int GetSNMPPort
    SR_PROTOTYPE((void));

int GetSNMPTrapPort
    SR_PROTOTYPE((void));

int SetUpEnviron
    SR_PROTOTYPE((int argc,
                  char **argv,
                  char **envp));

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif				/* SR_SRI_GETPORT_H */
