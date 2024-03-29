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
 *
 */

#if (! ( defined(lint) ) && defined(SR_RCSID))
static const char rcsid[] = "$Id: prse_pkt.c,v 1.27 1998/01/06 18:47:29 mark Exp $";
#endif	/* (! ( defined(lint) ) && defined(SR_RCSID)) */

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif	/* HAVE_MALLOC_H */

#include <string.h>


#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif	/* HAVE_MEMORY_H */

#include "sr_snmp.h"
#include "diag.h"
SR_FILENAME



/*
 * SrParsePdu:
 *
 *   This routine takes a serialized PDU in a buffer, and parses the
 *   information into the library's internal PDU format, including all
 *   varbind instances.  This routine is usually called with the
 *   buffer contained in the SnmpMessage structure returned by
 *   SrParseSnmpMessage (which is the same state as the header
 *   pointer after SrBuildSnmpMessage is called). 
 *
 *   The PDU pointer returned from this call is the same state as the
 *   PDU pointer on a building phase after BuildPdu() has been called.
 *   If this routine fails, it returns a NULL.  
 */
Pdu *
SrParsePdu(
    const unsigned char *packet,
    const SR_INT32 packet_len)
{
    const unsigned char *working_ptr, *end_ptr;
    Pdu            *pdu_ptr = NULL;
    VarBind        *vb_ptr;
    short           type, pdu_type;
    SR_INT32        length;

    if ((packet == NULL) || (packet_len == 0)) {
        DPRINTF((APPACKET, "SrParsePdu, bad packet\n"));
        goto fail;
    }

    working_ptr = packet;
    end_ptr = working_ptr + packet_len;

    if ((pdu_ptr = (Pdu *) malloc(sizeof(Pdu))) == NULL) {
        DPRINTF((APWARN, "ParsePdu, pdu_ptr malloc\n"));
        goto fail;
    }
#ifdef SR_CLEAR_MALLOC
    memset(pdu_ptr, 0, sizeof(Pdu));
#endif        /* SR_CLEAR_MALLOC */

    if ((pdu_type = ParseType(&working_ptr, end_ptr)) == -1) {
        DPRINTF((APPACKET, "ParsePdu, pdu_type\n"));
        goto fail;
    }

    switch ((int) pdu_type) {
    case GET_REQUEST_TYPE:
    case GET_NEXT_REQUEST_TYPE:
    case GET_RESPONSE_TYPE:
    case SET_REQUEST_TYPE:
#ifdef SR_SNMPv2_PDU
    case GET_BULK_REQUEST_TYPE:
    case INFORM_REQUEST_TYPE:
    case SNMPv2_TRAP_TYPE:
    case REPORT_TYPE:
#endif /* SR_SNMPv2_PDU */
        pdu_ptr->type = pdu_type;
        if ((length = ParseLength(&working_ptr, end_ptr)) == -1) {
            DPRINTF((APPACKET, "ParsePdu, length\n"));
            goto fail;
        }
        if (working_ptr + length > end_ptr) {
            DPRINTF((APPACKET, "ParsePdu, bad length:\n"));
            goto fail;
        }
        end_ptr = working_ptr + length;
        pdu_ptr->u.normpdu.request_id =
            ParseInt(&working_ptr, end_ptr, &type);
        if (type == -1) {
            DPRINTF((APPACKET, "ParsePdu, request_id\n"));
            goto fail;
        }
        pdu_ptr->u.normpdu.error_status =
            ParseInt(&working_ptr, end_ptr, &type);
        if (type == -1) {
            DPRINTF((APPACKET, "ParsePdu, error_status\n"));
            goto fail;
        }
        pdu_ptr->u.normpdu.error_index =
            ParseInt(&working_ptr, end_ptr, &type);
        if (type == -1) {
            DPRINTF((APPACKET, "ParsePdu, error_index\n"));
            goto fail;
        }
        break;

    case TRAP_TYPE:
        pdu_ptr->type = pdu_type;
        if ((length = ParseLength(&working_ptr, end_ptr)) == -1) {
            DPRINTF((APPACKET, "ParsePdu, length\n"));
            goto fail;
        }
        if (working_ptr + length > end_ptr) {
            DPRINTF((APPACKET, "ParsePdu, bad length:\n"));
            goto fail;
        }
        end_ptr = working_ptr + length;
        if ((pdu_ptr->u.trappdu.enterprise =
             ParseOID(&working_ptr, end_ptr)) == NULL) {
            DPRINTF((APPACKET, "ParsePdu, enterprise\n"));
            goto fail;
        }
        if ((pdu_ptr->u.trappdu.agent_addr =
             ParseOctetString(&working_ptr, end_ptr, &type)) == NULL) {
            DPRINTF((APPACKET, "ParsePdu, agent_addr\n"));
            goto fail;
        }
        pdu_ptr->u.trappdu.generic_trap =
            ParseInt(&working_ptr, end_ptr, &type);
        if (type == -1) {
            DPRINTF((APPACKET, "ParsePdu, generic_trap\n"));
            goto fail;
        }
        pdu_ptr->u.trappdu.specific_trap =
            ParseInt(&working_ptr, end_ptr, &type);
        if (type == -1) {
            DPRINTF((APPACKET, "ParsePdu, specific_trap\n"));
            goto fail;
        }
        pdu_ptr->u.trappdu.time_ticks =
            ParseUInt(&working_ptr, end_ptr, &type);
        if (type == -1) {
            DPRINTF((APPACKET, "ParsePdu, time-ticks\n"));
            goto fail;
        }
        break;

    default:
        DPRINTF((APPACKET, "ParsePdu, bad pdu_type: %x\n", pdu_type));
        goto fail;
    };                                /* end of switch */

    /* now strip out the sequence of */
    if (ParseSequence(&working_ptr, end_ptr, &type) == -1) {
        DPRINTF((APPACKET, "ParsePdu, ParseSequence failure\n"));
        goto fail;
    }

    /* now parse the varbind list */
    while (working_ptr < end_ptr) {
        if ((vb_ptr = ParseVarBind(&working_ptr, end_ptr)) == NULL) {
            DPRINTF((APPACKET, "ParsePdu, vb_ptr\n"));
            goto fail;
        }
        pdu_ptr->num_varbinds++;

        /* is this first one? */
        if (pdu_ptr->var_bind_list == NULL) {        /* start list */
            pdu_ptr->var_bind_list = vb_ptr;
            pdu_ptr->var_bind_end_ptr = vb_ptr;
        }
        else {                        /* tack onto end of list */
            pdu_ptr->var_bind_end_ptr->next_var = vb_ptr;
            pdu_ptr->var_bind_end_ptr = vb_ptr;
        }

        /* DO NOT FREE vb_ptr.  Just handed it off to pdu structure */
        vb_ptr = NULL;
    };                                /* end of while */

    return (pdu_ptr);

  fail:
    if (pdu_ptr != NULL) {
        FreePdu(pdu_ptr);
        NULLIT(pdu_ptr);
    }
    return NULL;
}     /* ParsePdu() */

