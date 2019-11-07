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

#include "sr_conf.h"

#include <stdio.h>

#include <string.h>

#include <ctype.h>

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif  /* HAVE_MALLOC_H */

#include <stdlib.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif	/* HAVE_MEMORY_H */

#include <sys/types.h>

#include <sys/stat.h>

#include <fcntl.h>

#include "sr_snmp.h"
#include "compat.h"
#include "sr_cfg.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "oid_lib.h"
#include "diag.h"



SR_FILENAME


#ifdef SR_CONFIG_FP

/* static functions */
static void FillInScalarVar
    SR_PROTOTYPE((const PARSER_RECORD_TYPE *rtp,
                  ValType *vp));
static int CreateTableEntry
    SR_PROTOTYPE((const PARSER_RECORD_TYPE *rtp,
                  ValType *vp));
static char *ReadLines
    SR_PROTOTYPE((char *buf,
                  int size,
                  ConfigFile *cfp));
static int ReadConfigLine
    SR_PROTOTYPE((ConfigFile *cfp,
                  char *buf,
                  int size));
static void nexttoken
    SR_PROTOTYPE((char **str1,
                  char **str2));
static void FreeValTypes
    SR_PROTOTYPE((const SnmpType *tt,
                  ValType *vp,
                  int count));
static int ProcessConfigRecord
    SR_PROTOTYPE((int linecount,
                  char *cfgline,
                  const SnmpType *type_table,
                  const PARSER_CONVERTER *converters,
                  ValType *values));
static int ProcessConfigRecordForcingIndexOrder
    SR_PROTOTYPE((int linecount,
                  char *cfgline,
                  const SnmpType *type_table,
                  const PARSER_CONVERTER *converters,
                  ValType *values));
#ifndef SR_NO_WRITE_CONFIG_FILE
static int WriteScalarEntry
    SR_PROTOTYPE((ConfigFile *cfgp,
                  const char *label,
                  const SnmpType *type_table,
                  const PARSER_CONVERTER *converters,
                  void *scalar));
static int WriteTableEntry
    SR_PROTOTYPE((ConfigFile *cfgp,
                  const char *label,
                  const SnmpType *type_table,
                  const PARSER_CONVERTER *converters,
                  char *entry,
                  SR_INT32 rs_offset,
                  SR_INT32 st_offset,
                  SR_INT32 us_offset));
static int WriteTableEntryForcingIndexOrder
    SR_PROTOTYPE((ConfigFile *cfgp,
                  const char *label,
                  const SnmpType *type_table,
                  const PARSER_CONVERTER *converters,
                  char *entry,
                  SR_INT32 rs_offset,
                  SR_INT32 st_offset,
                  SR_INT32 us_offset));

static int CreateBackupFile
    SR_PROTOTYPE((const char *filename,
                  const char *backup_filename));
#endif /* SR_NO_WRITE_CONFIG_FILE */

/*
 * ReadLines:
 *
 * This routine simulates a call to fgets, except that if a line ends with
 * a backslash (line continuation), the next line will be read and appended.
 * the maximum length of all lines read is still specified by the size arg.
 */
static char *
ReadLines(buf, size, cfp)
    char *buf;
    int size;
    ConfigFile *cfp;
{
    char *orig_buf = buf;

    int len;
    cfp->startline = 0;
    while (sr_fgets(buf, size, cfp) != NULL) {
        cfp->linecount++;
        len = strlen(buf);
        if (len == 0) {
            continue;
        }
        if (buf[len-1] == '\n') {
            buf[len-1] = '\0';
            len--;
        }
        if (len == 0) {
            continue;
        }
        if (cfp->startline == 0) {
            cfp->startline = cfp->linecount;
        }
        if (buf[len-1] != '\\') {
            return(orig_buf);
        }
        buf += len - 1;
        buf[0] = '\0';
        size -= len + 1;
    }
    if (orig_buf == buf) {
        return NULL;
    }
    return orig_buf;
}

/*
 * ReadConfigLine:
 * this routine reads through a config file until it finds a non-blank,
 * non-comment line.  It strips out any comments, then returns the line
 */
static int
ReadConfigLine(cfp, buf, size)
    ConfigFile *cfp;
    char *buf;
    int size;
{
    int status;
    char *p;

    /* skip comments and whitespace */
    status = 0;
    while(status == 0 && ReadLines(buf, size, cfp) != 0) {
        /* keep track of the line number */

        /* strip out comments */
/*
        if((p = strchr(buf, '#')) != 0) {
            *p = 0;
        }
*/

        /* see if there is anything left on the line */
        for(p = buf; *p != 0; p++) {
            if(!isspace(*p)) {
                if (*p == '#') {
                    break;
                }
                status = 1;
                break;
            }
        }
    }

    DPRINTF((APCONFIG, "ReadConfigLine: buffer is: %s\n", buf));
    /* return nonzero if there is anything in buf */
    return(status);
}

static void
nexttoken(str1, str2)
    char **str1, **str2;
{
    int bsc = 0;
    char *s1 = *str2, *s2;
    char tokend;

    if (s1 == NULL) {
        *str1 = NULL;
        return;
    }

    while ((*s1 == ' ') || (*s1 == '\t')) {
        s1++;
    }
    tokend = ' ';
    if (*s1 == '\"') {
        tokend = '\"';
        s1++;
    } else
    if (*s1 == '\'') {
        tokend = '\'';
        s1++;
    }
    s2 = s1;
/*
    while ((*s2 != '\0') && (*s2 != tokend)) {
        s2++;
    }
*/
    while (1) {
        if (*s2 == '\0') {
            break;
        }
        if ((*s2 == tokend) && (bsc == 0)) {
            break;
        }
        if (*s2 == '\\') {
            bsc = (bsc + 1) % 2;
        } else {
            bsc = 0;
        }
        s2++;
    }
    if (*s2 != '\0') {
        *s2 = '\0';
        s2++;
    } else {
        s2 = NULL;
    }
    if (*s1 == '\0') {
        s1 = NULL;
    }

    *str1 = s1;
    *str2 = s2;
}

SR_INT32
ConvToken_integer(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    SR_INT32 *local = (SR_INT32 *)value;
    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            *local = strtol(*token, NULL, 10);
            return 0;
        case PARSER_CONVERT_TO_TOKEN:
            *token = (char *)malloc(32);
            if (*token == NULL) {
                return -1;
            }
            sprintf(*token, "%d", *local);
#if SIZEOF_INT == 4
            sprintf(*token, "%d", *local);
#else	/*  SIZEOF_INT == 4 */
            sprintf(*token, "%ld", *local);
#endif	/*  SIZEOF_INT == 4 */
            return 0;
        default:
            return -1;
    }
}

