/************************************************************
*
*					AccessIntegrator
*
*	Copyright (C)  Siemens AG 2002  All Rights reserved.
*
************************************************************/

/**
*	@file		UExterns.h
*	@brief		Declarations of externally linked identifiers.
*	@author		Artur Perwenis
*	@date		10-Jan-2002
*
*	@history
*	@item		10-Jan-2002		Artur Perwenis		created
*/

#define SendOutput() OutputString (pszOutputBuffer)

extern void OutputString (const char *p_buf);
extern char *pszOutputBuffer;



