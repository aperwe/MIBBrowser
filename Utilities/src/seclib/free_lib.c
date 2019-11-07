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
static const char rcsid[] = "$Id: free_lib.c,v 1.16 1998/01/06 18:44:14 mark Exp $";
#endif	/* (! ( defined(lint) ) && defined(SR_RCSID)) */

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif	/* HAVE_MALLOC_H */

#include "sr_snmp.h"

#include "diag.h"
SR_FILENAME



/* 
 * FreeOctetString: free the data structures allocated and built by 
 *                  MakeOctetString().  
 *
 * The former name (pre-snmp12.1.0.0 release) was free_octetstring().
 */
void
FreeOctetString(os_ptr)
    OctetString    *os_ptr;
{
    if (os_ptr != NULL) {
	if(os_ptr->octet_ptr !=
	   (((unsigned char *)os_ptr) + sizeof(OctetString))) {
	    if(os_ptr->octet_ptr != NULL) {
		DPRINTF((APWARN, "freed octet string with separate octet_ptr\n"));
#ifdef STDC_HEADERS
		free((void *) os_ptr->octet_ptr);
#else /* STDC_HEADERS */
		free((char *) os_ptr->octet_ptr);
#endif /* STDC_HEADERS */
	    }
	}
#ifdef STDC_HEADERS
	free((void *) os_ptr);
#else /* STDC_HEADERS */
	free((char *) os_ptr);
#endif /* STDC_HEADERS */
    }
}

/* 
 * FreeObjectSyntax:
 *
 */
void
FreeObjectSyntax(os)
    ObjectSyntax    *os;
{
    if (os != NULL) {
        switch (os->type) {
            case OBJECT_ID_TYPE:
                FreeOID(os->oid_value);
                break;
            case OCTET_PRIM_TYPE:
            case IP_ADDR_PRIM_TYPE:
            case OPAQUE_PRIM_TYPE:
                FreeOctetString(os->os_value);
                break;
            case COUNTER_64_TYPE:
                FreeUInt64(os->uint64_value);
                break;
        }

#ifdef STDC_HEADERS
	free((void *) os);
#else /* STDC_HEADERS */
	free((char *) os);
#endif /* STDC_HEADERS */
    }
}

/* 
 * FreeOID: free the data structures allocated and built by MakeOIDFromDot,
 *          etc.
 *
 * The former name (pre-snmp12.1.0.0 release) was free_oid().
 */
void
FreeOID(oid)
    OID            *oid;
{
    if (oid != NULL) {
	if(oid->oid_ptr != (SR_UINT32 *)
	   (((char *)oid) + sizeof(OID))) {
	    DPRINTF((APWARN, "freed oid with separate oid_ptr\n"));
	    if(oid->oid_ptr != NULL) {
#ifdef STDC_HEADERS
		free((void *) oid->oid_ptr);
#else /* STDC_HEADERS */
		free((char *) oid->oid_ptr);
#endif /* STDC_HEADERS */
	    }
	}
#ifdef STDC_HEADERS
	free((void *) oid);
#else /* STDC_HEADERS */
	free((char *) oid);
#endif /* STDC_HEADERS */
    }
}

/* 
 * FreeUInt64:
 *
 * The former name (pre-snmp12.1.0.0 release) was free_uint64().
 */
void
FreeUInt64(uint64_ptr)
    UInt64         *uint64_ptr;
{
    if (uint64_ptr != NULL) {
#ifdef STDC_HEADERS
	free((void *) uint64_ptr);
#else /* STDC_HEADERS */
	free((char *) uint64_ptr);
#endif /* STDC_HEADERS */
    }
}

/* 
 * FreeVarBind: free the data structures allocated and built by calls to
 *              MakeVarBindWithNull(), MakeVarBindWithValue(), etc.
 *
 * The former name (pre-snmp12.1.0.0 release) was free_varbind().
 */
void
FreeVarBind(vb_ptr)
    VarBind        *vb_ptr;
{
    if (vb_ptr != NULL) {
	FreeOID(vb_ptr->name);
	switch (vb_ptr->value.type) {
	    case OBJECT_ID_TYPE:
	        FreeOID(vb_ptr->value.oid_value);
	        break;
	    case OCTET_PRIM_TYPE:
	    case IP_ADDR_PRIM_TYPE:
	    case OPAQUE_PRIM_TYPE:
	        FreeOctetString(vb_ptr->value.os_value);
	        break;
	    case COUNTER_64_TYPE:
	        FreeUInt64(vb_ptr->value.uint64_value);
	        break;
	}
#ifdef STDC_HEADERS
	free((void *) vb_ptr);
#else /* STDC_HEADERS */
	free((char *) vb_ptr);
#endif /* STDC_HEADERS */
    }
}

/* 
 * FreePdu: free the data structures allocated and built by calls to
 *          MakePdu().
 *
 * The former name (pre-snmp12.1.0.0 release) was free_pdu().
 */
void
FreePdu(pdu_ptr)
    Pdu            *pdu_ptr;
{
    if (pdu_ptr != NULL) {
	FreeOctetString(pdu_ptr->packlet);

#ifdef SR_SNMPv1_PACKET
	if (pdu_ptr->type == TRAP_TYPE) {
	    FreeOID(pdu_ptr->u.trappdu.enterprise);
	    FreeOctetString(pdu_ptr->u.trappdu.agent_addr);
	}
#endif /* SR_SNMPv1_PACKET */

	FreeVarBindList(pdu_ptr->var_bind_list);
#ifdef STDC_HEADERS
	free((void *) pdu_ptr);
#else /* STDC_HEADERS */
	free((char *) pdu_ptr);
#endif /* STDC_HEADERS */
    }
}

/* 
 * FreeVarBindList: free the data structures allocated and built by calls to
 *                  MakeVarBindWithNull(), MakeVarBindWithValue(), etc.
 *
 * The former name (pre-snmp12.1.0.0 release) was free_varbind_list().
 */
void
FreeVarBindList(vb_ptr)
    VarBind        *vb_ptr;
{
    VarBind *next;

    while (vb_ptr != NULL) {
	next = vb_ptr->next_var;
	FreeVarBind(vb_ptr);
	vb_ptr = next;
    }
}
