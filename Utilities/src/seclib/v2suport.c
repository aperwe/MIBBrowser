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

#include <windows.h>
#include <winsock.h>

#include <stdlib.h>

#include <errno.h>

#include <sys/types.h>



#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif  /* HAVE_MEMORY_H */


#include <string.h>

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif  /* HAVE_MALLOC_H */

#ifdef SR_WINDOWS
#include <windows.h>
#endif /* SR_WINDOWS */

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif /* HAVE_NETINET_IN_H */


#include "sr_snmp.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"
#include "snmpv2.h"
#include "sr_ad.h"

#include "diag.h"
SR_FILENAME

#include "global.h"
#include "sr_trans.h"
#ifndef SR_UNSECURABLE
#include "md5key.h"
#endif /* SR_UNSECURABLE */

#include "sr_ip.h"    /* for GetLocalIPAddress() */



#ifdef SR_CONFIG_FP

static const ParserIntegerTranslation viewType_Translations[] = {
    { "included", INCLUDED },
    { "excluded", EXCLUDED },
    { NULL, 0 }
};
const ParserIntegerTranslationTable viewType_TranslationTable = {
    INTEGER_TYPE,
    "view type",
    10,
    viewType_Translations
};

static const ParserIntegerTranslation contextType_Translations[] = {
    { "local", 1 },
    { "remote", 2 },
    { "proxy", 3 },
    { NULL, 0 }
};
const ParserIntegerTranslationTable contextType_TranslationTable = {
    INTEGER_TYPE,
    "context type",
    8,
    contextType_Translations
};

static const ParserIntegerTranslation locale_Translations[] = {
    { "local", 1 },
    { "remote", 2 },
    { NULL, 0 }
};
const ParserIntegerTranslationTable locale_TranslationTable = {
    INTEGER_TYPE,
    "locale",
    8,
    locale_Translations
};

static const ParserIntegerTranslation spi_Translations[] = {
    { "snmpv1", SR_SPI_SNMPv1 },
    { "snmpv2c", SR_SPI_SNMPv2c },
    { "usecNoAuth", SR_SPI_USEC_NOAUTH },
    { "usmNoAuth", SR_SPI_USM_NOAUTH },
#ifndef SR_UNSECURABLE
    { "usecAuth", SR_SPI_USEC_AUTH },
    { "usmAuth", SR_SPI_USM_AUTH },
#endif /* SR_UNSECURABLE */
    { NULL, 0 }
};
const ParserIntegerTranslationTable spi_TranslationTable = {
    INTEGER_TYPE,
    "sPI",
    16,
    spi_Translations
};

static const ParserIntegerTranslation privs_Translations[] = {
    { "nothing", 1 },
    { "readOnly", 2 },
    { "readWrite", 3 },
    { "1", 1 },
    { "2", 2 },
    { "3", 3 },
    { NULL, 0 }
};
const ParserIntegerTranslationTable privs_TranslationTable = {
    INTEGER_TYPE,
    "privs",
    16,
    privs_Translations
};


/*
 * The following ConvToken routines are used by the parser code to convert
 * data types specific to SNMPv2.
 */



OctetString *ConvToken_snmpID_localSnmpID = NULL;
OctetString *ConvToken_snmpID_prevSnmpID = NULL;

