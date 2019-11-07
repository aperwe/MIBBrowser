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
static const char rcsid[] = "$Id: v_view.c,v 1.4.4.2 1998/04/27 13:38:26 levi Exp $";
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
#include "inst_lib.h"
#include "oid_lib.h"
#include "tmq.h"
#include "rowstatf.h"
#include "i_view.h"
#include "diag.h"
SR_FILENAME


/* note: the following prototypes may be included automatically now.
         check for duplicates  -cws   */
void v2AdminViewTree_free
    SR_PROTOTYPE((v2AdminViewTree_t *data));

viewTreeEntry_t *Clone_viewTreeEntry
    SR_PROTOTYPE((viewTreeEntry_t *viewTreeEntry));


#ifdef SR_DISABLE_CACHE
#define v2AdminViewTree_DISABLE_CACHE 1
#endif /* SR_DISABLE_CACHE */

/*---------------------------------------------------------------------
 * Retrieve data from the v2AdminViewTree family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
v2AdminViewTree_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;

#ifdef v2AdminViewTree_DISABLE_CACHE
    v2AdminViewTree_t *data;
#else /* v2AdminViewTree_DISABLE_CACHE */
    static v2AdminViewTree_t *data;
    static int             last_serialNum;
    static OID             *last_incoming;
    /* if the cache is not valid */
    if ( (serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
         (CmpOIDInst(incoming, last_incoming, (int)object->oid.length) != 0) ||
         (!VALID(object->nominator, data->valid)) ) {
     
#endif /* v2AdminViewTree_DISABLE_CACHE */
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
     if ( (arg == -1) || (data = k_v2AdminViewTree_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

#ifndef v2AdminViewTree_DISABLE_CACHE
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
#endif /* v2AdminViewTree_DISABLE_CACHE */

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_viewTreeSpinLock
    case I_viewTreeSpinLock:
       dp = (void *) (&data->viewTreeSpinLock);
       break;
#endif /* I_viewTreeSpinLock */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the v2AdminViewTree data object.
 *---------------------------------------------------------------------*/
void
v2AdminViewTree_free(data)
   v2AdminViewTree_t *data;
{
   if (data != NULL) {
#ifdef U_v2AdminViewTree
       /* free user-defined data */
       k_v2AdminViewTreeFreeUserpartData (data);
#endif /* U_v2AdminViewTree */


       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after v2AdminViewTree set/undo
 *---------------------------------------------------------------------*/
static int v2AdminViewTree_cleanup SR_PROTOTYPE((doList_t *trash));

static int
v2AdminViewTree_cleanup(trash)
   doList_t *trash;
{
   v2AdminViewTree_free((v2AdminViewTree_t *) trash->data);
   v2AdminViewTree_free((v2AdminViewTree_t *) trash->undodata);
   return NO_ERROR;
}

#ifdef SR_v2AdminViewTree_UNDO
/*----------------------------------------------------------------------
 * clone the v2AdminViewTree family
 *---------------------------------------------------------------------*/
v2AdminViewTree_t *
Clone_v2AdminViewTree(v2AdminViewTree)
    v2AdminViewTree_t *v2AdminViewTree;
{
    v2AdminViewTree_t *data;

    if ((data = (v2AdminViewTree_t *) malloc (sizeof(v2AdminViewTree_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (v2AdminViewTree), sizeof(v2AdminViewTree_t));


    return data;
}
#endif /* SR_v2AdminViewTree_UNDO */

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
v2AdminViewTree_test(incoming, object, value, doHead, doCur, contextInfo)
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
#ifdef SR_v2AdminViewTree_UNDO
    v2AdminViewTree_t     *v2AdminViewTree;
#endif /* SR_v2AdminViewTree_UNDO */

   /*
    * Validate the object instance: 1) It must be of length 1  2) and the
    * instance must be 0.
    */
    if (instLength != 1 || incoming->oid_ptr[incoming->length - 1] != 0) {
        return (NO_CREATION_ERROR); 
    }

#ifdef SR_v2AdminViewTree_UNDO
    v2AdminViewTree = k_v2AdminViewTree_get(-1, contextInfo, -1);
#endif /* SR_v2AdminViewTree_UNDO */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same family.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == v2AdminViewTree_set) &&
            (((v2AdminViewTree_t *) (dp->data)) != NULL) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;
        if ( (dp->data = (void *) malloc (sizeof(v2AdminViewTree_t))) == NULL) { 
            DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
            return RESOURCE_UNAVAILABLE_ERROR;
        }
#ifdef SR_CLEAR_MALLOC
        memset(dp->data, 0, sizeof(v2AdminViewTree_t));
#endif	/* SR_CLEAR_MALLOC */

#ifdef SR_v2AdminViewTree_UNDO
        if ((dp->undodata = (void *) Clone_v2AdminViewTree(v2AdminViewTree)) == NULL) {
            DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
            error_status = RESOURCE_UNAVAILABLE_ERROR;
        }
#endif /* SR_v2AdminViewTree_UNDO */

        dp->setMethod = v2AdminViewTree_set;
        dp->cleanupMethod = v2AdminViewTree_cleanup;
#ifdef SR_v2AdminViewTree_UNDO
        dp->undoMethod = v2AdminViewTree_undo;
#else /* SR_v2AdminViewTree_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_v2AdminViewTree_UNDO */
        dp->state = SR_UNKNOWN;

    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_viewTreeSpinLock
   case I_viewTreeSpinLock:

     if ( ((value->sl_value < 0) || (value->sl_value > 2147483647)) ) {
         return WRONG_VALUE_ERROR;
     }

     /* since this is a TestAndIncr variable, make sure that it is being set to
      * its current value, if one exists */
     if (dp->undodata != NULL) {
         /* compare against the cached value.  Use the undodata value,
          * in case this object is set in multiple varbinds. */
         if(value->sl_value != ((v2AdminViewTree_t *) (dp->undodata))->viewTreeSpinLock) {
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

     ((v2AdminViewTree_t *) (dp->data))->viewTreeSpinLock = value->sl_value;
     break;
#endif /* I_viewTreeSpinLock */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in v2AdminViewTree_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_v2AdminViewTree_test */
   error_status = k_v2AdminViewTree_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((v2AdminViewTree_t *) (dp->data))->valid);
        error_status = k_v2AdminViewTree_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
v2AdminViewTree_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_v2AdminViewTree_set((v2AdminViewTree_t *) (doCur->data),
            contextInfo, (int)doCur->state));
}

#endif /* SETS */


#ifdef SR_DISABLE_CACHE
#define viewTreeEntry_DISABLE_CACHE 1
#endif /* SR_DISABLE_CACHE */

/*---------------------------------------------------------------------
 * Retrieve data from the viewTreeEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
viewTreeEntry_get(incoming, object, searchType, contextInfo, serialNum)
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
    OctetString *   viewTreeName = NULL;
    int             viewTreeName_offset;
    OID *           viewTreeSubTree = NULL;
    int             viewTreeSubTree_offset;
    int             index;
    int             final_index;

#ifdef viewTreeEntry_DISABLE_CACHE
    viewTreeEntry_t *data;
    SR_UINT32   buffer[MAX_OID_SIZE];
    OID             inst;
#else /* viewTreeEntry_DISABLE_CACHE */
    static viewTreeEntry_t *data;
    static SR_UINT32   buffer[MAX_OID_SIZE];
    static OID             inst;
    static int             last_serialNum;
    static OID             *last_incoming;
    /* if the cache is not valid */
    if ( (serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
         (CmpOIDInst(incoming, last_incoming, (int)object->oid.length) != 0) ||
         (!NEXT_ROW(data->valid)) ||
         (!VALID(object->nominator, data->valid)) ) {
     
#endif /* viewTreeEntry_DISABLE_CACHE */
    viewTreeName_offset = object->oid.length;
    viewTreeSubTree_offset = viewTreeName_offset + GetVariableIndexLength(incoming, viewTreeName_offset);
    final_index = viewTreeSubTree_offset + GetImpliedIndexLength(incoming, viewTreeSubTree_offset);

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToImpliedOID(incoming, viewTreeSubTree_offset, &viewTreeSubTree, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToVariableOctetString(incoming, viewTreeName_offset, &viewTreeName, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_viewTreeEntry_get(serialNum, contextInfo, arg ,searchType, viewTreeName, viewTreeSubTree)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = data->viewTreeName->length;
        for(i = 0; i < data->viewTreeName->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->viewTreeName->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        for(i = 0; i < data->viewTreeSubTree->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->viewTreeSubTree->oid_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.length = index;
     }

        FreeOctetString(viewTreeName);
        FreeOID(viewTreeSubTree);

#ifndef viewTreeEntry_DISABLE_CACHE
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
#endif /* viewTreeEntry_DISABLE_CACHE */

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_viewTreeMask
    case I_viewTreeMask:
       dp = (void *) (CloneOctetString(data->viewTreeMask));
       break;
#endif /* I_viewTreeMask */

#ifdef I_viewTreeType
    case I_viewTreeType:
       dp = (void *) (&data->viewTreeType);
       break;
#endif /* I_viewTreeType */

#ifdef I_viewTreeMemoryType
    case I_viewTreeMemoryType:
       dp = (void *) (&data->viewTreeMemoryType);
       break;
#endif /* I_viewTreeMemoryType */

#ifdef I_viewTreeStatus
    case I_viewTreeStatus:
       dp = (void *) (&data->viewTreeStatus);
       break;
#endif /* I_viewTreeStatus */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * cleanup after viewTreeEntry set/undo
 *---------------------------------------------------------------------*/
static int viewTreeEntry_cleanup SR_PROTOTYPE((doList_t *trash));

static int
viewTreeEntry_cleanup(trash)
   doList_t *trash;
{
    FreeEntries(viewTreeEntryTypeTable, trash->data);
    free(trash->data);
    FreeEntries(viewTreeEntryTypeTable, trash->undodata);
    if (trash->undodata != NULL) {
        free(trash->undodata);
    }
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the viewTreeEntry family
 *---------------------------------------------------------------------*/
viewTreeEntry_t *
Clone_viewTreeEntry(viewTreeEntry)
    viewTreeEntry_t *viewTreeEntry;
{
    viewTreeEntry_t *data;

    if ((data = (viewTreeEntry_t *) malloc (sizeof(viewTreeEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (viewTreeEntry), sizeof(viewTreeEntry_t));

    data->viewTreeName = CloneOctetString(viewTreeEntry->viewTreeName);
    data->viewTreeSubTree = CloneOID(viewTreeEntry->viewTreeSubTree);
    data->viewTreeMask = CloneOctetString(viewTreeEntry->viewTreeMask);
#ifdef U_viewTreeEntry
    k_viewTreeEntryCloneUserpartData(data, viewTreeEntry);
#endif /* U_viewTreeEntry */

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
viewTreeEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    viewTreeEntry_t     *viewTreeEntry;
    OctetString *  viewTreeName = NULL;
    int            viewTreeName_offset;
    OID *          viewTreeSubTree = NULL;
    int            viewTreeSubTree_offset;
    int            final_index;

   /*
    * Validate the object instance
    *
    */
    viewTreeName_offset = object->oid.length;
    viewTreeSubTree_offset = viewTreeName_offset + GetVariableIndexLength(incoming, viewTreeName_offset);
    final_index = viewTreeSubTree_offset + GetImpliedIndexLength(incoming, viewTreeSubTree_offset);

    if (final_index != incoming->length) {
          return(NO_CREATION_ERROR);
    }

    if ( (InstToVariableOctetString(incoming, viewTreeName_offset, &viewTreeName, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToImpliedOID(incoming, viewTreeSubTree_offset, &viewTreeSubTree, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        FreeOctetString(viewTreeName);
        FreeOID(viewTreeSubTree);
        return error_status;
    }

    viewTreeEntry = k_viewTreeEntry_get(-1, contextInfo, -1, EXACT, viewTreeName, viewTreeSubTree);
    if (viewTreeEntry != NULL) {
        if (viewTreeEntry->viewTreeMemoryType == D_viewTreeMemoryType_readOnly) {
        FreeOctetString(viewTreeName);
        FreeOID(viewTreeSubTree);
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
         if ( (dp->setMethod == viewTreeEntry_set) &&
            (((viewTreeEntry_t *) (dp->data)) != NULL) &&
            (CmpOctetStrings(((viewTreeEntry_t *) (dp->data))->viewTreeName, viewTreeName) == 0) &&
            (CmpOID(((viewTreeEntry_t *) (dp->data))->viewTreeSubTree, viewTreeSubTree) == 0) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = viewTreeEntry_set;
        dp->cleanupMethod = viewTreeEntry_cleanup;
#ifdef SR_viewTreeEntry_UNDO
        dp->undoMethod = viewTreeEntry_undo;
#else /* SR_viewTreeEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_viewTreeEntry_UNDO */
        dp->state = SR_UNKNOWN;

        if (viewTreeEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_viewTreeEntry(viewTreeEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            if ((dp->undodata = (void *) Clone_viewTreeEntry(viewTreeEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }

        }
        else {
            if ( (dp->data = (void *) malloc (sizeof(viewTreeEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
#ifdef SR_CLEAR_MALLOC
                memset(dp->data, 0, sizeof(viewTreeEntry_t));
#endif	/* SR_CLEAR_MALLOC */
                dp->undodata = NULL;

                /* Fill in reasonable default values for this new entry */
                ((viewTreeEntry_t *) (dp->data))->viewTreeName = CloneOctetString(viewTreeName);
                SET_VALID(I_viewTreeName, ((viewTreeEntry_t *) (dp->data))->valid);

                ((viewTreeEntry_t *) (dp->data))->viewTreeSubTree = CloneOID(viewTreeSubTree);
                SET_VALID(I_viewTreeSubTree, ((viewTreeEntry_t *) (dp->data))->valid);

                error_status = k_viewTreeEntry_set_defaults(dp);
            }
        }
    }

        FreeOctetString(viewTreeName);
        FreeOID(viewTreeSubTree);
    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_viewTreeMask
   case I_viewTreeMask:

     if (((viewTreeEntry_t *) (dp->data))->viewTreeMask != NULL) {
        FreeOctetString(((viewTreeEntry_t *) (dp->data))->viewTreeMask);
     }

     ((viewTreeEntry_t *) (dp->data))->viewTreeMask = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_viewTreeMask */

#ifdef I_viewTreeType
   case I_viewTreeType:

     if ((value->sl_value < 1) || (value->sl_value > 2)) {
         return WRONG_VALUE_ERROR;
     }

     ((viewTreeEntry_t *) (dp->data))->viewTreeType = value->sl_value;
     break;
#endif /* I_viewTreeType */

#ifdef I_viewTreeMemoryType
   case I_viewTreeMemoryType:

     if ((value->sl_value < 1) || (value->sl_value > 5)) {
         return WRONG_VALUE_ERROR;
     }

     if (viewTreeEntry != NULL) {
         /* check for attempts to change 'permanent' to other value */
         if ((viewTreeEntry->viewTreeMemoryType == D_viewTreeMemoryType_permanent) &&
             (value->sl_value != D_viewTreeMemoryType_permanent)) {
             /* permanent storageType cannot be changed */
             return INCONSISTENT_VALUE_ERROR;
         }
         /* check for attempts to change 'permanent' to other value */
         if ((viewTreeEntry->viewTreeMemoryType < D_viewTreeMemoryType_permanent) &&
             (value->sl_value >= D_viewTreeMemoryType_permanent)) {
             /* permanent storageType cannot be changed */
             return INCONSISTENT_VALUE_ERROR;
         }
     }
     ((viewTreeEntry_t *) (dp->data))->viewTreeMemoryType = value->sl_value;
     break;
#endif /* I_viewTreeMemoryType */

#ifdef I_viewTreeStatus
   case I_viewTreeStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     error_status = CheckRowStatus(value->sl_value,
         viewTreeEntry == NULL ? 0L : viewTreeEntry->viewTreeStatus);
     if (error_status != 0) return(error_status);

     /* check for attempts to delete 'permanent' rows */
     if(viewTreeEntry != NULL
     && viewTreeEntry->viewTreeMemoryType == D_viewTreeMemoryType_permanent
     && value->sl_value == D_viewTreeStatus_destroy) {
         /* permanent rows can be changed but not deleted */
         return INCONSISTENT_VALUE_ERROR;
     }

     ((viewTreeEntry_t *) (dp->data))->viewTreeStatus = value->sl_value;
     break;
#endif /* I_viewTreeStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in viewTreeEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_viewTreeEntry_test */
   error_status = k_viewTreeEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((viewTreeEntry_t *) (dp->data))->valid);
        error_status = k_viewTreeEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
viewTreeEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_viewTreeEntry_set((viewTreeEntry_t *) (doCur->data),
            contextInfo, (int)doCur->state));
}

#endif /* SETS */


