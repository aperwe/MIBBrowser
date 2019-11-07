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
static const char rcsid[] = "$Id: k_comm.c,v 1.8.4.1 1998/03/19 14:21:18 partain Exp $";
#endif	/* (! ( defined(lint) ) && defined(SR_RCSID)) */

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#include <sys/types.h>

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif	/* HAVE_MEMORY_H */

#include <string.h>

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif  /* HAVE_MALLOC_H */

#include <stddef.h>

#include "sr_type.h"
#include "sr_time.h"

#include "sr_snmp.h"
#include "sr_proto.h"
#include "comunity.h"
#include "v2clssc.h"
#include "sr_trans.h"
#include "context.h"
#include "method.h"
#include "lookup.h"
#include "v2table.h"
#include "sr_cfg.h"
#include "scan.h"
#include "mibout.h"
#include "tmq.h"
#include "rowstatf.h"
#include "mapctx.h"

#include "i_comm.h"

/* note: the following prototypes may be included automatically now.
         check for duplicates  -cws   */
int srCommunityEntryReadyToActivate
     SR_PROTOTYPE((srCommunityEntry_t *data));

contextMappingList_t contextMappingList_srCommunityMIBData = { NULL, 0, NULL };
contextMappingList_t contextMappingList_srCommunityTable = { NULL, 0, NULL };

static srCommunityMIB_t *access_srCommunityMIBData;
static SnmpV2Table *access_srCommunityTable;

void DeletesrCommunityEntry
    SR_PROTOTYPE((int index));

/* initialize support for srCommunityMIB objects */
int
k_srCommunityMIB_initialize(
    char *contextName_text,
    srCommunityMIB_t *srCommunityMIBData)
{
    SET_ALL_VALID(srCommunityMIBData->valid);
    if (contextName_text == NULL) {
        AddContextMappingText(&contextMappingList_srCommunityMIBData,
                              NULL,
                              srCommunityMIBData);
        AddContextMappingText(&contextMappingList_srCommunityMIBData,
                              "default",
                              srCommunityMIBData);
    } else {
        AddContextMappingText(&contextMappingList_srCommunityMIBData,
                              contextName_text,
                              srCommunityMIBData);
    }

    return 1;
}

/* terminate support for srCommunityMIB objects */
int
k_srCommunityMIB_terminate(void)
{
    return 1;
}

/* initialize support for srCommunityEntry objects */
int
k_srCommunityEntry_initialize(
    char *contextName_text,
    SnmpV2Table *srCommunityTable)
{
    if (contextName_text == NULL) {
        AddContextMappingText(&contextMappingList_srCommunityTable,
                              NULL,
                              srCommunityTable);
        AddContextMappingText(&contextMappingList_srCommunityTable,
                              "default",
                              srCommunityTable);
    } else {
        AddContextMappingText(&contextMappingList_srCommunityTable,
                              contextName_text,
                              srCommunityTable);
    }

    return 1;
}

/* terminate support for srCommunityEntry objects */
int
k_srCommunityEntry_terminate(void)
{
    return 1;
}

#ifdef U_srCommunityMIB
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_srCommunityMIBFreeUserpartData (data)
    srCommunityMIB_t *data;
{
    /* nothing to free by default */
}
#endif /* U_srCommunityMIB */

srCommunityMIB_t *
k_srCommunityMIB_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
    static int prev_serialNum = -1;
    static int prev_serialNum_initialized = 0;

    if (!prev_serialNum_initialized) {
        if (serialNum != prev_serialNum) {
            MapContext(&contextMappingList_srCommunityMIBData,
                       contextInfo,
                       (void *)&access_srCommunityMIBData);
        }
    } else {
        MapContext(&contextMappingList_srCommunityMIBData,
                   contextInfo,
                   (void *)&access_srCommunityMIBData);
        prev_serialNum_initialized = 1;
    }
    prev_serialNum = serialNum;

   return(access_srCommunityMIBData);
}