SR_INT32
ConvToken_unsigned(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    SR_UINT32 *local = (SR_UINT32 *)value;
    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            *local = (unsigned long)strtol(*token, NULL, 10);
            return 0;
        case PARSER_CONVERT_TO_TOKEN:
            *token = (char *)malloc(32);
            if (*token == NULL) {
                return -1;
            }
            sprintf(*token, "%lu", (unsigned long)(*local));
            return 0;
        default:
            return -1;
    }
}

#endif /* SR_CONFIG_FP */

#ifdef NOT_DEFINED
/*
 *  Note - ConvToken_OID is used for purposes other than
 *  parsing config files, so it needs to be outside of SR_CONFIG_FP
 *  controlled ifdefs
 */
#endif /* NOT_DEFINED */

SR_INT32
ConvToken_OID(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    OID **local = (OID **)value;
    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            if(strcmp(*token, "-") == 0) {
                *local = MakeOID(NULL, 0);
                if (*local == NULL) {
                    DPRINTF((APWARN, "Can't make '%s' into an OID\n", *token));
                    return -1;
                }
            } else {
                *local = MakeOIDFromDot(*token);
                if (*local == NULL) {
                    DPRINTF((APWARN, "Can't make '%s' into an OID\n", *token));
                    return -1;
                }
            }
            return 0;
        case PARSER_CONVERT_TO_TOKEN:
            if (*local == NULL) {
                *token = (char *)malloc(2);
                if (*token == NULL) {
                    return -1;
                }
                strcpy(*token, "-");
            } else if ((*local)->length == 0) {
                *token = (char *)malloc(2);
                if (*token == NULL) {
                    return -1;
                }
                strcpy(*token, "-");
            } else {
                *token = (char *)malloc(128 + ((*local)->length * 12));
                if (*token == NULL) {
                    return -1;
                }
                MakeDotFromOID(*local, *token);
            }
            return 0;
        default:
            return -1;
    }
}

#ifdef SR_CONFIG_FP

SR_INT32
ConvToken_textOctetString(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    OctetString **local = (OctetString **)value;
    int i, j, printable, needquotes;
    int len;
    char *tmp;

    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            if (strcmp(*token, "-") == 0) {
                *local = MakeOctetString(NULL, 0);
            } else {
                len = strlen(*token);
                if ((len % 3) == 2) {
                    for (i = 2; i < len; i += 3) {
                        if ((*token)[i] != ':') {
                            break;
                        }
                    }
                    if (i >= len) {
                        *local = MakeOctetStringFromHex(*token);
                        if (*local == NULL)  {
                            *local = MakeOctetStringFromText(*token);
                        }
                    } else {
                        *local = MakeOctetStringFromText(*token);
                    }
                } else {
                    *local = MakeOctetStringFromText(*token);
                }
                if (*local == NULL) {
                    /* print a warning and assign a fake string */
                    DPRINTF((APWARN, "Can't make '%s' into an OctetString\n",
                             *token));
                    *local = MakeOctetString(NULL, 0);
                }
            }
            return 0;
        case PARSER_CONVERT_TO_TOKEN:
            if (*local == NULL) {
                *token = (char *)malloc(2);
                if (*token == NULL) {
                    return -1;
                }
                strcpy(*token, "-");
            } else if ((*local)->length == 0) {
                *token = (char *)malloc(2);
                if (*token == NULL) {
                    return -1;
                }
                strcpy(*token, "-");
            } else {
                printable = 0;
                for (i = 0, j = 0; i < (*local)->length; i++, j++) {
                    if ((j % 3) < 2) {
                        if ((((*local)->octet_ptr[i] < '0') ||
                             ((*local)->octet_ptr[i] > '9')) &&
                            (((*local)->octet_ptr[i] < 'a') ||
                             ((*local)->octet_ptr[i] > 'f')) &&
                            (((*local)->octet_ptr[i] < 'A') ||
                             ((*local)->octet_ptr[i] > 'F'))) {
                            printable = 1;
                            break;
                        }
                    } else {
                        if ((*local)->octet_ptr[i] != ':') {
                            printable = 1;
                            break;
                        }
                    }
                }
                if (printable) {
                    for (i = 0; i < (*local)->length; i++) {
                        if (!isprint((char)(*local)->octet_ptr[i])) {
                            printable = 0;
                            if ((char)(*local)->octet_ptr[i] == '\n') {
                                printable = 1;
                                continue;
                            }
                            if ((char)(*local)->octet_ptr[i] == '\r') {
                                printable = 1;
                                continue;
                            }
                            if ((char)(*local)->octet_ptr[i] == '\t') {
                                printable = 1;
                                continue;
                            }
                            break;
                        }
                    }
                }
                if (printable) {
                    *token = (char *)malloc((*local)->length * 2 + 3);
                    if (*token == NULL) {
                        return -1;
                    }
                    needquotes = 0;
                    for (i = 0; i < (*local)->length; i++) {
                        if ((*local)->octet_ptr[i] == ' ' ||
			    (*local)->octet_ptr[i] == '\n' ||
			    (*local)->octet_ptr[i] == '\r' ||
			    (*local)->octet_ptr[i] == '\t') {
                            needquotes = 1;
                        }
                    }
                    if (needquotes) {
                        memcpy((*token)+1,(*local)->octet_ptr,
                               (int)(*local)->length);
                        (*token)[0] = '\"';
                        for (i=0,j=1; i<(int)(*local)->length; i++,j++) {
                            if ((*local)->octet_ptr[i] == '"') {
                                (*token)[j++] = '\\';
                                (*token)[j] = '"';
                            } else if ((*local)->octet_ptr[i] == '\\') {
                                (*token)[j++] = '\\';
                                (*token)[j] = '\\';
                            } else if ((*local)->octet_ptr[i] == '\n') {
                                (*token)[j++] = '\\';
                                (*token)[j] = 'n';
                            } else if ((*local)->octet_ptr[i] == '\r') {
                                (*token)[j++] = '\\';
                                (*token)[j] = 'r';
                            } else if ((*local)->octet_ptr[i] == '\t') {
                                (*token)[j++] = '\\';
                                (*token)[j] = 't';
                            } else {
                                (*token)[j] = (*local)->octet_ptr[i];
                            }
                        }
                        (*token)[j++] = '\"';
                        (*token)[j] = '\0';
                    } else {
                        memcpy((*token), (*local)->octet_ptr, 
                               (int)(*local)->length);
                        (*token)[(*local)->length] = '\0';
                    }
                } else {
                    *token = (char *)malloc((*local)->length * 3 + 3);
                    if (*token == NULL) {
                        return -1;
                    }
                    tmp = *token;
                    for (i = 0; i < (*local)->length; i++) {
                        sprintf(tmp, "%02x:", (*local)->octet_ptr[i]);
                        tmp += 3;
                    }
                    tmp--;
                    tmp[0] = '\0';
                }
            }
            return 0;
        default:
            return -1;
    }
}

