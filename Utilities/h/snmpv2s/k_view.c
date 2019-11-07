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
static const char rcsid[] = "$Id: k_view.c,v 1.7.4.1 1998/03/19 14:21:20 partain Exp $";
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
#include "i_view.h"

/* note: the following prototypes may be included automatically now.
         check for duplicates  -cws   */
int viewTreeEntryReadyToActivate
     SR_PROTOTYPE((viewTreeEntry_t *data));

static v2AdminViewTree_t *access_v2AdminViewTreeData;

static SnmpV2Table *access_viewTreeTable;

void DeleteviewTreeEntry
    SR_PROTOTYPE((int index));

/* initialize support for v2AdminViewTree objects */
int
k_v2AdminViewTree_initialize(
    v2AdminViewTree_t *v2AdminViewTreeData)
{
    access_v2AdminViewTreeData = v2AdminViewTreeData;
    SET_ALL_VALID(v2AdminViewTreeData->valid);
    return 1;
}

/* terminate support for v2AdminViewTree objects */
int
k_v2AdminViewTree_terminate(void)
{
    access_v2AdminViewTreeData = NULL;
    return 1;
}

/* initialize support for viewTreeEntry objects */
int
k_viewTreeEntry_initialize(
    SnmpV2Table *viewTreeTable)
{
    access_viewTreeTable = viewTreeTable;
    return 1;
}

/* terminate support for viewTreeEntry objects */
int
k_viewTreeEntry_terminate(void)
{
    access_viewTreeTable = NULL;
    return 1;
}

#ifdef U_v2AdminViewTree
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_v2AdminViewTreeFreeUserpartData (data)
    v2AdminViewTree_t *data;
{
    /* nothing to free by default */
}

/* This routine can be used to clone data which
 * is defined in the userpart part of the structure */
void
k_v2AdminViewTreeCloneUserpartData (dst, src)
    v2AdminViewTree_t *dst, *src;
{
    /* nothing to clone by default */
}
#endif /* U_v2AdminViewTree */

v2AdminViewTree_t *
k_v2AdminViewTree_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
   return(access_v2AdminViewTreeData);
}

#ifdef SETS
int
k_v2AdminViewTree_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{
    if (value->sl_value != access_v2AdminViewTreeData->viewTreeSpinLock) {
        return INCONSISTENT_VALUE_ERROR;
    }
    return NO_ERROR;
}

int
k_v2AdminViewTree_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_v2AdminViewTree_set(data, contextInfo, function)
   v2AdminViewTree_t *data;
   ContextInfo *contextInfo;
   int function;
{
   if (data->viewTreeSpinLock == 2147483647) {
       access_v2AdminViewTreeData->viewTreeSpinLock = 0;
   } else {
       access_v2AdminViewTreeData->viewTreeSpinLock = data->viewTreeSpinLock + 1;
   }
   return NO_ERROR;
}

#ifdef SR_v2AdminViewTree_UNDO
/* add #define SR_v2AdminViewTree_UNDO in sitedefs.h to
 * include the undo routine for the v2AdminViewTree family.
 */
int
v2AdminViewTree_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_v2AdminViewTree_UNDO */

#endif /* SETS */

/* This routine is called by the timeout code to 
 * delete a pending creation of a Table entry */ 
void
viewTreeTableDeleteCallback (TimeOutDescriptor *tdp)
{
    viewTreeEntry_t *data;

    /* dummy up an entry to delete */
    data = (viewTreeEntry_t *) tdp->UserData2;
    data->viewTreeStatus = D_viewTreeStatus_destroy;
    data->RowStatusTimerId = -1;

    /* free the timeout descriptor */
    free(tdp);

    /* call the set method */
    k_viewTreeEntry_set(data, (ContextInfo *) NULL, 0);
}

/* This routine deletes an entry from the viewTreeTable */
void
DeleteviewTreeEntry (index)
    int index;
{
    FreeEntries(viewTreeEntryTypeTable, access_viewTreeTable->tp[index]);
    free(access_viewTreeTable->tp[index]);
    RemoveTableEntry(access_viewTreeTable, index);
}

/* This routine returns nonzero if the table entry contains sufficient
 * information to be in the active (or notInService) state. */
