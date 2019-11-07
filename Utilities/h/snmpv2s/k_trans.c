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
static const char rcsid[] = "$Id: k_trans.c,v 1.7.4.1 1998/03/19 14:21:20 partain Exp $";
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
#include "v2init.h"
#include "oid_lib.h"
#include "tmq.h"
#include "rowstatf.h"
#include "i_trans.h"
#include "sr_msg.h"
#include "v2_msg.h"
#include "userauth.h"
#include "tdomain.h"

static v2AdminTransport_t *access_v2AdminTransportData;

static SnmpV2Table *access_transportTable;

void DeletetransportEntry SR_PROTOTYPE((int index));

/* initialize support for v2AdminTransport objects */
int
k_v2AdminTransport_initialize(
v2AdminTransport_t *v2AdminTransportData)
{
    access_v2AdminTransportData = v2AdminTransportData;
    SET_ALL_VALID(v2AdminTransportData->valid);
    return 1;
}

/* terminate support for v2AdminTransport objects */
int
k_v2AdminTransport_terminate(void)
{
    access_v2AdminTransportData = NULL;
    return 1;
}

/* initialize support for transportEntry objects */
int
k_transportEntry_initialize(
    SnmpV2Table *transportTable)
{
    access_transportTable = transportTable;
    return 1;
}

/* terminate support for transportEntry objects */
int
k_transportEntry_terminate(void)
{
    access_transportTable = NULL;
    return 1;
}

#ifdef U_v2AdminTransport
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_v2AdminTransportFreeUserpartData (data)
    v2AdminTransport_t *data;
{
    /* nothing to free by default */
}
#endif /* U_v2AdminTransport */

v2AdminTransport_t *
k_v2AdminTransport_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
   return(access_v2AdminTransportData);
}

#ifdef SETS
int
k_v2AdminTransport_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{
    if (value->sl_value != access_v2AdminTransportData->transportSpinLock) {
        return INCONSISTENT_VALUE_ERROR;
    }
    return NO_ERROR;
}

int
k_v2AdminTransport_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_v2AdminTransport_set(data, contextInfo, function)
   v2AdminTransport_t *data;
   ContextInfo *contextInfo;
   int function;
{
    access_v2AdminTransportData->transportSpinLock++;
    return NO_ERROR;
}

#ifdef SR_v2AdminTransport_UNDO
/* add #define SR_v2AdminTransport_UNDO in sitedefs.h to
 * include the undo routine for the v2AdminTransport family.
 */
int
v2AdminTransport_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_v2AdminTransport_UNDO */

#endif /* SETS */

/* This routine is called by the timeout code to 
 * delete a pending creation of a Table entry */ 
void
transportTableDeleteCallback (TimeOutDescriptor *tdp)
{
    transportEntry_t *data;

    /* dummy up an entry to delete */
    data = (transportEntry_t *) tdp->UserData2;
    data->transportStatus = D_transportStatus_destroy;
    data->RowStatusTimerId = -1;

    /* free the timeout descriptor */
    free(tdp);

    /* call the set method */
    k_transportEntry_set(data, (ContextInfo *) NULL, 0);
}

/* This routine deletes an entry from the transportTable */
void
DeletetransportEntry (index)
    int index;
{
    FreeEntries(transportEntryTypeTable, access_transportTable->tp[index]);
    free(access_transportTable->tp[index]);
    RemoveTableEntry(access_transportTable, index);
}

/* This routine returns nonzero if the table entry contains sufficient
 * information to be in the active (or notInService) state. */
int
transportEntryReadyToActivate(transportEntry_t *data)
{
    if (data->transportDomain == NULL) {
        return 0;
    }

    if ((CmpOID(data->transportDomain, snmpUDPDomainOID) == 0) ||
        (CmpOID(data->transportDomain, rfc1157DomainOID) == 0)) {
        if (data->transportAddress == NULL) {
            return 0;
        }
        if (data->transportAddress->length != 6) {
            return 0;
        }
        if (data->transportReceiveMask == NULL) {
            return 0;
        }
        if (data->transportReceiveMask->length != 6) {
            return 0;
        }
        return 1;
    }

    return 0;
}

#ifdef U_transportEntry
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_transportEntryFreeUserpartData (data)
    transportEntry_t *data;
{
    /* nothing to free by default */
}

/* This routine can be used to clone data which
 * is defined in the userpart part of the structure */
void
k_transportEntryCloneUserpartData (dst, src)
    transportEntry_t *dst, *src;
{
    /* nothing to clone by default */
}
#endif /* U_transportEntry */