SR_INT32
ConvToken_octetString(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    OctetString **local = (OctetString **)value;
    char *p;
    SR_INT32 i;
    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            if(strcmp(*token, "-") != 0) {
                for (p = *token; *p != '\0'; p++) {
                    if (*p == ':') {
                        *p = ' ';
                    }
                }
                *local = MakeOctetStringFromHex(*token);
                if (*local == NULL) {
                    /* print a warning and assign a fake string */
                    DPRINTF((APWARN, "Can't make '%s' into an OctetString\n",
                             *token));
                    *local = MakeOctetString(NULL, 0);
                }
            } else {
                /* make a null octet string */
                *local = MakeOctetString(NULL, 0);
            }
            return 0;
        case PARSER_CONVERT_TO_TOKEN:
            if (*local == NULL) {
                *token = (char *)malloc(2);
                if (*token == NULL) {
                    return -1;
                }
                strcpy(*token, "-");
            } else if ((*local)->length == 0) {
                *token = (char *)malloc(2);
                if (*token == NULL) {
                    return -1;
                }
                strcpy(*token, "-");
            } else {
                *token = (char *)malloc((*local)->length * 3 + 4);
                if (*token == NULL) {
                    return -1;
                }
                p = *token;
                for (i = 0; i < (*local)->length; i++) {
                    sprintf(p, "%02x:", (int)(*local)->octet_ptr[i]);
                    p += 3;
                }
                *(p-1) = '\0';
            }
            return 0;
        default:
            return -1;
    }
}

static const ParserIntegerTranslation boolean_Translations[] = {
    { "true", 1 },
    { "false", 0 },
    { "yes", 1 },
    { "no", 0 },
    { "1", 1 },
    { "0", 0 },
    { NULL, 0 }
};
const ParserIntegerTranslationTable boolean_TranslationTable = {
    INTEGER_TYPE,
    "boolean",
    16,
    boolean_Translations
};

static const ParserIntegerTranslation storageType_Translations[] = {
    { "other", OTHER },
    { "volatile", VOLATILE },
    { "nonVolatile", NONVOLATILE },
    { "readOnly", READONLY },
    { "permanent", PERMANENT },
    { "1", OTHER },
    { "2", VOLATILE },
    { "3", NONVOLATILE },
    { "4", READONLY },
    { "5", PERMANENT },
    { NULL, 0 }
};
const ParserIntegerTranslationTable storageType_TranslationTable = {
    INTEGER_TYPE,
    "StorageType",
    16,
    storageType_Translations
};

SR_INT32
ConvToken_integerTranslation(
    SR_INT32 direction,
    char **token,
    void *value,
    ParserIntegerTranslationTable *pitt)
{
    SR_INT32 *local = (SR_INT32 *)value;
    int i;
    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            for (i = 0; pitt->pit[i].token != NULL; i++) {
                if (strcmp(*token, pitt->pit[i].token) == 0) {
                    *local = pitt->pit[i].val;
                    return 0;
                }
            }
            DPRINTF((APWARN, "Unknown %s '%s'\n", pitt->descr, *token));
            return -1;
        case PARSER_CONVERT_TO_TOKEN:
            *token = (char *)malloc(pitt->max_token_length);
            if (*token == NULL) {
                return -1;
            }
            for (i = 0; pitt->pit[i].token != NULL; i++) {
                if (*local == pitt->pit[i].val) {
                    strcpy(*token, pitt->pit[i].token);
                    return 0;
                }
            }
            return -1;
        default:
            return -1;
    }
}

/*
 * FreeValTypes
 *
 * The functions frees the members of a ValType array.  The count argument
 * specifies how many members of the array are valid.  The tt argument
 * specifies the type_table which describes the types of the ValType
 * elements.  If count is equal to -1, the whole tt array is scanned.
 */
static void
FreeValTypes(tt, vp, count)
    const SnmpType *tt;
    ValType *vp;
    int count;
{
    int i;

    if (count == -1) {
        count = 32768;
    }

    for (i = 0; (tt[i].type != -1) && (i < count); i++) {
        switch (tt[i].type) {
            case OCTET_PRIM_TYPE:
                if (vp[i].osval != NULL) {
                    FreeOctetString(vp[i].osval);
                }
                break;
            case OBJECT_ID_TYPE:
                if (vp[i].oidval != NULL) {
                    FreeOID(vp[i].oidval);
                }
                break;
            default:
                break;
        }
    }
}

/*
 * ProcessConfigRecord:
 *   This routine attempts to parse a config file line according to a format.
 *
 *   Arguments:
 *      cfgline - The text to be parsed
 *      type_table - describes the type of each element in the entry
 *      converters - contains pointers to token converter functions and
 *                   default values for elements not stored in the file
 *      vp      - Array of ValType structure where the results are placed
 *
 *   The return value is 0 on success, and on failure is the number of
 *   tokens successfully matched before the error occured.
 *
 *   It is always the responsibility of the caller to free the contents
 *   of the ValType array, including when an error occurs.
 */