int
viewTreeEntryReadyToActivate(data)
viewTreeEntry_t *data;
{
    /* by default, assume sufficient information */
    return(1);
}

#ifdef U_viewTreeEntry
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_viewTreeEntryFreeUserpartData (viewTreeEntry_t *data)
{
    /* nothing to free by default */
}

/* This routine can be used to clone data which
 * is defined in the userpart part of the structure */
void
k_viewTreeEntryCloneUserpartData (viewTreeEntry_t *dst, viewTreeEntry_t *src)
{
    /* nothing to clone by default */
}
#endif /* U_viewTreeEntry */

viewTreeEntry_t *
k_viewTreeEntry_get(serialNum, contextInfo, nominator, searchType, viewTreeName, viewTreeSubTree)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   OctetString * viewTreeName;
   OID * viewTreeSubTree;
{
    int index;

    access_viewTreeTable->tip[0].value.octet_val = viewTreeName;
    access_viewTreeTable->tip[1].value.oid_val = viewTreeSubTree;
    if ((index = SearchTable(access_viewTreeTable, searchType)) == -1) {
        return NULL;
    }

    return (viewTreeEntry_t *) access_viewTreeTable->tp[index];

}

#ifdef SETS
int
k_viewTreeEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{
    if (object->nominator == I_viewTreeName) {
        if (CheckAuthName(value->os_value)) {
            return WRONG_VALUE_ERROR;
        }
    }

    return NO_ERROR;
}