/*
 * ParseVarBind:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_varbind().
 */
VarBind *
ParseVarBind(working_ptr, end_ptr)
    const unsigned char **working_ptr;
    const unsigned char  *end_ptr;
{
    VarBind        *vb_ptr;
    short           type;
    const unsigned char *temp_ptr;

    if (ParseSequence(working_ptr, end_ptr, &type) == -1) {
	DPRINTF((APPACKET, "ParseVarBind, ParseSequence failure\n"));
	return (NULL);
    }

    if ((vb_ptr = (VarBind *) malloc(sizeof(VarBind))) == NULL) {
	DPRINTF((APWARN, "ParseVarBind, vb_ptr\n"));
	return (NULL);
    }
#ifdef SR_CLEAR_MALLOC
    memset(vb_ptr, 0, sizeof(VarBind));
#endif	/* SR_CLEAR_MALLOC */

    temp_ptr = *working_ptr;

    if ((vb_ptr->name = ParseOID(working_ptr, end_ptr)) == NULL) {
	DPRINTF((APPACKET, "ParseVarBind, vb_ptr->name\n"));
	FreeVarBind(vb_ptr);
	NULLIT(vb_ptr);
	return (NULL);
    }

    vb_ptr->name_size = *working_ptr - temp_ptr;

    temp_ptr = *working_ptr;

    switch (*(*working_ptr)) {
    case COUNTER_TYPE:		/* handle unsigned integers including
				 * COUNTER_32 */
    case GAUGE_TYPE:		/* includes GAUGE_32 */
    case TIME_TICKS_TYPE:
	vb_ptr->value.ul_value =
	    ParseUInt(working_ptr, end_ptr, &vb_ptr->value.type);
	if (vb_ptr->value.type == -1) {
	    DPRINTF((APPACKET, "ParseVarBind, vb_ptr->value.ul_value\n"));
	    FreeVarBind(vb_ptr);
	    NULLIT(vb_ptr);
	    return (NULL);
	}
	break;
    case INTEGER_TYPE:		/* handle signed integers including
				 * INTEGER_32 */
	vb_ptr->value.sl_value =
	    ParseInt(working_ptr, end_ptr, &vb_ptr->value.type);
	if (vb_ptr->value.type == -1) {
	    DPRINTF((APPACKET, "ParseVarBind, vb_ptr->value.sl_value\n"));
	    FreeVarBind(vb_ptr);
	    NULLIT(vb_ptr);
	    return (NULL);
	}
	break;
    case OBJECT_ID_TYPE:	/* handle quasi-octet strings */
	vb_ptr->value.type = OBJECT_ID_TYPE;
	if ((vb_ptr->value.oid_value = ParseOID(working_ptr, end_ptr)) == NULL) {
	    DPRINTF((APPACKET, "ParseVarBind, vb_ptr->value.oid_value\n"));
	    FreeVarBind(vb_ptr);
	    NULLIT(vb_ptr);
	    return (NULL);
	}
	break;

#ifdef SR_SNMPv2_PDU
    case COUNTER_64_TYPE:	/* handle large unsigned integers */
	vb_ptr->value.type = COUNTER_64_TYPE;
	if ((vb_ptr->value.uint64_value = ParseUInt64(working_ptr, end_ptr)) == NULL) {
	    DPRINTF((APPACKET, "ParseVarBind, vb_ptr->value.uint64_value\n"));
	    FreeVarBind(vb_ptr);
	    NULLIT(vb_ptr);
	    return (NULL);
	}
	break;
#endif /* SR_SNMPv2_PDU */

    case OCTET_PRIM_TYPE:
    case IP_ADDR_PRIM_TYPE:
    case OPAQUE_PRIM_TYPE:
	if ((vb_ptr->value.os_value =
	     ParseOctetString(working_ptr, end_ptr, &vb_ptr->value.type)) == NULL) {
	    DPRINTF((APPACKET, "ParseVarBind, vb_ptr->value.os_value\n"));
	    FreeVarBind(vb_ptr);
	    NULLIT(vb_ptr);
	    return (NULL);
	}
	break;

    case NULL_TYPE:
#ifdef SR_SNMPv2_PDU
    case NO_SUCH_OBJECT_EXCEPTION:
    case NO_SUCH_INSTANCE_EXCEPTION:
    case END_OF_MIB_VIEW_EXCEPTION:
#endif /* SR_SNMPv2_PDU */
	if (ParseNull(working_ptr, end_ptr, &vb_ptr->value.type) == -1) {
	    DPRINTF((APPACKET, "ParseVarBind, ParseNull\n"));
	    FreeVarBind(vb_ptr);
	    NULLIT(vb_ptr);
	    return (NULL);
	}
	break;
    default:
	DPRINTF((APPACKET, "ParseVarBind, value: Illegal type: 0x%x\n",
		 vb_ptr->value.type));
	FreeVarBind(vb_ptr);
	NULLIT(vb_ptr);
	return (NULL);
    };				/* end of switch */

    vb_ptr->value_size = *working_ptr - temp_ptr;

    if (*working_ptr > end_ptr) {
	DPRINTF((APWARN, "ParseVarBind, past end of packet.\n"));
	return (NULL);
    }

    return (vb_ptr);
}     /* ParseVarBind() */

