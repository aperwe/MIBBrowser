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
static const char rcsid[] = "$Id: k_usec.c,v 1.7.4.1 1998/03/19 14:21:20 partain Exp $";
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
#include "sr_user.h"
#include "prnt_lib.h"
#include "global.h"
#ifndef SR_UNSECURABLE
#include "md5.h"
#endif /* SR_UNSECURABLE */
#include "tmq.h"
#include "rowstatf.h"
#include "i_usec.h"
#include "mapctx.h"

#include "sr_ad.h"

/* note: the following prototypes may be included automatically now.
         check for duplicates  -cws   */
int userNameEntryReadyToActivate
     SR_PROTOTYPE((userNameEntry_t *data));

#ifndef SR_UNSECURABLE
OctetString *UpdateSecret
     SR_PROTOTYPE((OctetString *change, OctetString *secret));
#endif /* SR_UNSECURABLE */

void userNameTableDeleteCallback
    SR_PROTOTYPE((TimeOutDescriptor *tdp));

contextMappingList_t contextMappingList_usecUserData = { NULL, 0, NULL };
contextMappingList_t contextMappingList_usecStatsData = { NULL, 0, NULL };
contextMappingList_t contextMappingList_usecScalarsData = { NULL, 0, NULL };
contextMappingList_t contextMappingList_userNameTable = { NULL, 0, NULL };

static usecUser_t *access_usecUserData;
static usecStats_t *access_usecStatsData;
static usecScalars_t *access_usecScalarsData;
static SnmpV2Table *access_userNameTable;

#define MIN_CLONEFROM_LENGTH LNuserGroupName + 1 + 12 + 1

void DeleteuserNameEntry SR_PROTOTYPE((int index));

/* initialize support for usecScalars objects */
int
k_usecScalars_initialize(
    char *contextName_text,
    usecScalars_t *usecScalarsData)
{
    SET_ALL_VALID(usecScalarsData->valid);
    if (contextName_text == NULL) {
        AddContextMappingText(&contextMappingList_usecScalarsData,
                              NULL,
                              usecScalarsData);
        AddContextMappingText(&contextMappingList_usecScalarsData,
                              "default",
                              usecScalarsData);
    } else {
        AddContextMappingText(&contextMappingList_usecScalarsData,
                              contextName_text,
                              usecScalarsData);
    }

    return 1;
}

/* terminate support for usecScalars objects */
int
k_usecScalars_terminate(void)
{
    return 1;
}

/* initialize support for usecStats objects */
int
k_usecStats_initialize(
    char *contextName_text,
    usecStats_t *usecStatsData)
{
    SET_ALL_VALID(usecStatsData->valid);
    if (contextName_text == NULL) {
        AddContextMappingText(&contextMappingList_usecStatsData,
                              NULL,
                              usecStatsData);
        AddContextMappingText(&contextMappingList_usecStatsData,
                              "default",
                              usecStatsData);
    } else {
        AddContextMappingText(&contextMappingList_usecStatsData,
                              contextName_text,
                              usecStatsData);
    }

    return 1;
}

/* terminate support for usecStats objects */
int
k_usecStats_terminate(void)
{
    return 1;
}

/* initialize support for usecUser objects */
int
k_usecUser_initialize(
    char *contextName_text,
    usecUser_t *usecUserData)
{
    SET_ALL_VALID(usecUserData->valid);
    if (contextName_text == NULL) {
        AddContextMappingText(&contextMappingList_usecUserData,
                              NULL,
                              usecUserData);
        AddContextMappingText(&contextMappingList_usecUserData,
                              "default",
                              usecUserData);
    } else {
        AddContextMappingText(&contextMappingList_usecUserData,
                              contextName_text,
                              usecUserData);
    }

    return 1;
}

/* terminate support for usecUser objects */
int
k_usecUser_terminate(void)
{
    return 1;
}

/* initialize support for userNameEntry objects */
int
k_userNameEntry_initialize(
    char *contextName_text,
    SnmpV2Table *userNameTable)
{
    if (contextName_text == NULL) {
        AddContextMappingText(&contextMappingList_userNameTable,
                              NULL,
                              userNameTable);
        AddContextMappingText(&contextMappingList_userNameTable,
                              "default",
                              userNameTable);
    } else {
        AddContextMappingText(&contextMappingList_userNameTable,
                              contextName_text,
                              userNameTable);
    }

    return 1;
}

/* terminate support for userNameEntry objects */
int
k_userNameEntry_terminate(void)
{
    return 1;
}