int
k_viewTreeEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    int doListReady = 0;
    viewTreeEntry_t *data = (viewTreeEntry_t *) dp->data;

    /* no state information needed for destroy */
    if (data->viewTreeStatus == D_viewTreeStatus_destroy) {
        dp->state = SR_DELETE;
    } else 
    if (data->viewTreeStatus == RS_UNINITIALIZED) {
        /* status hasn't been set yet */
        dp->state = SR_UNKNOWN;
    } else {
        /* check row status state machine transitions */
        if(data->viewTreeStatus == D_viewTreeStatus_createAndGo) {
            data->viewTreeStatus = D_viewTreeStatus_active;
        } 
        doListReady = 1;

        /* if transitioning (or remaining) in the active or notInService states,
         * make sure that the new values are not inconsistent enough to 
         * make the entry notReady */
        if(data->viewTreeStatus == D_viewTreeStatus_active
        || data->viewTreeStatus == D_viewTreeStatus_notInService) {
            if(viewTreeEntryReadyToActivate(data) == 0) {
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
k_viewTreeEntry_set_defaults(dp)
    doList_t       *dp;
{
    viewTreeEntry_t *data = (viewTreeEntry_t *) (dp->data);

    if ((data->viewTreeMask = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    data->viewTreeType = D_viewTreeType_included;

    data->viewTreeMemoryType = D_viewTreeMemoryType_volatile;

    data->RowStatusTimerId = -1;

    SET_ALL_VALID(data->valid);

    return NO_ERROR;
}

int
k_viewTreeEntry_set(data, contextInfo, function)
   viewTreeEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

    int index;
    viewTreeEntry_t *newrow = NULL;

    /* find this entry in the table */
    access_viewTreeTable->tip[0].value.octet_val = data->viewTreeName;
    access_viewTreeTable->tip[1].value.oid_val = data->viewTreeSubTree;
    if ((index = SearchTable(access_viewTreeTable, EXACT)) != -1) {
        newrow = (viewTreeEntry_t *) access_viewTreeTable->tp[index];
    }

    /* perform the table entry operation on it */
    if (data->viewTreeStatus == D_viewTreeStatus_destroy) {
        if (data->RowStatusTimerId != -1) {
            CancelRowStatusTimeout(data->RowStatusTimerId);
        }

        if (index == -1) {
            return NO_ERROR;
        } else {
            /* delete the table entry */
            int flag = newrow->viewTreeMemoryType;
            DeleteviewTreeEntry(index);
            if (flag >= NONVOLATILE) {
                writeConfigFileFlag = TRUE;
            }
            return NO_ERROR;
        }
    } else if (index == -1) {
        /* add the entry */
        access_viewTreeTable->tip[0].value.octet_val = data->viewTreeName;
        access_viewTreeTable->tip[1].value.oid_val = data->viewTreeSubTree;
        if ((index = NewTableEntry(access_viewTreeTable)) == -1) {
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        newrow = (viewTreeEntry_t *) access_viewTreeTable->tp[index];

        /* set a timeout */
        if ((newrow->RowStatusTimerId = SetRowStatusTimeout(MAX_ROW_CREATION_TIME, (void *) newrow, (TodCallbackFunction) viewTreeTableDeleteCallback)) == -1) {
            DeleteviewTreeEntry(index);
            return RESOURCE_UNAVAILABLE_ERROR;
        }
    }

    /* if we are executing a createAndWait command, determine whether
     * to transition to notReady or notInService.  If we are notReady,
     * transition to notInService whenever enough information becomes
     * available to the agent. */
    if(data->viewTreeStatus == D_viewTreeStatus_createAndWait
    || data->viewTreeStatus == D_viewTreeStatus_notReady) {
        /* is the entry ready? */
        if(viewTreeEntryReadyToActivate(data) != 0) {
            /* yes, it can go to notInService */
            data->viewTreeStatus = D_viewTreeStatus_notInService;
        } else {
            /* no, it isn't ready yet */
            data->viewTreeStatus = D_viewTreeStatus_notReady;
        }
    }

    if (newrow->viewTreeMemoryType >= NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    } else if (data->viewTreeMemoryType >= NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    }

    /* copy values from the scratch copy to live data */
    TransferEntries(I_viewTreeStatus, viewTreeEntryTypeTable, (void *) newrow, (void *) data);
    SET_ALL_VALID(newrow->valid);

    /* if the new row entry is now valid, cancel the creation timeout */
    if (newrow->viewTreeStatus == D_viewTreeStatus_active && newrow->RowStatusTimerId != -1) {
        CancelRowStatusTimeout(newrow->RowStatusTimerId);
        newrow->RowStatusTimerId = -1;
    } else
    if ((newrow->viewTreeStatus == D_viewTreeStatus_notInService || newrow->viewTreeStatus == D_viewTreeStatus_notReady)
    && newrow->RowStatusTimerId == -1) {
        /* we are transitioning to an inactive state.  Set a timer to delete
         * the row after an appropriate interval */
        newrow->RowStatusTimerId = SetRowStatusTimeout(MAX_ROW_CREATION_TIME, (void *) newrow, (TodCallbackFunction) viewTreeTableDeleteCallback);
    }

    return NO_ERROR;

}

#ifdef SR_viewTreeEntry_UNDO
/* add #define SR_viewTreeEntry_UNDO in sitedefs.h to
 * include the undo routine for the viewTreeEntry family.
 */
int
viewTreeEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    viewTreeEntry_t *data = (viewTreeEntry_t *) doCur->data;
    viewTreeEntry_t *undodata = (viewTreeEntry_t *) doCur->undodata;
    viewTreeEntry_t *setdata = NULL;
    int function;

    /* we are either trying to undo an add, a delete, or a modify */
    if (undodata == NULL) {
        /* undoing an add, so delete */
        data->viewTreeStatus = D_viewTreeStatus_destroy;
        setdata = data;
        function = SR_DELETE;
    } else {
        /* undoing a delete or modify, replace the original data */
        if(undodata->viewTreeStatus == D_viewTreeStatus_notReady
        || undodata->viewTreeStatus == D_viewTreeStatus_notInService) {
            undodata->viewTreeStatus = D_viewTreeStatus_createAndWait;
        } else 
        if(undodata->viewTreeStatus == D_viewTreeStatus_active) {
            undodata->viewTreeStatus = D_viewTreeStatus_createAndGo;
        }
        setdata = undodata;
        function = SR_ADD_MODIFY;
    }

    /* use the set method for the undo */
    if ((setdata != NULL) && k_viewTreeEntry_set(setdata, contextInfo, function) == NO_ERROR) {
        return NO_ERROR;
    }

   return UNDO_FAILED_ERROR;
}
#endif /* SR_viewTreeEntry_UNDO */

#endif /* SETS */