/*
 * ParseOctetString:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_octet_string().
 */
OctetString *
ParseOctetString(working_ptr, end_ptr, type)
    const unsigned char **working_ptr;
    const unsigned char  *end_ptr;
    short                *type;
{
    OctetString    *os_ptr;
    SR_INT32        length;
    SR_INT32        i;

    if ((*type = ParseType(working_ptr, end_ptr)) == -1) {
	DPRINTF((APPACKET, "ParseOctetString, type error.\n"));
	return (NULL);
    }

    if ((length = ParseLength(working_ptr, end_ptr)) == -1) {
	DPRINTF((APPACKET, "ParseOctetString, length error.\n"));
	return (NULL);
    }

    if ((os_ptr = MakeOctetString(*working_ptr, length)) == NULL) {
	DPRINTF((APPACKET, "ParseOctetString, os_ptr\n"));
	*type = -1;
	return (NULL);
    }

    os_ptr->length = length;

    /*
     * call to MakeOctetString just copied - did not advance working_ptr. 
     * Earlier versions of MSC did not like += and did some weird castings of 
     * types (at least that is what codeview showed).
     */
    for (i = 0; i < length; i++) {
	(*working_ptr)++;
    }

    if (*working_ptr > end_ptr) {
	DPRINTF((APWARN, "ParseOctetString, past end of packet.\n"));
	*type = -1;
	return (NULL);
    }

    return (os_ptr);
}				/* end of ParseOctetString() */

