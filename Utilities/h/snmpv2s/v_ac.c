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
static const char rcsid[] = "$Id: v_ac.c,v 1.5.4.2 1998/04/27 13:38:14 levi Exp $";
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

#include "sr_time.h"

#include "mibout.h"
#include "inst_lib.h"
#include "tmq.h"
#include "rowstatf.h"
#include "i_ac.h"
#include "diag.h"
SR_FILENAME


/* note: the following prototypes may be included automatically now.
         check for duplicates  -cws   */
void v2AdminAccessControl_free
    SR_PROTOTYPE((v2AdminAccessControl_t *data));

acEntry_t *Clone_acEntry
     SR_PROTOTYPE((acEntry_t *acEntry));


#ifdef SR_DISABLE_CACHE
#define v2AdminAccessControl_DISABLE_CACHE 1
#endif /* SR_DISABLE_CACHE */

/*---------------------------------------------------------------------
 * Retrieve data from the v2AdminAccessControl family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
v2AdminAccessControl_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;

#ifdef v2AdminAccessControl_DISABLE_CACHE
    v2AdminAccessControl_t *data;
#else /* v2AdminAccessControl_DISABLE_CACHE */
    static v2AdminAccessControl_t *data;
    static int             last_serialNum;
    static OID             *last_incoming;
    /* if the cache is not valid */
    if ( (serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
         (CmpOIDInst(incoming, last_incoming, (int)object->oid.length) != 0) ||
         (!VALID(object->nominator, data->valid)) ) {
     
#endif /* v2AdminAccessControl_DISABLE_CACHE */
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
     if ( (arg == -1) || (data = k_v2AdminAccessControl_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

#ifndef v2AdminAccessControl_DISABLE_CACHE
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
#endif /* v2AdminAccessControl_DISABLE_CACHE */

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_acSpinLock
    case I_acSpinLock:
       dp = (void *) (&data->acSpinLock);
       break;
#endif /* I_acSpinLock */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the v2AdminAccessControl data object.
 *---------------------------------------------------------------------*/
void
v2AdminAccessControl_free(data)
   v2AdminAccessControl_t *data;
{
   if (data != NULL) {
#ifdef U_v2AdminAccessControl
       /* free user-defined data */
       k_v2AdminAccessControlFreeUserpartData (data);
#endif /* U_v2AdminAccessControl */


       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after v2AdminAccessControl set/undo
 *---------------------------------------------------------------------*/
static int v2AdminAccessControl_cleanup SR_PROTOTYPE((doList_t *trash));

static int
v2AdminAccessControl_cleanup(trash)
   doList_t *trash;
{
   v2AdminAccessControl_free((v2AdminAccessControl_t *) trash->data);
   v2AdminAccessControl_free((v2AdminAccessControl_t *) trash->undodata);
   return NO_ERROR;
}

#ifdef SR_v2AdminAccessControl_UNDO
/*----------------------------------------------------------------------
 * clone the v2AdminAccessControl family
 *---------------------------------------------------------------------*/
v2AdminAccessControl_t *
Clone_v2AdminAccessControl(v2AdminAccessControl)
    v2AdminAccessControl_t *v2AdminAccessControl;
{
    v2AdminAccessControl_t *data;

    if ((data = (v2AdminAccessControl_t *) malloc (sizeof(v2AdminAccessControl_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (v2AdminAccessControl), sizeof(v2AdminAccessControl_t));


    return data;
}
#endif /* SR_v2AdminAccessControl_UNDO */

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
v2AdminAccessControl_test(incoming, object, value, doHead, doCur, contextInfo)
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
#ifdef SR_v2AdminAccessControl_UNDO
    v2AdminAccessControl_t     *v2AdminAccessControl;
#endif /* SR_v2AdminAccessControl_UNDO */

   /*
    * Validate the object instance: 1) It must be of length 1  2) and the
    * instance must be 0.
    */
    if (instLength != 1 || incoming->oid_ptr[incoming->length - 1] != 0) {
        return (NO_CREATION_ERROR); 
    }

#ifdef SR_v2AdminAccessControl_UNDO
    v2AdminAccessControl = k_v2AdminAccessControl_get(-1, contextInfo, -1);
#endif /* SR_v2AdminAccessControl_UNDO */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same family.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == v2AdminAccessControl_set) &&
            (((v2AdminAccessControl_t *) (dp->data)) != NULL) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;
        if ( (dp->data = (void *) malloc (sizeof(v2AdminAccessControl_t))) == NULL) { 
            DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
            return RESOURCE_UNAVAILABLE_ERROR;
        }
#ifdef SR_CLEAR_MALLOC
        memset(dp->data, 0, sizeof(v2AdminAccessControl_t));
#endif	/* SR_CLEAR_MALLOC */

#ifdef SR_v2AdminAccessControl_UNDO
        if ((dp->undodata = (void *) Clone_v2AdminAccessControl(v2AdminAccessControl)) == NULL) {
            DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
            error_status = RESOURCE_UNAVAILABLE_ERROR;
        }
#endif /* SR_v2AdminAccessControl_UNDO */

        dp->setMethod = v2AdminAccessControl_set;
        dp->cleanupMethod = v2AdminAccessControl_cleanup;
#ifdef SR_v2AdminAccessControl_UNDO
        dp->undoMethod = v2AdminAccessControl_undo;
#else /* SR_v2AdminAccessControl_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_v2AdminAccessControl_UNDO */
        dp->state = SR_UNKNOWN;

    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_acSpinLock
   case I_acSpinLock:

     if ( ((value->sl_value < 0) || (value->sl_value > 2147483647)) ) {
         return WRONG_VALUE_ERROR;
     }

     /* since this is a TestAndIncr variable, make sure that it is being set to
      * its current value, if one exists */
     if (dp->undodata != NULL) {
         /* compare against the cached value.  Use the undodata value,
          * in case this object is set in multiple varbinds. */
         if(value->sl_value != ((v2AdminAccessControl_t *) (dp->undodata))->acSpinLock) {
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

     ((v2AdminAccessControl_t *) (dp->data))->acSpinLock = value->sl_value;
     break;
#endif /* I_acSpinLock */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in v2AdminAccessControl_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_v2AdminAccessControl_test */
   error_status = k_v2AdminAccessControl_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((v2AdminAccessControl_t *) (dp->data))->valid);
        error_status = k_v2AdminAccessControl_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
v2AdminAccessControl_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_v2AdminAccessControl_set((v2AdminAccessControl_t *) (doCur->data),
            contextInfo, (int)doCur->state));
}

#endif /* SETS */


#ifdef SR_DISABLE_CACHE
#define acEntry_DISABLE_CACHE 1
#endif /* SR_DISABLE_CACHE */

/*---------------------------------------------------------------------
 * Retrieve data from the acEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
acEntry_get(incoming, object, searchType, contextInfo, serialNum)
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
    SR_INT32        acSPI;
    int             acSPI_offset;
    OctetString *   acGroupName = NULL;
    int             acGroupName_offset;
    OctetString *   acContextName = NULL;
    int             acContextName_offset;
    int             index;
    int             final_index;

#ifdef acEntry_DISABLE_CACHE
    acEntry_t       *data;
    SR_UINT32   buffer[MAX_OID_SIZE];
    OID             inst;
#else /* acEntry_DISABLE_CACHE */
    static acEntry_t       *data;
    static SR_UINT32   buffer[MAX_OID_SIZE];
    static OID             inst;
    static int             last_serialNum;
    static OID             *last_incoming;
    /* if the cache is not valid */
    if ( (serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
         (CmpOIDInst(incoming, last_incoming, (int)object->oid.length) != 0) ||
         (!NEXT_ROW(data->valid)) ||
         (!VALID(object->nominator, data->valid)) ) {
     
#endif /* acEntry_DISABLE_CACHE */
    acSPI_offset = object->oid.length;
    acGroupName_offset = acSPI_offset + 1;
    acContextName_offset = acGroupName_offset + GetVariableIndexLength(incoming, acGroupName_offset);
    final_index = acContextName_offset + GetImpliedIndexLength(incoming, acContextName_offset);

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToImpliedOctetString(incoming, acContextName_offset, &acContextName, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToVariableOctetString(incoming, acGroupName_offset, &acGroupName, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, acSPI_offset, &acSPI, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_acEntry_get(serialNum, contextInfo, arg ,searchType, acSPI, acGroupName, acContextName)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = (unsigned long) data->acSPI;
        inst.oid_ptr[index++] = data->acGroupName->length;
        for(i = 0; i < data->acGroupName->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->acGroupName->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        for(i = 0; i < data->acContextName->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->acContextName->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.length = index;
     }

        FreeOctetString(acGroupName);
        FreeOctetString(acContextName);

#ifndef acEntry_DISABLE_CACHE
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
#endif /* acEntry_DISABLE_CACHE */

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_acContextNameMask
    case I_acContextNameMask:
       dp = (void *) (CloneOctetString(data->acContextNameMask));
       break;
#endif /* I_acContextNameMask */

#ifdef I_acPrivs
    case I_acPrivs:
       dp = (void *) (&data->acPrivs);
       break;
#endif /* I_acPrivs */

#ifdef I_acReadViewName
    case I_acReadViewName:
       dp = (void *) (CloneOctetString(data->acReadViewName));
       break;
#endif /* I_acReadViewName */

#ifdef I_acWriteViewName
    case I_acWriteViewName:
       dp = (void *) (CloneOctetString(data->acWriteViewName));
       break;
#endif /* I_acWriteViewName */

#ifdef I_acMemoryType
    case I_acMemoryType:
       dp = (void *) (&data->acMemoryType);
       break;
#endif /* I_acMemoryType */

#ifdef I_acStatus
    case I_acStatus:
       dp = (void *) (&data->acStatus);
       break;
#endif /* I_acStatus */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * cleanup after acEntry set/undo
 *---------------------------------------------------------------------*/
static int acEntry_cleanup SR_PROTOTYPE((doList_t *trash));

static int
acEntry_cleanup(trash)
   doList_t *trash;
{
    FreeEntries(acEntryTypeTable, trash->data);
    free(trash->data);
    FreeEntries(acEntryTypeTable, trash->undodata);
    if (trash->undodata != NULL) {
        free(trash->undodata);
    }
    return NO_ERROR;
}


/*----------------------------------------------------------------------
 * clone the acEntry family
 *---------------------------------------------------------------------*/
acEntry_t *
Clone_acEntry(acEntry)
    acEntry_t *acEntry;
{
    acEntry_t *data;

    if ((data = (acEntry_t *) malloc (sizeof(acEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (acEntry), sizeof(acEntry_t));

    data->acGroupName = CloneOctetString(acEntry->acGroupName);
    data->acContextName = CloneOctetString(acEntry->acContextName);
    data->acContextNameMask = CloneOctetString(acEntry->acContextNameMask);
    data->acReadViewName = CloneOctetString(acEntry->acReadViewName);
    data->acWriteViewName = CloneOctetString(acEntry->acWriteViewName);
#ifdef U_acEntry
    k_acEntryCloneUserpartData(data, acEntry);
#endif /* U_acEntry */

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
acEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    acEntry_t     *acEntry;
    SR_INT32       acSPI;
    int            acSPI_offset;
    OctetString *  acGroupName = NULL;
    int            acGroupName_offset;
    OctetString *  acContextName = NULL;
    int            acContextName_offset;
    int            final_index;

   /*
    * Validate the object instance
    *
    */
    acSPI_offset = object->oid.length;
    acGroupName_offset = acSPI_offset + 1;
    acContextName_offset = acGroupName_offset + GetVariableIndexLength(incoming, acGroupName_offset);
    final_index = acContextName_offset + GetImpliedIndexLength(incoming, acContextName_offset);

    if (final_index != incoming->length) {
          return(NO_CREATION_ERROR);
    }

    if ( (InstToInt(incoming, acSPI_offset, &acSPI, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToVariableOctetString(incoming, acGroupName_offset, &acGroupName, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToImpliedOctetString(incoming, acContextName_offset, &acContextName, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        FreeOctetString(acGroupName);
        FreeOctetString(acContextName);
        return error_status;
    }

    acEntry = k_acEntry_get(-1, contextInfo, -1, EXACT, acSPI, acGroupName, acContextName);
    if (acEntry != NULL) {
        if (acEntry->acMemoryType == D_acMemoryType_readOnly) {
        FreeOctetString(acGroupName);
        FreeOctetString(acContextName);
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
         if ( (dp->setMethod == acEntry_set) &&
            (((acEntry_t *) (dp->data)) != NULL) &&
            (((acEntry_t *) (dp->data))->acSPI == acSPI) &&
            (CmpOctetStrings(((acEntry_t *) (dp->data))->acGroupName, acGroupName) == 0) &&
            (CmpOctetStrings(((acEntry_t *) (dp->data))->acContextName, acContextName) == 0) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = acEntry_set;
        dp->cleanupMethod = acEntry_cleanup;
#ifdef SR_acEntry_UNDO
        dp->undoMethod = acEntry_undo;
#else /* SR_acEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_acEntry_UNDO */
        dp->state = SR_UNKNOWN;

        if (acEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_acEntry(acEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            if ((dp->undodata = (void *) Clone_acEntry(acEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }

        }
        else {
            if ( (dp->data = (void *) malloc (sizeof(acEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
#ifdef SR_CLEAR_MALLOC
                memset(dp->data, 0, sizeof(acEntry_t));
#endif	/* SR_CLEAR_MALLOC */
                dp->undodata = NULL;

                /* Fill in reasonable default values for this new entry */
                ((acEntry_t *) (dp->data))->acSPI = (acSPI);
                SET_VALID(I_acSPI, ((acEntry_t *) (dp->data))->valid);

                ((acEntry_t *) (dp->data))->acGroupName = CloneOctetString(acGroupName);
                SET_VALID(I_acGroupName, ((acEntry_t *) (dp->data))->valid);

                ((acEntry_t *) (dp->data))->acContextName = CloneOctetString(acContextName);
                SET_VALID(I_acContextName, ((acEntry_t *) (dp->data))->valid);

                error_status = k_acEntry_set_defaults(dp);
            }
        }
    }

        FreeOctetString(acGroupName);
        FreeOctetString(acContextName);
    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_acContextNameMask
   case I_acContextNameMask:

     if (((acEntry_t *) (dp->data))->acContextNameMask != NULL) {
        FreeOctetString(((acEntry_t *) (dp->data))->acContextNameMask);
     }

     ((acEntry_t *) (dp->data))->acContextNameMask = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_acContextNameMask */

#ifdef I_acPrivs
   case I_acPrivs:

     if ((value->sl_value < 1) || (value->sl_value > 3)) {
         return WRONG_VALUE_ERROR;
     }

     ((acEntry_t *) (dp->data))->acPrivs = value->sl_value;
     break;
#endif /* I_acPrivs */

#ifdef I_acReadViewName
   case I_acReadViewName:

     if (((acEntry_t *) (dp->data))->acReadViewName != NULL) {
        FreeOctetString(((acEntry_t *) (dp->data))->acReadViewName);
     }

     ((acEntry_t *) (dp->data))->acReadViewName = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_acReadViewName */

#ifdef I_acWriteViewName
   case I_acWriteViewName:

     if (((acEntry_t *) (dp->data))->acWriteViewName != NULL) {
        FreeOctetString(((acEntry_t *) (dp->data))->acWriteViewName);
     }

     ((acEntry_t *) (dp->data))->acWriteViewName = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_acWriteViewName */

#ifdef I_acMemoryType
   case I_acMemoryType:

     if ((value->sl_value < 1) || (value->sl_value > 5)) {
         return WRONG_VALUE_ERROR;
     }

     /* check for attempts to change 'permanent' to other value */
     if (acEntry != NULL) {
         if ((acEntry->acMemoryType == D_acMemoryType_permanent) &&
             (value->sl_value != D_acMemoryType_permanent)) {
             /* permanent storageType cannot be changed */
             return INCONSISTENT_VALUE_ERROR;
         }
         /* check for attempts to change 'permanent' to other value */
         if ((acEntry->acMemoryType < D_acMemoryType_permanent) &&
             (value->sl_value >= D_acMemoryType_permanent)) {
             /* permanent storageType cannot be changed */
             return INCONSISTENT_VALUE_ERROR;
         }
     }
     ((acEntry_t *) (dp->data))->acMemoryType = value->sl_value;
     break;
#endif /* I_acMemoryType */

#ifdef I_acStatus
   case I_acStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     error_status = CheckRowStatus(value->sl_value,
         acEntry == NULL ? 0L : acEntry->acStatus);
     if (error_status != 0) return(error_status);

     /* check for attempts to delete 'permanent' rows */
     if(acEntry != NULL
     && acEntry->acMemoryType == D_acMemoryType_permanent
     && value->sl_value == D_acStatus_destroy) {
         /* permanent rows can be changed but not deleted */
         return INCONSISTENT_VALUE_ERROR;
     }

     ((acEntry_t *) (dp->data))->acStatus = value->sl_value;
     break;
#endif /* I_acStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in acEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_acEntry_test */
   error_status = k_acEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((acEntry_t *) (dp->data))->valid);
        error_status = k_acEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
acEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_acEntry_set((acEntry_t *) (doCur->data),
            contextInfo, (int)doCur->state));
}

#endif /* SETS */


