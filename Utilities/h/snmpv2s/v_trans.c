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
static const char rcsid[] = "$Id: v_trans.c,v 1.4.4.2 1998/04/27 13:38:22 levi Exp $";
#endif	/* (! ( defined(lint) ) && defined(SR_RCSID)) */

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif	/* HAVE_MALLOC_H */

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif	/* HAVE_MEMORY_H */

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
#include "tmq.h"
#include "rowstatf.h"
#include "i_trans.h"
#include "inst_lib.h"
#include "diag.h"
SR_FILENAME


/*---------------------------------------------------------------------
 * Retrieve data from the v2AdminTransport family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
v2AdminTransport_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;

#ifdef v2AdminTransport_DISABLE_CACHE
    v2AdminTransport_t *data;
#else /* v2AdminTransport_DISABLE_CACHE */
    static v2AdminTransport_t *data;
    static int             last_serialNum;
    static OID             *last_incoming;
    /* if the cache is not valid */
    if ( (serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
         (CmpOIDInst(incoming, last_incoming, (int)object->oid.length) != 0) ||
         (!VALID(object->nominator, data->valid)) ) {
     
#endif /* v2AdminTransport_DISABLE_CACHE */
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
     if ( (arg == -1) || (data = k_v2AdminTransport_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

#ifndef v2AdminTransport_DISABLE_CACHE
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
#endif /* v2AdminTransport_DISABLE_CACHE */

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_transportSpinLock
    case I_transportSpinLock:
       dp = (void *) (&data->transportSpinLock);
       break;
#endif /* I_transportSpinLock */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the v2AdminTransport data object.
 *---------------------------------------------------------------------*/
void
v2AdminTransport_free(data)
   v2AdminTransport_t *data;
{
   if (data != NULL) {
#ifdef U_v2AdminTransport
       /* free user-defined data */
       k_v2AdminTransportFreeUserpartData (data);
#endif /* U_v2AdminTransport */


       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after v2AdminTransport set/undo
 *---------------------------------------------------------------------*/
static int v2AdminTransport_cleanup SR_PROTOTYPE((doList_t *trash));

static int
v2AdminTransport_cleanup(trash)
   doList_t *trash;
{
   v2AdminTransport_free((v2AdminTransport_t *) trash->data);
   v2AdminTransport_free((v2AdminTransport_t *) trash->undodata);
   return NO_ERROR;
}

#ifdef SR_v2AdminTransport_UNDO
/*----------------------------------------------------------------------
 * clone the v2AdminTransport family
 *---------------------------------------------------------------------*/
v2AdminTransport_t *
Clone_v2AdminTransport(v2AdminTransport)
    v2AdminTransport_t *v2AdminTransport;
{
    v2AdminTransport_t *data;

    if ((data = (v2AdminTransport_t *) malloc (sizeof(v2AdminTransport_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (v2AdminTransport), sizeof(v2AdminTransport_t));


    return data;
}
#endif /* SR_v2AdminTransport_UNDO */

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
v2AdminTransport_test(incoming, object, value, doHead, doCur, contextInfo)
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
#ifdef SR_v2AdminTransport_UNDO
    v2AdminTransport_t     *v2AdminTransport;
#endif /* SR_v2AdminTransport_UNDO */

   /*
    * Validate the object instance: 1) It must be of length 1  2) and the
    * instance must be 0.
    */
    if (instLength != 1 || incoming->oid_ptr[incoming->length - 1] != 0) {
        return (NO_CREATION_ERROR); 
    }

#ifdef SR_v2AdminTransport_UNDO
    v2AdminTransport = k_v2AdminTransport_get(-1, contextInfo, -1);
#endif /* SR_v2AdminTransport_UNDO */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same family.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == v2AdminTransport_set) &&
            (((v2AdminTransport_t *) (dp->data)) != NULL) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;
        if ( (dp->data = (void *) malloc (sizeof(v2AdminTransport_t))) == NULL) { 
            DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
            return RESOURCE_UNAVAILABLE_ERROR;
        }
#ifdef SR_CLEAR_MALLOC
        memset(dp->data, 0, sizeof(v2AdminTransport_t));
#endif	/* SR_CLEAR_MALLOC */
#ifdef SR_v2AdminTransport_UNDO
        if ((dp->undodata = (void *) Clone_v2AdminTransport(v2AdminTransport)) == NULL) {
            DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
            error_status = RESOURCE_UNAVAILABLE_ERROR;
        }
#endif /* SR_v2AdminTransport_UNDO */

        dp->setMethod = v2AdminTransport_set;
        dp->cleanupMethod = v2AdminTransport_cleanup;
#ifdef SR_v2AdminTransport_UNDO
        dp->undoMethod = v2AdminTransport_undo;
#else /* SR_v2AdminTransport_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_v2AdminTransport_UNDO */
        dp->state = SR_UNKNOWN;

    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_transportSpinLock
   case I_transportSpinLock:

     if ( ((value->sl_value < 0) || (value->sl_value > 2147483647)) ) {
         return WRONG_VALUE_ERROR;
     }

     /* since this is a TestAndIncr variable, make sure that it is being set to
      * its current value, if one exists */
     if (dp->undodata != NULL) {
         /* compare against the cached value.  Use the undodata value,
          * in case this object is set in multiple varbinds. */
         if(value->sl_value != ((v2AdminTransport_t *) (dp->undodata))->transportSpinLock) {
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

     ((v2AdminTransport_t *) (dp->data))->transportSpinLock = value->sl_value;
     break;
#endif /* I_transportSpinLock */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in v2AdminTransport_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_v2AdminTransport_test */
   error_status = k_v2AdminTransport_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((v2AdminTransport_t *) (dp->data))->valid);
        error_status = k_v2AdminTransport_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
v2AdminTransport_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_v2AdminTransport_set((v2AdminTransport_t *) (doCur->data),
            contextInfo, (int)doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the transportEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
transportEntry_get(incoming, object, searchType, contextInfo, serialNum)
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
    OctetString *   transportLabel = NULL;
    int             transportLabel_offset;
    SR_INT32        transportSubindex;
    int             transportSubindex_offset;
    int             index;
    int             final_index;

#ifdef transportEntry_DISABLE_CACHE
    transportEntry_t *data;
    SR_UINT32   buffer[MAX_OID_SIZE];
    OID             inst;
#else /* transportEntry_DISABLE_CACHE */
    static transportEntry_t *data;
    static SR_UINT32   buffer[MAX_OID_SIZE];
    static OID             inst;
    static int             last_serialNum;
    static OID             *last_incoming;
    /* if the cache is not valid */
    if ( (serialNum != last_serialNum) || (serialNum == -1) || (data == NULL) ||
         (CmpOIDInst(incoming, last_incoming, (int)object->oid.length) != 0) ||
         (!NEXT_ROW(data->valid)) ||
         (!VALID(object->nominator, data->valid)) ) {
     
#endif /* transportEntry_DISABLE_CACHE */
    transportLabel_offset = object->oid.length;
    transportSubindex_offset = transportLabel_offset + GetVariableIndexLength(incoming, transportLabel_offset);
    final_index = transportSubindex_offset + 1;

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToInt(incoming, transportSubindex_offset, &transportSubindex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        transportSubindex = MAX(0, transportSubindex);
    }

    if ( (InstToVariableOctetString(incoming, transportLabel_offset, &transportLabel, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_transportEntry_get(serialNum, contextInfo, arg ,searchType, transportLabel, transportSubindex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = data->transportLabel->length;
        for(i = 0; i < data->transportLabel->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->transportLabel->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.oid_ptr[index++] = (unsigned long) data->transportSubindex;
        inst.length = index;
     }

        FreeOctetString(transportLabel);

#ifndef transportEntry_DISABLE_CACHE
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
#endif /* transportEntry_DISABLE_CACHE */

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_transportDomain
    case I_transportDomain:
       dp = (void *) (CloneOID(data->transportDomain));
       break;
#endif /* I_transportDomain */

#ifdef I_transportAddress
    case I_transportAddress:
       dp = (void *) (CloneOctetString(data->transportAddress));
       break;
#endif /* I_transportAddress */

#ifdef I_transportReceiveMask
    case I_transportReceiveMask:
       dp = (void *) (CloneOctetString(data->transportReceiveMask));
       break;
#endif /* I_transportReceiveMask */

#ifdef I_transportMMS
    case I_transportMMS:
       dp = (void *) (&data->transportMMS);
       break;
#endif /* I_transportMMS */

#ifdef I_transportMemoryType
    case I_transportMemoryType:
       dp = (void *) (&data->transportMemoryType);
       break;
#endif /* I_transportMemoryType */

#ifdef I_transportStatus
    case I_transportStatus:
       dp = (void *) (&data->transportStatus);
       break;
#endif /* I_transportStatus */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * cleanup after transportEntry set/undo
 *---------------------------------------------------------------------*/
static int transportEntry_cleanup SR_PROTOTYPE((doList_t *trash));

static int
transportEntry_cleanup(trash)
   doList_t *trash;
{
    FreeEntries(transportEntryTypeTable, trash->data);
    free(trash->data);
    FreeEntries(transportEntryTypeTable, trash->undodata);
    if (trash->undodata != NULL) {
        free(trash->undodata);
    }
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the transportEntry family
 *---------------------------------------------------------------------*/
transportEntry_t *
Clone_transportEntry(transportEntry)
    transportEntry_t *transportEntry;
{
    transportEntry_t *data;

    if ((data = (transportEntry_t *) malloc (sizeof(transportEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (transportEntry), sizeof(transportEntry_t));

    data->transportLabel = CloneOctetString(transportEntry->transportLabel);
    data->transportDomain = CloneOID(transportEntry->transportDomain);
    data->transportAddress = CloneOctetString(transportEntry->transportAddress);
    data->transportReceiveMask = CloneOctetString(transportEntry->transportReceiveMask);
#ifdef U_transportEntry
    k_transportEntryCloneUserpartData(data, transportEntry);
#endif /* U_transportEntry */

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
transportEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    transportEntry_t     *transportEntry;
    OctetString *  transportLabel = NULL;
    int            transportLabel_offset;
    SR_INT32       transportSubindex;
    int            transportSubindex_offset;
    int            final_index;

   /*
    * Validate the object instance
    *
    */
    transportLabel_offset = object->oid.length;
    transportSubindex_offset = transportLabel_offset + GetVariableIndexLength(incoming, transportLabel_offset);
    final_index = transportSubindex_offset + 1;

    if (final_index != incoming->length) {
          return(NO_CREATION_ERROR);
    }

    if ( (InstToVariableOctetString(incoming, transportLabel_offset, &transportLabel, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, transportSubindex_offset, &transportSubindex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        FreeOctetString(transportLabel);
        return error_status;
    }

    transportEntry = k_transportEntry_get(-1, contextInfo, -1, EXACT, transportLabel, transportSubindex);
    if (transportEntry != NULL) {
        if (transportEntry->transportMemoryType == D_transportMemoryType_readOnly) {
        FreeOctetString(transportLabel);
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
         if ( (dp->setMethod == transportEntry_set) &&
            (((transportEntry_t *) (dp->data)) != NULL) &&
            (CmpOctetStrings(((transportEntry_t *) (dp->data))->transportLabel, transportLabel) == 0) &&
            (((transportEntry_t *) (dp->data))->transportSubindex == transportSubindex) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = transportEntry_set;
        dp->cleanupMethod = transportEntry_cleanup;
#ifdef SR_transportEntry_UNDO
        dp->undoMethod = transportEntry_undo;
#else /* SR_transportEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_transportEntry_UNDO */
        dp->state = SR_UNKNOWN;

        if (transportEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_transportEntry(transportEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            if ((dp->undodata = (void *) Clone_transportEntry(transportEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }

        }
        else {
            if ( (dp->data = (void *) malloc (sizeof(transportEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
#ifdef SR_CLEAR_MALLOC
                memset(dp->data, 0, sizeof(transportEntry_t));
#endif	/* SR_CLEAR_MALLOC */
                dp->undodata = NULL;

                /* Fill in reasonable default values for this new entry */
                ((transportEntry_t *) (dp->data))->transportLabel = CloneOctetString(transportLabel);
                SET_VALID(I_transportLabel, ((transportEntry_t *) (dp->data))->valid);

                ((transportEntry_t *) (dp->data))->transportSubindex = (transportSubindex);
                SET_VALID(I_transportSubindex, ((transportEntry_t *) (dp->data))->valid);

                error_status = k_transportEntry_set_defaults(dp);
            }
        }
    }

        FreeOctetString(transportLabel);
    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_transportDomain
   case I_transportDomain:

     if (((transportEntry_t *) (dp->data))->transportDomain != NULL) {
        FreeOID(((transportEntry_t *) (dp->data))->transportDomain);
     }

     ((transportEntry_t *) (dp->data))->transportDomain = 
            CloneOID(value->oid_value);

     break;
#endif /* I_transportDomain */

#ifdef I_transportAddress
   case I_transportAddress:

     if (((transportEntry_t *) (dp->data))->transportAddress != NULL) {
        FreeOctetString(((transportEntry_t *) (dp->data))->transportAddress);
     }

     ((transportEntry_t *) (dp->data))->transportAddress = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_transportAddress */

#ifdef I_transportReceiveMask
   case I_transportReceiveMask:

     if (((transportEntry_t *) (dp->data))->transportReceiveMask != NULL) {
        FreeOctetString(((transportEntry_t *) (dp->data))->transportReceiveMask);
     }

     ((transportEntry_t *) (dp->data))->transportReceiveMask = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_transportReceiveMask */

#ifdef I_transportMMS
   case I_transportMMS:

     ((transportEntry_t *) (dp->data))->transportMMS = value->sl_value;
     break;
#endif /* I_transportMMS */

#ifdef I_transportMemoryType
   case I_transportMemoryType:

     if ((value->sl_value < 1) || (value->sl_value > 5)) {
         return WRONG_VALUE_ERROR;
     }

     if (transportEntry != NULL) {
         /* check for attempts to change 'permanent' to other value */
         if ((transportEntry->transportMemoryType == D_transportMemoryType_permanent) &&
             (value->sl_value != D_transportMemoryType_permanent)) {
             /* permanent storageType cannot be changed */
             return INCONSISTENT_VALUE_ERROR;
         }
         /* check for attempts to change 'permanent' to other value */
         if ((transportEntry->transportMemoryType < D_transportMemoryType_permanent) &&
             (value->sl_value >= D_transportMemoryType_permanent)) {
             /* permanent storageType cannot be changed */
             return INCONSISTENT_VALUE_ERROR;
         }
     }
     ((transportEntry_t *) (dp->data))->transportMemoryType = value->sl_value;
     break;
#endif /* I_transportMemoryType */

#ifdef I_transportStatus
   case I_transportStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     error_status = CheckRowStatus(value->sl_value,
         transportEntry == NULL ? 0L : transportEntry->transportStatus);
     if (error_status != 0) return(error_status);

     /* check for attempts to delete 'permanent' rows */
     if(transportEntry != NULL
     && transportEntry->transportMemoryType == D_transportMemoryType_permanent
     && value->sl_value == D_transportStatus_destroy) {
         /* permanent rows can be changed but not deleted */
         return INCONSISTENT_VALUE_ERROR;
     }

     ((transportEntry_t *) (dp->data))->transportStatus = value->sl_value;
     break;
#endif /* I_transportStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in transportEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_transportEntry_test */
   error_status = k_transportEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((transportEntry_t *) (dp->data))->valid);
        error_status = k_transportEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
transportEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_transportEntry_set((transportEntry_t *) (doCur->data),
            contextInfo, (int)doCur->state));
}

#endif /* SETS */