/*
 * ParseOID:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_oid().
 */
OID *
ParseOID(working_ptr, end_ptr)
    const unsigned char **working_ptr;
    const unsigned char  *end_ptr;
{
    OID            *oid_ptr;
    SR_INT32        i;
    SR_INT32        length;
    short           type;
    register int    oidlen, count;
    unsigned char   first;

    if ((type = ParseType(working_ptr, end_ptr)) == -1) {
	DPRINTF((APERROR, "ParseOID, type error.\n"));
	return (NULL);
    }

    if (type != OBJECT_ID_TYPE) {
	DPRINTF((APERROR, "ParseOID, type %x not OBJECT_ID_TYPE\n", type));
	return (NULL);
    }

    if ((length = ParseLength(working_ptr, end_ptr)) == -1) {
	DPRINTF((APERROR, "ParseOID, length error.\n"));
	return (NULL);
    }

    if ((*(*working_ptr) & 0x80) != 0) {
	DPRINTF((APERROR, "ParseOID, more bit set in first subidentifier.\n"));
	return (NULL);
    }

    if (length == 0) {		/* zero lengthed OIDs are not a good idea. */
        /*
         * Used to return oid_ptr here. Zero length OIDs are illegal according
         * to IS8824, p. 18. So do not accept them and increment the 
         * appropriate counter. CMU's snmpget utility can send packets with
         * zero length OIDs, which is broken. We have a routine called
         * NormalizeVarBindListLengths() which would fix this problem, but
         * cause the agent to send back a different varbind list on an error 
         * pdu.
         */
	return NULL;
    }

    if ((oid_ptr = MakeOID(NULL, MIN(length+2,MAX_OID_SIZE))) == NULL) {
	DPRINTF((APWARN, "ParseOID, oid_ptr MakeOID\n"));
	return (NULL);
    }

    oid_ptr->oid_ptr[0] = (SR_UINT32) (*(*working_ptr) / 40);
    oid_ptr->oid_ptr[1] = (SR_UINT32) (*(*working_ptr)++
					   - (oid_ptr->oid_ptr[0] * 40));
    oidlen = 2;

    if(length == 1) {
	oid_ptr->length = oidlen;
	return oid_ptr;
    }

    if(*working_ptr >= end_ptr) {
	DPRINTF((APWARN, "ParseOID, past end of packet.\n"));
	goto fail;
    }

    oid_ptr->oid_ptr[2] = 0;
    count = 0;
    first = *(*working_ptr);
    for (i = 0; i < length - 1; i++) {

	oid_ptr->oid_ptr[oidlen] =
	    (oid_ptr->oid_ptr[oidlen] << 7) + (*(*working_ptr) & 0x7F);

	if ((*(*working_ptr)++ & 0x80) == 0) {	/* is last octet */
	    count = 0;
            if (*working_ptr < end_ptr) {
	        first = *(*working_ptr);
            }
	    oidlen++;
	    if (i < length - 2)	{ /* -1 - i++ (from the loop) */
		oid_ptr->oid_ptr[oidlen] = 0;
	    }
	} else {
            count++;
            if (count == 5) {
                if (first & 0x70) {
                    DPRINTF((APWARN, "ParseOID, sid value too big.\n"));
		    goto fail;
                }
            }
            if (count > 5) {
                DPRINTF((APWARN, "ParseOID, sid value too big.\n"));
		goto fail;
            }
	}

        if (*working_ptr > end_ptr) {
	    DPRINTF((APWARN, "ParseOID, past end of packet.\n"));
	    goto fail;
        }
    }
    if (oidlen > MAX_OID_SIZE) {
        DPRINTF((APPACKET, "ParseOID: Bad sid string length: %d\n", oidlen));
	goto fail;
    }
    oid_ptr->length = oidlen;

    return (oid_ptr);
  fail:
    FreeOID(oid_ptr);
    NULLIT(oid_ptr);
    return (NULL);
}				/* end of ParseOID() */

