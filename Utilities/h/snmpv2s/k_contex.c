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
static const char rcsid[] = "$Id: k_contex.c,v 1.7.4.1 1998/03/19 14:21:19 partain Exp $";
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

#include "i_contex.h"

/* note: the following prototypes may be included automatically now.
         check for duplicates  -cws   */
int v2ContextEntryReadyToActivate
    SR_PROTOTYPE((v2ContextEntry_t *data));

void Deletev2ContextEntry 
    SR_PROTOTYPE((int index));

static SnmpV2Table *access_v2ContextTable;

/* initialize support for v2ContextEntry objects */
int
k_v2ContextEntry_initialize(
    SnmpV2Table *v2ContextTable)
{
    access_v2ContextTable = v2ContextTable;
    return 1;
}

/* terminate support for v2ContextEntry objects */
int
k_v2ContextEntry_terminate(void)
{
    access_v2ContextTable = NULL;
    return 1;
}

/* This routine is called by the timeout code to 
 * delete a pending creation of a Table entry */ 
void
v2ContextTableDeleteCallback (TimeOutDescriptor *tdp)
{
    v2ContextEntry_t *data;

    /* dummy up an entry to delete */
    data = (v2ContextEntry_t *) tdp->UserData2;
    data->v2ContextStatus = D_v2ContextStatus_destroy;
    data->RowStatusTimerId = -1;

    /* free the timeout descriptor */
    free(tdp);

    /* call the set method */
    k_v2ContextEntry_set(data, (ContextInfo *) NULL, 0);
}

/* This routine deletes an entry from the v2ContextTable */
void
Deletev2ContextEntry (index)
    int index;
{
    FreeEntries(v2ContextEntryTypeTable, access_v2ContextTable->tp[index]);
    free(access_v2ContextTable->tp[index]);
    RemoveTableEntry(access_v2ContextTable, index);
}

/* This routine returns nonzero if the table entry contains sufficient
 * information to be in the active (or notInService) state. */
int
v2ContextEntryReadyToActivate(data)
v2ContextEntry_t *data;
{
    /* by default, assume sufficient information */
    return(1);
}

#ifdef U_v2ContextEntry
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_v2ContextEntryFreeUserpartData (v2ContextEntry_t *data)
{
    /* nothing to free by default */
}

/* This routine can be used to clone data which
 * is defined in the userpart part of the structure */
void
k_v2ContextEntryCloneUserpartData (v2ContextEntry_t *dst, v2ContextEntry_t *src)
{
    /* nothing to clone by default */
}
#endif /* U_v2ContextEntry */

v2ContextEntry_t *
k_v2ContextEntry_get(serialNum, contextInfo, nominator, searchType, v2ContextSnmpID, v2ContextName)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   OctetString * v2ContextSnmpID;
   OctetString * v2ContextName;
{
    int index;

    access_v2ContextTable->tip[0].value.octet_val = v2ContextSnmpID;
    access_v2ContextTable->tip[1].value.octet_val = v2ContextName;
    if ((index = SearchTable(access_v2ContextTable, searchType)) == -1) {
        return NULL;
    }

    return (v2ContextEntry_t *) access_v2ContextTable->tp[index];

}

#ifdef SETS
int
k_v2ContextEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{
    if (object->nominator == I_v2ContextName) {
        if (CheckAuthName(value->os_value)) {
            return WRONG_VALUE_ERROR;
        }
    }

    return NO_ERROR;
}

