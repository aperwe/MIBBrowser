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
static const char rcsid[] = "$Id: v_notify.c,v 1.4.4.2 1998/04/27 13:38:19 levi Exp $";
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
#include "tmq.h"
#include "rowstatf.h"
#include "i_notify.h"
#include "inst_lib.h"
#include "oid_lib.h"
#include "diag.h"
SR_FILENAME

/* note: the following prototypes may be included automatically now.
         check for duplicates  -cws   */
void v2AdminNotify_free
    SR_PROTOTYPE((v2AdminNotify_t *data));

notifyEntry_t *Clone_notifyEntry
    SR_PROTOTYPE((notifyEntry_t *notifyEntry));

#ifdef SR_DISABLE_CACHE
#define v2AdminNotify_DISABLE_CACHE 1
#endif /* SR_DISABLE_CACHE */

/*---------------------------------------------------------------------
 * Retrieve data from the v2AdminNotify family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
v2AdminNotify_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;

#ifdef v2AdminNotify_DISABLE_CACHE
    v2AdminNotify_t *data;
#else /* v2AdminNotify_DISABLE_CACHE */
    static v2AdminNotify_t *data;
    static int             last_serialNum;
    static OID             *last_incoming;
    /* if the cache is not valid */
    if ( (serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
         (CmpOIDInst(incoming, last_incoming, (int)object->oid.length) != 0) ||
         (!VALID(object->nominator, data->valid)) ) {
     
#endif /* v2AdminNotify_DISABLE_CACHE */
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
     if ( (arg == -1) || (data = k_v2AdminNotify_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

#ifndef v2AdminNotify_DISABLE_CACHE
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
#endif /* v2AdminNotify_DISABLE_CACHE */

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_notifySpinLock
    case I_notifySpinLock:
       dp = (void *) (&data->notifySpinLock);
       break;
#endif /* I_notifySpinLock */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the v2AdminNotify data object.
 *---------------------------------------------------------------------*/
void
v2AdminNotify_free(data)
   v2AdminNotify_t *data;
{
   if (data != NULL) {
#ifdef U_v2AdminNotify
       /* free user-defined data */
       k_v2AdminNotifyFreeUserpartData (data);
#endif /* U_v2AdminNotify */


       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after v2AdminNotify set/undo
 *---------------------------------------------------------------------*/
static int v2AdminNotify_cleanup SR_PROTOTYPE((doList_t *trash));

static int
v2AdminNotify_cleanup(trash)
   doList_t *trash;
{
   v2AdminNotify_free((v2AdminNotify_t *) trash->data);
   v2AdminNotify_free((v2AdminNotify_t *) trash->undodata);
   return NO_ERROR;
}

#ifdef SR_v2AdminNotify_UNDO
/*----------------------------------------------------------------------
 * clone the v2AdminNotify family
 *---------------------------------------------------------------------*/
v2AdminNotify_t *
Clone_v2AdminNotify(v2AdminNotify)
    v2AdminNotify_t *v2AdminNotify;
{
    v2AdminNotify_t *data;

    if ((data = (v2AdminNotify_t *) malloc (sizeof(v2AdminNotify_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (v2AdminNotify), sizeof(v2AdminNotify_t));


    return data;
}
#endif /* SR_v2AdminNotify_UNDO */

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
v2AdminNotify_test(incoming, object, value, doHead, doCur, contextInfo)
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
#ifdef SR_v2AdminNotify_UNDO
    v2AdminNotify_t     *v2AdminNotify;
#endif /* SR_v2AdminNotify_UNDO */

   /*
    * Validate the object instance: 1) It must be of length 1  2) and the
    * instance must be 0.
    */
    if (instLength != 1 || incoming->oid_ptr[incoming->length - 1] != 0) {
        return (NO_CREATION_ERROR); 
    }

#ifdef SR_v2AdminNotify_UNDO
    v2AdminNotify = k_v2AdminNotify_get(-1, contextInfo, -1);
#endif /* SR_v2AdminNotify_UNDO */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same family.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == v2AdminNotify_set) &&
            (((v2AdminNotify_t *) (dp->data)) != NULL) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;
        if ( (dp->data = (void *) malloc (sizeof(v2AdminNotify_t))) == NULL) { 
            DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
            return RESOURCE_UNAVAILABLE_ERROR;
        }
#ifdef SR_CLEAR_MALLOC
        memset(dp->data, 0, sizeof(v2AdminNotify_t));
#endif	/* SR_CLEAR_MALLOC */

#ifdef SR_v2AdminNotify_UNDO
        if ((dp->undodata = (void *) Clone_v2AdminNotify(v2AdminNotify)) == NULL) {
            DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
            error_status = RESOURCE_UNAVAILABLE_ERROR;
        }
#endif /* SR_v2AdminNotify_UNDO */

        dp->setMethod = v2AdminNotify_set;
        dp->cleanupMethod = v2AdminNotify_cleanup;
#ifdef SR_v2AdminNotify_UNDO
        dp->undoMethod = v2AdminNotify_undo;
#else /* SR_v2AdminNotify_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_v2AdminNotify_UNDO */
        dp->state = SR_UNKNOWN;

    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_notifySpinLock
   case I_notifySpinLock:

     if ( ((value->sl_value < 0) || (value->sl_value > 2147483647)) ) {
         return WRONG_VALUE_ERROR;
     }

     /* since this is a TestAndIncr variable, make sure that it is being set to
      * its current value, if one exists */
     if (dp->undodata != NULL) {
         /* compare against the cached value.  Use the undodata value,
          * in case this object is set in multiple varbinds. */
         if(value->sl_value != ((v2AdminNotify_t *) (dp->undodata))->notifySpinLock) {
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

     ((v2AdminNotify_t *) (dp->data))->notifySpinLock = value->sl_value;
     break;
#endif /* I_notifySpinLock */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in v2AdminNotify_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_v2AdminNotify_test */
   error_status = k_v2AdminNotify_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((v2AdminNotify_t *) (dp->data))->valid);
        error_status = k_v2AdminNotify_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
v2AdminNotify_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_v2AdminNotify_set((v2AdminNotify_t *) (doCur->data),
            contextInfo, (int)doCur->state));
}

#endif /* SETS */


#ifdef SR_DISABLE_CACHE
#define notifyEntry_DISABLE_CACHE 1
#endif /* SR_DISABLE_CACHE */

/*---------------------------------------------------------------------
 * Retrieve data from the notifyEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
notifyEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    int             carry;
    SR_INT32        notifyIndex;

#ifdef notifyEntry_DISABLE_CACHE
    notifyEntry_t   *data;
    SR_UINT32   buffer[1];
    OID             inst;
#else /* notifyEntry_DISABLE_CACHE */
    static notifyEntry_t   *data;
    static SR_UINT32   buffer[1];
    static OID             inst;
    static int             last_serialNum;
    static OID             *last_incoming;
    /* if the cache is not valid */
    if ( (serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
         (CmpOIDInst(incoming, last_incoming, (int)object->oid.length) != 0) ||
         (!NEXT_ROW(data->valid)) ||
         (!VALID(object->nominator, data->valid)) ) {
     
#endif /* notifyEntry_DISABLE_CACHE */
    /*
     * Check the object instance.
     *
     * An EXACT search requires that the instance be of length 1 
     *
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
        if (instLength != 1) {
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToInt(incoming, (int)(0 + object->oid.length), &notifyIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        notifyIndex = MAX(0, notifyIndex);
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_notifyEntry_get(serialNum, contextInfo, arg ,searchType, notifyIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->notifyIndex;
     }


#ifndef notifyEntry_DISABLE_CACHE
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
#endif /* notifyEntry_DISABLE_CACHE */

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_notifySPI
    case I_notifySPI:
       dp = (void *) (&data->notifySPI);
       break;
#endif /* I_notifySPI */

#ifdef I_notifyIdentityName
    case I_notifyIdentityName:
       dp = (void *) (CloneOctetString(data->notifyIdentityName));
       break;
#endif /* I_notifyIdentityName */

#ifdef I_notifyTransportLabel
    case I_notifyTransportLabel:
       dp = (void *) (CloneOctetString(data->notifyTransportLabel));
       break;
#endif /* I_notifyTransportLabel */

#ifdef I_notifyContextName
    case I_notifyContextName:
       dp = (void *) (CloneOctetString(data->notifyContextName));
       break;
#endif /* I_notifyContextName */

#ifdef I_notifyViewName
    case I_notifyViewName:
       dp = (void *) (CloneOctetString(data->notifyViewName));
       break;
#endif /* I_notifyViewName */

#ifdef I_notifyMemoryType
    case I_notifyMemoryType:
       dp = (void *) (&data->notifyMemoryType);
       break;
#endif /* I_notifyMemoryType */

#ifdef I_notifyStatus
    case I_notifyStatus:
       dp = (void *) (&data->notifyStatus);
       break;
#endif /* I_notifyStatus */

#ifdef I_notifyConfirm
    case I_notifyConfirm:
       dp = (void *) (&data->notifyConfirm);
       break;
#endif /* I_notifyConfirm */

#ifdef I_notifyAuthSnmpID
    case I_notifyAuthSnmpID:
       dp = (void *) (CloneOctetString(data->notifyAuthSnmpID));
       break;
#endif /* I_notifyAuthSnmpID */

#ifdef I_notifyTimeout
    case I_notifyTimeout:
       dp = (void *) (&data->notifyTimeout);
       break;
#endif /* I_notifyTimeout */

#ifdef I_notifyMaxRetry
    case I_notifyMaxRetry:
       dp = (void *) (&data->notifyMaxRetry);
       break;
#endif /* I_notifyMaxRetry */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * cleanup after notifyEntry set/undo
 *---------------------------------------------------------------------*/
static int notifyEntry_cleanup SR_PROTOTYPE((doList_t *trash));

static int
notifyEntry_cleanup(trash)
   doList_t *trash;
{
    FreeEntries(notifyEntryTypeTable, trash->data);
    free(trash->data);
    FreeEntries(notifyEntryTypeTable, trash->undodata);
    if (trash->undodata != NULL) {
        free(trash->undodata);
    }
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the notifyEntry family
 *---------------------------------------------------------------------*/
notifyEntry_t *
Clone_notifyEntry(notifyEntry)
    notifyEntry_t *notifyEntry;
{
    notifyEntry_t *data;

    if ((data = (notifyEntry_t *) malloc (sizeof(notifyEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (notifyEntry), sizeof(notifyEntry_t));

    data->notifyIdentityName = CloneOctetString(notifyEntry->notifyIdentityName);
    data->notifyTransportLabel = CloneOctetString(notifyEntry->notifyTransportLabel);
    data->notifyContextName = CloneOctetString(notifyEntry->notifyContextName);
    data->notifyViewName = CloneOctetString(notifyEntry->notifyViewName);
#ifdef U_notifyEntry
    k_notifyEntryCloneUserpartData(data, notifyEntry);
#endif /* U_notifyEntry */

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
notifyEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    int            carry = 0;
    notifyEntry_t     *notifyEntry;
    SR_INT32       notifyIndex;

   /*
    * Validate the object instance
    *
    */
    if (instLength != 1) {
        return NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, (int)(0 + object->oid.length), &notifyIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    notifyEntry = k_notifyEntry_get(-1, contextInfo, -1, EXACT, notifyIndex);
    if (notifyEntry != NULL) {
        if (notifyEntry->notifyMemoryType == D_notifyMemoryType_readOnly) {
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
         if ( (dp->setMethod == notifyEntry_set) &&
            (((notifyEntry_t *) (dp->data)) != NULL) &&
            (((notifyEntry_t *) (dp->data))->notifyIndex == notifyIndex) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = notifyEntry_set;
        dp->cleanupMethod = notifyEntry_cleanup;
#ifdef SR_notifyEntry_UNDO
        dp->undoMethod = notifyEntry_undo;
#else /* SR_notifyEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_notifyEntry_UNDO */
        dp->state = SR_UNKNOWN;

        if (notifyEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_notifyEntry(notifyEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            if ((dp->undodata = (void *) Clone_notifyEntry(notifyEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }

        }
        else {
            if ( (dp->data = (void *) malloc (sizeof(notifyEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
#ifdef SR_CLEAR_MALLOC
                memset(dp->data, 0, sizeof(notifyEntry_t));
#endif	/* SR_CLEAR_MALLOC */
                dp->undodata = NULL;

                /* Fill in reasonable default values for this new entry */
                ((notifyEntry_t *) (dp->data))->notifyIndex = (notifyIndex);
                SET_VALID(I_notifyIndex, ((notifyEntry_t *) (dp->data))->valid);

                error_status = k_notifyEntry_set_defaults(dp);
            }
        }
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_notifySPI
   case I_notifySPI:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     ((notifyEntry_t *) (dp->data))->notifySPI = value->sl_value;
     break;
#endif /* I_notifySPI */

#ifdef I_notifyIdentityName
   case I_notifyIdentityName:

     if (((notifyEntry_t *) (dp->data))->notifyIdentityName != NULL) {
        FreeOctetString(((notifyEntry_t *) (dp->data))->notifyIdentityName);
     }

     ((notifyEntry_t *) (dp->data))->notifyIdentityName = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_notifyIdentityName */

#ifdef I_notifyTransportLabel
   case I_notifyTransportLabel:

     if (((notifyEntry_t *) (dp->data))->notifyTransportLabel != NULL) {
        FreeOctetString(((notifyEntry_t *) (dp->data))->notifyTransportLabel);
     }

     ((notifyEntry_t *) (dp->data))->notifyTransportLabel = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_notifyTransportLabel */

#ifdef I_notifyContextName
   case I_notifyContextName:

     if (((notifyEntry_t *) (dp->data))->notifyContextName != NULL) {
        FreeOctetString(((notifyEntry_t *) (dp->data))->notifyContextName);
     }

     ((notifyEntry_t *) (dp->data))->notifyContextName = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_notifyContextName */

#ifdef I_notifyViewName
   case I_notifyViewName:

     if (((notifyEntry_t *) (dp->data))->notifyViewName != NULL) {
        FreeOctetString(((notifyEntry_t *) (dp->data))->notifyViewName);
     }

     ((notifyEntry_t *) (dp->data))->notifyViewName = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_notifyViewName */

#ifdef I_notifyMemoryType
   case I_notifyMemoryType:

     if ((value->sl_value < 1) || (value->sl_value > 5)) {
         return WRONG_VALUE_ERROR;
     }

     if (notifyEntry != NULL) {
         /* check for attempts to change 'permanent' to other value */
         if ((notifyEntry->notifyMemoryType == D_notifyMemoryType_permanent) &&
             (value->sl_value != D_notifyMemoryType_permanent)) {
             /* permanent storageType cannot be changed */
             return INCONSISTENT_VALUE_ERROR;
         }
         /* check for attempts to change 'permanent' to other value */
         if ((notifyEntry->notifyMemoryType < D_notifyMemoryType_permanent) &&
             (value->sl_value >= D_notifyMemoryType_permanent)) {
             /* permanent storageType cannot be changed */
             return INCONSISTENT_VALUE_ERROR;
         }
     }
     ((notifyEntry_t *) (dp->data))->notifyMemoryType = value->sl_value;
     break;
#endif /* I_notifyMemoryType */

#ifdef I_notifyStatus
   case I_notifyStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     error_status = CheckRowStatus(value->sl_value,
         notifyEntry == NULL ? 0L : notifyEntry->notifyStatus);
     if (error_status != 0) return(error_status);

     /* check for attempts to delete 'permanent' rows */
     if(notifyEntry != NULL
     && notifyEntry->notifyMemoryType == D_notifyMemoryType_permanent
     && value->sl_value == D_notifyStatus_destroy) {
         /* permanent rows can be changed but not deleted */
         return INCONSISTENT_VALUE_ERROR;
     }

     ((notifyEntry_t *) (dp->data))->notifyStatus = value->sl_value;
     break;
#endif /* I_notifyStatus */

#ifdef I_notifyConfirm
   case I_notifyConfirm:

     if ((value->sl_value < 1) || (value->sl_value > 2)) {
         return WRONG_VALUE_ERROR;
     }

     ((notifyEntry_t *) (dp->data))->notifyConfirm = value->sl_value;
     break;
#endif /* I_notifyConfirm */

#ifdef I_notifyAuthSnmpID
   case I_notifyAuthSnmpID:

     if (((notifyEntry_t *) (dp->data))->notifyAuthSnmpID != NULL) {
        FreeOctetString(((notifyEntry_t *) (dp->data))->notifyAuthSnmpID);
     }

     ((notifyEntry_t *) (dp->data))->notifyAuthSnmpID = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_notifyAuthSnmpID */

#ifdef I_notifyTimeout
   case I_notifyTimeout:

     ((notifyEntry_t *) (dp->data))->notifyTimeout = value->sl_value;
     break;
#endif /* I_notifyTimeout */

#ifdef I_notifyMaxRetry
   case I_notifyMaxRetry:

     ((notifyEntry_t *) (dp->data))->notifyMaxRetry = value->sl_value;
     break;
#endif /* I_notifyMaxRetry */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in notifyEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_notifyEntry_test */
   error_status = k_notifyEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((notifyEntry_t *) (dp->data))->valid);
        error_status = k_notifyEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
notifyEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_notifyEntry_set((notifyEntry_t *) (doCur->data),
            contextInfo, (int)doCur->state));
}

#endif /* SETS */