#ifdef SR_SNMPv2_PDU
/*
 * ParseUInt64:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_uint64().
 */
UInt64 *
ParseUInt64(working_ptr, end_ptr)
    const unsigned char **working_ptr;
    const unsigned char  *end_ptr;
{
    UInt64         *uint64_ptr;
    SR_INT32        i;
    SR_INT32        length;
    short           type;

    if ((type = ParseType(working_ptr, end_ptr)) == -1) {
	DPRINTF((APPACKET, "ParseUInt64, type error.\n"));
	return (NULL);
    }

    if (type != COUNTER_64_TYPE) {
	DPRINTF((APPACKET, "ParseUInt64, type %x not COUNTER_64_TYPE\n", type));
	return (NULL);
    }

    if ((length = ParseLength(working_ptr, end_ptr)) == -1) {
	DPRINTF((APPACKET, "ParseUInt64, length error.\n"));
	return (NULL);
    }

    /* check length */
    if ((length > 9) || ((length > 8) && (*(*working_ptr) != 0x00))) {
	DPRINTF((APPACKET, "ParseUInt64, length error: %ld\n", length));
	return (NULL);
    }

    if ((uint64_ptr = (UInt64 *) malloc(sizeof(UInt64))) == NULL) {
	DPRINTF((APWARN, "ParseUInt64, uint64_ptr malloc\n"));
	return (NULL);
    }

    /* do we have that funky leading 0 octet because first bit is 1? */
    if (*(*working_ptr) == 0x00) {	/* if so, skip */
	(*working_ptr)++;
	length--;		/* and adjust length accordingly */
    }

    uint64_ptr->big_end = ((SR_INT32)0);
    uint64_ptr->little_end = ((SR_INT32)0);

    for (i = 0; i < length; i++) {
	uint64_ptr->big_end = (uint64_ptr->big_end << 8) +
	    (uint64_ptr->little_end >> 24);
	uint64_ptr->little_end = (uint64_ptr->little_end << 8) +
	    (SR_UINT32) *(*working_ptr)++;
    }

    if (*working_ptr > end_ptr) {
	DPRINTF((APWARN, "ParseUInt64, past end of packet.\n"));
	return (0);
    }

    return (uint64_ptr);
}				/* end of ParseUInt64() */
#endif /* SR_SNMPv2_PDU */

/*
 * ParseUInt:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_unsignedinteger().
 */
SR_UINT32
ParseUInt(working_ptr, end_ptr, type)
    const unsigned char **working_ptr;
    const unsigned char  *end_ptr;
    short                *type;
{
    SR_UINT32       value;
    SR_INT32        length;
    SR_INT32        i;

    if ((*type = ParseType(working_ptr, end_ptr)) == -1) {
	DPRINTF((APPACKET, "ParseUInt, type error.\n"));
	return (0);
    }

    if ((length = ParseLength(working_ptr, end_ptr)) == -1) {
	DPRINTF((APPACKET, "ParseUInt, length error.\n"));
        *type = -1;
	return (0);
    }

    /* check length */
    if ((length > 5) || ((length > 4) && (*(*working_ptr) != 0x00))) {
	DPRINTF((APPACKET, "ParseUInt, length error: %ld\n", length));
	*type = -1;		/* signal error */
	return (0);
    }

    /* do we have that funky leading 0 octet because first bit is 1? */
    if (*(*working_ptr) == 0x00) {	/* if so, skip */
	(*working_ptr)++;
	length--;		/* and adjust length accordingly */
    }

    value = ((SR_INT32)0);
    for (i = 0; i < length; i++) {
	value = (value << 8) + (SR_UINT32) *(*working_ptr)++;
    }

    if (*working_ptr > end_ptr) {
	DPRINTF((APWARN, "ParseUInt, past end of packet.\n"));
	*type = -1;
	return (0);
    }

    return (value);
}				/* end of ParseUInt() */

