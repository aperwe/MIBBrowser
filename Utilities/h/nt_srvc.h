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

/* $Id: nt_srvc.h,v 1.4 1998/01/08 02:33:57 mark Exp $ */

/*
 * This file should be included just before the main() function of a
 * program which wishes to be run as a Windows NT service.  There must
 * also be a pair of #define statements just before this file is
 * included as follows:
 *
 *     #define SR_NT_SRVC_NAME "name"
 *     #define SR_NT_SRVC_DESC "description"
 *
 * SR_NT_SRVC_NAME defines the short name of the service.  This should
 * be a single word, possibly just the name of the binary to be built.
 * SR_NT_SRVC_DESC provides a short description of the service.  This
 * string will be displayed in the NT Service dialog window.
 */

#ifndef SR_NT_SRVC_H
#define SR_NT_SRVC_H

#ifdef  __cplusplus
extern "C" {
#endif

int service__main(int argc, char **argv, char **envp);

int
main( int argc, char **argv, char **envp )
{
    return init_service(argc, argv, envp, SR_NT_SRVC_NAME, SR_NT_SRVC_DESC);
}

#define main service__main

#ifdef  __cplusplus
}
#endif

/* DO NOT PUT ANYTHING AFTER THIS #endif */
#endif	/* SR_NT_SRVC_H */