SR_INT32
ConvToken_snmpID(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    FNAME("ConvToken_snmpID")
    OctetString **local = (OctetString **)value;
    char agtmgr[4];
    unsigned long sid1, sid2, sid3, sid4, port, p1, p2;
    int cc;
    SR_INT32 status;

    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            if (strcmp(*token, "localSnmpID") == 0) {
                *local = CloneOctetString(ConvToken_snmpID_localSnmpID);
                ConvToken_snmpID_prevSnmpID = ConvToken_snmpID_localSnmpID;
                if (*local == NULL) {
                    return -1;
                } else {
                    return 0;
                }
            }
            if (!strncmp(*token, "99:1:", 5)) {
                cc = sscanf((*token)+5, "%3s:%lu.%lu.%lu.%lu:%lu",
                            agtmgr, &sid1, &sid2, &sid3, &sid4, &port);
                if (cc != 6) {
                    DPRINTF((APERROR, "%s: bad snmpID\n", Fname));
                    return -1;
                }
                if (!strcmp(agtmgr, "agt")) {
                    cc = 0;
                } else
                if (!strcmp(agtmgr, "mgr")) {
                    cc = 1;
                } else {
                    DPRINTF((APERROR, "%s: bad snmpID\n", Fname));
                    return -1;
                }
                p1 = port & 0xff;
                p2 = (port & 0xff00) >> 8;
                *local = MakeOctetString(NULL, 12);
                if (*local == NULL) {
                    return -1;
                }
                (*local)->octet_ptr[3] = (unsigned char)99;
                (*local)->octet_ptr[4] = (unsigned char)1;
                (*local)->octet_ptr[5] = (unsigned char)cc;
                (*local)->octet_ptr[6] = (unsigned char)p2;
                (*local)->octet_ptr[7] = (unsigned char)p1;
                (*local)->octet_ptr[8] = (unsigned char)sid1;
                (*local)->octet_ptr[9] = (unsigned char)sid2;
                (*local)->octet_ptr[10] = (unsigned char)sid3;
                (*local)->octet_ptr[11] = (unsigned char)sid4;
                ConvToken_snmpID_prevSnmpID = *local;
                return 0;
            }
            status = ConvToken_octetString(direction, token, value);
            ConvToken_snmpID_prevSnmpID = *local;
            return status;
        case PARSER_CONVERT_TO_TOKEN:
            if (CmpOctetStrings(*local, ConvToken_snmpID_localSnmpID) == 0) {
                *token = (char *)malloc(16);
                if (*token == NULL) {
                    return -1;
                }
                strcpy(*token, "localSnmpID");
                return 0;
            }
            if ((ntohl(*(SR_UINT32 *)(*local)->octet_ptr) == 99) &&
                ((*local)->octet_ptr[4] == (unsigned char)1)) {
                *token = (char *)malloc(48);
                if (*token == NULL) {
                    return -1;
                }
                sprintf(*token, "99:1:%s:%lu.%lu.%lu.%lu:%lu",
                        ((*local)->octet_ptr[5] == '\0') ? "agt" : "mgr",
                        (unsigned long)(*local)->octet_ptr[8],
                        (unsigned long)(*local)->octet_ptr[9],
                        (unsigned long)(*local)->octet_ptr[10],
                        (unsigned long)(*local)->octet_ptr[11],
                        (unsigned long)(*local)->octet_ptr[6] * 256 +
                        (unsigned long)(*local)->octet_ptr[7]);
                return 0;
            }
            return ConvToken_octetString(direction, token, value);
        default:
            return -1;
    }
}


#ifndef SR_UNSECURABLE
SR_INT32
ConvToken_authPrivPassword(direction, token, value)
    SR_INT32 direction;
    char **token;
    void *value;
{
    OctetString **local = (OctetString **)value;
    OctetString pw;
    SR_INT32 pos, len;
    switch (direction) {
        case PARSER_CONVERT_FROM_TOKEN:
            len = (SR_INT32)strlen(*token);
            for (pos = 0; pos < len; pos++) {
                if (((*token)[pos] == ':') ||
                    ((*token)[pos] == '-')) {
                    return ConvToken_octetString(direction, token, value);
                 }
            }
            pw.octet_ptr = (unsigned char *)(*token);
            pw.length = len;
            DPRINTF((APCONFIG, "converting password to key . . .\n"));
            *local = MakeMD5PasswordKey(&pw);
            if (*local == NULL) {
                return -1;
            } else {
                return 0;
            }
        case PARSER_CONVERT_TO_TOKEN:
            return ConvToken_octetString(direction, token, value);
        default:
            return -1;
    }
}
#endif /* SR_UNSECURABLE */
#endif /* SR_CONFIG_FP */