/*
 * ParseInt:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_signedinteger().
 */
SR_INT32
ParseInt(working_ptr, end_ptr, type)
    const unsigned char **working_ptr;
    const unsigned char  *end_ptr;
    short                *type;
{
    SR_INT32        value;
    SR_INT32        length;
    SR_INT32        i;
    SR_INT32        sign;

    value = ((SR_INT32)0);

    if ((*type = ParseType(working_ptr, end_ptr)) == -1) {
	DPRINTF((APPACKET, "parsed_signedinteger, type error.\n"));
	return (0);
    }

    if ((length = ParseLength(working_ptr, end_ptr)) == -1) {
	DPRINTF((APPACKET, "ParseInt, length error.\n"));
	return (0);
    }

    if (length > 4) {
	DPRINTF((APPACKET, "parsed_signedinteger, length error: %ld\n", length));
	*type = -1;
	return (0);
    }

    sign = ((*(*working_ptr) & 0x80) == 0x00) ? 0x00 : 0xff;

    for (i = 0; i < length; i++) {
	value = (value << 8) + (SR_UINT32) *(*working_ptr)++;
    }

    /* now fill in the upper bits with the appropriate sign extension. */
    for (i = length; i < 4; i++) {
	value = value + (sign << i * 8);
    }

    if (*working_ptr > end_ptr) {
	DPRINTF((APWARN, "ParseInt, past end of packet.\n"));
	*type = -1;
	return (0);
    }

    return (value);
}				/* end of ParseInt() */

/*
 * ParseNull:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_null().
 */