#ifdef U_usecScalars
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_usecScalarsFreeUserpartData (data)
    usecScalars_t *data;
{
    /* nothing to free by default */
}
#endif /* U_usecScalars */

#ifdef U_usecScalars
/* This routine can be used to clone data which
 * is defined in the userpart part of the structure */
void
k_usecScalarsCloneUserpartData (dst, src)
    usecScalars_t *dst, *src;
{
    /* nothing to clone by default */
}
#endif /* U_usecScalars */

usecScalars_t *
k_usecScalars_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
    static int prev_serialNum = -1;
    static int prev_serialNum_initialized = 0;

    if (!prev_serialNum_initialized) {
        if (serialNum != prev_serialNum) {
            MapContext(&contextMappingList_usecScalarsData,
                       contextInfo,
                       (void *)&access_usecScalarsData);
        }
    } else {
        MapContext(&contextMappingList_usecScalarsData,
                   contextInfo,
                   (void *)&access_usecScalarsData);
        prev_serialNum_initialized = 1;
    }
    prev_serialNum = serialNum;

    if (access_usecScalarsData != NULL) {
        access_usecScalarsData->snmpTime = GetTimeNow() / 100;
    }
    return(access_usecScalarsData);
}

#ifdef U_usecStats
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_usecStatsFreeUserpartData (data)
    usecStats_t *data;
{
    /* nothing to free by default */
}
#endif /* U_usecStats */

#ifdef U_usecStats
/* This routine can be used to clone data which
 * is defined in the userpart part of the structure */
void
k_usecStatsCloneUserpartData (dst, src)
    usecStats_t *dst, *src;
{
    /* nothing to clone by default */
}
#endif /* U_usecStats */

usecStats_t *
k_usecStats_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
    static int prev_serialNum = -1;
    static int prev_serialNum_initialized = 0;

    if (!prev_serialNum_initialized) {
        if (serialNum != prev_serialNum) {
            MapContext(&contextMappingList_usecStatsData,
                       contextInfo,
                       (void *)&access_usecStatsData);
        }
    } else {
        MapContext(&contextMappingList_usecStatsData,
                   contextInfo,
                   (void *)&access_usecStatsData);
        prev_serialNum_initialized = 1;
    }
    prev_serialNum = serialNum;

   return(access_usecStatsData);
}

#ifdef U_usecUser
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_usecUserFreeUserpartData (data)
    usecUser_t *data;
{
    /* nothing to free by default */
}
#endif /* U_usecUser */

#ifdef U_usecUser
/* This routine can be used to clone data which
 * is defined in the userpart part of the structure */
void
k_usecUserCloneUserpartData (dst, src)
    usecUser_t *dsr, *src;
{
    /* nothing to clone by default */
}
#endif /* U_usecUser */

usecUser_t *
k_usecUser_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
    static int prev_serialNum = -1;
    static int prev_serialNum_initialized = 0;

    if (!prev_serialNum_initialized) {
        if (serialNum != prev_serialNum) {
            MapContext(&contextMappingList_userNameTable,
                       contextInfo,
                       (void *)&access_userNameTable);
        }
    } else {
        MapContext(&contextMappingList_userNameTable,
                   contextInfo,
                   (void *)&access_userNameTable);
        prev_serialNum_initialized = 1;
    }
    prev_serialNum = serialNum;

   return(access_usecUserData);
}

#ifdef SETS
int
k_usecUser_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{
    MapContext(&contextMappingList_usecUserData,
               contextInfo,
               (void *)&access_usecUserData);
    if (access_usecUserData == NULL) {
        return NO_CREATION_ERROR;
    }

    if (value->sl_value != access_usecUserData->userSpinLock) {
        return INCONSISTENT_VALUE_ERROR;
    }
    return NO_ERROR;
}

int
k_usecUser_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_usecUser_set(data, contextInfo, function)
   usecUser_t *data;
   ContextInfo *contextInfo;
   int function;
{
    MapContext(&contextMappingList_usecUserData,
               contextInfo,
               (void *)&access_usecUserData);

   if (data->userSpinLock == 2147483647) {
       access_usecUserData->userSpinLock = 0;
   } else {
       access_usecUserData->userSpinLock = data->userSpinLock + 1;
   }
   return NO_ERROR;
}

#ifdef SR_usecUser_UNDO
/* add #define SR_usecUser_UNDO in sitedefs.h to
 * include the undo routine for the usecUser family.
 */
int
usecUser_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_usecUser_UNDO */