static int
ProcessConfigRecord(linecount, cfgline, type_table, converters, values)
    int linecount;
    char *cfgline;
    const SnmpType *type_table;
    const PARSER_CONVERTER *converters;
    ValType *values;
{
    FNAME("ProcessConfigRecord")
    char *cfgp, *tok = NULL;
    int matchcount = 0;
    SR_INT32 cv;
    const SnmpType *st;
    const PARSER_CONVERTER *pc;
    ValType *vp;

    /* get initial pointers to the line from the file */
    cfgp = cfgline;

    /* start parsing entries */
    for (pc = converters, st = type_table, vp = values;
         pc->conv_func != NULL; pc++, st++, vp++) {

        /* Extract the token from the configuration entry */
        if ((pc->defval == NULL) ||
            (pc->conv_func == SR_INTEGER_TRANSLATION)) {
            nexttoken(&tok, &cfgp);
            if (tok == NULL) {
                DPRINTF((APWARN, "%s: Error, incomplete entry at line %d\n",
                         Fname, linecount));
                goto fail;
            }
        } else {
            tok = pc->defval;
        }

        /* Parse the token */
        switch (st->type) {
            case INTEGER_TYPE:
                if (pc->conv_func == SR_INTEGER_TRANSLATION) {
                    cv = ConvToken_integerTranslation(PARSER_CONVERT_FROM_TOKEN,
                        &tok, &vp->slval,
                        (ParserIntegerTranslationTable *)pc->defval);
                    break;
                }
                cv = (*pc->conv_func)(PARSER_CONVERT_FROM_TOKEN,
                                      &tok, &vp->slval);
                break;
            case COUNTER_TYPE:
            case GAUGE_TYPE:
            case TIME_TICKS_TYPE:
                cv = (*pc->conv_func)(PARSER_CONVERT_FROM_TOKEN,
                                      &tok, &vp->ulval);
                break;
            case OCTET_PRIM_TYPE:
                cv = (*pc->conv_func)(PARSER_CONVERT_FROM_TOKEN,
                                      &tok, &vp->osval);
                break;
            case OBJECT_ID_TYPE:
                cv = (*pc->conv_func)(PARSER_CONVERT_FROM_TOKEN,
                                      &tok, &vp->oidval);
                break;
            default:
                DPRINTF((APWARN, 
			 "%s: Error, unsupported token type %d at line %d\n",
			 Fname, st->type, linecount));
                goto fail;
        }
        if (cv != 0) {
            DPRINTF((APWARN,
		     "%s: Error, cannot parse token %s\n", Fname, tok));
            goto fail;
        }

        /* count matched tokens */
        matchcount++;
    }

    /* issue warning if there's anything else on the line */
    nexttoken(&tok, &cfgp);
    if (cfgp != NULL) {
        DPRINTF((APERROR, "%s: Warning, extra characters on line %d\n",
                 Fname, linecount));
        return 1;
    }

    return 0;

  fail:
    FreeValTypes(type_table, values, matchcount);
    return -1;
}

/*
 * ProcessConfigRecordForcingIndexOrder:
 *   This routine attempts to parse a config file line according to a format.
 *
 *   Arguments:
 *      cfgline - The text to be parsed
 *      type_table - describes the type of each element in the entry
 *      converters - contains pointers to token converter functions and
 *                   default values for elements not stored in the file
 *      vp      - Array of ValType structure where the results are placed
 *
 *   The return value is 0 on success, and on failure is the number of
 *   tokens successfully matched before the error occured.
 *
 *   It is always the responsibility of the caller to free the contents
 *   of the ValType array, including when an error occurs.
 */
static int
ProcessConfigRecordForcingIndexOrder(
    int linecount,
    char *cfgline,
    const SnmpType *type_table,
    const PARSER_CONVERTER *converters,
    ValType *values)
{
    FNAME("ProcessConfigRecordForcingIndexOrder")
    char *cfgp, *tok = NULL;
    SR_INT32 cv;
    const SnmpType *st;
    const PARSER_CONVERTER *pc;
    ValType *vp;
    int i;
    int num_idx, cur_idx;
    int num_cvtrs, cur_cvtrs;

    /* get initial pointers to the line from the file */
    cfgp = cfgline;

    /* Count number of indices and converters */
    num_idx = 0;
    num_cvtrs = 0;
    for (pc = converters, st = type_table; pc->conv_func != NULL; pc++, st++) {
        if (st->indextype != -1) {
            num_idx++;
        }
        num_cvtrs++;
    }
    memset(values, 0, sizeof(ValType) * num_cvtrs);

    /* start parsing entries */
    cur_idx = 0;
    cur_cvtrs = 0;
    while (cur_cvtrs < num_cvtrs) {
/*
    for (pc = converters, st = type_table, vp = values;
         pc->conv_func != NULL; pc++, st++, vp++) {
*/
        if (cur_idx < num_idx) {
            /* need to process the next index */
            for (pc = converters, st = type_table, vp = values;
                 pc->conv_func != NULL; pc++, st++, vp++) {
                if (st->indextype == cur_idx) {
                    break;
                }
            }
            cur_idx++;
            cur_cvtrs++;
        } else {
            /* need to process the next non-index */
            for (pc = converters, st = type_table, vp = values, i = 0;
                 pc->conv_func != NULL; pc++, st++, vp++) {
                if (st->indextype == -1) {
                    if (i == (cur_cvtrs - cur_idx)) {
                        break;
                    }
                    i++;
                }
            }
            cur_cvtrs++;
        }

        /* Extract the token from the configuration entry */
        if ((pc->defval == NULL) ||
            (pc->conv_func == SR_INTEGER_TRANSLATION)) {
            nexttoken(&tok, &cfgp);
            if (tok == NULL) {
                DPRINTF((APWARN, "%s: Error, incomplete entry at line %d\n",
                         Fname, linecount));
                goto fail;
            }
        } else {
            tok = pc->defval;
        }

        /* Parse the token */
        switch (st->type) {
            case INTEGER_TYPE:
                if (pc->conv_func == SR_INTEGER_TRANSLATION) {
                    cv = ConvToken_integerTranslation(PARSER_CONVERT_FROM_TOKEN,
                        &tok, &vp->slval,
                        (ParserIntegerTranslationTable *)pc->defval);
                    break;
                }
                cv = (*pc->conv_func)(PARSER_CONVERT_FROM_TOKEN,
                                      &tok, &vp->slval);
                break;
            case COUNTER_TYPE:
            case GAUGE_TYPE:
            case TIME_TICKS_TYPE:
                cv = (*pc->conv_func)(PARSER_CONVERT_FROM_TOKEN,
                                      &tok, &vp->ulval);
                break;
            case OCTET_PRIM_TYPE:
                cv = (*pc->conv_func)(PARSER_CONVERT_FROM_TOKEN,
                                      &tok, &vp->osval);
                break;
            case OBJECT_ID_TYPE:
                cv = (*pc->conv_func)(PARSER_CONVERT_FROM_TOKEN,
                                      &tok, &vp->oidval);
                break;
            default:
                DPRINTF((APWARN, 
			 "%s: Error, unsupported token type %d at line %d\n",
			 Fname, st->type, linecount));
                goto fail;
        }
        if (cv != 0) {
            DPRINTF((APWARN,
		     "%s: Error, cannot parse token %s\n", Fname, tok));
            goto fail;
        }
    }

    /* issue warning if there's anything else on the line */
    nexttoken(&tok, &cfgp);
    if (cfgp != NULL) {
        DPRINTF((APERROR, "%s: Warning, extra characters on line %d\n",
                 Fname, linecount));
        return 1;
    }

    return 0;

  fail:
    FreeValTypes(type_table, values, num_cvtrs);
    return -1;
}



