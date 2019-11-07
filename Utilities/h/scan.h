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

/* $Id: scan.h,v 1.32 1998/01/08 02:34:27 mark Exp $ */

#ifndef SR_SCAN_H
#define SR_SCAN_H

#ifdef  __cplusplus
extern "C" {
#endif

#define	CFGLINESIZE	2048	/* max config file line */

#define PARSER_SCALAR   0x0001
#define PARSER_TABLE    0x0002

#define PARSER_FORCE_INDEX_ORDER 0x0010

#define PARSER_CONVERT_TO_TOKEN    1
#define PARSER_CONVERT_FROM_TOKEN  2

#define SR_INTEGER_TRANSLATION ((ConvToken *)ConvToken_integerTranslation)

#define CS_STRINGLEN 256

typedef SR_INT32 ConvToken(
    SR_INT32 direction,
    char **token,
    void *value);

/* this structure defines a configuration file */
typedef struct configFileS {
    char filename[CS_STRINGLEN];	/* name of the file */
    FILE *fp;				/* pointer to the file */
    int linecount;			/* line # of the file */
    int startline;			/* line # starting a stanza */
} ConfigFile;

/* this defines a value array */
typedef union _valTypeU {
    SR_UINT32    	ulval;
    SR_INT32		slval;
    char		*strval;
    OctetString		*osval;
    OID			*oidval;
} ValType;

/*
 *  An array of PARSER_CONVERTER specifies how objects are written to/read 
 *  from config files.  ConvToken specifies the conversion function (e.g.
 *  ConvToken_integer).  If defval is NULL, the object is written or
 *  read from the configuration file, otherwise, the supplied value
 *  is used.
 */
typedef struct _PARSER_CONVERTER {
   ConvToken *conv_func;
   char *defval;
} PARSER_CONVERTER;

typedef struct _PARSER_RECORD_TYPE {
   SR_INT32 type; /* Type of record (table or scalar) */
   char *label; /* label at beginning of this record type */
   const PARSER_CONVERTER *converters; /* List of functions to convert tokens */
   const SnmpType *type_table; /* table entry descriptions */
   SnmpV2Table *table; /* The table which this record describes */
   void *scalar; /* Pointer to the scalar variable this record describes */
   SR_INT32 rs_offset; /* Offset of rowStatus member in table entries */
   SR_INT32 st_offset; /* Offset of storageType member in table entries */
   SR_INT32 us_offset; /* Offset of userSkip member in table entries */
   char *comment; /* Comment to be placed in config file when written */
} PARSER_RECORD_TYPE;

typedef struct {
    char *token;
    SR_INT32 val;
} ParserIntegerTranslation;

typedef struct {
    short type;
    char *descr;
    int max_token_length;
    const ParserIntegerTranslation *pit;
} ParserIntegerTranslationTable;

char *sr_fgets
    SR_PROTOTYPE((char *buf,
                  int size,
                  ConfigFile *cfp));

int sr_feof
    SR_PROTOTYPE((ConfigFile *cfp));

int sr_fprintf
    SR_PROTOTYPE((ConfigFile *cfp,
                  const char *buf));

void write_config_log
    SR_PROTOTYPE((const char *logfilename,
                  const char *logmessage));

int OpenConfigFile
    SR_PROTOTYPE((const char *filename,
                  int mode,
                  ConfigFile **cfgp));

void CloseConfigFile
    SR_PROTOTYPE((ConfigFile *cfgp));

int WriteConfigFile
    SR_PROTOTYPE((const char *filename,
                  const PARSER_RECORD_TYPE *rt[]));

/*
 * Basic token types.  These will be used for converting token in configuration
 * file entries.  Addition tokens can be added for converting special formats.
 */
SR_INT32 ConvToken_integer(
    SR_INT32 direction,
    char **token,
    void *value);

SR_INT32 ConvToken_unsigned(
    SR_INT32 direction,
    char **token,
    void *value);

SR_INT32 ConvToken_OID(
    SR_INT32 direction,
    char **token,
    void *value);

SR_INT32 ConvToken_textOctetString(
    SR_INT32 direction,
    char **token,
    void *value);

SR_INT32 ConvToken_octetString(
    SR_INT32 direction,
    char **token,
    void *value);

SR_INT32 ConvToken_integerTranslation(
    SR_INT32 direction,
    char **token,
    void *value,
    ParserIntegerTranslationTable *pitt);

extern const ParserIntegerTranslationTable boolean_TranslationTable;
extern const ParserIntegerTranslationTable storageType_TranslationTable;

#ifdef  __cplusplus
}
#endif

#endif	/* SR_SCAN_H */
