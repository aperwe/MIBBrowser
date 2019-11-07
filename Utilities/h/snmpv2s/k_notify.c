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
static const char rcsid[] = "$Id: k_notify.c,v 1.7.4.1 1998/03/19 14:21:19 partain Exp $";
#endif	/* (! ( defined(lint) ) && defined(SR_RCSID)) */

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#include <sys/types.h>

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif  /* HAVE_MEMORY_H */

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
#include "i_notify.h"

/* note: the following prototypes may be included automatically now.
         check for duplicates  -cws   */
int notifyEntryReadyToActivate
     SR_PROTOTYPE((notifyEntry_t *data));


static v2AdminNotify_t *access_v2AdminNotifyData;

static SnmpV2Table *access_notifyTable;

void DeletenotifyEntry SR_PROTOTYPE((int index));

/* initialize support for v2AdminNotify objects */
int
k_v2AdminNotify_initialize(
    v2AdminNotify_t *v2AdminNotifyData)
{
    access_v2AdminNotifyData = v2AdminNotifyData;
    SET_ALL_VALID(v2AdminNotifyData->valid);
    return 1;
}

/* terminate support for v2AdminNotify objects */
int
k_v2AdminNotify_terminate(void)
{
    access_v2AdminNotifyData = NULL;
    return 1;
}

/* initialize support for notifyEntry objects */
int
k_notifyEntry_initialize(
    SnmpV2Table *notifyTable)
{
    access_notifyTable = notifyTable;
    return 1;
}

/* terminate support for notifyEntry objects */
int
k_notifyEntry_terminate(void)
{
    access_notifyTable = NULL;
    return 1;
}

#ifdef U_v2AdminNotify
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_v2AdminNotifyFreeUserpartData (data)
    v2AdminNotify_t *data;
{
    /* nothing to free by default */
}
#endif /* U_v2AdminNotify */

v2AdminNotify_t *
k_v2AdminNotify_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
   return(access_v2AdminNotifyData);
}

#ifdef SETS
int
k_v2AdminNotify_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{
    if (value->sl_value != access_v2AdminNotifyData->notifySpinLock) {
        return INCONSISTENT_VALUE_ERROR;
    }
    return NO_ERROR;
}

int
k_v2AdminNotify_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_v2AdminNotify_set(data, contextInfo, function)
   v2AdminNotify_t *data;
   ContextInfo *contextInfo;
   int function;
{
   if (data->notifySpinLock == 2147483647) {
       access_v2AdminNotifyData->notifySpinLock = 0;
   } else {
       access_v2AdminNotifyData->notifySpinLock = data->notifySpinLock + 1;
   }
   return NO_ERROR;
}

#ifdef SR_v2AdminNotify_UNDO
/* add #define SR_v2AdminNotify_UNDO in sitedefs.h to
 * include the undo routine for the v2AdminNotify family.
 */
int
v2AdminNotify_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_v2AdminNotify_UNDO */

#endif /* SETS */

/* This routine is called by the timeout code to 
 * delete a pending creation of a Table entry */ 
void
notifyTableDeleteCallback (TimeOutDescriptor *tdp)
{
    notifyEntry_t *data;

    /* dummy up an entry to delete */
    data = (notifyEntry_t *) tdp->UserData2;
    data->notifyStatus = D_notifyStatus_destroy;
    data->RowStatusTimerId = -1;

    /* free the timeout descriptor */
    free(tdp);

    /* call the set method */
    k_notifyEntry_set(data, (ContextInfo *) NULL, 0);
}

/* This routine deletes an entry from the notifyTable */
void
DeletenotifyEntry (index)
    int index;
{
    FreeEntries(notifyEntryTypeTable, access_notifyTable->tp[index]);
    free(access_notifyTable->tp[index]);
    RemoveTableEntry(access_notifyTable, index);
}

/* This routine returns nonzero if the table entry contains sufficient
 * information to be in the active (or notInService) state. */
int
notifyEntryReadyToActivate(data)
notifyEntry_t *data;
{
    /* by default, assume sufficient information */
    return(1);
}

