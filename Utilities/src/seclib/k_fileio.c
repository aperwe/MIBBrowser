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
static const char rcsid[] = "$Id: k_fileio.c,v 1.26 1998/01/06 18:44:39 mark Exp $";
#endif	/* (! ( defined(lint) ) && defined(SR_RCSID)) */

/*
 * This file contains routines which perform I/O to the world outside the
 * system.  For most platforms, that implies file I/O.  The file is expected 
 * to provide the following data and routines:
 *
 * Data:
 *	None.
 *
 * Routines:
 *	int OpenSnmpInitFile()
 *		This routine initializes access to the master snmp 
 *	configuration file.  The associated "read" routine should be used
 * 	to get records from the file, and the associated "close" routine
 *	should be used when all records have been read.
 *
 *      int ReadSnmpInitEntry()
 *              This routine gets a record from the master snmp
 *      configuration file at the current read location and passes it back
 *      to the caller.  Return codes are given by the include file "config.h".
 *      The appropriate "open" routine must be called before this routine
 *      is invoked, and the appropriate "close" routine should be called
 *      after all records are read.
 *
 *	void CloseSnmpInitFile()
 *		This routine terminates access to the master snmp
 *	configuration file.  The associated "open" routine must be invoked
 *	before this routine is called.
 *
 *	int WriteSnmpInitFile()
 *		This routine is called to save the master snmp configuration
 *	information in nonvolatile storage.  On many systems this implies
 *	disk I/O, but some systems may choose to implement FLASH RAM, NOVRAM,
 *	or even TFTP-based network storage.  It may be appropriate for the
 *	storage routines to interact with work routines executing out of the
 *	idle loop, for instance to bundle all I/O to FLASH devices.
 *
 */

#include "sr_conf.h"

#include <stdio.h>

#include <stdlib.h>

#include <sys/types.h>

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif  /* HAVE_MALLOC_H */

#include <string.h>

#include "sr_snmp.h"
#include "diag.h"
SR_FILENAME

#include "sr_cfg.h"
#include "lookup.h"
#include "v2table.h"
#include "scan.h"




char *
sr_fgets(buf, size, cfp)
char *buf;
int size;
ConfigFile *cfp;
{
    return(fgets(buf, size, cfp->fp));
}    


int
sr_feof(cfp)
ConfigFile *cfp;
{
    return feof(cfp->fp);
}

int
sr_fprintf(cfp, buf)
ConfigFile *cfp;
const char *buf;
{
    return fputs(buf, cfp->fp);
}

void
write_config_log(logfilename, logmessage)
    const char *logfilename;
    const char *logmessage;
{
    FILE *fp;

    if (logfilename == NULL) {
        return;
    }

    if (logmessage == NULL) {
        return;
    }

    fp = fopen(logfilename, "a");
    if (fp == NULL) {
        DPRINTF((APERROR, "Cannot write bad config record to log file."));
        DPRINTF((APERROR, logmessage));
        return;
    }
    fprintf(fp, logmessage);
    fclose(fp);
    return;
}

/* 
 * This routine opens a config file and keeps information about it.
 */
int
OpenConfigFile(filename, mode, cfgp)
const char *filename;
int mode;
ConfigFile **cfgp;
{
    int retval =  FS_OK;
    ConfigFile *tmpcfgp;
    char *modestr;

    
    if((tmpcfgp = (ConfigFile *) malloc(sizeof(ConfigFile))) == 0) {
	DPRINTF((APWARN, "OpenConfigFile: couldn't get ConfigFile struct\n"));
	retval = FS_ERROR;
    } else {
	/* save the filename and init the line count */
	strcpy(tmpcfgp->filename, filename);
	tmpcfgp->linecount = 0;

	/* determine the open mode */
	if(mode == FM_WRITE) {
	    modestr = "w";
	} else {
	    modestr = "r";
	}
	    
	/* open the file */
	if((tmpcfgp->fp = fopen(tmpcfgp->filename, modestr)) == 0) {
	    DPRINTF((APWARN, "OpenConfigFile: can't open %s with mode %d\n", 
		tmpcfgp->filename, mode));
	    retval = FS_ERROR;
	}

    }
    /* pass back the pointer to the config structure */
    *cfgp = tmpcfgp;
    
    return(retval);
}

/*
 * This routine closes configuration files and frees their info structs.
 */
void
CloseConfigFile(cfgp)
ConfigFile *cfgp;
{
    if(cfgp) {
	if(cfgp->fp) fclose(cfgp->fp);
	free((char *) cfgp);
    }
}
