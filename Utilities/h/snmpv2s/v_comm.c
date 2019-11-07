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
 */

#if (! ( defined(lint) ) && defined(SR_RCSID))
static const char rcsid[] = "$Id: v_comm.c,v 1.5.4.2 1998/04/27 13:38:16 levi Exp $";
#endif	/* (! ( defined(lint) ) && defined(SR_RCSID)) */

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif  /* HAVE_MALLOC_H */

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif  /* HAVE_MEMORY_H */

#include <string.h>

#include "sr_type.h"
#include "sr_time.h"

#include "sr_snmp.h"
#include "sr_proto.h"
#include "comunity.h"
#include "v2clssc.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "makevb.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "mibout.h"
#include "inst_lib.h"
#include "tmq.h"
#include "rowstatf.h"
#include "i_comm.h"
#include "diag.h"
SR_FILENAME



/* note: the following prototypes may be included automatically now.
         check for duplicates  -cws   */
void srCommunityMIB_free
    SR_PROTOTYPE((srCommunityMIB_t  *data));

srCommunityEntry_t *Clone_srCommunityEntry
    SR_PROTOTYPE((srCommunityEntry_t *srCommunityEntry));


#ifdef SR_DISABLE_CACHE
#define srCommunityMIB_DISABLE_CACHE 1
#endif /* SR_DISABLE_CACHE */

