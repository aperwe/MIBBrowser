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
static const char rcsid[] = "$Id: v_usec.c,v 1.4.4.2 1998/04/27 13:38:24 levi Exp $";
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
#include "inst_lib.h"
#include "rowstatf.h"
#include "i_usec.h"
#include "diag.h"
SR_FILENAME


/* note: the following prototypes may be included automatically now.
         check for duplicates  -cws   */
void usecUser_free
    SR_PROTOTYPE((usecUser_t *data));

userNameEntry_t *Clone_userNameEntry
    SR_PROTOTYPE((userNameEntry_t *userNameEntry));


#ifdef SR_DISABLE_CACHE
#define usecScalars_DISABLE_CACHE 1
#endif /* SR_DISABLE_CACHE */

/*---------------------------------------------------------------------
 * Retrieve data from the usecScalars family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
usecScalars_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;

#ifdef usecScalars_DISABLE_CACHE
    usecScalars_t   *data;
#else /* usecScalars_DISABLE_CACHE */
    static usecScalars_t   *data;
    static int             last_serialNum;
    static OID             *last_incoming;
    /* if the cache is not valid */
    if ( (serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
         (CmpOIDInst(incoming, last_incoming, (int)object->oid.length) != 0) ||
         (!VALID(object->nominator, data->valid)) ) {
     
#endif /* usecScalars_DISABLE_CACHE */
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
     if ( (arg == -1) || (data = k_usecScalars_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

#ifndef usecScalars_DISABLE_CACHE
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
#endif /* usecScalars_DISABLE_CACHE */

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_snmpBoots
    case I_snmpBoots:
       dp = (void *) (&data->snmpBoots);
       break;
#endif /* I_snmpBoots */

#ifdef I_snmpTime
    case I_snmpTime:
       dp = (void *) (&data->snmpTime);
       break;
#endif /* I_snmpTime */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

#ifdef SR_DISABLE_CACHE
#define usecStats_DISABLE_CACHE 1
#endif /* SR_DISABLE_CACHE */

/*---------------------------------------------------------------------
 * Retrieve data from the usecStats family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
usecStats_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;

#ifdef usecStats_DISABLE_CACHE
    usecStats_t     *data;
#else /* usecStats_DISABLE_CACHE */
    static usecStats_t     *data;
    static int             last_serialNum;
    static OID             *last_incoming;
    /* if the cache is not valid */
    if ( (serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
         (CmpOIDInst(incoming, last_incoming, (int)object->oid.length) != 0) ||
         (!VALID(object->nominator, data->valid)) ) {
     
#endif /* usecStats_DISABLE_CACHE */
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
     if ( (arg == -1) || (data = k_usecStats_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

#ifndef usecStats_DISABLE_CACHE
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
#endif /* usecStats_DISABLE_CACHE */

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_usecStatsNotInTimeWindows
    case I_usecStatsNotInTimeWindows:
       dp = (void *) (&data->usecStatsNotInTimeWindows);
       break;
#endif /* I_usecStatsNotInTimeWindows */

#ifdef I_usecStatsUnknownUserNames
    case I_usecStatsUnknownUserNames:
       dp = (void *) (&data->usecStatsUnknownUserNames);
       break;
#endif /* I_usecStatsUnknownUserNames */

#ifdef I_usecStatsWrongDigestValues
    case I_usecStatsWrongDigestValues:
       dp = (void *) (&data->usecStatsWrongDigestValues);
       break;
#endif /* I_usecStatsWrongDigestValues */

#ifdef I_usecStatsBadPrivacys
    case I_usecStatsBadPrivacys:
       dp = (void *) (&data->usecStatsBadPrivacys);
       break;
#endif /* I_usecStatsBadPrivacys */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}


#ifdef SR_DISABLE_CACHE
#define usecUser_DISABLE_CACHE 1
#endif /* SR_DISABLE_CACHE */

/*---------------------------------------------------------------------
 * Retrieve data from the usecUser family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
usecUser_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;

#ifdef usecUser_DISABLE_CACHE
    usecUser_t      *data;
#else /* usecUser_DISABLE_CACHE */
    static usecUser_t      *data;
    static int             last_serialNum;
    static OID             *last_incoming;
    /* if the cache is not valid */
    if ( (serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
         (CmpOIDInst(incoming, last_incoming, (int)object->oid.length) != 0) ||
         (!VALID(object->nominator, data->valid)) ) {
     
#endif /* usecUser_DISABLE_CACHE */
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
     if ( (arg == -1) || (data = k_usecUser_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

#ifndef usecUser_DISABLE_CACHE
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
#endif /* usecUser_DISABLE_CACHE */

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_userSpinLock
    case I_userSpinLock:
       dp = (void *) (&data->userSpinLock);
       break;
#endif /* I_userSpinLock */

#ifdef I_maxUserNameLength
    case I_maxUserNameLength:
       dp = (void *) (&data->maxUserNameLength);
       break;
#endif /* I_maxUserNameLength */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the usecUser data object.
 *---------------------------------------------------------------------*/
void
usecUser_free(data)
   usecUser_t      *data;
{
   if (data != NULL) {
#ifdef U_usecUser
       /* free user-defined data */
       k_usecUserFreeUserpartData (data);
#endif /* U_usecUser */


       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after usecUser set/undo
 *---------------------------------------------------------------------*/
static int usecUser_cleanup SR_PROTOTYPE((doList_t *trash));

static int
usecUser_cleanup(trash)
   doList_t *trash;
{
   usecUser_free((usecUser_t *) trash->data);
   usecUser_free((usecUser_t *) trash->undodata);
   return NO_ERROR;
}

#ifdef SR_usecUser_UNDO
/*----------------------------------------------------------------------
 * clone the usecUser family
 *---------------------------------------------------------------------*/
usecUser_t *
Clone_usecUser(usecUser)
    usecUser_t *usecUser;
{
    usecUser_t *data;

    if ((data = (usecUser_t *) malloc (sizeof(usecUser_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (usecUser), sizeof(usecUser_t));


    return data;
}
#endif /* SR_usecUser_UNDO */

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
usecUser_test(incoming, object, value, doHead, doCur, contextInfo)
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
#ifdef SR_usecUser_UNDO
    usecUser_t     *usecUser;
#endif /* SR_usecUser_UNDO */

   /*
    * Validate the object instance: 1) It must be of length 1  2) and the
    * instance must be 0.
    */
    if (instLength != 1 || incoming->oid_ptr[incoming->length - 1] != 0) {
        return (NO_CREATION_ERROR); 
    }

#ifdef SR_usecUser_UNDO
    usecUser = k_usecUser_get(-1, contextInfo, -1);
#endif /* SR_usecUser_UNDO */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same family.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == usecUser_set) &&
            (((usecUser_t *) (dp->data)) != NULL) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;
        if ( (dp->data = (void *) malloc (sizeof(usecUser_t))) == NULL) { 
            DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
            return RESOURCE_UNAVAILABLE_ERROR;
        }
#ifdef SR_CLEAR_MALLOC
        memset(dp->data, 0, sizeof(usecUser_t));
#endif	/* SR_CLEAR_MALLOC */

#ifdef SR_usecUser_UNDO
        if ((dp->undodata = (void *) Clone_usecUser(usecUser)) == NULL) {
            DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
            error_status = RESOURCE_UNAVAILABLE_ERROR;
        }
#endif /* SR_usecUser_UNDO */

        dp->setMethod = usecUser_set;
        dp->cleanupMethod = usecUser_cleanup;
#ifdef SR_usecUser_UNDO
        dp->undoMethod = usecUser_undo;
#else /* SR_usecUser_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_usecUser_UNDO */
        dp->state = SR_UNKNOWN;

    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_userSpinLock
   case I_userSpinLock:

     if ( ((value->sl_value < 0) || (value->sl_value > 2147483647)) ) {
         return WRONG_VALUE_ERROR;
     }

     /* since this is a TestAndIncr variable, make sure that it is being set to
      * its current value, if one exists */
     if (dp->undodata != NULL) {
         /* compare against the cached value.  Use the undodata value,
          * in case this object is set in multiple varbinds. */
         if(value->sl_value != ((usecUser_t *) (dp->undodata))->userSpinLock) {
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

     ((usecUser_t *) (dp->data))->userSpinLock = value->sl_value;
     break;
#endif /* I_userSpinLock */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in usecUser_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_usecUser_test */
   error_status = k_usecUser_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((usecUser_t *) (dp->data))->valid);
        error_status = k_usecUser_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
usecUser_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_usecUser_set((usecUser_t *) (doCur->data),
            contextInfo, (int)doCur->state));
}

#endif /* SETS */


#ifdef SR_DISABLE_CACHE
#define userNameEntry_DISABLE_CACHE 1
#endif /* SR_DISABLE_CACHE */

/*---------------------------------------------------------------------
 * Retrieve data from the userNameEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
userNameEntry_get(incoming, object, searchType, contextInfo, serialNum)
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
    OctetString *   userAuthSnmpID = NULL;
    int             userAuthSnmpID_offset;
    OctetString *   userName = NULL;
    int             userName_offset;
    int             index;
    int             final_index;

#ifdef userNameEntry_DISABLE_CACHE
    userNameEntry_t *data;
    SR_UINT32   buffer[MAX_OID_SIZE];
    OID             inst;
#else /* userNameEntry_DISABLE_CACHE */
    static userNameEntry_t *data;
    static SR_UINT32   buffer[MAX_OID_SIZE];
    static OID             inst;
    static int             last_serialNum;
    static OID             *last_incoming;
    /* if the cache is not valid */
    if ( (serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
         (CmpOIDInst(incoming, last_incoming, (int)object->oid.length) != 0) ||
         (!NEXT_ROW(data->valid)) ||
         (!VALID(object->nominator, data->valid)) ) {
     
#endif /* userNameEntry_DISABLE_CACHE */
    userAuthSnmpID_offset = object->oid.length;
    userName_offset = userAuthSnmpID_offset + GetVariableIndexLength(incoming, userAuthSnmpID_offset);
    final_index = userName_offset + GetImpliedIndexLength(incoming, userName_offset);

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToImpliedOctetString(incoming, userName_offset, &userName, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToVariableOctetString(incoming, userAuthSnmpID_offset, &userAuthSnmpID, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_userNameEntry_get(serialNum, contextInfo, arg ,searchType, userAuthSnmpID, userName)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = data->userAuthSnmpID->length;
        for(i = 0; i < data->userAuthSnmpID->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->userAuthSnmpID->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        for(i = 0; i < data->userName->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->userName->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.length = index;
     }

        FreeOctetString(userAuthSnmpID);
        FreeOctetString(userName);

#ifndef userNameEntry_DISABLE_CACHE
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
#endif /* userNameEntry_DISABLE_CACHE */

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_userGroupName
    case I_userGroupName:
       dp = (void *) (CloneOctetString(data->userGroupName));
       break;
#endif /* I_userGroupName */

#ifdef I_userAuthChange
    case I_userAuthChange:
       dp = (void *) (CloneOctetString(data->userAuthChange));
       break;
#endif /* I_userAuthChange */

#ifdef I_userPrivChange
    case I_userPrivChange:
       dp = (void *) (CloneOctetString(data->userPrivChange));
       break;
#endif /* I_userPrivChange */

#ifdef I_userNovel
    case I_userNovel:
       dp = (void *) (CloneOctetString(data->userNovel));
       break;
#endif /* I_userNovel */

#ifdef I_userTransportLabel
    case I_userTransportLabel:
       dp = (void *) (CloneOctetString(data->userTransportLabel));
       break;
#endif /* I_userTransportLabel */

#ifdef I_userCloneFrom
    case I_userCloneFrom:
       dp = (void *) (CloneOID(data->userCloneFrom));
       break;
#endif /* I_userCloneFrom */

#ifdef I_userMemoryType
    case I_userMemoryType:
       dp = (void *) (&data->userMemoryType);
       break;
#endif /* I_userMemoryType */

#ifdef I_userStatus
    case I_userStatus:
       dp = (void *) (&data->userStatus);
       break;
#endif /* I_userStatus */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * cleanup after userNameEntry set/undo
 *---------------------------------------------------------------------*/
static int userNameEntry_cleanup SR_PROTOTYPE((doList_t *trash));

static int
userNameEntry_cleanup(trash)
   doList_t *trash;
{
    FreeEntries(userNameEntryTypeTable, trash->data);
    free(trash->data);
    FreeEntries(userNameEntryTypeTable, trash->undodata);
    if (trash->undodata != NULL) {
        free(trash->undodata);
    }
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the userNameEntry family
 *---------------------------------------------------------------------*/
userNameEntry_t *
Clone_userNameEntry(userNameEntry)
    userNameEntry_t *userNameEntry;
{
    userNameEntry_t *data;

    if ((data = (userNameEntry_t *) malloc (sizeof(userNameEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (userNameEntry), sizeof(userNameEntry_t));

    data->userAuthSnmpID = CloneOctetString(userNameEntry->userAuthSnmpID);
    data->userName = CloneOctetString(userNameEntry->userName);
    data->userGroupName = CloneOctetString(userNameEntry->userGroupName);
    data->userAuthChange = CloneOctetString(userNameEntry->userAuthChange);
    data->userPrivChange = CloneOctetString(userNameEntry->userPrivChange);
    data->userNovel = CloneOctetString(userNameEntry->userNovel);
    data->userTransportLabel = CloneOctetString(userNameEntry->userTransportLabel);
    data->userCloneFrom = CloneOID(userNameEntry->userCloneFrom);
#ifdef U_userNameEntry
    k_userNameEntryCloneUserpartData(data, userNameEntry);
#endif /* U_userNameEntry */

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
userNameEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    userNameEntry_t     *userNameEntry;
    OctetString *  userAuthSnmpID = NULL;
    int            userAuthSnmpID_offset;
    OctetString *  userName = NULL;
    int            userName_offset;
    int            final_index;

   /*
    * Validate the object instance
    *
    */
    userAuthSnmpID_offset = object->oid.length;
    userName_offset = userAuthSnmpID_offset + GetVariableIndexLength(incoming, userAuthSnmpID_offset);
    final_index = userName_offset + GetImpliedIndexLength(incoming, userName_offset);

    if (final_index != incoming->length) {
          return(NO_CREATION_ERROR);
    }

    if ( (InstToVariableOctetString(incoming, userAuthSnmpID_offset, &userAuthSnmpID, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToImpliedOctetString(incoming, userName_offset, &userName, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        FreeOctetString(userAuthSnmpID);
        FreeOctetString(userName);
        return error_status;
    }

    userNameEntry = k_userNameEntry_get(-1, contextInfo, -1, EXACT, userAuthSnmpID, userName);
    if (userNameEntry != NULL) {
        if (userNameEntry->userMemoryType == D_userMemoryType_readOnly) {
        FreeOctetString(userAuthSnmpID);
        FreeOctetString(userName);
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
         if ( (dp->setMethod == userNameEntry_set) &&
            (((userNameEntry_t *) (dp->data)) != NULL) &&
            (CmpOctetStrings(((userNameEntry_t *) (dp->data))->userAuthSnmpID, userAuthSnmpID) == 0) &&
            (CmpOctetStrings(((userNameEntry_t *) (dp->data))->userName, userName) == 0) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = userNameEntry_set;
        dp->cleanupMethod = userNameEntry_cleanup;
#ifdef SR_userNameEntry_UNDO
        dp->undoMethod = userNameEntry_undo;
#else /* SR_userNameEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_userNameEntry_UNDO */
        dp->state = SR_UNKNOWN;

        if (userNameEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_userNameEntry(userNameEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            if ((dp->undodata = (void *) Clone_userNameEntry(userNameEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }

        }
        else {
            if ( (dp->data = (void *) malloc (sizeof(userNameEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
#ifdef SR_CLEAR_MALLOC
                memset(dp->data, 0, sizeof(userNameEntry_t));
#endif	/* SR_CLEAR_MALLOC */
                dp->undodata = NULL;

                /* Fill in reasonable default values for this new entry */
                ((userNameEntry_t *) (dp->data))->userAuthSnmpID = CloneOctetString(userAuthSnmpID);
                SET_VALID(I_userAuthSnmpID, ((userNameEntry_t *) (dp->data))->valid);

                ((userNameEntry_t *) (dp->data))->userName = CloneOctetString(userName);
                SET_VALID(I_userName, ((userNameEntry_t *) (dp->data))->valid);

                error_status = k_userNameEntry_set_defaults(dp);
            }
        }
    }

        FreeOctetString(userAuthSnmpID);
        FreeOctetString(userName);
    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_userGroupName
   case I_userGroupName:

     if (((userNameEntry_t *) (dp->data))->userGroupName != NULL) {
        FreeOctetString(((userNameEntry_t *) (dp->data))->userGroupName);
     }

     ((userNameEntry_t *) (dp->data))->userGroupName = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_userGroupName */

#ifdef I_userAuthChange
   case I_userAuthChange:

     if (((userNameEntry_t *) (dp->data))->userAuthChange != NULL) {
        FreeOctetString(((userNameEntry_t *) (dp->data))->userAuthChange);
     }

     ((userNameEntry_t *) (dp->data))->userAuthChange = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_userAuthChange */

#ifdef I_userPrivChange
   case I_userPrivChange:

     if (((userNameEntry_t *) (dp->data))->userPrivChange != NULL) {
        FreeOctetString(((userNameEntry_t *) (dp->data))->userPrivChange);
     }

     ((userNameEntry_t *) (dp->data))->userPrivChange = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_userPrivChange */

#ifdef I_userNovel
   case I_userNovel:

     if (((userNameEntry_t *) (dp->data))->userNovel != NULL) {
        FreeOctetString(((userNameEntry_t *) (dp->data))->userNovel);
     }

     ((userNameEntry_t *) (dp->data))->userNovel = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_userNovel */

#ifdef I_userTransportLabel
   case I_userTransportLabel:

     if (((userNameEntry_t *) (dp->data))->userTransportLabel != NULL) {
        FreeOctetString(((userNameEntry_t *) (dp->data))->userTransportLabel);
     }

     ((userNameEntry_t *) (dp->data))->userTransportLabel = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_userTransportLabel */

#ifdef I_userCloneFrom
   case I_userCloneFrom:

     if (((userNameEntry_t *) (dp->data))->userCloneFrom != NULL) {
        FreeOID(((userNameEntry_t *) (dp->data))->userCloneFrom);
     }

     ((userNameEntry_t *) (dp->data))->userCloneFrom = 
            CloneOID(value->oid_value);

     break;
#endif /* I_userCloneFrom */

#ifdef I_userMemoryType
   case I_userMemoryType:

     if ((value->sl_value < 1) || (value->sl_value > 5)) {
         return WRONG_VALUE_ERROR;
     }

     if (userNameEntry != NULL) {
         /* check for attempts to change 'permanent' to other value */
         if ((userNameEntry->userMemoryType == D_userMemoryType_permanent) &&
             (value->sl_value != D_userMemoryType_permanent)) {
             /* permanent storageType cannot be changed */
             return INCONSISTENT_VALUE_ERROR;
         }
         /* check for attempts to change 'permanent' to other value */
         if ((userNameEntry->userMemoryType < D_userMemoryType_permanent) &&
             (value->sl_value >= D_userMemoryType_permanent)) {
             /* permanent storageType cannot be changed */
             return INCONSISTENT_VALUE_ERROR;
         }
     }
     ((userNameEntry_t *) (dp->data))->userMemoryType = value->sl_value;
     break;
#endif /* I_userMemoryType */

#ifdef I_userStatus
   case I_userStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     error_status = CheckRowStatus(value->sl_value,
         userNameEntry == NULL ? 0L : userNameEntry->userStatus);
     if (error_status != 0) return(error_status);

     /* check for attempts to delete 'permanent' rows */
     if(userNameEntry != NULL
     && userNameEntry->userMemoryType == D_userMemoryType_permanent
     && value->sl_value == D_userStatus_destroy) {
         /* permanent rows can be changed but not deleted */
         return INCONSISTENT_VALUE_ERROR;
     }

     ((userNameEntry_t *) (dp->data))->userStatus = value->sl_value;
     break;
#endif /* I_userStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in userNameEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_userNameEntry_test */
   error_status = k_userNameEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((userNameEntry_t *) (dp->data))->valid);
        error_status = k_userNameEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
userNameEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_userNameEntry_set((userNameEntry_t *) (doCur->data),
            contextInfo, (int)doCur->state));
}

#endif /* SETS */