short
ParseNull(working_ptr, end_ptr, type)
    const unsigned char **working_ptr;
    const unsigned char  *end_ptr;
    short                *type;
{
    SR_INT32        length;

    *type = ParseType(working_ptr, end_ptr);

#ifndef SR_SNMPv2_PDU
    if (*type != NULL_TYPE) {
#else /* SR_SNMPv2_PDU */
    if ((*type != NULL_TYPE) &&
	(*type != NO_SUCH_OBJECT_EXCEPTION) &&
	(*type != NO_SUCH_INSTANCE_EXCEPTION) &&
	(*type != END_OF_MIB_VIEW_EXCEPTION)) {
#endif /* SR_SNMPv2_PDU */
	DPRINTF((APPACKET, "ParseNull, Unexpected type: %x\n", *type));
	*type = -1;
	return (-1);
    }

    if ((length = ParseLength(working_ptr, end_ptr)) == -1) {
	DPRINTF((APPACKET, "ParseNull, length error.\n"));
	return (-1);
    }

    if (length != 0) {
	DPRINTF((APPACKET, "ParseNull, length nonzero.\n"));
	return (-1);
    }

    if (*working_ptr > end_ptr) {
	DPRINTF((APWARN, "ParseNull, past end of packet.\n"));
	return (-1);
    }
    return (0);
}				/* end of ParseNull() */

/*
 * ParseSequence:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_sequence().
 */
SR_INT32
ParseSequence(working_ptr, end_ptr, type)
    const unsigned char **working_ptr;
    const unsigned char  *end_ptr;
    short                *type;
{
    SR_INT32        length;

    if ((*type = ParseType(working_ptr, end_ptr)) != SEQUENCE_TYPE) {
	DPRINTF((APPACKET, "ParseSequence, Unexpected type: %x\n", *type));
	*type = -1;
	return (-1);
    }

    if ((length = ParseLength(working_ptr, end_ptr)) == -1) {
	DPRINTF((APPACKET, "ParseSequence, length error.\n"));
	return (-1);
    }

    if (*working_ptr > end_ptr) {
	DPRINTF((APWARN, "ParseSequence, past end of packet.\n"));
	return (-1);
    }

    return (length);
}				/* end of ParseSequence() */

/*
 * ParseType:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_type().
 */
short
ParseType(working_ptr, end_ptr)
    const unsigned char **working_ptr;
    const unsigned char  *end_ptr;
{
    short           type;

    type = *(*working_ptr)++;

    if (*working_ptr > end_ptr) {
	DPRINTF((APWARN, "ParseType, past end of packet.\n"));
	return (-1);
    }

    switch (type) {
    case INTEGER_TYPE:
    case OCTET_PRIM_TYPE:
    case NULL_TYPE:
    case OBJECT_ID_TYPE:
    case SEQUENCE_TYPE:
    case IP_ADDR_PRIM_TYPE:
    case COUNTER_TYPE:
    case GAUGE_TYPE:
    case TIME_TICKS_TYPE:
    case OPAQUE_PRIM_TYPE:
    case GET_REQUEST_TYPE:
    case GET_NEXT_REQUEST_TYPE:
    case GET_RESPONSE_TYPE:
    case SET_REQUEST_TYPE:
#ifdef SR_SNMPv1_PACKET
    case TRAP_TYPE:
#endif /* SR_SNMPv1_PACKET */
#ifdef SR_SNMPv2_PDU
    case COUNTER_64_TYPE:
    case GET_BULK_REQUEST_TYPE:
    case INFORM_REQUEST_TYPE:
    case SNMPv2_TRAP_TYPE:
    case REPORT_TYPE:
    case NO_SUCH_OBJECT_EXCEPTION:
    case NO_SUCH_INSTANCE_EXCEPTION:
    case END_OF_MIB_VIEW_EXCEPTION:
#endif /* SR_SNMPv2_PDU */

#ifdef SR_SNMPv2_PACKET
    case V2_AUTH_MSG_TYPE:
#endif /* SR_SNMPv2_PACKET */

	return (type);
    default:
	DPRINTF((APWARN, "no such type in ParseType (%d) (0x%x)\n", type,type));
	break;
    };				/* switch */
    return (-1);
}				/* end of ParseType() */

/*
 * ParseLength:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_length().
 */
SR_INT32
ParseLength(working_ptr, end_ptr)
    const unsigned char **working_ptr;
    const unsigned char  *end_ptr;
{
    SR_INT32        length;
    SR_INT32        lenlen;
    SR_INT32        i;

    length = (SR_INT32) *(*working_ptr)++;

    if (length < 0x80)
	return (length);

    /* long form. */
    lenlen = length & 0x7f;

    if ((lenlen > 4) || (lenlen < 1)) {	/* not in standard, but I cannot
					 * handle it */
	DPRINTF((APPACKET, "ParseLength: lenlen invalid: (%d).\n", lenlen));
	return (-1);
    }

    length = ((SR_INT32)0);
    for (i = 0; i < lenlen; i++) {
	length = (length << 8) + *(*working_ptr)++;
    }

    if (*working_ptr > end_ptr) {
	DPRINTF((APWARN, "ParseLength, past end of packet.\n"));
	return (-1);
    }

    return (length);
}				/* end of ParseLength() */

/*
 * ParseImplicitSequence:
 *
 * The former name (pre-snmp12.1.0.0 release) was parse_implicit_sequence().
 */
SR_INT32
ParseImplicitSequence(working_ptr, end_ptr, type)
    const unsigned char **working_ptr;
    const unsigned char  *end_ptr;
    short                *type;
{
    SR_INT32        length;
    short           expected_type;

    expected_type = *type;

    if ((*type = ParseType(working_ptr, end_ptr)) != expected_type) {
	DPRINTF((APPACKET, "ParseImplicitSequence, Unexpected type: %x (expected %x)\n",
		 *(*working_ptr - 1), expected_type));
	*type = -1;
	return (-1);
    }

    if ((length = ParseLength(working_ptr, end_ptr)) == -1) {
	DPRINTF((APPACKET, "ParseImplicitSequence, length error.\n"));
	return (-1);
    }

    if (*working_ptr > end_ptr) {
	DPRINTF((APWARN, "ParseImplicitSequence, past end of packet.\n"));
	return (-1);
    }

    return (length);
}				/* end of ParseImplicitSequence() */