/*---------------------------------------------------------------------
 * Retrieve data from the srCommunityMIB family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
srCommunityMIB_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;

#ifdef srCommunityMIB_DISABLE_CACHE
    srCommunityMIB_t  *data;
#else /* srCommunityMIB_DISABLE_CACHE */
    static srCommunityMIB_t  *data;
    static int             last_serialNum;
    static OID             *last_incoming = NULL;
    /* if the cache is not valid */
    if ( (serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
         (CmpOIDInst(incoming, last_incoming, (int)object->oid.length) != 0) ||
         (!VALID(object->nominator, data->valid)) ) {
     
#endif /* srCommunityMIB_DISABLE_CACHE */
    /*
     * Check the object instance.
     *
     * An EXACT search requires that the instance be of length 1 and
     * the single instance element be 0.
     *
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    switch (searchType) {
      case EXACT:
        if (instLength == 1 && incoming->oid_ptr[incoming->length - 1] == 0) {
          arg = object->nominator;
        }
        break;

      case NEXT:
        if (instLength <= 0) {
          arg = object->nominator;
        }
        break;

    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_srCommunityMIB_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

#ifndef srCommunityMIB_DISABLE_CACHE
     if (arg == -1) {
         data = NULL;
     }
     else {
         last_serialNum = serialNum;
         if (last_incoming != NULL) {
            FreeOID(last_incoming);
         }
         last_incoming = CloneOID(incoming);
     }
  }
  else {
     arg = object->nominator;
  }
#endif /* srCommunityMIB_DISABLE_CACHE */

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_srCommunitySpinLock
    case I_srCommunitySpinLock:
       dp = (void *) (&data->srCommunitySpinLock);
       break;
#endif /* I_srCommunitySpinLock */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the srCommunityMIB data object.
 *---------------------------------------------------------------------*/
void
srCommunityMIB_free(data)
   srCommunityMIB_t  *data;
{
   if (data != NULL) {
#ifdef U_srCommunityMIB
       /* free user-defined data */
       k_srCommunityMIBFreeUserpartData (data);
#endif /* U_srCommunityMIB */


       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after srCommunityMIB set/undo
 *---------------------------------------------------------------------*/
static int srCommunityMIB_cleanup SR_PROTOTYPE((doList_t *trash));

static int
srCommunityMIB_cleanup(trash)
   doList_t *trash;
{
   srCommunityMIB_free((srCommunityMIB_t *) trash->data);
   srCommunityMIB_free((srCommunityMIB_t *) trash->undodata);
   return NO_ERROR;
}

#ifdef SR_srCommunityMIB_UNDO
/*----------------------------------------------------------------------
 * clone the srCommunityMIB family
 *---------------------------------------------------------------------*/
srCommunityMIB_t *
Clone_srCommunityMIB(srCommunityMIB)
    srCommunityMIB_t *srCommunityMIB;
{
    srCommunityMIB_t *data;

    if ((data = (srCommunityMIB_t *) malloc (sizeof(srCommunityMIB_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (srCommunityMIB), sizeof(srCommunityMIB_t));


    return data;
}
#endif /* SR_srCommunityMIB_UNDO */

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 * This operation is performed in 4 steps:
 *
 *   1) Validate the object instance.
 *   2) Locate a "related" do-list element, or create a new do-list
 *      element.
 *   3) Add the SET request to the selected do-list element.
 *   4) Finally, determine if the do-list element (all "related" set
 *      requests) is completely valid.
 *---------------------------------------------------------------------*/
int 
srCommunityMIB_test(incoming, object, value, doHead, doCur, contextInfo)
    OID            *incoming;
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    int            instLength = incoming->length - object->oid.length;
    doList_t       *dp;
    int            found;
    int            error_status = NO_ERROR;
#ifdef SR_srCommunityMIB_UNDO
    srCommunityMIB_t     *srCommunityMIB;
#endif /* SR_srCommunityMIB_UNDO */

   /*
    * Validate the object instance: 1) It must be of length 1  2) and the
    * instance must be 0.
    */
    if (instLength != 1 || incoming->oid_ptr[incoming->length - 1] != 0) {
        return (NO_CREATION_ERROR); 
    }

#ifdef SR_srCommunityMIB_UNDO
    srCommunityMIB = k_srCommunityMIB_get(-1, contextInfo, -1);
#endif /* SR_srCommunityMIB_UNDO */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same family.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == srCommunityMIB_set) &&
            (((srCommunityMIB_t *) (dp->data)) != NULL) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;
        if ( (dp->data = (void *) malloc (sizeof(srCommunityMIB_t))) == NULL) { 
            DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
            return RESOURCE_UNAVAILABLE_ERROR;
        }
#ifdef SR_CLEAR_MALLOC
        memset(dp->data, 0, sizeof(srCommunityMIB_t));
#endif	/* SR_CLEAR_MALLOC */

#ifdef SR_srCommunityMIB_UNDO
        if ((dp->undodata = (void *) Clone_srCommunityMIB(srCommunityMIB)) == NULL) {
            DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
            error_status = RESOURCE_UNAVAILABLE_ERROR;
        }
#endif /* SR_srCommunityMIB_UNDO */

        dp->setMethod = srCommunityMIB_set;
        dp->cleanupMethod = srCommunityMIB_cleanup;
#ifdef SR_srCommunityMIB_UNDO
        dp->undoMethod = srCommunityMIB_undo;
#else /* SR_srCommunityMIB_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_srCommunityMIB_UNDO */
        dp->state = SR_UNKNOWN;

    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_srCommunitySpinLock
   case I_srCommunitySpinLock:

     if ( ((value->sl_value < 0) || (value->sl_value > 2147483647)) ) {
         return WRONG_VALUE_ERROR;
     }

     /* since this is a TestAndIncr variable, make sure that it is being set to
      * its current value, if one exists */
     if (dp->undodata != NULL) {
         /* compare against the cached value.  Use the undodata value,
          * in case this object is set in multiple varbinds. */
         if(value->sl_value != ((srCommunityMIB_t *) (dp->undodata))->srCommunitySpinLock) {
             /* value doesn't match, return an error */
             return INCONSISTENT_VALUE_ERROR;
         } else {
             /* update the value */
             if(value->sl_value != 2147483647) {
                 /* increment the value */
                 value->sl_value++;
             } else {
                 /* wrap the value */
                 value->sl_value = 0;
             }
         }
     }

     ((srCommunityMIB_t *) (dp->data))->srCommunitySpinLock = value->sl_value;
     break;
#endif /* I_srCommunitySpinLock */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in srCommunityMIB_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_srCommunityMIB_test */
   error_status = k_srCommunityMIB_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((srCommunityMIB_t *) (dp->data))->valid);
        error_status = k_srCommunityMIB_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
srCommunityMIB_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_srCommunityMIB_set((srCommunityMIB_t *) (doCur->data),
            contextInfo, (int)doCur->state));
}

#endif /* SETS */

#ifdef SR_DISABLE_CACHE
#define srCommunityEntry_DISABLE_CACHE 1
#endif /* SR_DISABLE_CACHE */