#endif /* SETS */

/* This routine is called by the timeout code to 
 * delete a pending creation of a Table entry */ 
void
userNameTableDeleteCallback (tdp)
    TimeOutDescriptor *tdp;
{
    cidata_t *ud2 = (cidata_t *)tdp->UserData2;
    userNameEntry_t *data;

    /* dummy up an entry to delete */
    data = (userNameEntry_t *) ud2->data;
    data->userStatus = D_userStatus_destroy;
    data->RowStatusTimerId = -1;

    /* free the timeout descriptor */
    free(tdp);

    /* call the set method */
    k_userNameEntry_set(data, ud2->cip, 0);
    FreeContextInfo(ud2->cip);
    free(ud2);
}

/* This routine deletes an entry from the userNameTable */
void
DeleteuserNameEntry (index)
    int index;
{
    FreeEntries(userNameEntryTypeTable, access_userNameTable->tp[index]);
    free(access_userNameTable->tp[index]);
    RemoveTableEntry(access_userNameTable, index);
}

/* This routine returns nonzero if the table entry contains sufficient
 * information to be in the active (or notInService) state. */
int
userNameEntryReadyToActivate(data)
userNameEntry_t *data;
{
    /*
     * To be ready, a set must have occurred on both userAuthChange and
     * userPrivChange, after a set has occurred on userCloneFrom.
     */
    return 
	VALID(I_userAuthChange, data->valid) &&
	VALID(I_userPrivChange, data->valid) &&
        VALID(I_userCloneFrom, data->valid);
}

#ifdef U_userNameEntry
/* This routine can be used to free data which
 * is defined in the userpart part of the structure */
void
k_userNameEntryFreeUserpartData (userNameEntry_t *data)
{
#ifndef SR_UNSECURABLE
    if (data->auth_secret != NULL) {
        FreeOctetString(data->auth_secret);
    }
    if (data->new_auth_secret != NULL) {
        FreeOctetString(data->new_auth_secret);
    }
#endif /* SR_UNSECURABLE */
}
#endif /* U_userNameEntry */

#ifdef U_userNameEntry
/* This routine can be used to clone data which
 * is defined in the userpart part of the structure */
void
k_userNameEntryCloneUserpartData (userNameEntry_t *dst, userNameEntry_t *src)
{
#ifndef SR_UNSECURABLE
    dst->auth_secret = CloneOctetString(src->auth_secret);
    dst->new_auth_secret = CloneOctetString(src->new_auth_secret);
#endif /* SR_UNSECURABLE */
}
#endif /* U_userNameEntry */

userNameEntry_t *
k_userNameEntry_get(serialNum, contextInfo, nominator, searchType, userAuthSnmpID, userName)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   OctetString * userAuthSnmpID;
   OctetString * userName;
{
    int index;
    static int prev_serialNum = -1;
    static int prev_serialNum_initialized = 0;

    if (!prev_serialNum_initialized) {
        if (serialNum != prev_serialNum) {
            MapContext(&contextMappingList_userNameTable,
                       contextInfo,
                       (void *)&access_userNameTable);
        }
    } else {
        MapContext(&contextMappingList_userNameTable,
                   contextInfo,
                   (void *)&access_userNameTable);
        prev_serialNum_initialized = 1;
    }
    prev_serialNum = serialNum;

    if (access_userNameTable == NULL) {
        return NULL;
    }

    access_userNameTable->tip[0].value.octet_val = userAuthSnmpID;
    access_userNameTable->tip[1].value.octet_val = userName;
    if ((index = SearchTable(access_userNameTable, searchType)) == -1) {
        return NULL;
    }

    return (userNameEntry_t *) access_userNameTable->tp[index];

}