int
k_v2ContextEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    int doListReady = 0;
    v2ContextEntry_t *data = (v2ContextEntry_t *) dp->data;

    /* no state information needed for destroy */
    if (data->v2ContextStatus == D_v2ContextStatus_destroy) {
        dp->state = SR_DELETE;
    } else 
    if (data->v2ContextStatus == RS_UNINITIALIZED) {
        /* status hasn't been set yet */
        dp->state = SR_UNKNOWN;
    } else {
        /* check row status state machine transitions */
        if(data->v2ContextStatus == D_v2ContextStatus_createAndGo) {
            data->v2ContextStatus = D_v2ContextStatus_active;
        } 
        doListReady = 1;

        /* if transitioning (or remaining) in the active or notInService states,
         * make sure that the new values are not inconsistent enough to 
         * make the entry notReady */
        if(data->v2ContextStatus == D_v2ContextStatus_active
        || data->v2ContextStatus == D_v2ContextStatus_notInService) {
            if(v2ContextEntryReadyToActivate(data) == 0) {
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
k_v2ContextEntry_set_defaults(dp)
    doList_t       *dp;
{
    v2ContextEntry_t *data = (v2ContextEntry_t *) (dp->data);

    if ((data->v2ContextLocalEntity = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    data->v2ContextLocalTime = D_v2ContextLocalTime_currentTime;

    data->v2ContextMemoryType = D_v2ContextMemoryType_volatile;

    data->RowStatusTimerId = -1;

    SET_ALL_VALID(data->valid);

    return NO_ERROR;
}

int
k_v2ContextEntry_set(data, contextInfo, function)
   v2ContextEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

    int index;
    v2ContextEntry_t *newrow = NULL;

    /* find this entry in the table */
    access_v2ContextTable->tip[0].value.octet_val = data->v2ContextSnmpID;
    access_v2ContextTable->tip[1].value.octet_val = data->v2ContextName;
    if ((index = SearchTable(access_v2ContextTable, EXACT)) != -1) {
        newrow = (v2ContextEntry_t *) access_v2ContextTable->tp[index];
    }

    /* perform the table entry operation on it */
    if (data->v2ContextStatus == D_v2ContextStatus_destroy) {
        if (data->RowStatusTimerId != -1) {
            CancelRowStatusTimeout(data->RowStatusTimerId);
        }

        if (index == -1) {
            return NO_ERROR;
        } else {
            /* delete the table entry */
            int flag = newrow->v2ContextMemoryType;
            Deletev2ContextEntry(index);
            if (flag >= NONVOLATILE) {
                writeConfigFileFlag = TRUE;
            }
            return NO_ERROR;
        }
    } else if (index == -1) {
        /* add the entry */
        access_v2ContextTable->tip[0].value.octet_val = data->v2ContextSnmpID;
        access_v2ContextTable->tip[1].value.octet_val = data->v2ContextName;
        if ((index = NewTableEntry(access_v2ContextTable)) == -1) {
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        newrow = (v2ContextEntry_t *) access_v2ContextTable->tp[index];

        /* set a timeout */
        if ((newrow->RowStatusTimerId = SetRowStatusTimeout(MAX_ROW_CREATION_TIME, (void *) newrow, (TodCallbackFunction) v2ContextTableDeleteCallback)) == -1) {
            Deletev2ContextEntry(index);
            return RESOURCE_UNAVAILABLE_ERROR;
        }
    }

    /* if we are executing a createAndWait command, determine whether
     * to transition to notReady or notInService.  If we are notReady,
     * transition to notInService whenever enough information becomes
     * available to the agent. */
    if(data->v2ContextStatus == D_v2ContextStatus_createAndWait
    || data->v2ContextStatus == D_v2ContextStatus_notReady) {
        /* is the entry ready? */
        if(v2ContextEntryReadyToActivate(data) != 0) {
            /* yes, it can go to notInService */
            data->v2ContextStatus = D_v2ContextStatus_notInService;
        } else {
            /* no, it isn't ready yet */
            data->v2ContextStatus = D_v2ContextStatus_notReady;
        }
    }

    if (newrow->v2ContextMemoryType >= NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    } else if (data->v2ContextMemoryType >= NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    }

    /* copy values from the scratch copy to live data */
    TransferEntries(I_v2ContextStatus, v2ContextEntryTypeTable, (void *) newrow, (void *) data);
    SET_ALL_VALID(newrow->valid);

    /* if the new row entry is now valid, cancel the creation timeout */
    if (newrow->v2ContextStatus == D_v2ContextStatus_active && newrow->RowStatusTimerId != -1) {
        CancelRowStatusTimeout(newrow->RowStatusTimerId);
        newrow->RowStatusTimerId = -1;
    } else
    if ((newrow->v2ContextStatus == D_v2ContextStatus_notInService || newrow->v2ContextStatus == D_v2ContextStatus_notReady)
    && newrow->RowStatusTimerId == -1) {
        /* we are transitioning to an inactive state.  Set a timer to delete
         * the row after an appropriate interval */
        newrow->RowStatusTimerId = SetRowStatusTimeout(MAX_ROW_CREATION_TIME, (void *) newrow, (TodCallbackFunction) v2ContextTableDeleteCallback);
    }

    return NO_ERROR;

}

#ifdef SR_v2ContextEntry_UNDO
/* add #define SR_v2ContextEntry_UNDO in sitedefs.h to
 * include the undo routine for the v2ContextEntry family.
 */
int
v2ContextEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    v2ContextEntry_t *data = (v2ContextEntry_t *) doCur->data;
    v2ContextEntry_t *undodata = (v2ContextEntry_t *) doCur->undodata;
    v2ContextEntry_t *setdata = NULL;
    int function;

    /* we are either trying to undo an add, a delete, or a modify */
    if (undodata == NULL) {
        /* undoing an add, so delete */
        data->v2ContextStatus = D_v2ContextStatus_destroy;
        setdata = data;
        function = SR_DELETE;
    } else {
        /* undoing a delete or modify, replace the original data */
        if(undodata->v2ContextStatus == D_v2ContextStatus_notReady
        || undodata->v2ContextStatus == D_v2ContextStatus_notInService) {
            undodata->v2ContextStatus = D_v2ContextStatus_createAndWait;
        } else 
        if(undodata->v2ContextStatus == D_v2ContextStatus_active) {
            undodata->v2ContextStatus = D_v2ContextStatus_createAndGo;
        }
        setdata = undodata;
        function = SR_ADD_MODIFY;
    }

    /* use the set method for the undo */
    if ((setdata != NULL) && k_v2ContextEntry_set(setdata, contextInfo, function) == NO_ERROR) {
        return NO_ERROR;
    }

   return UNDO_FAILED_ERROR;
}
#endif /* SR_v2ContextEntry_UNDO */

#endif /* SETS */
