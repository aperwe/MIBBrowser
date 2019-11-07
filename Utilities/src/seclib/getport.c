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
static const char rcsid[] = "$Id: getport.c,v 1.24.4.1 1998/03/27 22:38:01 moulton Exp $";
#endif	/* (! ( defined(lint) ) && defined(SR_RCSID)) */

#include "sr_conf.h"
#include <stdio.h>

#include <stdlib.h>

#include <stddef.h>

#include <sys/types.h>

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif	/* HAVE_MEMORY_H */

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif	/* HAVE_MALLOC_H */

#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif  /* HAVE_NETDB_H */



#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif  /* HAVE_NETINET_IN_H */

#include <string.h>


#include "sr_snmp.h"
#include "diag.h"
SR_FILENAME


#define SR_SNMP_ENV_NAME "SR_SNMP_TEST_PORT"
#define SR_TRAP_ENV_NAME "SR_TRAP_TEST_PORT"

#ifdef HAVE_GETSERVBYNAME
#define SR_SNMP_SVC_NAME "snmp"
#define SR_TRAP_SVC_NAME "snmp-trap"
#endif	/* HAVE_GETSERVBYNAME */

#define SNMP_PORT 161
#define SNMP_TRAP_PORT 162


/*
 * GetSNMPPort()
 *
 * If the environment variable SR_SNMP_ENV_NAME is set, then use that
 * as the port to send to when sending SNMP requests. Otherwise, if the
 * function getservbyname(SR_SNMP_SVC_NAME, "udp") returns a valid port (from
 * /etc/services or equivalent database), use that.  Otherwise use the
 * #define SNMP_PORT if it exists, otherwise, use 161.
 *
 * Returns the port in HOST byte order.
 */

