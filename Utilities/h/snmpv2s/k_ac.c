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
static const char rcsid[] = "$Id: k_ac.c,v 1.8.4.1 1998/03/19 14:21:18 partain Exp $";
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
#endif	/* HAVE_MALLOC_H */

#include <stddef.h>

#include "sr_type.h"
#include "sr_time.h"

#ifdef NOTICE
#undef NOTICE   /* DO NOT REMOVE */
#endif  /* NOTICE */
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

#include "i_ac.h"

/* note: the following prototypes may be included automatically now.
         check for duplicates  -cws   */
int acEntryReadyToActivate
     SR_PROTOTYPE((acEntry_t *data));

static v2AdminAccessControl_t *v2AdminAccessControlData;
static SnmpV2Table *acTable;

void DeleteacEntry 
    SR_PROTOTYPE((int index));

/* initialize support for v2AdminAccessControl objects */
int
k_v2AdminAccessControl_initialize(
    v2AdminAccessControl_t *v2aacd)
{
    v2AdminAccessControlData = v2aacd;
    return 1;
}

/* terminate support for v2AdminAccessControl objects */
int
k_v2AdminAccessControl_terminate(void)
{
    return 1;
}

/* initialize support for acEntry objects */
int
k_acEntry_initialize(
    SnmpV2Table *at)
{
    acTable = at;
    return 1;
}

/* terminate support for acEntry objects */
int
k_acEntry_terminate(void)
{
    acTable = NULL;
    return 1;
}

#ifdef U_v2AdminAccessControl
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_v2AdminAccessControlFreeUserpartData (data)
    v2AdminAccessControl_t *data;
{
    /* nothing to free by default */
}

/* This routine can be used to clone data which
 * is defined in the userpart part of the structure */
void
k_v2AdminAcessControlCloneUserpartData (dst, src)
    v2AdminAcessControl_t *dst, *src;
{
    /* nothing to clone by default */
}
#endif /* U_v2AdminAccessControl */

v2AdminAccessControl_t *
k_v2AdminAccessControl_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
   return(v2AdminAccessControlData);
}

#ifdef SETS
int
k_v2AdminAccessControl_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{
    if (value->sl_value != v2AdminAccessControlData->acSpinLock) {
        return INCONSISTENT_VALUE_ERROR;
    }
    return NO_ERROR;
}

int
k_v2AdminAccessControl_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_v2AdminAccessControl_set(data, contextInfo, function)
   v2AdminAccessControl_t *data;
   ContextInfo *contextInfo;
   int function;
{
   if (data->acSpinLock == 2147483647) {
       v2AdminAccessControlData->acSpinLock = 0;
   } else {
       v2AdminAccessControlData->acSpinLock = data->acSpinLock + 1;
   }
   return NO_ERROR;
}

#ifdef SR_v2AdminAccessControl_UNDO
/* add #define SR_v2AdminAccessControl_UNDO in sitedefs.h to
 * include the undo routine for the v2AdminAccessControl family.
 */
int
v2AdminAccessControl_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_v2AdminAccessControl_UNDO */

#endif /* SETS */

/* This routine is called by the timeout code to 
 * delete a pending creation of a Table entry */ 
void
acTableDeleteCallback (TimeOutDescriptor *tdp)
{
    acEntry_t *data;

    /* dummy up an entry to delete */
    data = (acEntry_t *) tdp->UserData2;
    data->acStatus = D_acStatus_destroy;
    data->RowStatusTimerId = -1;

    /* free the timeout descriptor */
    free(tdp);

    /* call the set method */
    k_acEntry_set(data, (ContextInfo *) NULL, 0);
}

/* This routine deletes an entry from the acTable */
void
DeleteacEntry (index)
    int index;
{
    FreeEntries(acEntryTypeTable, acTable->tp[index]);
    free(acTable->tp[index]);
    RemoveTableEntry(acTable, index);
}

/* This routine returns nonzero if the table entry contains sufficient
 * information to be in the active (or notInService) state. */