/*---------------------------------------------------------------------
 * Retrieve data from the srCommunityEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
srCommunityEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void            *dp;
    int             carry;
    int             i;
    OctetString *   srCommunityAuthSnmpID = NULL;
    int             srCommunityAuthSnmpID_offset;
    OctetString *   srCommunityName = NULL;
    int             srCommunityName_offset;
    int             index;
    int             final_index;

#ifdef srCommunityEntry_DISABLE_CACHE
    srCommunityEntry_t *data;
    SR_UINT32   buffer[MAX_OID_SIZE];
    OID             inst;
#else /* srCommunityEntry_DISABLE_CACHE */
    static srCommunityEntry_t *data;
    static SR_UINT32   buffer[MAX_OID_SIZE];
    static OID             inst;
    static int             last_serialNum;
    static OID             *last_incoming;
    /* if the cache is not valid */
    if ( (serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
         (CmpOIDInst(incoming, last_incoming, (int)object->oid.length) != 0) ||
         (!NEXT_ROW(data->valid)) ||
         (!VALID(object->nominator, data->valid)) ) {
     
#endif /* srCommunityEntry_DISABLE_CACHE */
    srCommunityAuthSnmpID_offset = object->oid.length;
    srCommunityName_offset = srCommunityAuthSnmpID_offset + GetVariableIndexLength(incoming, srCommunityAuthSnmpID_offset);
    final_index = srCommunityName_offset + GetImpliedIndexLength(incoming, srCommunityName_offset);

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToImpliedOctetString(incoming, srCommunityName_offset, &srCommunityName, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToVariableOctetString(incoming, srCommunityAuthSnmpID_offset, &srCommunityAuthSnmpID, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_srCommunityEntry_get(serialNum, contextInfo, arg ,searchType, srCommunityAuthSnmpID, srCommunityName)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = data->srCommunityAuthSnmpID->length;
        for(i = 0; i < data->srCommunityAuthSnmpID->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->srCommunityAuthSnmpID->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        for(i = 0; i < data->srCommunityName->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->srCommunityName->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.length = index;
     }

        FreeOctetString(srCommunityAuthSnmpID);
        FreeOctetString(srCommunityName);

#ifndef srCommunityEntry_DISABLE_CACHE
     if (arg == -1) {
         data = NULL;
     }
     else {
         last_serialNum = serialNum;
         if (last_incoming != NULL) {
            FreeOID(last_incoming);
         }
         last_incoming = CloneOID(incoming);
     }
  }
#endif /* srCommunityEntry_DISABLE_CACHE */

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_srCommunityAuthSnmpID
    case I_srCommunityAuthSnmpID:
       dp = (void *) (CloneOctetString(data->srCommunityAuthSnmpID));
       break;
#endif /* I_srCommunityAuthSnmpID */

#ifdef I_srCommunityGroupName
    case I_srCommunityGroupName:
       dp = (void *) (CloneOctetString(data->srCommunityGroupName));
       break;
#endif /* I_srCommunityGroupName */

#ifdef I_srCommunityContextSnmpID
    case I_srCommunityContextSnmpID:
       dp = (void *) (CloneOctetString(data->srCommunityContextSnmpID));
       break;
#endif /* I_srCommunityContextSnmpID */

#ifdef I_srCommunityContextName
    case I_srCommunityContextName:
       dp = (void *) (CloneOctetString(data->srCommunityContextName));
       break;
#endif /* I_srCommunityContextName */

#ifdef I_srCommunityTransportLabel
    case I_srCommunityTransportLabel:
       dp = (void *) (CloneOctetString(data->srCommunityTransportLabel));
       break;
#endif /* I_srCommunityTransportLabel */

#ifdef I_srCommunityMemoryType
    case I_srCommunityMemoryType:
       dp = (void *) (&data->srCommunityMemoryType);
       break;
#endif /* I_srCommunityMemoryType */

#ifdef I_srCommunityStatus
    case I_srCommunityStatus:
       dp = (void *) (&data->srCommunityStatus);
       break;
#endif /* I_srCommunityStatus */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * cleanup after srCommunityEntry set/undo
 *---------------------------------------------------------------------*/
static int srCommunityEntry_cleanup SR_PROTOTYPE((doList_t *trash));

static int
srCommunityEntry_cleanup(trash)
   doList_t *trash;
{
    FreeEntries(srCommunityEntryTypeTable, trash->data);
    free(trash->data);
    FreeEntries(srCommunityEntryTypeTable, trash->undodata);
    if (trash->undodata != NULL) {
        free(trash->undodata);
    }
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the srCommunityEntry family
 *---------------------------------------------------------------------*/
srCommunityEntry_t *
Clone_srCommunityEntry(srCommunityEntry)
    srCommunityEntry_t *srCommunityEntry;
{
    srCommunityEntry_t *data;

    if ((data = (srCommunityEntry_t *) malloc (sizeof(srCommunityEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (srCommunityEntry), sizeof(srCommunityEntry_t));

    data->srCommunityAuthSnmpID = CloneOctetString(srCommunityEntry->srCommunityAuthSnmpID);
    data->srCommunityName = CloneOctetString(srCommunityEntry->srCommunityName);
    data->srCommunityGroupName = CloneOctetString(srCommunityEntry->srCommunityGroupName);
    data->srCommunityContextSnmpID = CloneOctetString(srCommunityEntry->srCommunityContextSnmpID);
    data->srCommunityContextName = CloneOctetString(srCommunityEntry->srCommunityContextName);
    data->srCommunityTransportLabel = CloneOctetString(srCommunityEntry->srCommunityTransportLabel);
#ifdef U_srCommunityEntry
    k_srCommunityEntryCloneUserpartData(data, srCommunityEntry);
#endif /* U_srCommunityEntry */

    return data;
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 * This operation is performed in 4 steps:
 *
 *   1) Validate the object instance.
 *   2) Locate a "related" do-list element, or create a new do-list
 *      element.
 *   3) Add the SET request to the selected do-list element.
 *   4) Finally, determine if the do-list element (all "related" set
 *      requests) is completely valid.
 *---------------------------------------------------------------------*/
int 
srCommunityEntry_test(incoming, object, value, doHead, doCur, contextInfo)
    OID            *incoming;
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    doList_t       *dp;
    int            found;
    int            error_status = NO_ERROR;
    int            carry = 0;
    srCommunityEntry_t     *srCommunityEntry;
    OctetString *  srCommunityAuthSnmpID = NULL;
    int            srCommunityAuthSnmpID_offset;
    OctetString *  srCommunityName = NULL;
    int            srCommunityName_offset;
    int            final_index;

   /*
    * Validate the object instance
    *
    */
    srCommunityAuthSnmpID_offset = object->oid.length;
    srCommunityName_offset = srCommunityAuthSnmpID_offset + GetVariableIndexLength(incoming, srCommunityAuthSnmpID_offset);
    final_index = srCommunityName_offset + GetImpliedIndexLength(incoming, srCommunityName_offset);

    if (final_index != incoming->length) {
          return(NO_CREATION_ERROR);
    }

    if ( (InstToVariableOctetString(incoming, srCommunityAuthSnmpID_offset, &srCommunityAuthSnmpID, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToImpliedOctetString(incoming, srCommunityName_offset, &srCommunityName, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        FreeOctetString(srCommunityAuthSnmpID);
        FreeOctetString(srCommunityName);
        return error_status;
    }

    srCommunityEntry = k_srCommunityEntry_get(-1, contextInfo, -1, EXACT, srCommunityAuthSnmpID, srCommunityName);
    if (srCommunityEntry != NULL) {
        if (srCommunityEntry->srCommunityMemoryType == D_srCommunityMemoryType_readOnly) {
        FreeOctetString(srCommunityAuthSnmpID);
        FreeOctetString(srCommunityName);
            return NOT_WRITABLE_ERROR;
        }
    }

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == srCommunityEntry_set) &&
            (((srCommunityEntry_t *) (dp->data)) != NULL) &&
            (CmpOctetStrings(((srCommunityEntry_t *) (dp->data))->srCommunityAuthSnmpID, srCommunityAuthSnmpID) == 0) &&
            (CmpOctetStrings(((srCommunityEntry_t *) (dp->data))->srCommunityName, srCommunityName) == 0) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = srCommunityEntry_set;
        dp->cleanupMethod = srCommunityEntry_cleanup;
#ifdef SR_srCommunityEntry_UNDO
        dp->undoMethod = srCommunityEntry_undo;
#else /* SR_srCommunityEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_srCommunityEntry_UNDO */
        dp->state = SR_UNKNOWN;

        if (srCommunityEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_srCommunityEntry(srCommunityEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            if ((dp->undodata = (void *) Clone_srCommunityEntry(srCommunityEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }

        }
        else {
            if ( (dp->data = (void *) malloc (sizeof(srCommunityEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
#ifdef SR_CLEAR_MALLOC
                memset(dp->data, 0, sizeof(srCommunityEntry_t));
#endif	/* SR_CLEAR_MALLOC */
                dp->undodata = NULL;

                /* Fill in reasonable default values for this new entry */
                ((srCommunityEntry_t *) (dp->data))->srCommunityAuthSnmpID = CloneOctetString(srCommunityAuthSnmpID);
                SET_VALID(I_srCommunityAuthSnmpID, ((srCommunityEntry_t *) (dp->data))->valid);

                ((srCommunityEntry_t *) (dp->data))->srCommunityName = CloneOctetString(srCommunityName);
                SET_VALID(I_srCommunityName, ((srCommunityEntry_t *) (dp->data))->valid);

                error_status = k_srCommunityEntry_set_defaults(dp);
            }
        }
    }

        FreeOctetString(srCommunityAuthSnmpID);
        FreeOctetString(srCommunityName);
    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_srCommunityAuthSnmpID
   case I_srCommunityAuthSnmpID:

     if (srCommunityAuthSnmpID->length != value->os_value->length) {
         return WRONG_VALUE_ERROR;
      }
     if (memcmp(srCommunityAuthSnmpID->octet_ptr, value->os_value->octet_ptr,  value->os_value->length) != 0) {
         return WRONG_VALUE_ERROR;
     }
     
     if (((srCommunityEntry_t *) (dp->data))->srCommunityAuthSnmpID != NULL) {
        FreeOctetString(((srCommunityEntry_t *) (dp->data))->srCommunityAuthSnmpID);
     }

     ((srCommunityEntry_t *) (dp->data))->srCommunityAuthSnmpID = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_srCommunityAuthSnmpID */

#ifdef I_srCommunityGroupName
   case I_srCommunityGroupName:

     if (((srCommunityEntry_t *) (dp->data))->srCommunityGroupName != NULL) {
        FreeOctetString(((srCommunityEntry_t *) (dp->data))->srCommunityGroupName);
     }

     ((srCommunityEntry_t *) (dp->data))->srCommunityGroupName = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_srCommunityGroupName */

#ifdef I_srCommunityContextSnmpID
   case I_srCommunityContextSnmpID:

     if (((srCommunityEntry_t *) (dp->data))->srCommunityContextSnmpID != NULL) {
        FreeOctetString(((srCommunityEntry_t *) (dp->data))->srCommunityContextSnmpID);
     }

     ((srCommunityEntry_t *) (dp->data))->srCommunityContextSnmpID = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_srCommunityContextSnmpID */

#ifdef I_srCommunityContextName
   case I_srCommunityContextName:

     if (((srCommunityEntry_t *) (dp->data))->srCommunityContextName != NULL) {
        FreeOctetString(((srCommunityEntry_t *) (dp->data))->srCommunityContextName);
     }

     ((srCommunityEntry_t *) (dp->data))->srCommunityContextName = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_srCommunityContextName */

#ifdef I_srCommunityTransportLabel
   case I_srCommunityTransportLabel:

     if (((srCommunityEntry_t *) (dp->data))->srCommunityTransportLabel != NULL) {
        FreeOctetString(((srCommunityEntry_t *) (dp->data))->srCommunityTransportLabel);
     }

     ((srCommunityEntry_t *) (dp->data))->srCommunityTransportLabel = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_srCommunityTransportLabel */

#ifdef I_srCommunityMemoryType
   case I_srCommunityMemoryType:

     if ((value->sl_value < 1) || (value->sl_value > 5)) {
         return WRONG_VALUE_ERROR;
     }

     if (srCommunityEntry != NULL) {
         /* check for attempts to change 'permanent' to other value */
         if ((srCommunityEntry->srCommunityMemoryType == D_srCommunityMemoryType_permanent) &&
             (value->sl_value != D_srCommunityMemoryType_permanent)) {
             /* permanent storageType cannot be changed */
             return INCONSISTENT_VALUE_ERROR;
         }
         /* check for attempts to change 'permanent' to other value */
         if ((srCommunityEntry->srCommunityMemoryType < D_srCommunityMemoryType_permanent) &&
             (value->sl_value >= D_srCommunityMemoryType_permanent)) {
             /* permanent storageType cannot be changed */
             return INCONSISTENT_VALUE_ERROR;
         }
     }
     ((srCommunityEntry_t *) (dp->data))->srCommunityMemoryType = value->sl_value;
     break;
#endif /* I_srCommunityMemoryType */

#ifdef I_srCommunityStatus
   case I_srCommunityStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     error_status = CheckRowStatus(value->sl_value,
         srCommunityEntry == NULL ? 0L : srCommunityEntry->srCommunityStatus);
     if (error_status != 0) return(error_status);

     /* check for attempts to delete 'permanent' rows */
     if(srCommunityEntry != NULL
     && srCommunityEntry->srCommunityMemoryType == D_srCommunityMemoryType_permanent
     && value->sl_value == D_srCommunityStatus_destroy) {
         /* permanent rows can be changed but not deleted */
         return INCONSISTENT_VALUE_ERROR;
     }

     ((srCommunityEntry_t *) (dp->data))->srCommunityStatus = value->sl_value;
     break;
#endif /* I_srCommunityStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in srCommunityEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_srCommunityEntry_test */
   error_status = k_srCommunityEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((srCommunityEntry_t *) (dp->data))->valid);
        error_status = k_srCommunityEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
srCommunityEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_srCommunityEntry_set((srCommunityEntry_t *) (doCur->data),
            contextInfo, (int)doCur->state));
}

#endif /* SETS */