int
GetSNMPPort()
{
    FNAME("GetSNMPPort")
    static int      snmp_port;
#ifdef HAVE_GETSERVBYNAME
    struct servent *snmp_serv;
#endif	/* HAVE_GETSERVBYNAME */
    /* JMV -- Removed the zero initialization. static variables are
     * initialized to zero by the compiler.
     */
    static int      called;
    char           *temp;

    if (!called) {
        called = 1;
	if ((temp = (char *)getenv(SR_SNMP_ENV_NAME)) != NULL) {
	    snmp_port = atoi(temp);
	    DPRINTF((APTRACE, "%s: snmp port is %d from getenv(\"%s\")\n",
		     Fname, snmp_port, SR_SNMP_ENV_NAME));
	}
#ifndef HAVE_GETSERVBYNAME
        else {
#else /* HAVE_GETSERVBYNAME */
	else if ((snmp_serv =
		  getservbyname(SR_SNMP_SVC_NAME, "udp")) != NULL) {
	    snmp_port = ntohs(snmp_serv->s_port);
	    DPRINTF((APTRACE,
		     "%s: snmp port is %d from %s in /etc/services\n",
		     Fname, snmp_port, SR_SNMP_SVC_NAME));
	}
	else {
	    DPRINTF((APWARN, "%s: no %s entry in /etc/services; ", Fname,
		     SR_SNMP_SVC_NAME));
#endif	/* HAVE_GETSERVBYNAME */

#ifdef SNMP_PORT
	    snmp_port = SNMP_PORT;
	    DPRINTF((APTRACE, "snmp port is %d from #define SNMP_PORT\n",
		     snmp_port));
#else
	    snmp_port = 161;
	    DPRINTF((APWARN, "snmp port is %d from constant\n", snmp_port));
#endif

	}
    }

    return snmp_port;
}

/*
 * GetSNMPTrapPort()
 *
 * If the environment variable SR_TRAP_ENV_NAME is set, then use that
 * as the port to send to when sending SNMP traps. Else, if the environment
 * variable SR_SNMP_ENV_NAME exits, use it's value + 1. Otherwise, if the
 * function getservbyname("snmp-trap", "udp") returns a valid port (from
 * /etc/services or equivalent database), use that.  Otherwise if the
 * #define SNMP_TRAP_PORT exits, use it's value, otherwise use 161.
 *
 * Returns the port in HOST byte order.
 */

int
GetSNMPTrapPort()
{
    FNAME("GetSNMPTrapPort")
    static int      trap_port;
    /* JMV -- Removed the zero initialization. static variables are
     * initialized to zero by the compiler.
     */
    static          called;
    char           *temp;
#ifdef HAVE_GETSERVBYNAME
    struct servent *snmp_trap_serv;
#endif /* HAVE_GETSERVBYNAME */

    if (!called) {
        called = 1;
	if ((temp = (char *)getenv(SR_TRAP_ENV_NAME)) != NULL) {
	    trap_port = atoi(temp);
	    DPRINTF((APTRAP, "%s: trap port is %d from getenv(\"%s\")\n",
		     Fname, trap_port, SR_TRAP_ENV_NAME));
	}
	else if ((temp = (char *)getenv(SR_SNMP_ENV_NAME)) != NULL) {
	    trap_port = atoi(temp) + 1;
	    DPRINTF((APTRAP, "%s: trap port is %d from getenv(\"%s\") + 1\n",
		     Fname, trap_port, SR_SNMP_ENV_NAME));
	}
#ifdef HAVE_GETSERVBYNAME
	else if ((snmp_trap_serv =
		  getservbyname(SR_TRAP_SVC_NAME, "udp")) != NULL) {
	    trap_port = ntohs(snmp_trap_serv->s_port);
	    DPRINTF((APTRAP, "%s: trap port %d from %s in /etc/services\n",
		     Fname, trap_port, SR_TRAP_SVC_NAME));
	}
	else {
	    DPRINTF((APWARN, "%s: no %s entry in /etc/services; ", Fname,
		     SR_TRAP_SVC_NAME));
#else /* HAVE_GETSERVBYNAME */
        else {
#endif	/* HAVE_GETSERVBYNAME */

#ifdef SNMP_TRAP_PORT
	    trap_port = SNMP_TRAP_PORT;
	    DPRINTF((APTRAP, "trap port is %d from #define SNMP_TRAP_PORT\n",
		     trap_port));
#else				/* SNMP_TRAP_PORT */
	    trap_port = 162;
	    DPRINTF((APWARN, "trap port is %d from constant\n", trap_port));
#endif				/* SNMP_TRAP_PORT */

	}
    }

    return trap_port;
}

#ifdef WANT_SETUPENVIRON

extern char   **environ;

int SetUpEnviron
    SR_PROTOTYPE((int argc, 
		  char **argv,
		  char **envp));

/* SetUpEnviron() scribbles on argv, so call it after all arg processing */
/* It puts the port number in argv[0] so it will show up in ps */

int
SetUpEnviron(argc, argv, envp)
    int             argc;
    char          **argv;
    char          **envp;
{
    int             s_port = GetSNMPPort();
    int             i;
    int             totavail = 0;
    char          **dupenviron = NULL;
    char            buf[256];

    if (envp < argv) return 0;

    if (argc > 0) {
	totavail = (argv[argc - 1] - argv[0]) + strlen(argv[argc - 1]) - 2;
	if (envp != NULL) {
	    /* save the user environment so we can write over it */
	    for (i = 0; envp[i] != NULL; i++) {
	    }
	    if ((dupenviron = (char **) malloc((i + 1) * sizeof(char *))) == NULL) {
		DPRINTF((APERROR, "can't duplicate environment\n"));
		goto fail;
	    }
#ifdef SR_CLEAR_MALLOC
	    memset(dupenviron, 0, (i + 1) * sizeof(char *));
#endif	/* SR_CLEAR_MALLOC */
	    for (i = 0; envp[i] != NULL; i++) {
		if ((dupenviron[i] = strdup(envp[i])) == NULL) {
		    DPRINTF((APERROR, "can't duplicate environment string\n"));
		    goto fail;
		}
	    }
	    totavail = (envp[i - 1] - argv[0]) + strlen(envp[i - 1]) - 2;
	    environ = dupenviron;
	}

#ifdef SR_CLEAR_AUTOS
	memset(buf, 0, sizeof(buf));
#endif	/* SR_CLEAR_AUTOS */
	sprintf(buf, "%s (%d)", argv[0], s_port);

	if (strlen(buf) < (unsigned)totavail) {
	    strcpy(argv[0], buf);
	    memset(argv[0] + strlen(buf) + 1, ' ', totavail - 1 - strlen(buf));
	}
    }

    return 0;
fail:
    if (dupenviron != NULL) {
	for (i = 0; dupenviron[i] != NULL; i++) {
	    free(dupenviron[i]);
	}
	free(dupenviron);
    }
    return -1;
}
#endif	/* WANT_SETUPENVIRON */