#ifdef SETS
int
k_srCommunityMIB_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{
    MapContext(&contextMappingList_srCommunityMIBData,
               contextInfo,
               (void *)&access_srCommunityMIBData);
    if (access_srCommunityMIBData == NULL) {
        return NO_CREATION_ERROR;
    }

    if (value->sl_value != access_srCommunityMIBData->srCommunitySpinLock) {
        return INCONSISTENT_VALUE_ERROR;
    }
    return NO_ERROR;
}

int
k_srCommunityMIB_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_srCommunityMIB_set(data, contextInfo, function)
   srCommunityMIB_t *data;
   ContextInfo *contextInfo;
   int function;
{
    MapContext(&contextMappingList_srCommunityMIBData,
               contextInfo,
               (void *)&access_srCommunityMIBData);

   if (data->srCommunitySpinLock == 2147483647) {
       access_srCommunityMIBData->srCommunitySpinLock = 0;
   } else {
       access_srCommunityMIBData->srCommunitySpinLock = data->srCommunitySpinLock + 1;
   }
   return NO_ERROR;
}

#ifdef SR_srCommunityMIB_UNDO
/* add #define SR_srCommunityMIB_UNDO in sitedefs.h to
 * include the undo routine for the srCommunityMIB family.
 */
int
srCommunityMIB_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_srCommunityMIB_UNDO */

#endif /* SETS */

/* This routine is called by the timeout code to 
 * delete a pending creation of a Table entry */ 
void
srCommunityTableDeleteCallback (TimeOutDescriptor *tdp)
{
    cidata_t *ud2 = (cidata_t *)tdp->UserData2;
    srCommunityEntry_t *data;

    /* dummy up an entry to delete */
    data = (srCommunityEntry_t *) tdp->UserData2;
    data->srCommunityStatus = D_srCommunityStatus_destroy;
    data->RowStatusTimerId = -1;

    /* free the timeout descriptor */
    free(tdp);

    /* call the set method */
    k_srCommunityEntry_set(data, ud2->cip, 0);
    FreeContextInfo(ud2->cip);
    free(ud2);
}

/* This routine deletes an entry from the srCommunityTable */
void
DeletesrCommunityEntry (index)
    int index;
{
    FreeEntries(srCommunityEntryTypeTable, access_srCommunityTable->tp[index]);
    free(access_srCommunityTable->tp[index]);
    RemoveTableEntry(access_srCommunityTable, index);
}

/* This routine returns nonzero if the table entry contains sufficient
 * information to be in the active (or notInService) state. */
int
srCommunityEntryReadyToActivate(data)
srCommunityEntry_t *data;
{
    /* by default, assume sufficient information */
    return(1);
}

#ifdef U_srCommunityEntry
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_srCommunityEntryFreeUserpartData (data)
    srCommunityEntry_t *data;
{
    /* nothing to free by default */
}

/* This routine can be used to clone data which
 * is defined in the userpart part of the structure */
void
k_srCommunityEntryCloneUserpartData (dst, src)
    srCommunityEntry_t *dst, *src;
{
    /* nothing to clone by default */
}
#endif /* U_srCommunityEntry */

srCommunityEntry_t *
k_srCommunityEntry_get(serialNum, contextInfo, nominator, searchType, srCommunityAuthSnmpID, srCommunityName)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   OctetString * srCommunityAuthSnmpID;
   OctetString * srCommunityName;
{
    int index;
    static int prev_serialNum = -1;
    static int prev_serialNum_initialized = 0;

    if (!prev_serialNum_initialized) {
        if (serialNum != prev_serialNum) {
            MapContext(&contextMappingList_srCommunityTable,
                       contextInfo,
                       (void *)&access_srCommunityTable);
        }
    } else {
        MapContext(&contextMappingList_srCommunityTable,
                   contextInfo,
                   (void *)&access_srCommunityTable);
        prev_serialNum_initialized = 1;
    }
    prev_serialNum = serialNum;

    if (access_srCommunityTable == NULL) {
        return NULL;
    }

    access_srCommunityTable->tip[0].value.octet_val = srCommunityAuthSnmpID;
    access_srCommunityTable->tip[1].value.octet_val = srCommunityName;
    if ((index = SearchTable(access_srCommunityTable, searchType)) == -1) {
        return NULL;
    }

    return (srCommunityEntry_t *) access_srCommunityTable->tp[index];

}