#ifdef SETS
int
k_userNameEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{
    userNameEntry_t *data, *userCloneFrom;
    int index;
    int i;

    MapContext(&contextMappingList_userNameTable,
               contextInfo,
               (void *)&access_userNameTable);
    if (access_userNameTable == NULL) {
        return NO_CREATION_ERROR;
    }

    data = (userNameEntry_t *)dp->data;

    if (object->nominator == I_userGroupName) {
        if (CheckAuthName(value->os_value)) {
            return WRONG_VALUE_ERROR;
        }
    }

    if (object->nominator == I_userCloneFrom) {
        if (value->oid_value->length < MIN_CLONEFROM_LENGTH) {
            return WRONG_VALUE_ERROR;
        }
        if ((int)value->oid_value->oid_ptr[11] != 12) {
            return WRONG_VALUE_ERROR;
        }
        /* Must do the clone, so make sure the clonee exists */
        access_userNameTable->tip[0].value.octet_val = MakeOctetString(NULL, 12);
        if (access_userNameTable->tip[0].value.octet_val == NULL) {
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        for (i = 0; i < 12; i++) {
            access_userNameTable->tip[0].value.octet_val->octet_ptr[i] =
                (unsigned char)value->oid_value->oid_ptr[12 + i];
        }
        access_userNameTable->tip[1].value.octet_val =
            MakeOctetString(NULL, value->oid_value->length - 24);
        if (access_userNameTable->tip[1].value.octet_val == NULL) {
            FreeOctetString(access_userNameTable->tip[0].value.octet_val);
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        for (i = 0; i < value->oid_value->length - 24; i++) {
            access_userNameTable->tip[1].value.octet_val->octet_ptr[i] =
                (unsigned char)value->oid_value->oid_ptr[24 + i];
        }
        index = SearchTable(access_userNameTable, EXACT);
        FreeOctetString(access_userNameTable->tip[0].value.octet_val);
        FreeOctetString(access_userNameTable->tip[1].value.octet_val);
        if (index == -1) {
            return INCONSISTENT_VALUE_ERROR;
        }
        userCloneFrom = (userNameEntry_t *)access_userNameTable->tp[index];
        if ((userCloneFrom->userStatus != RS_ACTIVE) &&
            (userCloneFrom->userStatus != RS_NOT_IN_SERVICE)) {
            return INCONSISTENT_VALUE_ERROR;
        }
	/* Clone the secrets now */
#ifndef SR_UNSECURABLE
	if (data->new_auth_secret != NULL) {
	    FreeOctetString(data->new_auth_secret);
	}
	if ((data->new_auth_secret =
	     CloneOctetString(userCloneFrom->auth_secret)) == NULL) {
	    return RESOURCE_UNAVAILABLE_ERROR;
	}
#endif /* SR_UNSECURABLE */
    }

    if (object->nominator == I_userName && 
	(value->sl_value == D_userStatus_createAndWait || 
	 value->sl_value == D_userStatus_createAndGo) &&
        (data->userAuthSnmpID->length != 12 || 
	 CheckAuthName(data->userName))) {
	return NO_CREATION_ERROR;
    }

    return NO_ERROR;
}

int
k_userNameEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    int doListReady = 0;
    userNameEntry_t *data = (userNameEntry_t *) dp->data;

    /* no state information needed for destroy */
    if (data->userStatus == D_userStatus_destroy) {
        dp->state = SR_DELETE;
    } else 
    if (data->userStatus == RS_UNINITIALIZED) {
        /* status hasn't been set yet */
        dp->state = SR_UNKNOWN;
    } else {
        /* check row status state machine transitions */
        if(data->userStatus == D_userStatus_createAndGo) {
            data->userStatus = D_userStatus_active;
        } 
        doListReady = 1;

        /* if transitioning (or remaining) in the active or notInService states,
         * make sure that the new values are not inconsistent enough to 
         * make the entry notReady */
        if(data->userStatus == D_userStatus_active
        || data->userStatus == D_userStatus_notInService) {
            if(userNameEntryReadyToActivate(data) == 0) {
                /* inconsistent values, don't allow the set operation */
                doListReady = 0;
            }
        } else {
            if (VALID(I_userAuthChange, data->valid) &&
                !VALID(I_userCloneFrom, data->valid)) {
                doListReady = 0;
            }
            if (VALID(I_userPrivChange, data->valid) &&
                !VALID(I_userCloneFrom, data->valid)) {
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
k_userNameEntry_set_defaults(dp)
    doList_t       *dp;
{
    userNameEntry_t *data = (userNameEntry_t *) (dp->data);

    if ((data->userGroupName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    if ((data->userAuthChange = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    if ((data->userPrivChange = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    if ((data->userNovel = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    if ((data->userTransportLabel = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    if ((data->userCloneFrom = MakeOIDFromDot("0.0")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    data->userMemoryType = D_userMemoryType_volatile;

#ifndef SR_UNSECURABLE
    data->auth_secret = NULL;
    data->new_auth_secret = NULL;
#endif /* SR_UNSECURABLE */

    data->RowStatusTimerId = -1;

    SET_ALL_VALID(data->valid);
    CLR_VALID(I_userAuthChange, data->valid);
    CLR_VALID(I_userPrivChange, data->valid);
    CLR_VALID(I_userCloneFrom, data->valid);

    return NO_ERROR;
}

#ifndef SR_UNSECURABLE
OctetString *
UpdateSecret(change, secret)
    OctetString *change, *secret;
{
    unsigned char _temp[64], _new[64], _rval[64];
    OctetString temp, rval;
    MD5_CTX MD;
    int i, j, iter;

    if (change->length == 0) {
        return NULL;
    }

    rval.octet_ptr = _rval;
    rval.length = MD5_DIGEST_SIZE;
    memcpy(_rval, change->octet_ptr, MD5_DIGEST_SIZE);

    iter = (change->length - MD5_DIGEST_SIZE - 1) / 16;

    temp.octet_ptr = _temp;
    temp.length = secret->length;
    memcpy(_temp, secret->octet_ptr, temp.length);

    for (i = 0; i < iter; i++) {
        memcpy(_temp + temp.length, _rval, MD5_DIGEST_SIZE);
        MD5Init(&MD);
        MD5Update(&MD, _temp, (unsigned int)(temp.length + MD5_DIGEST_SIZE));
        MD5Final(_temp, &MD);
        for (j = 0; j < MD5_DIGEST_SIZE; j++) {
            _new[i*16+j] = _temp[j] ^ change->octet_ptr[i*16+MD5_DIGEST_SIZE+j];
        }
	temp.length = MD5_DIGEST_SIZE;
    }
    memcpy(_temp + temp.length, _rval, MD5_DIGEST_SIZE);
    MD5Init(&MD);
    MD5Update(&MD, _temp, (unsigned int)(temp.length + MD5_DIGEST_SIZE));
    MD5Final(_temp, &MD);
    for (j = 0; j < change->length - i * 16 - MD5_DIGEST_SIZE; j++) {
        _new[i*16+j] = _temp[j] ^ change->octet_ptr[i*16+MD5_DIGEST_SIZE+j];
    }

    return MakeOctetString(_new, change->length - MD5_DIGEST_SIZE);
}
#endif /* SR_UNSECURABLE */

int
k_userNameEntry_set(data, contextInfo, function)
   userNameEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

    int index;
    userNameEntry_t *newrow = NULL;
    OctetString *ns;
    int i;

    MapContext(&contextMappingList_userNameTable,
               contextInfo,
               (void *)&access_userNameTable);

    /* find this entry in the table */
    access_userNameTable->tip[0].value.octet_val = data->userAuthSnmpID;
    access_userNameTable->tip[1].value.octet_val = data->userName;
    if ((index = SearchTable(access_userNameTable, EXACT)) != -1) {
        newrow = (userNameEntry_t *) access_userNameTable->tp[index];
    }

    /* perform the table entry operation on it */
    if (data->userStatus == D_userStatus_destroy) {
        if (data->RowStatusTimerId != -1) {
            CancelRowStatusTimeout(data->RowStatusTimerId);
        }

        if (index == -1) {
            return NO_ERROR;
        } else {
            int flag = newrow->userMemoryType;
            /* delete the table entry */
            DeleteuserNameEntry(index);
            if (flag >= NONVOLATILE) {
                writeConfigFileFlag = TRUE;
            }
            return NO_ERROR;
        }
    } else if (index == -1) {
        /* add the entry */
        access_userNameTable->tip[0].value.octet_val = data->userAuthSnmpID;
        access_userNameTable->tip[1].value.octet_val = data->userName;
        if ((index = NewTableEntry(access_userNameTable)) == -1) {
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        newrow = (userNameEntry_t *) access_userNameTable->tp[index];

        /* set a timeout */
        if ((newrow->RowStatusTimerId = SetRowStatusTimeoutWithCI(MAX_ROW_CREATION_TIME, contextInfo, (void *) newrow, (TodCallbackFunction) userNameTableDeleteCallback)) == -1) {
            DeleteuserNameEntry(index);
            return RESOURCE_UNAVAILABLE_ERROR;
        }
    }

    /* Clone secrets */
#ifndef SR_UNSECURABLE
    if (VALID(I_userCloneFrom, data->valid) &&
	data->userCloneFrom->length >= MIN_CLONEFROM_LENGTH) {
        if (data->auth_secret != NULL) {
            FreeOctetString(data->auth_secret);
        }
	data->auth_secret = data->new_auth_secret;
	data->new_auth_secret = NULL;
        FreeOID(data->userCloneFrom);
        data->userCloneFrom = MakeOIDFromDot("0.0");
    }

    /* Update authentication key */
    if (VALID(I_userAuthChange, data->valid)) {
	if (data->userAuthChange->length > 0) {
            ns = UpdateSecret(data->userAuthChange, data->auth_secret);
            if (ns != NULL) {
                if (ns->length == MD5_DIGEST_SIZE) {
                    FreeOctetString(data->auth_secret);
                    data->auth_secret = ns;
                } else {
                    FreeOctetString(ns);
                }
            }
        } else {
            if (data->auth_secret != NULL) {
                FreeOctetString(data->auth_secret);
            }
            data->auth_secret = MakeOctetString(NULL, 0);
        }
        FreeOctetString(data->userAuthChange);
        data->userAuthChange = MakeOctetStringFromText("");
    }

#endif /* SR_UNSECURABLE */

    /* if we are executing a createAndWait command, determine whether
     * to transition to notReady or notInService.  If we are notReady,
     * transition to notInService whenever enough information becomes
     * available to the agent. */
    if(data->userStatus == D_userStatus_createAndWait
    || data->userStatus == D_userStatus_notReady) {
        /* is the entry ready? */
        if(userNameEntryReadyToActivate(data) != 0) {
            /* yes, it can go to notInService */
            data->userStatus = D_userStatus_notInService;
        } else {
            /* no, it isn't ready yet */
            data->userStatus = D_userStatus_notReady;
        }
    }

    if ((newrow->userMemoryType >= NONVOLATILE || 
	 data->userMemoryType >= NONVOLATILE) &&
	(newrow->userStatus == RS_ACTIVE ||
	 data->userStatus == RS_ACTIVE)) {
        writeConfigFileFlag = TRUE;
    }

    /* copy values from the scratch copy to live data */
    TransferEntries(I_userStatus, userNameEntryTypeTable, (void *) newrow, (void *) data);
    /*SET_ALL_VALID(newrow->valid);*/
    for (i = 0; i < 10 ; i++) {
        if (VALID(i, data->valid)) {
            SET_VALID(i, newrow->valid);
        }
    }
#ifndef SR_UNSECURABLE
    if (newrow->auth_secret != NULL) {
        FreeOctetString(newrow->auth_secret);
    }
    newrow->auth_secret = data->auth_secret;
    data->auth_secret = NULL;
#endif /* SR_UNSECURABLE */

    /* if the new row entry is now valid, cancel the creation timeout */
    if (newrow->userStatus == D_userStatus_active && newrow->RowStatusTimerId != -1) {
        CancelRowStatusTimeout(newrow->RowStatusTimerId);
        newrow->RowStatusTimerId = -1;
    } else
    if ((newrow->userStatus == D_userStatus_notInService || newrow->userStatus == D_userStatus_notReady)
    && newrow->RowStatusTimerId == -1) {
        /* we are transitioning to an inactive state.  Set a timer to delete
         * the row after an appropriate interval */
        newrow->RowStatusTimerId = SetRowStatusTimeoutWithCI(MAX_ROW_CREATION_TIME, contextInfo, (void *) newrow, (TodCallbackFunction) userNameTableDeleteCallback);
    }

    return NO_ERROR;

}

#ifdef SR_userNameEntry_UNDO
/* add #define SR_userNameEntry_UNDO in sitedefs.h to
 * include the undo routine for the userNameEntry family.
 */
int
userNameEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    userNameEntry_t *data = (userNameEntry_t *) doCur->data;
    userNameEntry_t *undodata = (userNameEntry_t *) doCur->undodata;
    userNameEntry_t *setdata = NULL;
    int function;

    /* we are either trying to undo an add, a delete, or a modify */
    if (undodata == NULL) {
        /* undoing an add, so delete */
        data->userStatus = D_userStatus_destroy;
        setdata = data;
        function = SR_DELETE;
    } else {
        /* undoing a delete or modify, replace the original data */
        if(undodata->userStatus == D_userStatus_notReady
        || undodata->userStatus == D_userStatus_notInService) {
            undodata->userStatus = D_userStatus_createAndWait;
        } else 
        if(undodata->userStatus == D_userStatus_active) {
            undodata->userStatus = D_userStatus_createAndGo;
        }
        setdata = undodata;
        function = SR_ADD_MODIFY;
    }

    /* use the set method for the undo */
    if ((setdata != NULL) && k_userNameEntry_set(setdata, contextInfo, function) == NO_ERROR) {
        return NO_ERROR;
    }

   return UNDO_FAILED_ERROR;
}
#endif /* SR_userNameEntry_UNDO */

#endif /* SETS */