int
acEntryReadyToActivate(data)
acEntry_t *data;
{
    /* by default, assume sufficient information */
    return(1);
}

#ifdef U_acEntry
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_acEntryFreeUserpartData (acEntry_t *data)
{
    /* nothing to free by default */
}

/* This routine can be used to clone data which
 * is defined in the userpart part of the structure */
void
k_acEntryCloneUserpartData (acEntry_t *dst, acEntry_t *src)
{
    /* nothing to clone by default */
}
#endif /* U_acEntry */

acEntry_t *
k_acEntry_get(serialNum, contextInfo, nominator, searchType, acSPI, acGroupName, acContextName)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   SR_INT32 acSPI;
   OctetString * acGroupName;
   OctetString * acContextName;
{
    int index;

    acTable->tip[0].value.uint_val = acSPI;
    acTable->tip[1].value.octet_val = acGroupName;
    acTable->tip[2].value.octet_val = acContextName;
    if ((index = SearchTable(acTable, searchType)) == -1) {
        return NULL;
    }

    return (acEntry_t *) acTable->tp[index];

}

#ifdef SETS
int
k_acEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{
    if (object->nominator == I_acGroupName) {
        if (CheckAuthName(value->os_value)) {
            return WRONG_VALUE_ERROR;
        }
    }

    if (object->nominator == I_acContextName) {
        if (CheckAuthName(value->os_value)) {
            return WRONG_VALUE_ERROR;
        }
    }

    if (object->nominator == I_acReadViewName) {
        if (CheckAuthName(value->os_value)) {
            return WRONG_VALUE_ERROR;
        }
    }

    if (object->nominator == I_acWriteViewName) {
        if (CheckAuthName(value->os_value)) {
            return WRONG_VALUE_ERROR;
        }
    }

    return NO_ERROR;
}