static void
FillInScalarVar(rtp, vp)
    const PARSER_RECORD_TYPE *rtp;
    ValType *vp;
{
    OctetString **os;
    OID **oid;

    switch (rtp->type_table[0].type) {
        case INTEGER_TYPE:
            *(SR_INT32 *)(rtp->scalar) = vp[0].slval;
            break;
        case COUNTER_TYPE:
        case GAUGE_TYPE:
        case TIME_TICKS_TYPE:
            *(SR_UINT32 *)(rtp->scalar) = vp[0].ulval;
            break;
        case OCTET_PRIM_TYPE:
            os = (OctetString **)(rtp->scalar);
            if (*os != NULL) {
                FreeOctetString(*os);
            }
            *os = vp[0].osval;
            break;
        case OBJECT_ID_TYPE:
            oid = (OID **)(rtp->scalar);
            if (*oid != NULL) {
                FreeOID(*oid);
            }
            *oid = vp[0].oidval;
            break;
        default:
            break;
    }
}

/*
 * This routine will create a row in a table and populate it with the
 * contents of vp, as read from a config file.
 */
static int
CreateTableEntry(rtp, vp)
    const PARSER_RECORD_TYPE *rtp;
    ValType *vp;
{
    FNAME("CreateTableEntry")
    int index;
    void *entry;
    int i;
    int nitems;
    int offset;
    int maxOffset = -1;
    int indexOfMaxOffset = 0;	/* initialized to eliminate diagnostic */

#ifdef EMANATE
    /* Sanity check the table pointer. */
     if (rtp->table->tp == NULL) {
         fprintf(stderr, "%s: Null table pointer for table %s\n",  Fname,
             rtp->label);
         fprintf(stderr,
             "%s: ParseConfigFile called before InitSubagent()?\n", Fname);
         return -1;
     }
#endif /* EMANATE */
    /* Plug in index fields */
    for (i = 0; rtp->type_table[i].type != -1; i++) {
        if (rtp->type_table[i].indextype != -1) {
            int indexno = rtp->type_table[i].indextype;
            switch (rtp->type_table[i].type) {
            case INTEGER_TYPE:
                rtp->table->tip[indexno].value.uint_val = vp[i].slval;
                break;
            case COUNTER_TYPE:
            case GAUGE_TYPE:
            case TIME_TICKS_TYPE:
                rtp->table->tip[indexno].value.uint_val = vp[i].ulval;
                break;
            case OCTET_PRIM_TYPE:
                rtp->table->tip[indexno].value.octet_val = vp[i].osval;
                break;
            case OBJECT_ID_TYPE:
                rtp->table->tip[indexno].value.oid_val = vp[i].oidval;
                break;
            }
        }
    }

    /* Create the new row */
    nitems = rtp->table->nitems;
    index = NewTableEntry(rtp->table);
    if (index == -1) {
        DPRINTF((APERROR, "%s: cannot create new row in table %s\n", Fname, rtp->label));
        return -1;
    }
    if (nitems == rtp->table->nitems) {
        DPRINTF((APERROR, "%s: attempt to create row that already exists in table: %s.\n",
                 Fname, rtp->label));
        return -1;
    }
    entry = (void *) rtp->table->tp[index];
    offset = 0;
   /*
    *  Stepping through the fields in the table entry, free the 
    *  index fields (they are copied into the row by InstallIndices()),
    *  and fill in the rest of the fields. 
    */ 
  
    for (i = 0; rtp->type_table[i].type != -1; i++) {
        offset = rtp->type_table[i].byteoffset;
        if (offset > maxOffset) {
           indexOfMaxOffset = i;
           maxOffset = rtp->type_table[i].byteoffset;
        }
        /* if we are an index, free the field, if it is a ptr */
        if (rtp->type_table[i].indextype != -1) {
            switch (rtp->type_table[i].type) {
                case OCTET_PRIM_TYPE:
                    FreeOctetString(vp[i].osval);
                    break;
                case OBJECT_ID_TYPE:
                    FreeOID(vp[i].oidval);
                    break;
                default:
                    break;
            }
        } else {
            /* 
             * Transfer config value to the table.  
             */
            switch (rtp->type_table[i].type) {
                case INTEGER_TYPE:
                    *(SR_INT32 *)((char *) entry + offset) = vp[i].slval;
                    break;
                case COUNTER_TYPE:
                case GAUGE_TYPE:
                case TIME_TICKS_TYPE:
                    *(SR_UINT32 *)((char *)entry + offset) = vp[i].ulval;
                    break;
                case OCTET_PRIM_TYPE:
                    *(OctetString **)((char *)entry + offset) = vp[i].osval;
                    break;
                case OBJECT_ID_TYPE:
                    *(OID **)((char *)entry + offset) = vp[i].oidval;
                    break;
                default:
                    break;
            }
        }
    }

    /*
     *  Clear out the rest of the table row.  Advance the
     *  place we start clearing beyond the end of the rightmost field.
     */
    switch (rtp->type_table[indexOfMaxOffset].type) {
        case INTEGER_TYPE:
            offset = maxOffset + sizeof(SR_INT32);
            break;
        case COUNTER_TYPE:
        case GAUGE_TYPE:
        case TIME_TICKS_TYPE:
            offset = maxOffset + sizeof(SR_UINT32);
            break;
        case OCTET_PRIM_TYPE:
            offset = maxOffset + sizeof(OctetString *);
            break;
        case OBJECT_ID_TYPE:
            offset = maxOffset + sizeof(OID *);
            break;
        default:
            break;
    }
    if (offset < rtp->table->rowsize && offset != 0) {
        memset((char *)entry + offset, 0xff, 
            (int)(rtp->table->rowsize - offset));
    }

    return 0;
}


