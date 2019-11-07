#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#include <ctype.h>

#include <windows.h>
#include <winsock.h>

#ifdef NOTICE
#undef NOTICE   /* DO NOT REMOVE */
#endif  /* NOTICE */
#include "sr_utils.h"
#include "snmpid.h"
#include "sr_trans.h"
#include "snmpv2.h"
#include "sr_msg.h"
#include "v1_msg.h"
#include "diag.h"
SR_FILENAME

SnmpLcd snmp_lcd;

#ifdef SR_MIB_TABL_H
#include "snmp-mib.h"
#endif /* SR_MIB_TABL_H */



#include <sys/types.h>

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif	/* HAVE_SYS_TIME_H */

#include <string.h>

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif	/* HAVE_MEMORY_H */

/* in snmpio.c */
extern int fd;
extern SR_INT32 packet_len;
extern unsigned char packet[SR_PACKET_LEN];

#if (defined(SR_SNMPv2_PACKET) || defined(SR_SNMPv3_PACKET))
OctetString *util_local_snmpID;
OctetString *defaultContextSnmpID = NULL;
OctetString *defaultContext = NULL;
const PARSER_RECORD_TYPE *util_rt[16];
int util_rt_count = 0;
#endif	/* (defined(SR_SNMPv2_PACKET) || defined(SR_SNMPv3_PACKET)) */

#ifdef SR_SNMPv2_PDU
/*
 * CheckEndOfMIB: This routine checks to see if every varbind's type
 *                field is equal to END_OF_MIB_VIEW_EXCEPTION.  If
 *                every varbind matches, this routine prints out a
 *                message and returns -1. Otherwise, it returns 0 (not
 *                end of mib). If an error occurs, this routine
 *                returns -2.
 *
 * The former name (pre-snmp12.1.0.0 release) was check_end_of_mib().
 */
int CheckEndOfMIB (Pdu *pdu_ptr)
{
    VarBind        *tmp_vb_ptr;
    int             num_exceptions;
    int             i;

    tmp_vb_ptr = NULL;
    num_exceptions = 0;

    for (i = 0; i < (int) pdu_ptr->num_varbinds; i++)
	{
		if (tmp_vb_ptr == NULL)
		{
			tmp_vb_ptr = pdu_ptr->var_bind_list;
		}
		else
		{
			tmp_vb_ptr = tmp_vb_ptr->next_var;
		}

		if (tmp_vb_ptr != NULL)
		{
			if (tmp_vb_ptr->value.type != END_OF_MIB_VIEW_EXCEPTION)
			{
				break;
			}
			else if (tmp_vb_ptr->value.type == END_OF_MIB_VIEW_EXCEPTION)
			{
				num_exceptions++;
			}

		}
		else
		{
			DPRINTF((APWARN, "CheckEndOfMIB: Could not assign tmp_vb_ptr.\n"));
			return (-2);
		}
    }

    if (num_exceptions == (int) pdu_ptr->num_varbinds)
	{
		printf("End of MIB.\n");
		return (-1);
    }
    else
	{
		return (0);
    }

}				/* CheckEndOfMIB() */

#endif				/* SR_SNMPv2_PDU */

/*
 * GetVariableType
 *
 * Convert the type constant from mib_oid_table into a type code (like -i or -D)
 *
 * t_codes contains the appropriate code for each type listed in t_values
 */
static char *t_codes[] = {
        "-D",
        "-o",
        "-o",
        "-o",
        "-a",
        "-d",
        "-i",
        "-i",
        "-g",
        "-c",
        "-i",
#ifdef SR_SNMPv2_PDU
        "-c",
        "-g",
        "",    /* NOT DEFINED FOR COUNTER64 */
        "-i",
#endif /* SR_SNMPv2_PDU */
        (char *)0
};

#ifdef SR_MIB_TREE_H
static short t_values[] = {
        DISPLAY_STRING_TYPE,
        OCTET_PRIM_TYPE,
        PHYS_ADDRESS_TYPE,
        NETWORK_ADDRESS_TYPE,
        IP_ADDR_PRIM_TYPE,
        OBJECT_ID_TYPE,
        TIME_TICKS_TYPE,
        INTEGER_TYPE,
        GAUGE_TYPE,
        COUNTER_TYPE,
        SERVICES_TYPE,
#ifdef SR_SNMPv2_PDU
        COUNTER_32_TYPE,
        GAUGE_32_TYPE,
        COUNTER_64_TYPE,
        INTEGER_32_TYPE,
#endif /* SR_SNMPv2_PDU */
        0
};

