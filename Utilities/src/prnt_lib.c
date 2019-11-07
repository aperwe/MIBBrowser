/************************************************************
*
*					AccessIntegrator
*
*	Copyright (C)  Siemens AG 2002  All Rights reserved.
*
************************************************************/

/**
*	@file		prnt_lib.c
*	@brief		<TODO>
*	@author		Artur Perwenis
*	@date		17-Jan-2002
*
*	@history
*	@item		17-Jan-2002		Artur Perwenis		Imported
*	@item		17-Jan-2002		Artur Perwenis		Calls to printf () replaced with calls to SendOutput ().
*/

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
static const char rcsid[] = "$Id: prnt_lib.c,v 1.32.4.1 1998/03/30 16:46:03 moulton Exp $";
#endif	/* (! ( defined(lint) ) && defined(SR_RCSID)) */

#include "sr_conf.h"

#include <stdio.h>

#include <ctype.h>

#include <stdlib.h>

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif	/* HAVE_MALLOC_H */

#include <string.h>


#include <sys/types.h>

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif /* HAVE_SYS_TIME_H */

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif	/* HAVE_SYS_SOCKET_H */

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif	/* HAVE_NETINET_IN_H */


#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif /* HAVE_ARPA_INET_H */

#include <winsock.h>


#define WANT_ALL_ERRORS

#include "sr_snmp.h"
#include "prnt_lib.h"
#include "oid_lib.h"
#include "diag.h"
//Artur Perwenis: 2002.01.17
#include "UExterns.h"
SR_FILENAME



char    *
inet_ltoa(l)
    SR_UINT32       l;
{
    struct in_addr  in;

    in.s_addr = l;
    return ((char *) inet_ntoa(in));
}

/*
 * PrintPacket:
 *
 *   This routine prints out the contents of a buffer in  hex  at
 *   16  bytes  per  line.   It  is  called with a pointer to the
 *   buffer to be displayed and the number of  bytes  to  display
 *   out  of  the buffer.  This call is frequently used in debug-
 *   ging code to display the actual SNMP message that  has  been
 *   received  to  allow hand parsing of the message.  It is gen-
 *   erally unsuitable for a production user interface.
 *
 * The name of this routine (pre-snmp12.1.0.0 release) was print_packet_out().
 */

void
PrintPacket(ptr, len)
    const unsigned char *ptr;
    SR_INT32        len;
{
    OctetString oct;

    printf("\nPacket Dump:");

    oct.octet_ptr = (unsigned char *) ptr;
    oct.length = len;

    PrintOctetString(&oct, 16);
}     /* PrintPacket() */


/*
 * PrintOctetString:
 *
 *   This routine prints out the contents  of  a  Octet  String's
 *   value  in  hex.   It  is called with a pointer to the Octet-
 *   String construct and the number of bytes to display  on  one
 *   line  (the  variable  'wrap').   This  call  is  used by the
 *   print_varbind() routine to actually print out  octet  string
 *   values.
 *
 * The name of this routine (pre-snmp12.1.0.0 release) was 
 * print_octet_string_out().
 */

void
PrintOctetString(oct_ptr, wrap)
    const OctetString *oct_ptr;
    int             wrap;
{
    SR_INT32        i;
    SR_INT32        pos;

    if (oct_ptr != NULL) {
	for (i = 0, pos = 1; i < oct_ptr->length; i++) {
	    if (((i % wrap) == 0) && (oct_ptr->length > 8)) {
		printf("\n");
	    }

#ifndef SIMPLE_PRINTF_FAMILY
	    printf("%2.2x ", oct_ptr->octet_ptr[i]);
#else
	    printf("%2x ", oct_ptr->octet_ptr[i]);
#endif

	    if ((oct_ptr->length >= 8) || ((oct_ptr->length % 4) == 0)) {
		if ((pos % 2) == 0) {
		    printf(" ");
		}
		if ((pos % 4) == 0) {
		    printf(" ");
		}
		if ((pos % 8) == 0) {
		    printf(" ");
		}
	    }

	    if ((pos % wrap) == 0) {
		pos = 1;
	    }
	    else {
		pos++;
	    }
	}
    }
    printf("\n");
}     /* PrintOctetString() */