#ifdef SETS
int
k_srCommunityEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{
    MapContext(&contextMappingList_srCommunityTable,
               contextInfo,
               (void *)&access_srCommunityTable);
    if (access_srCommunityTable == NULL) {
        return NO_CREATION_ERROR;
    }

    if (object->nominator == I_srCommunityContextSnmpID) {
        if (value->os_value->length != 12) {
            return WRONG_VALUE_ERROR;
        }
    }

    return NO_ERROR;
}

int
k_srCommunityEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    int doListReady = 0;
    srCommunityEntry_t *data = (srCommunityEntry_t *) dp->data;

    /* no state information needed for destroy */
    if (data->srCommunityStatus == D_srCommunityStatus_destroy) {
        dp->state = SR_DELETE;
    } else 
    if (data->srCommunityStatus == RS_UNINITIALIZED) {
        /* status hasn't been set yet */
        dp->state = SR_UNKNOWN;
    } else {
        /* check row status state machine transitions */
        if(data->srCommunityStatus == D_srCommunityStatus_createAndGo) {
            data->srCommunityStatus = D_srCommunityStatus_active;
        } 
        doListReady = 1;

        /* if transitioning (or remaining) in the active or notInService states,
         * make sure that the new values are not inconsistent enough to 
         * make the entry notReady */
        if(data->srCommunityStatus == D_srCommunityStatus_active
        || data->srCommunityStatus == D_srCommunityStatus_notInService) {
            if(srCommunityEntryReadyToActivate(data) == 0) {
                /* inconsistent values, don't allow the set operation */
                doListReady = 0;
            }
        }

        /* did we have all required data? */
        if(doListReady != 0) {
            /* yes, update the doList entry */
            dp->state = SR_ADD_MODIFY;
        } else {
            /* no, make sure the set doesn't proceed */
            dp->state = SR_UNKNOWN;
        }
    }

    return NO_ERROR;
}