transportEntry_t *
k_transportEntry_get(serialNum, contextInfo, nominator, searchType, transportLabel, transportSubindex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   OctetString * transportLabel;
   SR_INT32 transportSubindex;
{
    int index;

    access_transportTable->tip[0].value.octet_val = transportLabel;
    access_transportTable->tip[1].value.uint_val = transportSubindex;
    if ((index = SearchTable(access_transportTable, searchType)) == -1) {
        return NULL;
    }

    return (transportEntry_t *) access_transportTable->tp[index];

}

#ifdef SETS
int
k_transportEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_transportEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    int doListReady = 0;
    transportEntry_t *data = (transportEntry_t *) dp->data;

    /* no state information needed for destroy */
    if (data->transportStatus == D_transportStatus_destroy) {
        dp->state = SR_DELETE;
    } else 
    if (data->transportStatus == RS_UNINITIALIZED) {
        /* status hasn't been set yet */
        dp->state = SR_UNKNOWN;
    } else {
        /* check row status state machine transitions */
        if(data->transportStatus == D_transportStatus_createAndGo) {
            data->transportStatus = D_transportStatus_active;
        } 
        doListReady = 1;

        /* if transitioning (or remaining) in the active or notInService states,
         * make sure that the new values are not inconsistent enough to 
         * make the entry notReady */
        if(data->transportStatus == D_transportStatus_active
        || data->transportStatus == D_transportStatus_notInService) {
            if(transportEntryReadyToActivate(data) == 0) {
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
k_transportEntry_set_defaults(dp)
    doList_t       *dp;
{
    transportEntry_t *data = (transportEntry_t *) (dp->data);

    if ((data->transportDomain = MakeOIDFromDot("0.0")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    if ((data->transportAddress = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    if ((data->transportReceiveMask = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    data->transportMemoryType = D_transportMemoryType_volatile;

    data->RowStatusTimerId = -1;

    SET_ALL_VALID(data->valid);

    return NO_ERROR;
}

int
k_transportEntry_set(data, contextInfo, function)
   transportEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

    int index;
    transportEntry_t *newtrans = NULL;

    /* find this entry in the table */
    access_transportTable->tip[0].value.octet_val = data->transportLabel;
    access_transportTable->tip[1].value.uint_val = data->transportSubindex;
    if ((index = SearchTable(access_transportTable, EXACT)) != -1) {
        newtrans = (transportEntry_t *) access_transportTable->tp[index];
    }

    /* perform the table entry operation on it */
    if (data->transportStatus == D_transportStatus_destroy) {
        if (data->RowStatusTimerId != -1) {
            CancelRowStatusTimeout(data->RowStatusTimerId);
        }

        if (index == -1) {
            return NO_ERROR;
        } else {
            int flag = newtrans->transportMemoryType;
            /* delete the table entry */
            DeletetransportEntry(index);
            if (flag >= NONVOLATILE) {
                writeConfigFileFlag = TRUE;
            }
            return NO_ERROR;
        }
    } else if (index == -1) {
        /* add the entry */
        access_transportTable->tip[0].value.octet_val = data->transportLabel;
        access_transportTable->tip[1].value.uint_val = data->transportSubindex;
        if ((index = NewTableEntry(access_transportTable)) == -1) {
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        newtrans = (transportEntry_t *) access_transportTable->tp[index];


        /* set a timeout */
        if ((newtrans->RowStatusTimerId = SetRowStatusTimeout(MAX_ROW_CREATION_TIME, (void *) newtrans, (TodCallbackFunction) transportTableDeleteCallback)) == -1) {
            DeletetransportEntry(index);
            return RESOURCE_UNAVAILABLE_ERROR;
        }
    }

    /* if we are executing a createAndWait command, determine whether
     * to transition to notReady or notInService.  If we are notReady,
     * transition to notInService whenever enough information becomes
     * available to the agent. */
    if(data->transportStatus == D_transportStatus_createAndWait
    || data->transportStatus == D_transportStatus_notReady) {
        /* is the entry ready? */
        if(transportEntryReadyToActivate(data) != 0) {
            /* yes, it can go to notInService */
            data->transportStatus = D_transportStatus_notInService;
        } else {
            /* no, it isn't ready yet */
            data->transportStatus = D_transportStatus_notReady;
        }
    }

    if (newtrans->transportMemoryType >= NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    } else if (data->transportMemoryType >= NONVOLATILE) {
        writeConfigFileFlag = TRUE;
    }

    /* copy values from the scratch copy to live data */
    TransferEntries(I_transportStatus, transportEntryTypeTable, (void *) newtrans, (void *) data);
    SET_ALL_VALID(newtrans->valid);

    /* if the new row entry is now valid, cancel the creation timeout */
    if (newtrans->transportStatus == D_transportStatus_active && newtrans->RowStatusTimerId != -1) {
        CancelRowStatusTimeout(newtrans->RowStatusTimerId);
        newtrans->RowStatusTimerId = -1;
    } else
    if ((newtrans->transportStatus == D_transportStatus_notInService || newtrans->transportStatus == D_transportStatus_notReady)
    && newtrans->RowStatusTimerId == -1) {
        /* we are transitioning to an inactive state.  Set a timer to delete
         * the row after an appropriate interval */
        newtrans->RowStatusTimerId = SetRowStatusTimeout(MAX_ROW_CREATION_TIME, (void *) newtrans, (TodCallbackFunction) transportTableDeleteCallback);
    }

    return NO_ERROR;

}

#ifdef SR_transportEntry_UNDO
/* add #define SR_transportEntry_UNDO in sitedefs.h to
 * include the undo routine for the transportEntry family.
 */
int
transportEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    transportEntry_t *data = (transportEntry_t *) doCur->data;
    transportEntry_t *undodata = (transportEntry_t *) doCur->undodata;
    transportEntry_t *setdata = NULL;
    int function;

    /* we are either trying to undo an add, a delete, or a modify */
    if (undodata == NULL) {
        /* undoing an add, so delete */
        data->transportStatus = D_transportStatus_destroy;
        setdata = data;
        function = SR_DELETE;
    } else {
        /* undoing a delete or modify, replace the original data */
        if(undodata->transportStatus == D_transportStatus_notReady
        || undodata->transportStatus == D_transportStatus_notInService) {
            undodata->transportStatus = D_transportStatus_createAndWait;
        } else 
        if(undodata->transportStatus == D_transportStatus_active) {
            undodata->transportStatus = D_transportStatus_createAndGo;
        }
        setdata = undodata;
        function = SR_ADD_MODIFY;
    }

    /* use the set method for the undo */
    if ((setdata != NULL) && k_transportEntry_set(setdata, contextInfo, function) == NO_ERROR) {
        return NO_ERROR;
    }

   return UNDO_FAILED_ERROR;
}
#endif /* SR_transportEntry_UNDO */

#endif /* SETS */