int GetVariableType (OID_TREE_ELE *ote, char *type)
{
    int j;
    for (j=0; t_values[j]!=0; j++)
	{
        if (t_values[j] == ote->type)
		{
            if (strlen(t_codes[j]) == 0)
			{
                return -1;
            }
            strcpy(type, t_codes[j]);
            return 1;
        }
    }
    return -1;
}

/*
 * GetEnumFromString:
 *
 * The former name was get_enumerated_int_from_string() (pre-snmp12 release).
 */
int
GetEnumFromString(ote, value, number)
    OID_TREE_ELE   *ote;
    char           *value;
    SR_INT32       *number;
{
    struct enumer  *elist;

    if (ote == NULL) {
        return -1;
    }
    for (elist = ote->enumer; elist; elist = elist->next) {
        if (strcmp(elist->name, value) == 0) {
            *number = elist->number;
            return 1;
        }
    }

    return -1;
}
#endif /* SR_MIB_TREE_H */

/*
 * BuildNextVarBind:
 *
 * BuildNextVarBind takes the command line in the form of either a
 * <name type value> tuple or a <name value> tuple, determines the
 * variable type if necessary and returns a new varbind on success.
 *
 * The parameter argv is a pointer to what was typed on the command
 * line when invoking the manager utility.
 *
 * The parameter in_index is the index into the command line, argv.
 *
 *  The former name (pre-snmp12.1.0.0 release) was build_next_vb().
 */
#ifdef SR_MIB_TREE_H
VarBind        *
BuildNextVarBind(argv, in_index)
    char           *argv[];
    int            *in_index;
{
    SR_INT32        number;
    OID            *oid_ptr;
    char            type[3];
    int             offset;
    VarBind        *vb_ptr;
    int             i;
    OID_TREE_ELE   *ote = NULL;

    i = *in_index;

    if (!isdigit(*argv[i])) {
        ote = AP_GetMIBNodeFromDot(argv[i]);
    }

    if (!argv[i] || !argv[i + 1]) {
        FreeOTE(ote);
	return (NULL);
    }
    if ((*argv[i + 1] == '-') && (!isdigit(argv[i + 1][1]))) {
	if (!argv[i + 2]) {
            FreeOTE(ote);
	    return (NULL);
	}
	offset = 2;
	strncpy(type, argv[i + 1], 2);
	type[2] = '\0';
    }
    else {
	if (ote == NULL) {
	    DPRINTF((APWARN, "do not know type of %s\n", argv[i]));
	    return NULL;
	}
	offset = 1;
	if ((GetVariableType(ote, type)) < 0) {
	    DPRINTF((APWARN, "do not know type of %s\n", argv[i]));
            FreeOTE(ote);
	    return NULL;
	}
    }

    if (((ote != NULL) && strcmp("-i", type) == 0) &&
	(!isdigit(*argv[i + offset])) && (!isdigit(argv[i + offset][1])) &&
	(GetEnumFromString(ote, argv[i + offset], &number) > 0)) {


	oid_ptr = MakeOIDFromDot(argv[i]);
	if (oid_ptr == NULL) {
	    DPRINTF((APWARN, "Cannot translate variable class: %s\n", argv[i]));
            FreeOTE(ote);
	    return (NULL);
	}

	vb_ptr = MakeVarBindWithValue(oid_ptr, (OID *) NULL,
				      (unsigned char) INTEGER_TYPE,
				      (void *) &number);
	oid_ptr = NULL;

    }
    else {
	vb_ptr = CreateVarBind(argv[i], type, argv[i + offset]);
    }
    if (vb_ptr == NULL) {
	DPRINTF((APWARN, "Unable to create request from the given arguments\n"));
        FreeOTE(ote);
	return (NULL);
    }

    i = i + 1 + offset;
    *in_index = i;
    FreeOTE(ote);
    return (vb_ptr);
}
#endif /* SR_MIB_TREE_H */
#ifdef SR_MIB_TABL_H
VarBind        *
BuildNextVarBind(argv, in_index)
    char           *argv[];
    int            *in_index;
{
    SR_INT32        number;
    OID            *oid_ptr;
    char            type[3];
    VarBind        *vb_ptr;
    int             i;

    i = *in_index;

    if (!argv[i] || !argv[i + 1]) {
	return (NULL);
    }
    if ((*argv[i + 1] == '-') && (!isdigit(argv[i + 1][1]))) {
	if (!argv[i + 2]) {
	    return (NULL);
	}
	strncpy(type, argv[i + 1], 2);
	type[2] = '\0';
    }
    else {
	DPRINTF((APWARN, "must provide type of %s\n", argv[i]));
        return NULL;
    }

    vb_ptr = CreateVarBind(argv[i], type, argv[i + 2]);
    if (vb_ptr == NULL) {
	DPRINTF((APWARN, "Unable to create request from the given arguments\n"));
	return (NULL);
    }

    i = i + 3;
    *in_index = i;
    return (vb_ptr);
}
#endif /* SR_MIB_TABL_H */