int
k_srCommunityEntry_set_defaults(dp)
    doList_t       *dp;
{
    srCommunityEntry_t *data = (srCommunityEntry_t *) (dp->data);

    if ((data->srCommunityGroupName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    if ((data->srCommunityContextSnmpID = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    if ((data->srCommunityContextName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    if ((data->srCommunityTransportLabel = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    data->RowStatusTimerId = -1;

    SET_ALL_VALID(data->valid);

    return NO_ERROR;
}

int
k_srCommunityEntry_set(data, contextInfo, function)
   srCommunityEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

    int index;
    srCommunityEntry_t *newrow = NULL;

    MapContext(&contextMappingList_srCommunityTable,
               contextInfo,
               (void *)&access_srCommunityTable);

    /* find this entry in the table */
    access_srCommunityTable->tip[0].value.octet_val = data->srCommunityAuthSnmpID;
    access_srCommunityTable->tip[1].value.octet_val = data->srCommunityName;
    if ((index = SearchTable(access_srCommunityTable, EXACT)) != -1) {
        newrow = (srCommunityEntry_t *) access_srCommunityTable->tp[index];
    }

    /* perform the table entry operation on it */
    if (data->srCommunityStatus == D_srCommunityStatus_destroy) {
        if (data->RowStatusTimerId != -1) {
            CancelRowStatusTimeout(data->RowStatusTimerId);
        }

        if (index == -1) {
            return NO_ERROR;
        } else {
            /* delete the table entry */
            int flag = newrow->srCommunityMemoryType;
            DeletesrCommunityEntry(index);
            if (flag >= NONVOLATILE) {
                writeConfigFileFlag = TRUE;
            }
            return NO_ERROR;
        }
    } else if (index == -1) {
        /* add the entry */
        access_srCommunityTable->tip[0].value.octet_val = data->srCommunityAuthSnmpID;
        access_srCommunityTable->tip[1].value.octet_val = data->srCommunityName;
        if ((index = NewTableEntry(access_srCommunityTable)) == -1) {
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        newrow = (srCommunityEntry_t *) access_srCommunityTable->tp[index];

        /* set a timeout */
        if ((newrow->RowStatusTimerId = SetRowStatusTimeoutWithCI(MAX_ROW_CREATION_TIME, contextInfo, (void *) newrow, (TodCallbackFunction) srCommunityTableDeleteCallback)) == -1) {
            DeletesrCommunityEntry(index);
            return RESOURCE_UNAVAILABLE_ERROR;
        }
    }

    /* if we are executing a createAndWait command, determine whether
     * to transition to notReady or notInService.  If we are notReady,
     * transition to notInService whenever enough information becomes
     * available to the agent. */
    if(data->srCommunityStatus == D_srCommunityStatus_createAndWait
    || data->srCommunityStatus == D_srCommunityStatus_notReady) {
        /* is the entry ready? */
        if(srCommunityEntryReadyToActivate(data) != 0) {
            /* yes, it can go to notInService */
            data->srCommunityStatus = D_srCommunityStatus_notInService;
        } else {
            /* no, it isn't ready yet */
            data->srCommunityStatus = D_srCommunityStatus_notReady;
        }
    }

    if (newrow->srCommunityMemoryType >= NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    } else if (data->srCommunityMemoryType >= NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    }

    /* copy values from the scratch copy to live data */
    TransferEntries(I_srCommunityStatus, srCommunityEntryTypeTable, (void *) newrow, (void *) data);
    SET_ALL_VALID(newrow->valid);

    /* if the new row entry is now valid, cancel the creation timeout */
    if (newrow->srCommunityStatus == D_srCommunityStatus_active && newrow->RowStatusTimerId != -1) {
        CancelRowStatusTimeout(newrow->RowStatusTimerId);
        newrow->RowStatusTimerId = -1;
    } else
    if ((newrow->srCommunityStatus == D_srCommunityStatus_notInService || newrow->srCommunityStatus == D_srCommunityStatus_notReady)
    && newrow->RowStatusTimerId == -1) {
        /* we are transitioning to an inactive state.  Set a timer to delete
         * the row after an appropriate interval */
        newrow->RowStatusTimerId = SetRowStatusTimeoutWithCI(MAX_ROW_CREATION_TIME, contextInfo, (void *) newrow, (TodCallbackFunction) srCommunityTableDeleteCallback);
    }

    return NO_ERROR;

}

#ifdef SR_srCommunityEntry_UNDO
/* add #define SR_srCommunityEntry_UNDO in sitedefs.h to
 * include the undo routine for the srCommunityEntry family.
 */
int
srCommunityEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    srCommunityEntry_t *data = (srCommunityEntry_t *) doCur->data;
    srCommunityEntry_t *undodata = (srCommunityEntry_t *) doCur->undodata;
    srCommunityEntry_t *setdata = NULL;
    int function;

    /* we are either trying to undo an add, a delete, or a modify */
    if (undodata == NULL) {
        /* undoing an add, so delete */
        data->srCommunityStatus = D_srCommunityStatus_destroy;
        setdata = data;
        function = SR_DELETE;
    } else {
        /* undoing a delete or modify, replace the original data */
        if(undodata->srCommunityStatus == D_srCommunityStatus_notReady
        || undodata->srCommunityStatus == D_srCommunityStatus_notInService) {
            undodata->srCommunityStatus = D_srCommunityStatus_createAndWait;
        } else 
        if(undodata->srCommunityStatus == D_srCommunityStatus_active) {
            undodata->srCommunityStatus = D_srCommunityStatus_createAndGo;
        }
        setdata = undodata;
        function = SR_ADD_MODIFY;
    }

    /* use the set method for the undo */
    if ((setdata != NULL) && k_srCommunityEntry_set(setdata, contextInfo, function) == NO_ERROR) {
        return NO_ERROR;
    }

   return UNDO_FAILED_ERROR;
}
#endif /* SR_srCommunityEntry_UNDO */

#endif /* SETS */
