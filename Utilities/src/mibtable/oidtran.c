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
static const char rcsid[] = "$Id: oidtran.c,v 1.24 1998/01/06 18:53:34 mark Exp $";
#endif	/* (! ( defined(lint) ) && defined(SR_RCSID)) */

#include "sr_conf.h"

#include <stdio.h>

#include <string.h>

#include <ctype.h>

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif  /* HAVE_MALLOC_H */

#include <stdlib.h>


#ifdef NOTICE
#undef NOTICE   /* DO NOT REMOVE */
#endif  /* NOTICE */
#include "sr_snmp.h"


#include "mib_tabl.h"
#include "oid_lib.h"
#include "diag.h"
SR_FILENAME

extern int      global_InitNewOIDRoutines;


#ifdef SR_MIB_TABLE_HASHING
extern struct MIB_OID *name_oid_hash_array[NAME_OID_HASH_SIZE];
#endif /* SR_MIB_TABLE_HASHING */

/*
 * Routines to create an object identifer in an OID from
 * dot notation input.
 */

/*
 * MakeOIDFragFromDot:
 *
 *   This routine is called to create a library form object identifier
 *   from an character string.  The string input is usually in the
 *   format "integer.integer.integer...."  (i.e.,
 *   "1.3.6.1.2.1.1.1.0"), but can be shortened by using the names as
 *   they appear in RFC 1157 (i.e., "sysDescr.0").  It returns a
 *   pointer to a malloc'ed data structure containing the internal
 *   library representation for an object identifier.  This identifier
 *   can then be used in calls to make_varbind() and MakePdu() (in the
 *   case of traps).  This malloc'ed data structure will be free'ed by
 *   calls to FreePdu() after the pointer has been used.
 *
 */
OID *MakeOIDFragFromDot (const char *text_str)
{
	OID            *oid_ptr = NULL;
	const char     *temp_ptr, *dot_ptr, *english_dot;
	int             i, cc;
	int             dot_count;
	int             prefixlen;
#ifdef SR_MIB_TABLE_HASHING
	struct MIB_OID *hash_ptr;
	int             hash;
#endif   /* SR_MIB_TABLE_HASHING */

	if (global_InitNewOIDRoutines == 0)
	{
		InitNewOIDRoutines ();
	}

	if (text_str == NULL)
	{
		goto fail;
	}

	dot_ptr = text_str;
	english_dot = NULL;

    /* see if there is an alpha descriptor at begining */
    if (isalpha(*text_str))
	{
		dot_ptr = strchr(text_str, '.');
		prefixlen = strcspn(text_str, ".");
		if (dot_ptr != NULL) 
		{
			dot_ptr++;
		}

#ifndef SR_MIB_TABLE_HASHING
		english_dot = NULL;
		for (i = 0; i < num_mot_entries; i++)
		{
			if (!strncmp(mib_oid_table[i].name, text_str, MAX(prefixlen, strlen(mib_oid_table[i].name)))) 
			{
				english_dot = mib_oid_table[i].number;
				break;
			}
		}
		if (english_dot == NULL)
		{
			DPRINTF((APPACKET, "MakeOIDFragFromDot, table lookup failed: %s\n", text_str));
			goto fail;
		}
#else   /* SR_MIB_TABLE_HASHING */
		hash = 0;
		temp_ptr = text_str;
		i = 0;
		while (*temp_ptr != '\0' && i++ < prefixlen)
		{
			hash = (hash + (int)*temp_ptr) % (int) NAME_OID_HASH_SIZE;
			temp_ptr++;
		}

		for (hash_ptr = name_oid_hash_array[hash]; hash_ptr != NULL; hash_ptr = hash_ptr->hash_next) 
		{
			if (!strncmp(hash_ptr->name, text_str, MAX((unsigned)prefixlen, strlen (hash_ptr->name))))
			{
				break;
			}
		}
		if (hash_ptr == NULL)
		{
			DPRINTF((APPACKET, "MakeOIDFragFromDot, hash table lookup failed: %s\n", text_str));
			goto fail;
		}

		english_dot = hash_ptr->number;

#endif                          /* SR_MIB_TABLE_HASHING */
    }

	/* count the dots.  num +1 is the number of SIDs */
	dot_count = 0;
	if (english_dot && dot_ptr)
	{
		dot_count = 1;
	}
	if (english_dot)
	{
		for (i=0; english_dot[i] != '\0'; i++)
		{
			if (english_dot[i] == '.')
			{
				dot_count++;
			}
		}
	}
	if (dot_ptr)
	{
		for (i=0; dot_ptr[i] != '\0'; i++)
		{
			if (dot_ptr[i] == '.')
			{
				dot_count++;
			}
		}
	}
	if ((oid_ptr = MakeOID (NULL, dot_count + 1)) == NULL)
	{
		DPRINTF ((APPACKET, "MakeOIDFragFromDot, oid_ptr MakeOID\n"));
		goto fail;
	}

    /* now we convert number.number.... strings */
    if (english_dot)
	{
        temp_ptr = english_dot;
        cc = 0;
    }
	else
	{
        temp_ptr = dot_ptr;
        cc = 1;
    }

    for (i = 0; i < dot_count + 1; i++)
	{
        if (*temp_ptr == '0')
		{
            if ((temp_ptr[1] == 'x') || (temp_ptr[1] == 'X'))
			{
                if ((ParseSubIdHex ((const unsigned char **) &temp_ptr, &oid_ptr->oid_ptr[i])) == ~(unsigned) 0)
				{
                    DPRINTF ((APPACKET, "MakeOIDFragFromDot, hex:\n"));
					goto fail;
                }
            }
            else
			{
                if ((ParseSubIdOctal ((const unsigned char **) &temp_ptr, &oid_ptr->oid_ptr[i])) == ~(unsigned) 0) 
				{
                    DPRINTF ((APPACKET, "MakeOIDFragFromDot, octal:\n"));
					goto fail;
                }
            }
        }
		else
		{
			if (*temp_ptr >= '0' && *temp_ptr <= '9')
			{
				if ((ParseSubIdDecimal ((const unsigned char **) &temp_ptr, &oid_ptr->oid_ptr[i])) == ~(unsigned) 0)
				{
					DPRINTF((APPACKET, "MakeOIDFragFromDot, decimal:\n"));
					goto fail;
				}
			}
			else
			{
				DPRINTF ((APPACKET, "MakeOIDFragFromDot, bad character: %d, %s\n", *temp_ptr, temp_ptr));
				goto fail;
			}
		}

		if (*temp_ptr == '.')
		{
			temp_ptr++;         /* to skip over dot */
		}
		else if (*temp_ptr != '\0') 
		{
			DPRINTF((APPACKET, "MakeOIDFragFromDot, expected dot: %d %s\n", *temp_ptr, temp_ptr));
			goto fail;
		} 
		else if ((cc == 0) && dot_ptr) 
		{
			temp_ptr = dot_ptr;
		}
	}                           /* end of for loop */

    return (oid_ptr);

  fail:
    if(oid_ptr != NULL) 
	{
		FreeOID (oid_ptr);
		NULLIT (oid_ptr);
    }
    return NULL;
}     /* MakeOIDFragFromDot() */