#ifndef SR_NO_WRITE_CONFIG_FILE
static int
WriteScalarEntry(cfgp, label, type_table, converters, scalar)
    ConfigFile *cfgp;
    const char *label;
    const SnmpType *type_table;
    const PARSER_CONVERTER *converters;
    void *scalar;
{
    FNAME("WriteScalarEntry")
    SR_INT32 cv;
    char *token;

    sr_fprintf(cfgp, label);

    switch (type_table->type) {
        case INTEGER_TYPE:
            cv = (*converters->conv_func)(PARSER_CONVERT_TO_TOKEN,
                                          &token, scalar);
            break;
        case COUNTER_TYPE:
        case GAUGE_TYPE:
        case TIME_TICKS_TYPE:
            cv = (*converters->conv_func)(PARSER_CONVERT_TO_TOKEN,
                                          &token, scalar);
            break;
        case OCTET_PRIM_TYPE:
            cv = (*converters->conv_func)(PARSER_CONVERT_TO_TOKEN,
                                          &token, scalar);
            break;
        case OBJECT_ID_TYPE:
            cv = (*converters->conv_func)(PARSER_CONVERT_TO_TOKEN,
                                          &token, scalar);
            break;
        default:
            DPRINTF((APWARN, "%s: unsupported token type %d\n",
                              Fname, type_table->type));
            goto fail;
    }
    if (cv != 0) {
        DPRINTF((APWARN, "%s: error parsing token\n", Fname));
        goto fail;
    }

    sr_fprintf(cfgp, " ");
    sr_fprintf(cfgp, token);
    sr_fprintf(cfgp, "\n");
    free(token);
    return 0;

  fail:
    return -1;
}

static int
WriteTableEntry(cfgp, label, type_table, converters, entry,
                rs_offset, st_offset, us_offset)
    ConfigFile *cfgp;
    const char *label;
    const SnmpType *type_table;
    const PARSER_CONVERTER *converters;
    char *entry;
    SR_INT32 rs_offset;
    SR_INT32 st_offset;
    SR_INT32 us_offset;
{
    FNAME("WriteTableEntry")
    int nc, sl;
    SR_INT32 cv = 0;
    char *token;
    const SnmpType *tt;
    const PARSER_CONVERTER *pc;

    /* 
     *  Offset checks:
     *    rs_offset: row status offset.  Do not write if row status is
     *               not 1 (RS_ACTIVE)
     *    st_offset: storage type offset.  Do not write if storage type
     *               not NONVOLATILE, PERMANATE or READONLY.
     *    us_offset: user skip offset.  Do not write if user_skip is nonzero.
     */

    if (rs_offset >= 0) {
        if ((*(SR_INT32 *)(entry + rs_offset)) != 1) {
            return 0;
        }
    }

    if (st_offset >= 0) {
        if ((*(SR_INT32 *)(entry + st_offset)) < 3) {
            return 0;
        }
    }

    if (us_offset >= 0) {
        if ((*(SR_INT32 *)(entry + us_offset)) > 0) {
            return 0;
        }
    }

    sr_fprintf(cfgp, label);

    nc = strlen(label);
    for (pc = converters, tt = type_table; pc->conv_func != NULL; pc++, tt++) {

        switch (tt->type) {
            case INTEGER_TYPE:
                if (pc->defval == NULL) {
                    cv = (*pc->conv_func)(PARSER_CONVERT_TO_TOKEN,
                          &token, (void *)(entry + tt->byteoffset));
                } else if (pc->conv_func == SR_INTEGER_TRANSLATION) {
                    cv = ConvToken_integerTranslation(PARSER_CONVERT_TO_TOKEN,
                          &token, (void *)(entry + tt->byteoffset),
                          (ParserIntegerTranslationTable *)pc->defval);
                }
                break;
            case COUNTER_TYPE:
            case GAUGE_TYPE:
            case TIME_TICKS_TYPE:
                if (pc->defval == NULL) {
                    cv = (*pc->conv_func)(PARSER_CONVERT_TO_TOKEN,
                          &token, (void *)(entry + tt->byteoffset));
                }
                break;
            case OCTET_PRIM_TYPE:
                if (pc->defval == NULL) {
                    cv = (*pc->conv_func)(PARSER_CONVERT_TO_TOKEN,
                          &token, (void *)(entry + tt->byteoffset));
                }
                break;
            case OBJECT_ID_TYPE:
                if (pc->defval == NULL) {
                    cv = (*pc->conv_func)(PARSER_CONVERT_TO_TOKEN,
                          &token, (void *)(entry + tt->byteoffset));
                }
                break;
            default:
                DPRINTF((APWARN, "%s: unsupported token type %d\n",
                                  Fname, tt->type));
                goto fail;
        }
        if (cv != 0) {
            DPRINTF((APWARN, "%s: error parsing token\n", Fname));
            goto fail;
        }
        if ((pc->defval != NULL) &&
            (pc->conv_func != SR_INTEGER_TRANSLATION)) {
            continue;
        }

        sl = strlen(token);
#ifndef SR_SIEMENS_NIXDORF
        if ((nc + sl) > 77) {
            sr_fprintf(cfgp, " \\\n    ");
            sr_fprintf(cfgp, token);
            nc = sl + 4;
        } else {
#endif	/* SR_SIEMENS_NIXDORF */
            sr_fprintf(cfgp, " ");
            sr_fprintf(cfgp, token);
            nc += sl + 1;
#ifndef SR_SIEMENS_NIXDORF
        }
#endif	/* SR_SIEMENS_NIXDORF */
        free(token);
    }
    sr_fprintf(cfgp, "\n");
    return 0;

  fail:
    return -1;
}

