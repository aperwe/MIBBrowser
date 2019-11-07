#ifndef SR_MIB_TABL_H
#define SR_MIB_TABL_H

#ifdef  __cplusplus
extern "C" {
#endif


#ifndef DO_NOT_DECLARE_MIB_OID_TABLE
extern struct MIB_OID *mib_oid_table;
extern char *unassigned_text;
extern int num_mot_entries;
#endif				/* DO_NOT_DECLARE_MIB_OID_TABLE */


void InitNameOIDHashArray SR_PROTOTYPE((void));

void InitNewOIDRoutines
    SR_PROTOTYPE((void));
#ifdef SR_MIB_TABLE_HASHING
void InitNameOIDHashArray SR_PROTOTYPE((void));
#endif /* SR_MIB_TABLE_HASHING */

#ifdef  __cplusplus
}
#endif

/* Do Not put anything after this #endif */
#endif	/* !SR_MIB_TABL_H */