short MakeDotFromOID (const OID *oid_ptr, char buffer[])
{
	buffer[0] = '\0';
	if ((oid_ptr == NULL) || (oid_ptr->length == 0) || (oid_ptr->oid_ptr == NULL)) 
	{
		DPRINTF ((APPACKET, "MakeDotFromOID, bad (NULL) OID\n"));
		return (-1);
	}

	if (global_InitNewOIDRoutines == 0)
	{
		InitNewOIDRoutines();
	}

	if (GetStringFromSubIds (buffer, oid_ptr->oid_ptr, (short) oid_ptr->length) == -1) 
	{
		DPRINTF((APPACKET, "MakeDotFromOID, GetStringFromSubIds\n"));
		return (-1);
	}

	return (0);
}


/*
 * GetType:
 *
 *   The name of this routine (pre-snmp12.1.0.0 release) was get_type().
 */
char *GetType (const char *string, SR_INT32 value)
{
    static char     ret[32];
#if SIZEOF_INT == 4
    sprintf(ret, "%d", value);
#else	/*  SIZEOF_INT == 4 */
    sprintf(ret, "%ld", value);
#endif	/*  SIZEOF_INT == 4 */
    return (ret);
}     /* GetType() */

/*
 * GetBitsType:
 */
char *GetBitsType (const char *string, const OctetString *value)
{
	return NULL;
}     /* GetBitsType() */


/*
 * GetBaseOIDString
 */
char *GetBaseOIDString (const OID *oid)
{
	char *number, *p;
	SR_UINT32 i;

	number = (char *)malloc(oid->length * 11);
	if (number == NULL) 
	{
		return NULL;
	}

	for (p = number, i = 0; i < oid->length; p += strlen(p), i++)
	{
		sprintf(p, "%lu.", (unsigned long)oid->oid_ptr[i]);
	}
	p[0] = '\0';

	for (i = 0; mib_oid_table[i].name != NULL; i++)
	{
		if (!strcmp(mib_oid_table[i].number, number))
		{
			return mib_oid_table[i].name;
		}
	}
	return NULL;
}
