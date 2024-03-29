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

/*
 * This file contains structures which are used to configure the
 * agent.  The structures provide a platform-independent mechanism for
 * passing configuration information to the core agent.
 */

/* $Id: sr_cfg.h,v 1.12 1998/01/08 02:35:27 mark Exp $ */

#ifndef SR_SR_CFG_H
#define	SR_SR_CFG_H

#ifdef  __cplusplus
extern "C" {
#endif

/* file open modes */
#define	FM_READ		   1		/* read the file */
#define	FM_WRITE	   2		/* write the file */

/* possible return values for routines */
#define	FS_OK	   	   1		/* got an entry */
#define	FS_ERROR   	   2		/* fatal error */

#ifdef  __cplusplus
}
#endif

#endif	/* SR_SR_CFG_H */
