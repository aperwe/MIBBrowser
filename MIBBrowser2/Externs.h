
/**
*	@file		Externs.h
*	@brief		External definitions file.
*	@author		Artur Perwenis
*	@date		18-Jan-2002
*
*	@history
*	@item		18-Jan-2002		Artur Perwenis		Documented.
*/


#ifndef ___EXTERNS_H__
#define ___EXTERNS_H__

/*
	External definitions
*/
extern CMIBElement *LookupElementSyntaxElement (CMIBElement *pElement);
extern HTREEITEM LookupFirstElementByName (const char *lpszFindString, HTREEITEM hStartFromItem, CTreeCtrl *pTreeCtrl, CMIBFileList *pFileList, bool bCaseSensitive);
extern HTREEITEM LookupNextElementByName (const char *lpszFindString, HTREEITEM hStartFromItem, CTreeCtrl *pTreeCtrl, CMIBFileList *pFileList, bool bCaseSensitive);
extern CMIBElement *LookupFirstElementSubstring (const char *lpszFindString, CMIBElementList *pElementList, bool bCaseSensitive);
extern CMIBElement *LookupNextElementSubstring (const char *lpszFindString, CMIBElementList *pElementList, CMIBElement *pStartingElement, bool bCaseSensitive);

//Global variables
extern CString *p_csConsoleOutputString;
extern "C" char *pszOutputBuffer;

/**
*	@brief	SNMB Research Inc. utilities
*/
extern "C" int EntryPoint_GetBulk (int argc, char *argv[]);
/**
*	@brief	SNMB Research Inc. utilities
*/
extern "C" int EntryPoint_GetId (int argc, char *argv[]);
/**
*	@brief	SNMB Research Inc. utilities
*/
extern "C" int EntryPoint_GetMany (int argc, char *argv[]);
/**
*	@brief	SNMB Research Inc. utilities
*/
extern "C" int EntryPoint_GetMetrics (int argc, char *argv[]);
/**
*	@brief	SNMB Research Inc. utilities
*/
extern "C" int EntryPoint_GetNext (int argc, char *argv[]);
/**
*	@brief	SNMB Research Inc. utilities
*/
extern "C" int EntryPoint_GetOne (int argc, char *argv[]);
/**
*	@brief	SNMB Research Inc. utilities
*/
extern "C" int EntryPoint_GetRoute (int argc, char *argv[]);
/**
*	@brief	SNMB Research Inc. utilities
*/
extern "C" int EntryPoint_GetSub (int argc, char *argv[]);
/**
*	@brief	SNMB Research Inc. utilities
*/
extern "C" int EntryPoint_GetTab (int argc, char *argv[]);
/**
*	@brief	SNMB Research Inc. utilities
*/
extern "C" int EntryPoint_Inform (int argc, char *argv[]);
/**
*	@brief	SNMB Research Inc. utilities
*/
extern "C" int EntryPoint_SetAny (int argc, char *argv[]);
/**
*	@brief	SNMB Research Inc. utilities
*/
extern "C" int EntryPoint_TrapRcv (int argc, char *argv[]);
/**
*	@brief	SNMB Research Inc. utilities
*/
extern "C" int EntryPoint_TrapSend (int argc, char *argv[]);


#endif //___EXTERNS_H__