static int
WriteTableEntryForcingIndexOrder(
    ConfigFile *cfgp,
    const char *label,
    const SnmpType *type_table,
    const PARSER_CONVERTER *converters,
    char *entry,
    SR_INT32 rs_offset,
    SR_INT32 st_offset,
    SR_INT32 us_offset)
{
    FNAME("WriteTableEntry")
    int nc, sl;
    SR_INT32 cv = 0;
    char *token;
    const SnmpType *tt;
    const PARSER_CONVERTER *pc;
    int cur_idx = 0, found_index = 1;

    /* 
     *  Offset checks:
     *    rs_offset: row status offset.  Do not write if row status is
     *               not 1 (RS_ACTIVE)
     *    st_offset: storage type offset.  Do not write if storage type
     *               not NONVOLATILE, PERMANENT or READONLY.
     *    us_offset: user skip offset.  Do not write if user_skip is nonzero.
     */

    if (rs_offset >= 0) {
        if ((*(SR_INT32 *)(entry + rs_offset)) != 1) {
            return 0;
        }
    }

    if (st_offset >= 0) {
        if ((*(SR_INT32 *)(entry + st_offset)) < 3) {
            return 0;
        }
    }

    if (us_offset >= 0) {
        if ((*(SR_INT32 *)(entry + us_offset)) > 0) {
            return 0;
        }
    }

    sr_fprintf(cfgp, label);

    nc = strlen(label);

    while (found_index) {
        found_index = 0;
        for (pc = converters, tt = type_table;
             pc->conv_func != NULL; pc++, tt++) {

            if (tt->indextype != cur_idx) {
                continue;
            }
            found_index = 1;
            cur_idx++;

            switch (tt->type) {
                case INTEGER_TYPE:
                    if (pc->defval == NULL) {
                        cv = (*pc->conv_func)(PARSER_CONVERT_TO_TOKEN,
                              &token, (void *)(entry + tt->byteoffset));
                    } else if (pc->conv_func == SR_INTEGER_TRANSLATION) {
                        cv = ConvToken_integerTranslation(
                              PARSER_CONVERT_TO_TOKEN,
                              &token, (void *)(entry + tt->byteoffset),
                              (ParserIntegerTranslationTable *)pc->defval);
                    }
                    break;
                case COUNTER_TYPE:
                case GAUGE_TYPE:
                case TIME_TICKS_TYPE:
                    if (pc->defval == NULL) {
                        cv = (*pc->conv_func)(PARSER_CONVERT_TO_TOKEN,
                              &token, (void *)(entry + tt->byteoffset));
                    }
                    break;
                case OCTET_PRIM_TYPE:
                    if (pc->defval == NULL) {
                        cv = (*pc->conv_func)(PARSER_CONVERT_TO_TOKEN,
                              &token, (void *)(entry + tt->byteoffset));
                    }
                    break;
                case OBJECT_ID_TYPE:
                    if (pc->defval == NULL) {
                        cv = (*pc->conv_func)(PARSER_CONVERT_TO_TOKEN,
                              &token, (void *)(entry + tt->byteoffset));
                    }
                    break;
                default:
                    DPRINTF((APWARN, "%s: unsupported token type %d\n",
                                      Fname, tt->type));
                    goto fail;
            }
            if (cv != 0) {
                DPRINTF((APWARN, "%s: error parsing token\n", Fname));
                goto fail;
            }
            if ((pc->defval != NULL) &&
                (pc->conv_func != SR_INTEGER_TRANSLATION)) {
                continue;
            }

            sl = strlen(token);
            if ((nc + sl) > 77) {
                sr_fprintf(cfgp, " \\\n    ");
                sr_fprintf(cfgp, token);
                nc = sl + 4;
            } else {
                sr_fprintf(cfgp, " ");
                sr_fprintf(cfgp, token);
                nc += sl + 1;
            }
            free(token);
        }
    }

    for (pc = converters, tt = type_table; pc->conv_func != NULL; pc++, tt++) {
        if (tt->indextype != -1) {
            continue;
        }

        switch (tt->type) {
            case INTEGER_TYPE:
                if (pc->defval == NULL) {
                    cv = (*pc->conv_func)(PARSER_CONVERT_TO_TOKEN,
                          &token, (void *)(entry + tt->byteoffset));
                } else if (pc->conv_func == SR_INTEGER_TRANSLATION) {
                    cv = ConvToken_integerTranslation(PARSER_CONVERT_TO_TOKEN,
                          &token, (void *)(entry + tt->byteoffset),
                          (ParserIntegerTranslationTable *)pc->defval);
                }
                break;
            case COUNTER_TYPE:
            case GAUGE_TYPE:
            case TIME_TICKS_TYPE:
                if (pc->defval == NULL) {
                    cv = (*pc->conv_func)(PARSER_CONVERT_TO_TOKEN,
                          &token, (void *)(entry + tt->byteoffset));
                }
                break;
            case OCTET_PRIM_TYPE:
                if (pc->defval == NULL) {
                    cv = (*pc->conv_func)(PARSER_CONVERT_TO_TOKEN,
                          &token, (void *)(entry + tt->byteoffset));
                }
                break;
            case OBJECT_ID_TYPE:
                if (pc->defval == NULL) {
                    cv = (*pc->conv_func)(PARSER_CONVERT_TO_TOKEN,
                          &token, (void *)(entry + tt->byteoffset));
                }
                break;
            default:
                DPRINTF((APWARN, "%s: unsupported token type %d\n",
                                  Fname, tt->type));
                goto fail;
        }
        if (cv != 0) {
            DPRINTF((APWARN, "%s: error parsing token\n", Fname));
            goto fail;
        }
        if ((pc->defval != NULL) &&
            (pc->conv_func != SR_INTEGER_TRANSLATION)) {
            continue;
        }

        sl = strlen(token);
        if ((nc + sl) > 77) {
            sr_fprintf(cfgp, " \\\n    ");
            sr_fprintf(cfgp, token);
            nc = sl + 4;
        } else {
            sr_fprintf(cfgp, " ");
            sr_fprintf(cfgp, token);
            nc += sl + 1;
        }
        free(token);
    }

    sr_fprintf(cfgp, "\n");
    return 0;

  fail:
    return -1;
}

