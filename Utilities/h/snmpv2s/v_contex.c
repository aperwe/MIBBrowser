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
static const char rcsid[] = "$Id: v_contex.c,v 1.4.4.2 1998/04/27 13:38:18 levi Exp $";
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
#include "i_contex.h"
#include "diag.h"
SR_FILENAME



/* note: the following prototypes may be included automatically now.
         check for duplicates  -cws   */
v2ContextEntry_t *Clone_v2ContextEntry
    SR_PROTOTYPE((v2ContextEntry_t *v2ContextEntry));

#ifdef SR_DISABLE_CACHE
#define v2ContextEntry_DISABLE_CACHE 1
#endif /* SR_DISABLE_CACHE */

/*---------------------------------------------------------------------
 * Retrieve data from the v2ContextEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
v2ContextEntry_get(incoming, object, searchType, contextInfo, serialNum)
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
    OctetString *   v2ContextSnmpID = NULL;
    int             v2ContextSnmpID_offset;
    OctetString *   v2ContextName = NULL;
    int             v2ContextName_offset;
    int             index;
    int             final_index;

#ifdef v2ContextEntry_DISABLE_CACHE
    v2ContextEntry_t *data;
    SR_UINT32   buffer[MAX_OID_SIZE];
    OID             inst;
#else /* v2ContextEntry_DISABLE_CACHE */
    static v2ContextEntry_t *data;
    static SR_UINT32   buffer[MAX_OID_SIZE];
    static OID             inst;
    static int             last_serialNum;
    static OID             *last_incoming;
    /* if the cache is not valid */
    if ( (serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
         (CmpOIDInst(incoming, last_incoming, (int)object->oid.length) != 0) ||
         (!NEXT_ROW(data->valid)) ||
         (!VALID(object->nominator, data->valid)) ) {
     
#endif /* v2ContextEntry_DISABLE_CACHE */
    v2ContextSnmpID_offset = object->oid.length;
    v2ContextName_offset = v2ContextSnmpID_offset + GetVariableIndexLength(incoming, v2ContextSnmpID_offset);
    final_index = v2ContextName_offset + GetImpliedIndexLength(incoming, v2ContextName_offset);

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToImpliedOctetString(incoming, v2ContextName_offset, &v2ContextName, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToVariableOctetString(incoming, v2ContextSnmpID_offset, &v2ContextSnmpID, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_v2ContextEntry_get(serialNum, contextInfo, arg ,searchType, v2ContextSnmpID, v2ContextName)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = data->v2ContextSnmpID->length;
        for(i = 0; i < data->v2ContextSnmpID->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->v2ContextSnmpID->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        for(i = 0; i < data->v2ContextName->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->v2ContextName->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.length = index;
     }

        FreeOctetString(v2ContextSnmpID);
        FreeOctetString(v2ContextName);

#ifndef v2ContextEntry_DISABLE_CACHE
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
#endif /* v2ContextEntry_DISABLE_CACHE */

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_v2ContextLocalEntity
    case I_v2ContextLocalEntity:
       dp = (void *) (CloneOctetString(data->v2ContextLocalEntity));
       break;
#endif /* I_v2ContextLocalEntity */

#ifdef I_v2ContextLocalTime
    case I_v2ContextLocalTime:
       dp = (void *) (&data->v2ContextLocalTime);
       break;
#endif /* I_v2ContextLocalTime */

#ifdef I_v2ContextMemoryType
    case I_v2ContextMemoryType:
       dp = (void *) (&data->v2ContextMemoryType);
       break;
#endif /* I_v2ContextMemoryType */

#ifdef I_v2ContextStatus
    case I_v2ContextStatus:
       dp = (void *) (&data->v2ContextStatus);
       break;
#endif /* I_v2ContextStatus */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * cleanup after v2ContextEntry set/undo
 *---------------------------------------------------------------------*/
static int v2ContextEntry_cleanup SR_PROTOTYPE((doList_t *trash));

static int
v2ContextEntry_cleanup(trash)
   doList_t *trash;
{
    FreeEntries(v2ContextEntryTypeTable, trash->data);
    free(trash->data);
    FreeEntries(v2ContextEntryTypeTable, trash->undodata);
    if (trash->undodata != NULL) {
        free(trash->undodata);
    }
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the v2ContextEntry family
 *---------------------------------------------------------------------*/
v2ContextEntry_t *
Clone_v2ContextEntry(v2ContextEntry)
    v2ContextEntry_t *v2ContextEntry;
{
    v2ContextEntry_t *data;

    if ((data = (v2ContextEntry_t *) malloc (sizeof(v2ContextEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (v2ContextEntry), sizeof(v2ContextEntry_t));

    data->v2ContextSnmpID = CloneOctetString(v2ContextEntry->v2ContextSnmpID);
    data->v2ContextName = CloneOctetString(v2ContextEntry->v2ContextName);
    data->v2ContextLocalEntity = CloneOctetString(v2ContextEntry->v2ContextLocalEntity);
#ifdef U_v2ContextEntry
    k_v2ContextEntryCloneUserpartData(data, v2ContextEntry);
#endif /* U_v2ContextEntry */

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
v2ContextEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    v2ContextEntry_t     *v2ContextEntry;
    OctetString *  v2ContextSnmpID = NULL;
    int            v2ContextSnmpID_offset;
    OctetString *  v2ContextName = NULL;
    int            v2ContextName_offset;
    int            final_index;

   /*
    * Validate the object instance
    *
    */
    v2ContextSnmpID_offset = object->oid.length;
    v2ContextName_offset = v2ContextSnmpID_offset + GetVariableIndexLength(incoming, v2ContextSnmpID_offset);
    final_index = v2ContextName_offset + GetImpliedIndexLength(incoming, v2ContextName_offset);

    if (final_index != incoming->length) {
          return(NO_CREATION_ERROR);
    }

    if ( (InstToVariableOctetString(incoming, v2ContextSnmpID_offset, &v2ContextSnmpID, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToImpliedOctetString(incoming, v2ContextName_offset, &v2ContextName, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (v2ContextSnmpID->length != 12) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        FreeOctetString(v2ContextSnmpID);
        FreeOctetString(v2ContextName);
        return error_status;
    }

    v2ContextEntry = k_v2ContextEntry_get(-1, contextInfo, -1, EXACT, v2ContextSnmpID, v2ContextName);
    if (v2ContextEntry != NULL) {
        if (v2ContextEntry->v2ContextMemoryType == D_v2ContextMemoryType_readOnly) {
        FreeOctetString(v2ContextSnmpID);
        FreeOctetString(v2ContextName);
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
         if ( (dp->setMethod == v2ContextEntry_set) &&
            (((v2ContextEntry_t *) (dp->data)) != NULL) &&
            (CmpOctetStrings(((v2ContextEntry_t *) (dp->data))->v2ContextSnmpID, v2ContextSnmpID) == 0) &&
            (CmpOctetStrings(((v2ContextEntry_t *) (dp->data))->v2ContextName, v2ContextName) == 0) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = v2ContextEntry_set;
        dp->cleanupMethod = v2ContextEntry_cleanup;
#ifdef SR_v2ContextEntry_UNDO
        dp->undoMethod = v2ContextEntry_undo;
#else /* SR_v2ContextEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_v2ContextEntry_UNDO */
        dp->state = SR_UNKNOWN;

        if (v2ContextEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_v2ContextEntry(v2ContextEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            if ((dp->undodata = (void *) Clone_v2ContextEntry(v2ContextEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }

        }
        else {
            if ( (dp->data = (void *) malloc (sizeof(v2ContextEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
#ifdef SR_CLEAR_MALLOC
                memset(dp->data, 0, sizeof(v2ContextEntry_t));
#endif	/* SR_CLEAR_MALLOC */
                dp->undodata = NULL;

                /* Fill in reasonable default values for this new entry */
                ((v2ContextEntry_t *) (dp->data))->v2ContextSnmpID = CloneOctetString(v2ContextSnmpID);
                SET_VALID(I_v2ContextSnmpID, ((v2ContextEntry_t *) (dp->data))->valid);

                ((v2ContextEntry_t *) (dp->data))->v2ContextName = CloneOctetString(v2ContextName);
                SET_VALID(I_v2ContextName, ((v2ContextEntry_t *) (dp->data))->valid);

                error_status = k_v2ContextEntry_set_defaults(dp);
            }
        }
    }

        FreeOctetString(v2ContextSnmpID);
        FreeOctetString(v2ContextName);
    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_v2ContextLocalEntity
   case I_v2ContextLocalEntity:

     if (((v2ContextEntry_t *) (dp->data))->v2ContextLocalEntity != NULL) {
        FreeOctetString(((v2ContextEntry_t *) (dp->data))->v2ContextLocalEntity);
     }

     ((v2ContextEntry_t *) (dp->data))->v2ContextLocalEntity = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_v2ContextLocalEntity */

#ifdef I_v2ContextLocalTime
   case I_v2ContextLocalTime:

     if ((value->sl_value < 1) || (value->sl_value > 2)) {
         return WRONG_VALUE_ERROR;
     }

     ((v2ContextEntry_t *) (dp->data))->v2ContextLocalTime = value->sl_value;
     break;
#endif /* I_v2ContextLocalTime */

#ifdef I_v2ContextMemoryType
   case I_v2ContextMemoryType:

     if ((value->sl_value < 1) || (value->sl_value > 5)) {
         return WRONG_VALUE_ERROR;
     }

     if (v2ContextEntry != NULL) {
         /* check for attempts to change 'permanent' to other value */
         if ((v2ContextEntry->v2ContextMemoryType == D_v2ContextMemoryType_permanent) &&
             (value->sl_value != D_v2ContextMemoryType_permanent)) {
             /* permanent storageType cannot be changed */
             return INCONSISTENT_VALUE_ERROR;
         }
         /* check for attempts to change 'permanent' to other value */
         if ((v2ContextEntry->v2ContextMemoryType < D_v2ContextMemoryType_permanent) &&
             (value->sl_value >= D_v2ContextMemoryType_permanent)) {
             /* permanent storageType cannot be changed */
             return INCONSISTENT_VALUE_ERROR;
         }
     }
     ((v2ContextEntry_t *) (dp->data))->v2ContextMemoryType = value->sl_value;
     break;
#endif /* I_v2ContextMemoryType */

#ifdef I_v2ContextStatus
   case I_v2ContextStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     error_status = CheckRowStatus(value->sl_value,
         v2ContextEntry == NULL ? 0L : v2ContextEntry->v2ContextStatus);
     if (error_status != 0) return(error_status);

     /* check for attempts to delete 'permanent' rows */
     if(v2ContextEntry != NULL
     && v2ContextEntry->v2ContextMemoryType == D_v2ContextMemoryType_permanent
     && value->sl_value == D_v2ContextStatus_destroy) {
         /* permanent rows can be changed but not deleted */
         return INCONSISTENT_VALUE_ERROR;
     }

     ((v2ContextEntry_t *) (dp->data))->v2ContextStatus = value->sl_value;
     break;
#endif /* I_v2ContextStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in v2ContextEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_v2ContextEntry_test */
   error_status = k_v2ContextEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((v2ContextEntry_t *) (dp->data))->valid);
        error_status = k_v2ContextEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
v2ContextEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_v2ContextEntry_set((v2ContextEntry_t *) (doCur->data),
            contextInfo, (int)doCur->state));
}

#endif /* SETS */