#ifdef U_notifyEntry
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_notifyEntryFreeUserpartData (data)
    notifyEntry_t *data;
{
    /* nothing to free by default */
}

/* This routine can be used to clone data which
 * is defined in the userpart part of the structure */
void
k_notifyEntryCloneUserpartData (dst, src)
    notifyEntry_t *dst, *src;
{
    /* nothing to clone by default */
}
#endif /* U_notifyEntry */

notifyEntry_t *
k_notifyEntry_get(serialNum, contextInfo, nominator, searchType, notifyIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   SR_INT32 notifyIndex;
{
    int index;

    access_notifyTable->tip[0].value.uint_val = notifyIndex;
    if ((index = SearchTable(access_notifyTable, searchType)) == -1) {
        return NULL;
    }

    return (notifyEntry_t *) access_notifyTable->tp[index];

}

#ifdef SETS
int
k_notifyEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{
#ifdef I_notifyAuthSnmpID
    if (object->nominator == I_notifyAuthSnmpID) {
        if (value->os_value->length != 12) {
            return WRONG_VALUE_ERROR;
        }
    }
#endif /* I_notifyAuthSnmpID */

    if (object->nominator == I_notifyIdentityName) {
        if (CheckAuthName(value->os_value)) {
            return WRONG_VALUE_ERROR;
        }
    }

    if (object->nominator == I_notifyContextName) {
        if (CheckAuthName(value->os_value)) {
            return WRONG_VALUE_ERROR;
        }
    }

    if (object->nominator == I_notifyViewName) {
        if (CheckAuthName(value->os_value)) {
            return WRONG_VALUE_ERROR;
        }
    }

    return NO_ERROR;
}

int
k_notifyEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    int doListReady = 0;
    notifyEntry_t *data = (notifyEntry_t *) dp->data;

    /* no state information needed for destroy */
    if (data->notifyStatus == D_notifyStatus_destroy) {
        dp->state = SR_DELETE;
    } else 
    if (data->notifyStatus == RS_UNINITIALIZED) {
        /* status hasn't been set yet */
        dp->state = SR_UNKNOWN;
    } else {
        /* check row status state machine transitions */
        if(data->notifyStatus == D_notifyStatus_createAndGo) {
            data->notifyStatus = D_notifyStatus_active;
        } 
        doListReady = 1;

        /* if transitioning (or remaining) in the active or notInService states,
         * make sure that the new values are not inconsistent enough to 
         * make the entry notReady */
        if(data->notifyStatus == D_notifyStatus_active
        || data->notifyStatus == D_notifyStatus_notInService) {
            if(notifyEntryReadyToActivate(data) == 0) {
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
k_notifyEntry_set_defaults(dp)
    doList_t       *dp;
{
    notifyEntry_t *data = (notifyEntry_t *) (dp->data);

    if ((data->notifyIdentityName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    if ((data->notifyTransportLabel = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    if ((data->notifyContextName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    if ((data->notifyViewName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    data->notifyMemoryType = D_notifyMemoryType_volatile;

    data->RowStatusTimerId = -1;

    SET_ALL_VALID(data->valid);

    return NO_ERROR;
}

int
k_notifyEntry_set(data, contextInfo, function)
   notifyEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

    int index;
    notifyEntry_t *newrow = NULL;

    /* find this entry in the table */
    access_notifyTable->tip[0].value.uint_val = data->notifyIndex;
    if ((index = SearchTable(access_notifyTable, EXACT)) != -1) {
        newrow = (notifyEntry_t *) access_notifyTable->tp[index];
    }

    /* perform the table entry operation on it */
    if (data->notifyStatus == D_notifyStatus_destroy) {
        if (data->RowStatusTimerId != -1) {
            CancelRowStatusTimeout(data->RowStatusTimerId);
        }

        if (index == -1) {
            return NO_ERROR;
        } else {
            /* delete the table entry */
            int flag = newrow->notifyMemoryType;
            DeletenotifyEntry(index);
            if (flag >= NONVOLATILE) {
                writeConfigFileFlag = TRUE;
            }
            return NO_ERROR;
        }
    } else if (index == -1) {
        /* add the entry */
        access_notifyTable->tip[0].value.uint_val = data->notifyIndex;
        if ((index = NewTableEntry(access_notifyTable)) == -1) {
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        newrow = (notifyEntry_t *) access_notifyTable->tp[index];

        /* set a timeout */
        if ((newrow->RowStatusTimerId = SetRowStatusTimeout(MAX_ROW_CREATION_TIME, (void *) newrow, (TodCallbackFunction) notifyTableDeleteCallback)) == -1) {
            DeletenotifyEntry(index);
            return RESOURCE_UNAVAILABLE_ERROR;
        }
    }

    /* if we are executing a createAndWait command, determine whether
     * to transition to notReady or notInService.  If we are notReady,
     * transition to notInService whenever enough information becomes
     * available to the agent. */
    if(data->notifyStatus == D_notifyStatus_createAndWait
    || data->notifyStatus == D_notifyStatus_notReady) {
        /* is the entry ready? */
        if(notifyEntryReadyToActivate(data) != 0) {
            /* yes, it can go to notInService */
            data->notifyStatus = D_notifyStatus_notInService;
        } else {
            /* no, it isn't ready yet */
            data->notifyStatus = D_notifyStatus_notReady;
        }
    }

    if (newrow->notifyMemoryType >= NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    } else if (data->notifyMemoryType >= NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    }

    /* copy values from the scratch copy to live data */
    TransferEntries(I_notifyStatus, notifyEntryTypeTable, (void *) newrow, (void *) data);
    SET_ALL_VALID(newrow->valid);

    /* if the new row entry is now valid, cancel the creation timeout */
    if (newrow->notifyStatus == D_notifyStatus_active && newrow->RowStatusTimerId != -1) {
        CancelRowStatusTimeout(newrow->RowStatusTimerId);
        newrow->RowStatusTimerId = -1;
    } else
    if ((newrow->notifyStatus == D_notifyStatus_notInService || newrow->notifyStatus == D_notifyStatus_notReady)
    && newrow->RowStatusTimerId == -1) {
        /* we are transitioning to an inactive state.  Set a timer to delete
         * the row after an appropriate interval */
        newrow->RowStatusTimerId = SetRowStatusTimeout(MAX_ROW_CREATION_TIME, (void *) newrow, (TodCallbackFunction) notifyTableDeleteCallback);
    }

    return NO_ERROR;

}

#ifdef SR_notifyEntry_UNDO
/* add #define SR_notifyEntry_UNDO in sitedefs.h to
 * include the undo routine for the notifyEntry family.
 */
int
notifyEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    notifyEntry_t *data = (notifyEntry_t *) doCur->data;
    notifyEntry_t *undodata = (notifyEntry_t *) doCur->undodata;
    notifyEntry_t *setdata = NULL;
    int function;

    /* we are either trying to undo an add, a delete, or a modify */
    if (undodata == NULL) {
        /* undoing an add, so delete */
        data->notifyStatus = D_notifyStatus_destroy;
        setdata = data;
        function = SR_DELETE;
    } else {
        /* undoing a delete or modify, replace the original data */
        if(undodata->notifyStatus == D_notifyStatus_notReady
        || undodata->notifyStatus == D_notifyStatus_notInService) {
            undodata->notifyStatus = D_notifyStatus_createAndWait;
        } else 
        if(undodata->notifyStatus == D_notifyStatus_active) {
            undodata->notifyStatus = D_notifyStatus_createAndGo;
        }
        setdata = undodata;
        function = SR_ADD_MODIFY;
    }

    /* use the set method for the undo */
    if ((setdata != NULL) && k_notifyEntry_set(setdata, contextInfo, function) == NO_ERROR) {
        return NO_ERROR;
    }

   return UNDO_FAILED_ERROR;
}
#endif /* SR_notifyEntry_UNDO */

#endif /* SETS */