/*
 * InitializeUtility
 *
 */
int InitializeUtility (int type, int *argc, char *argv[],
                  char *default_util_dest, char *default_util_handle, char *usage_string)
{

#ifdef SR_MIB_TREE_H
    if (NewMIBFromFile(fn_snmpinfo_dat) < 0) {
        printf("Failure in snmpinfo.dat\n");
        goto failure;
    }
#endif /* SR_MIB_TREE_H */

#ifdef SR_MIB_TABL_H
    mib_oid_table = orig_mib_oid_table;
#endif /* SR_MIB_TABL_H */

    if (communication_type == SNMP_COMMUNICATIONS) {
        if (util_dest == NULL) {
            fprintf(stderr, usage_string, argv[0], argv[0]);
            goto failure;
        }
        InitializeIO(util_name, util_dest);
    }
    if (communication_type == TRAP_SEND_COMMUNICATIONS) {
        if (util_dest == NULL) {
            fprintf(stderr, usage_string, argv[0], argv[0]);
            goto failure;
        }
        InitializeTrapIO(util_name, util_dest);
    }
    if (communication_type == TRAP_RCV_COMMUNICATIONS) {
        InitializeTrapRcvIO(util_name);
    }

	InitTimeNow ();

	util_local_snmpID = SrGetSnmpID(SR_SNMPID_ALGORITHM_SIMPLE,
									SR_SNMPID_ALGORITHM_SIMPLE_MGR);
	if (util_local_snmpID == NULL) {
		printf("Cannot initialize local snmpID.\n");
		goto failure;
	}


#if (defined(SR_SNMPv2_PACKET) || defined(SR_SNMPv3_PACKET))
#ifdef SR_CONFIG_FP
    util_rt[util_rt_count++] = NULL;
    ConvToken_snmpID_localSnmpID = util_local_snmpID;
    ParseConfigFile(fn_mgr_file, util_rt);
#endif /* SR_CONFIG_FP */
#endif	/* (defined(SR_SNMPv2_PACKET) || defined(SR_SNMPv3_PACKET)) */

#ifdef SR_SNMPv2_PACKET
    if (util_version == SR_SNMPv2_VERSION) {
        if (!InitializeUser(util_handle, communication_type)) {
            printf("Bad username %s\n", util_handle);
            goto failure;
        }
    }
#endif /* SR_SNMPv2_PACKET */
#ifdef SR_SNMPv3_PACKET
    if (util_version == SR_SNMPv3_VERSION) {
        if (!InitializeV3User(util_handle, communication_type)) {
            printf("Bad username %s\n", util_handle);
            goto failure;
        }
    }
#endif /* SR_SNMPv3_PACKET */

    global_req_id = MakeReqId();

    return 0;

  failure:
    return -1;
}

/*
 * PerformCommunityRequest:
 *
 */