int
WriteConfigFile(filename, rt)
    const char *filename;
    const PARSER_RECORD_TYPE *rt[];
{
    FNAME("WriteConfigFile")
    ConfigFile *cfgp;
    char *backup_filename;
    int rti;
    int status;
    int i;
    const SnmpType *st;
    const PARSER_CONVERTER *ct;

    char *fprefix;
#ifdef EIGHT_PLUS_THREE
    char *p;
#endif /* EIGHT_PLUS_THREE */

    backup_filename = (char *)malloc(strlen(filename)
                                     + strlen(BACKUP_FILE_SUFFIX)
                                     + 8);

    if (backup_filename == NULL) {
        DPRINTF((APERROR, "%s: malloc error\n", Fname));
        goto fail;
    }
    strcpy(backup_filename, filename);
#ifdef EIGHT_PLUS_THREE
    if ((p = strrchr(backup_filename, '.')) != NULL) {
        *p = '\0';
    }
#endif	/* EIGHT_PLUS_THREE */
    strcat(backup_filename, BACKUP_FILE_SUFFIX);

    if (CreateBackupFile(filename, backup_filename) == 0) {
        DPRINTF((APERROR, "%s: Warning, cannot backup config file.\n", Fname));
        free(backup_filename);
        backup_filename = NULL;
    }

    /* Open config file */
    if (OpenConfigFile(filename, FM_WRITE, &cfgp) != FS_OK) {
        goto restore_and_fail;
    }

    status = 0;
    for (rti = 0; rt[rti] != NULL; rti++) {
        if (rt[rti]->comment == NULL) {
            sr_fprintf(cfgp, "# Entry type: ");
            sr_fprintf(cfgp, rt[rti]->label);
            sr_fprintf(cfgp, "\n");
            sr_fprintf(cfgp, "# Entry format: ");
            fprefix = "";
            for (st = rt[rti]->type_table, ct = rt[rti]->converters; 
                st->type != -1; st++, ct++) {
                if (ct->defval != NULL) 
                    continue;
                switch (st->type) {
                    case INTEGER_TYPE:
                        sr_fprintf(cfgp, fprefix);
                        sr_fprintf(cfgp, "integer\n");
                        break;
                    case COUNTER_TYPE:
                        sr_fprintf(cfgp, fprefix);
                        sr_fprintf(cfgp, "counter\n");
                        break;
                    case GAUGE_TYPE:
                        sr_fprintf(cfgp, fprefix);
                        sr_fprintf(cfgp, "gauge\n");
                        break;
                    case TIME_TICKS_TYPE:
                        sr_fprintf(cfgp, fprefix);
                        sr_fprintf(cfgp, "timeTicks\n");
                        break;
                    case OCTET_PRIM_TYPE:
                        sr_fprintf(cfgp, fprefix);
                        sr_fprintf(cfgp, "octetString\n");
                        break;
                    case OBJECT_ID_TYPE:
                        sr_fprintf(cfgp, fprefix);
                        sr_fprintf(cfgp, "OID\n");
                        break;
                }
                fprefix = "#               ";
            }
        } else {
            sr_fprintf(cfgp, rt[rti]->comment);
            sr_fprintf(cfgp, "\n");
        }
        if ((rt[rti]->type & 0x000f) == PARSER_SCALAR) {
            status = WriteScalarEntry(cfgp,
                                      rt[rti]->label,
                                      rt[rti]->type_table,
                                      rt[rti]->converters,
                                      (char *)rt[rti]->scalar);
        } else
        if ((rt[rti]->type & 0x000f) == PARSER_TABLE) {
            for (i = 0; i < rt[rti]->table->nitems; i++) {
                if ((rt[rti]->type & 0x00f0) == PARSER_FORCE_INDEX_ORDER) {
                    status = WriteTableEntryForcingIndexOrder(cfgp,
                                             rt[rti]->label,
                                             rt[rti]->type_table,
                                             rt[rti]->converters,
                                             (char *)rt[rti]->table->tp[i],
                                             rt[rti]->rs_offset,
                                             rt[rti]->st_offset,
                                             rt[rti]->us_offset);
                } else {
                    status = WriteTableEntry(cfgp,
                                             rt[rti]->label,
                                             rt[rti]->type_table,
                                             rt[rti]->converters,
                                             (char *)rt[rti]->table->tp[i],
                                             rt[rti]->rs_offset,
                                             rt[rti]->st_offset,
                                             rt[rti]->us_offset);
                }
            }
        } else {
            DPRINTF((APERROR, "%s: Bad PARSER_RECORD_TYPE\n", Fname));
            status = -1;
        }
        if (status) {
            goto restore_and_fail;
        }
        sr_fprintf(cfgp, "\n\n");
    }

    CloseConfigFile(cfgp);

    if (backup_filename) {
        free(backup_filename);
    }
    return 0;

  restore_and_fail:
    if (backup_filename) {
        if (CreateBackupFile(backup_filename, filename) == 0) {
            DPRINTF((APERROR, "%s: WARNING, CANNOT RESTORE OLD CONFIG FILE\n",
                     Fname));
        }
    } else {
        DPRINTF((APERROR, "%s: WARNING, CANNOT RESTORE OLD CONFIG FILE\n",
                 Fname));
    }

  fail:
    if (backup_filename) {
        free(backup_filename);
    }
    return -1;
}

/*
 * CreateBackupFile:  copies filename to backup_filename, using rename if the
 * system supports it 
 */
static int
CreateBackupFile(filename, backup_filename)
  const char *filename;
  const char *backup_filename;
{
    FNAME("BackupFile")
#ifndef HAVE_RENAME
    FILE *fp = NULL;
    FILE *fp2 = NULL;
    char buffer[256];
    char templine[512];
#endif	/* HAVE_RENAME */
#ifdef HAVE_RENAME
    struct stat buf;
    int fd;
#endif /* HAVE_RENAME */

#ifdef HAVE_RENAME
    if (stat(filename, &buf) != 0) {
	/* Should not happen. Be conservative. */
	buf.st_mode = S_IRUSR|S_IWUSR; 
    }
    if (rename(filename, backup_filename)) {
        DPRINTF((APERROR, "%s: Warning, cannot backup config file.\n", Fname));
        return 0;
    }
    if ((fd = creat(filename, buf.st_mode)) != -1) {
	close(fd);
    } else {
	DPRINTF((APWARN, "%s: Error creating %s with mode 0x%x\n", 
		 Fname, filename, (unsigned int) buf.st_mode));
    }
    return 1;
#else /* HAVE_RENAME */
    if ((fp = fopen(filename, "r")) == NULL) {
        DPRINTF((APWARN, "%s:  Error opening %s\n", Fname, filename));
        return 0;
    }
    if ((fp2 = fopen(backup_filename, "w")) == NULL) {
        DPRINTF((APWARN, "%s: Error opening %s\n", Fname, backup_filename));
        fclose(fp);
        return 0;
    }
    while (fgets((char *) buffer, 256, fp) != NULL) {
        (void) sprintf(templine, "%s", buffer);
        fputs(templine, fp2);
    }
    fclose(fp2);
    fclose(fp);
    return 1;
#endif /* !HAVE_RENAME */
}
#endif /* SR_NO_WRITE_CONFIG_FILE */
#endif /* SR_CONFIG_FP */