/*
 * PrintVarBindList: 
 *
 *   This routine prints out the contents of a varbind list in  a
 *   human  readable  form.   This  is a quick user interface for
 *   printing out the SNMP responses in  simple  SNMP  utilities.
 *   If  the PDU structure is pointed to by *pdu_ptr, the call is
 *   PrintVarBindList(pdu_ptr->var_bind_list).
 *
 * The name of this routine (pre-snmp12.1.0.0 release) was
 * print_varbind_list().
 *
 * 11/1/93 DBL
 * Pulled printing code out into PrintVarBind function, changed recursive
 * calls to PrintVarBindList into a while loop.  */

int
PrintVarBindList(vb_ptr)
    const VarBind *vb_ptr;
{
    int result;

    if (vb_ptr == NULL) {
        return (1);
    }
    while (vb_ptr != NULL) {
        result = PrintVarBind(vb_ptr);
        if (result) {
            return (result);
        }
        vb_ptr = vb_ptr->next_var;
    }
    return(0);
}				/* end of PrintVarBindList() */

/*
 * PrintVarBind: 
 *
 *   This routine sprint's out the contents of a single varbind in  a
 *   human  readable  form.   This  is a quick user interface for
 *   printing out the SNMP responses in  simple  SNMP  utilities.
 *   The output is in the form "name = value"
 *
 * 11/1/93 DBL
 * Pulled printing code out of PrintVarBindList into this function.
 */

int
PrintVarBind(vb_ptr)
    const VarBind *vb_ptr;
{
    char *buffer1 = 0;
    int status, malloc_stat;

    /* allocate a buffer for the MakeDotFromOID() call.  The way we figure 
     * the buffer size is this:
     *    The max number of characters in a 32-bit sub-identifier is 10, plus
     *	   	1 for a dot totals 11.
     *	  Multiply this by the number of subidentifiers.
     *	  Add 256 to avoid problems with variables which have short OIDs but
     *		map to long names. */
    if((buffer1 =
	(char *)malloc((size_t)(256 + (11 * vb_ptr->name->length)))) == 0) {
	DPRINTF((APTRACE, "PrintVarBind: couldn't get buffer\n"));

	/* give the buffer a noticable value */
	buffer1 = "LongOIDName";

	/* set a flag so we know we can't free this buffer later on */
	malloc_stat = 0;
    } else {
	/* we can free the buffer later */
	malloc_stat = 1;
    }

    /* make the string */
    if (MakeDotFromOID(vb_ptr->name, buffer1) == -1) {
	DPRINTF((APTRACE, "PrintVarBind, vb_ptr->name:\n"));
	return (-1);
    }

    /* print the string */
//    printf("%s = ", buffer1);
	sprintf (pszOutputBuffer, "%s = ", buffer1); SendOutput ();
    status = PrintVarBindValue(vb_ptr, buffer1);

    /* free the buffer */
    if(malloc_stat != 0) free(buffer1);
    return(status);
}


/*
 * PrintVarBindValue: 
 *
 *   This routine sprint's out the value of a single varbind in  a
 *   human  readable  form.   This  is a quick user interface for
 *   printing out the SNMP responses in  simple  SNMP  utilities.
 *
 * 11/3/93 DBL
 * Pulled printing code out of PrintVarBind into this function.
 *
 * Feb-4-1994 AMP
 * Made sure that the values are printed with newlines at the end.
 */