int
k_acEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    int doListReady = 0;
    acEntry_t *data = (acEntry_t *) dp->data;

    /* no state information needed for destroy */
    if (data->acStatus == D_acStatus_destroy) {
        dp->state = SR_DELETE;
    } else 
    if (data->acStatus == RS_UNINITIALIZED) {
        /* status hasn't been set yet */
        dp->state = SR_UNKNOWN;
    } else {
        /* check row status state machine transitions */
        if(data->acStatus == D_acStatus_createAndGo) {
            data->acStatus = D_acStatus_active;
        } 
        doListReady = 1;

        /* if transitioning (or remaining) in the active or notInService states,
         * make sure that the new values are not inconsistent enough to 
         * make the entry notReady */
        if(data->acStatus == D_acStatus_active
        || data->acStatus == D_acStatus_notInService) {
            if(acEntryReadyToActivate(data) == 0) {
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
k_acEntry_set_defaults(dp)
    doList_t       *dp;
{
    acEntry_t *data = (acEntry_t *) (dp->data);

    if ((data->acContextNameMask = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    if ((data->acReadViewName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    if ((data->acWriteViewName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    data->acMemoryType = D_acMemoryType_volatile;

    data->RowStatusTimerId = -1;

    SET_ALL_VALID(data->valid);

    return NO_ERROR;
}

int
k_acEntry_set(data, contextInfo, function)
   acEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

    int index;
    acEntry_t *newrow = NULL;

    /* find this entry in the table */
    acTable->tip[0].value.uint_val = data->acSPI;
    acTable->tip[1].value.octet_val = data->acGroupName;
    acTable->tip[2].value.octet_val = data->acContextName;
    if ((index = SearchTable(acTable, EXACT)) != -1) {
        newrow = (acEntry_t *) acTable->tp[index];
    }

    /* perform the table entry operation on it */
    if (data->acStatus == D_acStatus_destroy) {
        if (data->RowStatusTimerId != -1) {
            CancelRowStatusTimeout(data->RowStatusTimerId);
        }

        if (index == -1) {
            return NO_ERROR;
        } else {
            int flag = newrow->acMemoryType;
            /* delete the table entry */
            DeleteacEntry(index);
            if (flag >= NONVOLATILE) {
                writeConfigFileFlag = TRUE;
            }
            return NO_ERROR;
        }
    } else if (index == -1) {
        /* add the entry */
        acTable->tip[0].value.uint_val = data->acSPI;
        acTable->tip[1].value.octet_val = data->acGroupName;
        acTable->tip[2].value.octet_val = data->acContextName;
        if ((index = NewTableEntry(acTable)) == -1) {
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        newrow = (acEntry_t *) acTable->tp[index];

        /* set a timeout */
        if ((newrow->RowStatusTimerId = SetRowStatusTimeout(MAX_ROW_CREATION_TIME, (void *) newrow, (TodCallbackFunction) acTableDeleteCallback)) == -1) {
            DeleteacEntry(index);
            return RESOURCE_UNAVAILABLE_ERROR;
        }
    }

    /* if we are executing a createAndWait command, determine whether
     * to transition to notReady or notInService.  If we are notReady,
     * transition to notInService whenever enough information becomes
     * available to the agent. */
    if(data->acStatus == D_acStatus_createAndWait
    || data->acStatus == D_acStatus_notReady) {
        /* is the entry ready? */
        if(acEntryReadyToActivate(data) != 0) {
            /* yes, it can go to notInService */
            data->acStatus = D_acStatus_notInService;
        } else {
            /* no, it isn't ready yet */
            data->acStatus = D_acStatus_notReady;
        }
    }

    if (newrow->acMemoryType >= NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    } else if (data->acMemoryType >= NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    }

    /* copy values from the scratch copy to live data */
    TransferEntries(I_acStatus, acEntryTypeTable, (void *) newrow, (void *) data);
    SET_ALL_VALID(newrow->valid);

    /* if the new row entry is now valid, cancel the creation timeout */
    if (newrow->acStatus == D_acStatus_active && newrow->RowStatusTimerId != -1) {
        CancelRowStatusTimeout(newrow->RowStatusTimerId);
        newrow->RowStatusTimerId = -1;
    } else
    if ((newrow->acStatus == D_acStatus_notInService || newrow->acStatus == D_acStatus_notReady)
    && newrow->RowStatusTimerId == -1) {
        /* we are transitioning to an inactive state.  Set a timer to delete
         * the row after an appropriate interval */
        newrow->RowStatusTimerId = SetRowStatusTimeout(MAX_ROW_CREATION_TIME, (void *) newrow, (TodCallbackFunction) acTableDeleteCallback);
    }

    return NO_ERROR;

}

#ifdef SR_acEntry_UNDO
/* add #define SR_acEntry_UNDO in sitedefs.h to
 * include the undo routine for the acEntry family.
 */
int
acEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    acEntry_t *data = (acEntry_t *) doCur->data;
    acEntry_t *undodata = (acEntry_t *) doCur->undodata;
    acEntry_t *setdata = NULL;
    int function;

    /* we are either trying to undo an add, a delete, or a modify */
    if (undodata == NULL) {
        /* undoing an add, so delete */
        data->acStatus = D_acStatus_destroy;
        setdata = data;
        function = SR_DELETE;
    } else {
        /* undoing a delete or modify, replace the original data */
        if(undodata->acStatus == D_acStatus_notReady
        || undodata->acStatus == D_acStatus_notInService) {
            undodata->acStatus = D_acStatus_createAndWait;
        } else 
        if(undodata->acStatus == D_acStatus_active) {
            undodata->acStatus = D_acStatus_createAndGo;
        }
        setdata = undodata;
        function = SR_ADD_MODIFY;
    }

    /* use the set method for the undo */
    if ((setdata != NULL) && k_acEntry_set(setdata, contextInfo, function) == NO_ERROR) {
        return NO_ERROR;
    }

   return UNDO_FAILED_ERROR;
}
#endif /* SR_acEntry_UNDO */

#endif /* SETS */
