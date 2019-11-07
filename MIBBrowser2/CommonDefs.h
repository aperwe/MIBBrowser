/**
*	@file		CommonDefs.h
*	@brief		Common definitions file.
*	@author		Artur Perwenis
*	@date		18-Jan-2002
*
*	@history
*	@item		18-Jan-2002		Artur Perwenis		Documented.
*/


#ifndef ___COMMON_DEFS_H__
#define ___COMMON_DEFS_H__

#define MAINWND_MINWIDTH		784
#define MAINWND_MINHEIGHT		628

#define ICON_CX 19
#define ICON_CY 16

#define MY_TIMER 1
#define MY_TIMER_TIMEOUT 2000
#define MY_FINDCONSOLEWND_TIMEOUT 100

//Used for fast file I/O.
#define MIB_FILE_BUFFER_SIZE (2048L)
#define STR_APPEND	(65536L)


typedef enum eElementTypes {
	ETYPE_MIB_ROOT			=	0,	//This represents filename of the MIB loaded
	ETYPE_MIB_ROOT_KEY		=	1,	//This is the root element in a MIB
	ETYPE_CLOSED_FOLDER		=	2,	//Closed state of the folder (in a list view)
	ETYPE_OPEN_FOLDER		=	3,	//Open state of the folder (in a list view)
	ETYPE_READ_ONLY_ENTRY	=	4,	//An entry with read-only status
	ETYPE_TABLE_ROOT		=	5,  //When an element is a table.
	ETYPE_TABLE_ITEM		=	6,	//An enumerated item in a table
	ETYPE_TABLE_INDEX_FIELD	=	7,	//Table index field
	ETYPE_READ_WRITE_ENTRY	=	8,	//An entry with read-write status
	ETYPE_TEXTUAL_CONV		=	9,	//Textual conventions (usually placed at the beginning of the file)
	ETYPE_TRAP				=  10,  //NEW: icon and identifier for trap-type elements
	ETYPE_TABLE_EXTERNAL_INDEX_FIELD = 12, //Update. Represents the index entry that is defined in INDEX { } section of an oid, but is external to the MIB database file.
	ETYPE_IMPORTED_ELEMENT,			//NEW: represents an element from the IMPORTS section
	ETYPE_IMPORT_MODULE,			//NEW: represents the name of a module which elements are imported from
	ETYPE_OID,						//NEW: use this for file parsing when "OBJECT IDENTIFIER" is encountered (instead of ETYPE_MIB_ROOT_KEY, ETYPE_CLOSED_FOLDER, ETYPE_OPEN_FOLDER - these are only for appropriate icon selection).
	ETYPE_IMPORTS_SECTION,			//NEW: designates imports section element. Under this object every import-module will be listed.
	ETYPE_NONE						//Undefined (yet) element
};

//Type definitions for meanings of nSide parameter in CWnd::OnSizing () method
typedef enum eWindowSide {
		EWS_LEFT			=	1,
		EWS_RIGHT,
		EWS_TOP,
		EWS_LEFTTOP,
		EWS_RIGHTTOP,
		EWS_BOTTOM,
		EWS_LEFTBOTTOM,
		EWS_RIGHTBOTTOM,
		EWS_NONE
};


//Console handling definitions
#define CONSOLE_WIDTH			80
#define CONSOLE_HEIGHT			9900
#define EMPTY_ROW_LIMIT			4
//Buffer size
#define OUTPUT_LINE_LEN			1024

//Key codes for PreTranslateMessage
#define VK_F 0x46


#endif //___COMMON_DEFS_H__