int
PrintVarBindValue(vb_ptr, name)
    const VarBind *vb_ptr;
    const char *name;
{
    char *buffer, *bits;

    switch (vb_ptr->value.type) {
    case COUNTER_TYPE:		/* handle unsigned integers includes
				 * COUNTER_32 */
    case GAUGE_TYPE:		/* includes GAUGE_32 */
    case TIME_TICKS_TYPE:
//		printf("%lu\n", (unsigned long)vb_ptr->value.ul_value);
		sprintf (pszOutputBuffer, "%lu\n", (unsigned long)vb_ptr->value.ul_value); SendOutput ();
	break;
    case INTEGER_TYPE:		/* handle signed integers includes INTEGER_32 */

        if (name)
		{
//		    printf("%s\n", GetType(name, vb_ptr->value.sl_value));
		    sprintf (pszOutputBuffer, "%s\n", GetType(name, vb_ptr->value.sl_value)); SendOutput ();
        }
		else
		{
#if SIZEOF_INT == 4
//			printf("%d\n", vb_ptr->value.sl_value);
			sprintf (pszOutputBuffer, "%d\n", vb_ptr->value.sl_value); SendOutput ();
#else	/*  SIZEOF_INT == 4 */
//			printf("%ld\n", vb_ptr->value.sl_value);
			sprintf (pszOutputBuffer, "%ld\n", vb_ptr->value.sl_value); SendOutput ();
#endif	/*  SIZEOF_INT == 4 */
        }

	break;
    case IP_ADDR_PRIM_TYPE:
//		printf("%d.%d.%d.%d\n",
//			   vb_ptr->value.os_value->octet_ptr[0],
//			   vb_ptr->value.os_value->octet_ptr[1],
//			   vb_ptr->value.os_value->octet_ptr[2],
//			   vb_ptr->value.os_value->octet_ptr[3]);
		sprintf (pszOutputBuffer,
			"%d.%d.%d.%d\n",
			vb_ptr->value.os_value->octet_ptr[0],
			vb_ptr->value.os_value->octet_ptr[1],
			vb_ptr->value.os_value->octet_ptr[2],
			vb_ptr->value.os_value->octet_ptr[3]); SendOutput ();
	break;
    case OBJECT_ID_TYPE:
	/* allocate a buffer for the MakeDotFromOID() call.  The way we figure 
	 * the buffer size is this:
	 *  The max number of characters in a 32-bit sub-identifier is 10, 
	 *	plus 1 for a dot totals 11.
	 *  Multiply this by the number of subidentifiers.
	 *  Add 256 to avoid problems with variables which have short OIDs but
	 *	map to long names. */
	if((buffer = (char *)malloc((size_t)(256 + (11 * vb_ptr->value.oid_value->length)))) == 0) {
	    DPRINTF((APTRACE, "PrintVarBindValue: couldn't get buffer\n"));
	    return(-1);
	}

	/* format and print the oid string */
	if (MakeDotFromOID(vb_ptr->value.oid_value, buffer) == -1) {
	    DPRINTF((APTRACE, "PrintVarBindValue, vb_ptr->value.oid_value:\n"));
	    return (-1);
	}
//		printf("%s\n", buffer);
		sprintf (pszOutputBuffer, "%s\n", buffer); SendOutput ();

	/* free the buffer */
	free(buffer);
	break;
    case OCTET_PRIM_TYPE:	/* handle quasi-octet strings */
        /* GetBitsType() returns non null if it can resolve the bits */
	if ((bits = GetBitsType(name, vb_ptr->value.os_value)) != NULL) {
	    puts(bits);
	    free(bits);
            break;
        }
	/*FALLTHROUGH*/
    case OPAQUE_PRIM_TYPE:
	if (PrintAscii(vb_ptr->value.os_value) < 0) {
            /* cannot print ascii */
	    PrintOctetString(vb_ptr->value.os_value, 16);
	}
	break;
    case NULL_TYPE:
//		printf("NULL TYPE/VALUE\n");
		sprintf (pszOutputBuffer, "NULL TYPE/VALUE\n"); SendOutput ();
	break;
    case COUNTER_64_TYPE:
	printf("0x%x%08x\n",
                (unsigned int)vb_ptr->value.uint64_value->big_end,
	        (unsigned int)vb_ptr->value.uint64_value->little_end);
	break;
    case NO_SUCH_OBJECT_EXCEPTION:
	printf("NO_SUCH_OBJECT_EXCEPTION\n");
	break;
    case NO_SUCH_INSTANCE_EXCEPTION:
	printf("NO_SUCH_INSTANCE_EXCEPTION\n");
	break;
    case END_OF_MIB_VIEW_EXCEPTION:
	printf("END_OF_MIB_VIEW_EXCEPTION\n");
	break;

    default:
	DPRINTF((APTRACE,
		 "PrintVarBindValue: Illegal type: 0x%x\n", 
		 vb_ptr->value.type));

	return (0);
    };				/* end of switch */

    /* do next one now */
    return (0);
}				/* end of PrintVarBindValue() */

/*
 * PrintAscii:
 *
 *   This routine prints out the contents  of  a  Octet  String's
 *   value  as  an ascii string if the value only contains print-
 *   able characters.  It is called with a pointer to the  Octet-
 *   String  construct and checks if the string is printable.  If
 *   it is not it returns a -1 value, otherwise it returns a 1.
 *
 * The name of this routine (pre-snmp12.1.0.0 release) was print_ascii().
 *
 * 11/1/93 DBL
 * Did NOT convert this to be a wrapper around SPrintAscii, as this would
 * require a char string buffer of size > MAX_OCTET_STRING_SIZE.
 */