Pdu *
PerformCommunityRequest(out_pdu_ptr, status)
    Pdu *out_pdu_ptr;
    int *status;
{
    OctetString *community_ptr = NULL;
    SnmpMessage *snmp_msg = NULL;
    Pdu *in_pdu_ptr = NULL;

    *status = 0;

    community_ptr = MakeOctetStringFromText(util_handle);
    if (community_ptr == NULL) {
        DPRINTF((APWARN, "MakeOctetStringFromText failed.\n"));
        *status = SR_ERROR;
        goto done;
    }

#ifdef SR_SNMPv1_PACKET
    if (util_version == SR_SNMPv1_VERSION) {
        snmp_msg = SrCreateV1SnmpMessage(community_ptr, FALSE);
    }
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2c_PACKET
    if (util_version == SR_SNMPv2c_VERSION) {
        snmp_msg = SrCreateV2cSnmpMessage(community_ptr, FALSE);
    }
#endif /* SR_SNMPv2c_PACKET */
    community_ptr = NULL;
    if (snmp_msg == NULL) {
        DPRINTF((APWARN, "cannot create SnmpMessage.\n"));
        *status = SR_ERROR;
        goto done;
    }

    /* make final request packet */
    if (SrBuildSnmpMessage(snmp_msg, out_pdu_ptr, &snmp_lcd) != 0) {
        DPRINTF((APWARN, "SrBuildSnmpMessage failed.\n"));
        *status = SR_ERROR;
        goto done;
    }

    if (SrSendRequest(fd,
                      snmp_msg->packlet->octet_ptr,
                      snmp_msg->packlet->length) != TRUE) {
        /* could not send */
        *status = SR_ERROR;
        goto done;
    }

    SrFreeSnmpMessage(snmp_msg);
    snmp_msg = NULL;

#ifdef SR_SNMPv1_PACKET
    if (out_pdu_ptr->type == TRAP_TYPE) {
        goto done;
    }
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2_PDU
    if (out_pdu_ptr->type == SNMPv2_TRAP_TYPE) {
        goto done;
    }
#endif /* SR_SNMPv2_PDU */

  wait_again:
    *status = GetResponse();

    /* exit if error in receive routine */
    if (*status == SR_ERROR) {
        DPRINTF((APALWAYS, "%s:  receive error.\n", util_name));
        goto done;
    }

    if (*status == SR_TIMEOUT) {
        DPRINTF((APTRACE, "%s:  no response.\n", util_name));

        goto done;
    }

    snmp_msg = SrParseSnmpMessage(&snmp_lcd, NULL, NULL, packet, packet_len);
    if (snmp_msg == NULL) {
        DPRINTF((APALWAYS, "%s: error parsing packet.\n", util_name));
        *status = SR_ERROR;
        goto done;
    }

    in_pdu_ptr = SrParsePdu(snmp_msg->packlet->octet_ptr,
                            snmp_msg->packlet->length);
    if (in_pdu_ptr == NULL) {
        DPRINTF((APALWAYS, "%s: error parsing pdu.\n", util_name));
        *status = SR_ERROR;
        goto done;
    }

    /* Make sure request id matches */
    if (in_pdu_ptr->u.normpdu.request_id != out_pdu_ptr->u.normpdu.request_id) {
        DPRINTF((APALWAYS, "request id mismatch.\n"));
        FreePdu(in_pdu_ptr);
        in_pdu_ptr = NULL;
        goto wait_again;
    }

  done:
    if (community_ptr != NULL) {
        FreeOctetString(community_ptr);
        community_ptr = NULL;
    }
    if (snmp_msg != NULL) {
        SrFreeSnmpMessage(snmp_msg);
        snmp_msg = NULL;
    }
    return in_pdu_ptr;
}

/*
 * PerformRequest:
 *
 */
Pdu *
PerformRequest(out_pdu_ptr, status)
    Pdu *out_pdu_ptr;
    int *status;
{
    switch (util_version) {
#ifdef SR_SNMPv1_PACKET
        case SR_SNMPv1_VERSION:
            return PerformCommunityRequest(out_pdu_ptr, status);
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2c_PACKET
        case SR_SNMPv2c_VERSION:
            return PerformCommunityRequest(out_pdu_ptr, status);
#endif /* SR_SNMPv2c_PACKET */
#ifdef SR_SNMPv2_PACKET
        case SR_SNMPv2_VERSION:
            return PerformUserRequest(out_pdu_ptr, status);
#endif /* SR_SNMPv2_PACKET */
#ifdef SR_SNMPv3_PACKET
        case SR_SNMPv3_VERSION:
            return PerformV3UserRequest(out_pdu_ptr, status);
#endif /* SR_SNMPv3_PACKET */
    }
    return NULL;
}

#if (defined(SR_SNMPv2_PACKET) || defined(SR_SNMPv3_PACKET))
#ifndef SR_UNSECURABLE
int
get_password(prompt, pw, maxlen)
    char *prompt;
    char *pw;
    int maxlen;
{
    int pos = 0, ch;

    printf("%s", prompt);

    while (pos < maxlen) {
        ch = getchar();
        if (((ch >= 'a') && (ch <= 'z')) ||
            ((ch >= 'A') && (ch <= 'Z')) ||
            ((ch >= '0') && (ch <= '9')) ||
            (ch == ' ')) {
            pw[pos++] = ch;
        } else
        if (ch == '\n') {
            break;
        } else {
            pos = 0;
            printf("\n bad character %c\n", ch);
            while ((ch = getchar()) != '\n');
            printf("%s", prompt);
        }
    }
    pw[pos] = '\0';
    return pos;
}
#endif /* SR_UNSECURABLE */
#endif	/* (defined(SR_SNMPv2_PACKET) || defined(SR_SNMPv3_PACKET)) */