int
PrintAscii(os_ptr)
    const OctetString *os_ptr;
{
    SR_INT32        i;

    for (i = 0; i < os_ptr->length; i++) {
	/* ASCII space is not a printable character in MSC */
	if (((!isprint((int) os_ptr->octet_ptr[i])) && 
             (os_ptr->octet_ptr[i] != 0x20) &&
	     (os_ptr->octet_ptr[i] != 0x0a) && 
             (os_ptr->octet_ptr[i] != 0x0d)) && 
             (!((os_ptr->octet_ptr[i] == 0x00) && 
                (i == os_ptr->length - 1)))) {
	    return (-1);
	}
    }

    for (i = 0; i < os_ptr->length; i++) {
	if (os_ptr->octet_ptr[i]) {
	    printf("%c", os_ptr->octet_ptr[i]);
	}
    }

    printf("\n");
    return (1);
}     /* PrintAscii() */



/*
 * PrintErrorCode:
 *
 * The name of this routine (pre-snmp12.1.0.0 release) was print_error_code().
 */

void
PrintErrorCode(error_index, error_status)
    SR_INT32        error_index;
    SR_INT32        error_status;
{

    printf("Error code set in packet - ");
    switch ((short) error_status) {
    case TOO_BIG_ERROR:
	printf("Return packet too big.\n");
	break;
    case NO_SUCH_NAME_ERROR:
	printf("No such variable name.  Index:  %d.\n", (int)error_index);
	break;
    case BAD_VALUE_ERROR:
	printf("Bad variable value.  Index:  %d.\n", (int)error_index);
	break;
    case READ_ONLY_ERROR:
	printf("Read only variable:  %d.\n", (int)error_index);
	break;
    case GEN_ERROR:
	printf("General error:  %d.\n", (int)error_index);
	break;
#ifdef SR_SNMPv2_PDU
    case NO_ACCESS_ERROR:
	printf("NO_ACCESS_ERROR: %d.\n", (int)error_index);
	break;
    case WRONG_TYPE_ERROR:
	printf("WRONG_TYPE_ERROR: %d.\n", (int)error_index);
	break;
    case WRONG_LENGTH_ERROR:
	printf("WRONG_LENGTH_ERROR: %d.\n", (int)error_index);
	break;
    case WRONG_ENCODING_ERROR:
	printf("WRONG_ENCODING_ERROR: %d.\n", (int)error_index);
	break;
    case WRONG_VALUE_ERROR:
	printf("WRONG_VALUE_ERROR: %d.\n", (int)error_index);
	break;
    case NO_CREATION_ERROR:
	printf("NO_CREATION_ERROR: %d.\n", (int)error_index);
	break;
    case INCONSISTENT_VALUE_ERROR:
	printf("INCONSISTENT_VALUE_ERROR: %d.\n", (int)error_index);
	break;
    case RESOURCE_UNAVAILABLE_ERROR:
	printf("RESOURCE_UNAVAILABLE_ERROR: %d.\n", (int)error_index);
	break;
    case COMMIT_FAILED_ERROR:
	printf("COMMIT_FAILED_ERROR: %d.\n", (int)error_index);
	break;
    case UNDO_FAILED_ERROR:
	printf("UNDO_FAILED_ERROR: %d.\n", (int)error_index);
	break;
    case AUTHORIZATION_ERROR:
	printf("AUTHORIZATION_ERROR: %d.\n", (int)error_index);
	break;
    case NOT_WRITABLE_ERROR:
	printf("NOT_WRITABLE_ERROR: %d.\n", (int)error_index);
	break;
    case INCONSISTENT_NAME_ERROR:
	printf("INCONSISTENT_NAME_ERROR: %d.\n", (int)error_index);
	break;
#endif /* SR_SNMPv2_PDU */
    default:
	printf("Unknown status code:  %d.\n", (int)error_status);
	break;
    };

}     /* PrintErrorCode() */



#ifdef SR_DEBUG

/*
 * PrintOID:
 *
 * The name of this routine (pre-snmp12.1.0.0 release) was print_oid_out().
 */
void
PrintOID(oid)
    const OID *oid;
{
    SR_INT32        i;

    for (i = 0; i < oid->length; i++) {
	printf("%lu.", (unsigned long)oid->oid_ptr[i]);
    }
    printf("\n");
}     /* PrintOID() */


#endif				/* SR_DEBUG */
