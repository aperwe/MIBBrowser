/**
*	@file		MIBFile.h
*	@brief		Parser subsystem file.
*	@author		Artur Perwenis
*	@date		11-Oct-2001
*
*	@history
*	@item		11-Oct-2001		Artur Perwenis		First version
*	@item		11-Jan-2002		Artur Perwenis		Added comments
*	@item		18-Jan-2002		Artur Perwenis		Finished documentation.
*/

#ifndef ___MIB_FILE_H__
#define ___MIB_FILE_H__

#include "StdAfx.h"



//Forward cast class declarations
class CMIBElementList;
class CMIBElement;
class CIndexEntryList;
class CRuleList;


/**
*	@brief		This class will handle ALL file parsing operations.
*	@author		Artur Perwenis
*	@date		18-Jan-2002
*/
class CMIBFile : public CFile
{

//////////////
// Methods
//////////////

public:
	/**
	*	@brief	Default constructor.
	*/
	CMIBFile ();
	/**
	*	@brief	Default destructor.
	*/
	~CMIBFile ();
	/**
	*	@brief	The only public method exposed in the class interface.
				This is the main method for handling parsing of MIB file.
				------
				<b>Creation date</b>: 2001-10-14
				<b>Created by</b>: Artur Perwenis
				<b>------</b>
				<b>Last modification date</b>: 2001-10-14
				<b>Last modified by</b>: Artur Perwenis
				<b>Modification history (most recent first)</b>:
				<b>------</b>
				<b>Modification date</b>: 2001-10-14
				<b>Modification description</b>:
					First test version.
	*	@return	Returns true if parsing was successful. Returns false if any of the rules caused an error.
	*	@date		2001-10-14
	*/
	bool ParseFile ();

private:
	/**
	*	@brief	Internal method to be called only exclusively from within ParseFile () method.
	*			<b>Creation date</b>: 2001-10-14
	*			<b>Created by</b>: Artur Perwenis
	*			<b>Last modification date</b>: 2001-11-09
	*			<b>Last modified by</b>: Artur Perwenis
	*			<b>Modification history (most recent first)</b>:
	*			<b>------</b>
	*			<b>Modification date</b>: 2002-01-21
	*			<b>Modification description</b>:
	*				Extended CRule::RULE_OT_STATUS_DEFINITION to support the new keyword
	*				'optional' introduced in 'STATUS' field in the newest xldv20-mib.mib file.
	*			<b>Modification date</b>: 2001-11-09
	*			<b>Modification description</b>:
	*				CRule::PLACEHOLDER_FOR_INDEX_FIELD_NAME modified, since it has been
	*				discovered (reported as a bug) that sometimes index elements can be
	*				declared in the parent (in INDEX section) but are not actually
	*				defined. Therefore, in this rule a fake index element is created
	*				and it will be modified later once it is defined.
	*			<b>Modification date</b>: 2001-10-19
	*			<b>Modification description</b>:
	*				CRule::PLACEHOLDER_FOR_PARENT_OID_VALUE modified to handle correctly
	*				the forward-declarated parent objects. This is due the fact it has been
	*				discovered that objects can be declared with forward declarations
	*				of their parents.
	*			<b>Modification date</b>: 2001-10-14
	*			<b>Modification description</b>:
	*				First test version.
	*	@return			Parsing status.
	*	@date		2001-10-14
	*/
	bool Parse ();
	/**
	*	@brief	Function for retrieving regular words. Scans the file and returns one-word token.
	*			Internal method to be called only exclusively from within Parse () method.
	*			<b>Creation date</b>: 2001-10-14
	*			<b>Created by</b>: Artur Perwenis
	*			<b>Last modification date</b>: 2001-10-23
	*			<b>Last modified by</b>: Artur Perwenis
	*			<b>Modification history (most recent first)</b>:
	*			<b>------</b>
	*			<b>Modification date</b>: 2001-10-23
	*			<b>Modification description</b>:
	*				Modified the main function loop to handle cases, where OID is followed
	*				immediately by, for example, a colon. Such a case occurs in pmvgw-mib.mib file.
	*			<b>Modification date</b>: 2001-10-17
	*			<b>Modification description</b>:
	*				Added handling for one-character delimiters.
	*			<b>Modification date</b>: 2001-10-14
	*			<b>Modification description</b>:
	*				First test version.
	*	@return				The string token.
	*	@date		2001-10-14
	*/
	CString GetFileSingleToken ();
	/**
	*	@brief	For retrieving any string up to the first whitespace, including delimiters (like for "::=" literal).
	*			Scan the file and return the token that ends only on first whitespace (like the "::=" literal).
	*			Internal method to be called only exclusively from within Parse () method.
	*			<b>Creation date</b>: 2001-10-15
	*			<b>Created by</b>: Artur Perwenis
	*			<b>Last modification date</b>: 2001-10-23
	*			<b>Last modified by</b>: Artur Perwenis
	*			<b>Modification history (most recent first)</b>:
	*			<b>------</b>
	*			<b>Modification date</b>: 2001-10-23
	*			<b>Modification description</b>:
	*				Modified the main function loop to handle cases, where OID is followed
	*				immediately by, for example, a colon. Such a case occurs in pmvgw-mib.mib file.
	*			<b>Modification date</b>: 2001-10-17
	*			<b>Modification description</b>:
	*				Added handling for one-character delimiters.
	*			<b>Modification date</b>: 2001-10-15
	*			<b>Modification description</b>:
	*				First test version.
	*	@return				The string token.
	*	@date		2001-10-15
	*/
	CString GetFileTokenWS ();
	/**
	*	@brief		Omits all whitespaces up to the first non-whitespace character encountered
	*				and returns this character. Check the value of m_bLastByteOK flag to check
	*				if the file I/O was successful.
	*				Implemented comment skipping
	*	@return				The first non-whitespace character.
	*/
	char GetFirstNonWhiteSpace ();
	/**
	*	@brief	Use instead of CFile::Read () to retrieve one byte from the file. This function 
	*			is meant to implement fast file I/O, by using internal buffer. Due to its nature
	*			you cannot mix the usage of ReadFileByte () with CFile::Read (), or data loss will occur.
	*	@return				A character read from the file.
	*/
	char ReadFileByte ();
	/**
	*	@brief	Use this function to 'unread' a character if it has to be placed back for later
	*			retrieval. IMPORTANT: Only one char can be stored as unread.
	*	@param	cChar		A character to be put back to file.
	*	@return				Operation status.
	*/
	bool UnReadFileByte (char cChar);
	/**
	*	@brief	Checks if the character is a whitespace.
	*	@param	cChar		A character to be checked.
	*	@return				Returns true is the character is a whitespace. Otherwise false is returned.
	*/
	bool IsWhiteSpace (const char cChar);
	/**
	*	@brief	Checks delimiter state. Note that whitespaces are not treated as delimiters.
	*						Use IsWhiteSpace () to tell if a character is a whitespace character.
	*	@param	cChar		A character to be checked.
	*	@return				Returns true is the character is any of the known delimiters. Otherwise false
	*						is returned.
	*/
	bool IsDelimiter (const char cChar);
	/**
	*	@brief	Gets next single token from the file. Implements re-usage of the last token.
	*			Manual setting of m_bTokenInvalid to false causes the token to be re-used by 
	*			the next call to this function.
	*/
	void NextSingleToken ();
	/**
	*	@brief		Gets next token ending with whitespace from the file.
	*				Implements re-usage of the last token. Manual setting of m_bTokenInvalid
	*				to false causes the token to be re-used by the next call to this function.
	*/
	void NextTokenWS ();
	/**
	*	@brief	On encountering "(" in file this gets all the characters from file up to ")" character 
	*			and places them in m_Token. The ending ")" is included in the range string.
	*/
	void GetRangeString ();
	/**
	*	@brief	On encountering "{" in file this gets all the characters from file 
	*			up to "}" character and places them in m_Token.
	*			The ending ")" is included in the range string.
	*/
	void GetEnumString ();
	/**
	*	@brief	On encountering opening quotation mark """ in file this gets all the characters 
	*			from file up to the closing quotation mark and places them in m_Token. 
	*			The ending """ is not included in the description string.
	*/
	void GetDescriptionString ();
	/**
	*	@brief	Helper function: skips all the characters in the file up
	*			to the "}". "}" is placed back to the file (unread).
	*/
	void SkipCharactersUpToCloseCurlyBracket ();
	/**
	*	@brief	Validates the table of index entries added with AddIndexEntry () and sets
	*			the appropriate element type of respective elements.
	*/
	void ValidateIndiceEntries ();
	/**
	*	@brief	This method is to be called only from within CMIBFile destructor and before 
	*			deleting m_pFlatElementList member.
	*/
	void ReleaseAllFlatElements ();


//////////////
// Members
//////////////

public:
	/**
	*	@brief		Indicates whether the file is opened or not.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	bool m_bFileIsOpen;
	/**
	*	@brief		File name used to open the file. You set this member before calling ParseFile (). ParseFile () opens and closes the file.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	CString m_FileName;
	/**
	*	@brief		False indicates the token string from the previous pass is valid.
	*				In such a case, do not read the next token from file or the token will be lost.
	*				In Parse () method, set this to false in order that in the next pass of the parsing 
	*				loop the token will be retained. (This implements 'un-read-token' behaviour.)
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	bool m_bTokenInvalid;
	/**
	*	@brief		A parsed token.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	CString m_Token;
	/**
	*	@brief		The flat list of ALL elements in the file.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	CMIBElementList *m_pFlatElementList;
	/**
	*	@brief		The tree-structured elements in the file.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	CMIBElementList *m_pTreeElementList;
	/**
	*	@brief		This is a copy (necessary for parsing) of the module name as defined in DEFINITIONS ::= BEGIN section.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	CString m_ModuleName;
	/**
	*	@brief		Helps to identify the file if it is selected (for example for deletion).
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	HTREEITEM m_hTreeItem;
	/**
	*	@brief		For accessing the last element put in the m_pFlatElementList in case its properties 
	*				are defined at a later stage. This should be updated every time an element is created
	*				and added to the m_pFlatElementList.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	CMIBElement *m_pLastFlatElement;

private:
	/**
	*	@brief		A part of fast file I/O mechanism.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	char m_acIOBuffer[MIB_FILE_BUFFER_SIZE];
	/**
	*	@brief		Current position in the internal read buffer.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	DWORD m_dwIOBufferPos;
	/**
	*	@brief		Determines, which is the last valid byte in the read buffer.
	*				this is usually equal to MIB_FILE_BUFFER_SIZE, but when
	*				EOF has been reached, this will be less or 0.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	DWORD m_dwLastValidByte;
	/**
	*	@brief		Indicates whether the fast file I/O mechanism buffer contents is valid.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	bool m_bBufferValid;
	/**
	*	@brief		Set to false if couldn't read byte in ReadFileByte () routine.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	bool m_bLastByteOK;
	/**
	*	@brief		A character that has been 'unread' recently.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	char m_cUnreadChar;
	/**
	*	@brief		Set to true if the last operation on the file was UnreadChar ().
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	bool m_bUnreadChar;
	/**
	*	@brief		Holds information about all index entries defined across the MIB.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	CIndexEntryList *m_pIndexEntryList;
	/**
	*	@brief		Imported elements must, in addition to the flat element list,
	*				be kept on a separate list. This is the list.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	CMIBElementList *m_pImportElementList;
	/**
	*	@brief		The list of the rules associated with this parsed file. Used only during ParseFile ().
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	CRuleList *m_pRuleList;
};





///////////////////////////////////////////////////////////////////////////////
//
// class CRule
//
///////////////////////////////////////////////////////////////////////////////

/**
*	@brief		CRule is used in conjunction wigh CRuleList
*	@author		Artur Perwenis
*	@date		18-Jan-2002
*/
class CRule
{

//////////////
// Methods
//////////////

public:
	//CRule (int); //TO DO


//////////////
// Members
//////////////

public:
	/**
	*	@brief		See Grammar.txt for description on what each rule type means.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	enum ERuleTypes {
        RULE_EMPTY_RULE,						// This stands for {0}
		RULE_END_OF_FILE,						//Reaching this means: end of file expected. No further keywords.
        RULE_MIB,								//This stands for the whole file
        RULE_MIB_2,								//Implemented
        RULE_MIB_3,								//Implemented
        RULE_MIB_4,								//Implemented
        RULE_BODY,								//Implemented
        RULE_IMPORT,							//Implemented
        RULE_IMPORT_2,							//Implemented
        RULE_IMPORT_3,							//Implemented
        RULE_IMPORT_4,							//Implemented
        RULE_IMPORT_5,							//Implemented
        RULE_IMPORT_6,							//Implemented
        RULE_IMPORT_7,							//Implemented
        RULE_IMPORT_8,							//Implemented
        RULE_IMPORT_9,							//Implemented
        RULE_IMPORT_10,							//Implemented
		RULE_IMPORT_11,							//Implemented
        RULE_BODY_2,							//Implemented
        RULE_BODY_3,							//Implemented
        RULE_OBJECT,							//Implemented
        RULE_OBJECT_2,							//Implemented
        RULE_OID,								//Implemented
        RULE_TYPEDEF,							//Implemented
        RULE_TYPEDEF_2,							//Implemented
        RULE_WELL_KNOWN_NAME,					//Implemented
        RULE_OID_2,								//Implemented
        RULE_OID_3,								//Obsolete
        RULE_OID_4,								//Implemented
        RULE_OBJECT_TYPE,						//Implemented
		RULE_OBJECT_TYPE_ID,					//Implemented
        RULE_OT_2,								//Implemented
        RULE_OT_SYNTAX,							//Implemented
        RULE_OT_SYNTAX_2,						//Implemented
		RULE_SYNTAX_OBJECT_IDENTIFIER,			//Implemented
        RULE_SYNTAX_DEFINITION,					//Implemented
        RULE_SYNTAX_DEFINITION_2,				//Implemented
		RULE_SYNTAX_OBJECT_INTEGER,				//Implemented
        RULE_SD_MORE,							//Implemented
        RULE_SD_MORE_2,							//Implemented
		RULE_SD_MORE_3,							//Implemented
        RULE_SD_ENUM_DEF,						//Obsolete
        RULE_SD_ANY_TOKENS,						//Obsolete
        RULE_SD_CLOSING_BRACKET,				//Obsolete
        RULE_SD_ANY_TOKEN_BUT_CLOSING_BRACKET,	//Obsolete
        RULE_OT_3,								//Implemented
        RULE_OT_ACCESS,							//Implemented
        RULE_OT_ACCESS_2,						//Implemented
        RULE_OT_ACCESS_DEFINITION,				//Implemented
        RULE_OT_ACCESS_VALID_KEYWORDS,			//Implemented
        RULE_OT_4,								//Implemented
        RULE_OT_STATUS,							//Implemented
        RULE_OT_STATUS_2,						//Implemented
        RULE_OT_STATUS_DEFINITION,				//Implemented
        RULE_OT_STATUS_VALID_KEYWORDS,			//Implemented
        RULE_OT_5,								//Implemented
        RULE_OT_DESCRIPTION,					//Implemented
        RULE_OT_DESCRIPTION_2,					//Implemented
        RULE_OT_DESCRIPTION_DEFINITION,			//Implemented
        RULE_OT_DESCRIPTION_TEXT,				//Implemented
        RULE_OT_6,								//Implemented
        RULE_OT_INDEX,							//Implemented
        RULE_OT_INDEX_2,						//Implemented
        RULE_OT_INDEX_DEFINITION,				//Implemented
        RULE_OT_INDEX_FIELDS,					//Implemented
        RULE_OT_INDEX_FIELDS_MORE,				//Implemented
        RULE_OT_INDEX_FIELDS_2,					//Implemented
        RULE_SEQUENCE,							//Implemented
        RULE_SEQUENCE_2,						//Implemented
        RULE_SEQUENCE_3,						//Implemented
        RULE_SEQUENCE_MORE,						//Implemented
        RULE_SEQUENCE_MORE_2,					//Implemented
        RULE_SEQUENCE_MORE_3,					//Implemented
        RULE_SYNTAX_SEQUENCE,					//Implemented
        RULE_SYNTAX_SEQUENCE_2,					//Implemented
		//Custom field rules added after 2001-10-18
		RULE_OID_CUSTOM,						//Implemented
		RULE_OID_LAST_UPDATED,					//Implemented
		RULE_OID_LAST_UPDATED_2,				//Obsolete
		RULE_OID_CUSTOM_2,						//Obsolete
		RULE_OID_ORGANIZATION,					//Obsolete
		RULE_OID_ORGANIZATION_2,				//Obsolete
		RULE_OT_CUSTOM_3,						//Obsolete
		RULE_OID_CONTACT_INFO,					//Obsolete
		RULE_OID_CONTACT_INFO_2,				//Obsolete
		//New Version 010 fields
		RULE_OID_FIELDS,						//Implemented
		RULE_OT_LAST_UPDATED,					//Implemented
		RULE_OT_LAST_UPDATED_2,					//Implemented
		RULE_OT_LAST_UPDATED_DEFINITION,		//Implemented
		RULE_OT_LAST_UPDATED_TEXT,				//Implemented
		RULE_OT_ORGANIZATION,					//Implemented
		RULE_OT_ORGANIZATION_2,					//Implemented
		RULE_OT_ORGANIZATION_DEFINITION,		//Implemented
		RULE_OT_ORGANIZATION_TEXT,				//Implemented
		RULE_OT_CONTACT_INFO,					//Implemented
		RULE_OT_CONTACT_INFO_2,					//Implemented
		RULE_OT_CONTACT_INFO_TEXT,				//Implemented
		RULE_OT_CONTACT_INFO_DEFINITION,		//Implemented
		RULE_OT_DISPLAY_HINT,					//Implemented
		RULE_OT_DISPLAY_HINT_2,					//Implemented
		RULE_OT_DISPLAY_HINT_TEXT,				//Implemented
		RULE_OT_DISPLAY_HINT_DEFINITION,		//Implemented
		RULE_OT_EQUAL,							//Implemented
		RULE_OT_REFERENCE,						//Implemented
		RULE_OT_REFERENCE_2,					//Implemented
		RULE_OT_REFERENCE_TEXT,					//Implemented
		RULE_OT_REFERENCE_DEFINITION,			//Implemented
		RULE_OT_DEFVAL,							//Implemented
		RULE_OT_DEFVAL_2,						//Implemented
		RULE_OT_DEFVAL_MORE,					//Implemented
		RULE_OT_DEFVAL_MORE_2,					//Implemented
		RULE_OT_DEFVAL_MORE_3,					//Implemented
		RULE_OT_REVISION,						//Implemented
		RULE_OT_REVISION_2,						//Implemented
		RULE_OT_REVISION_TEXT,					//Implemented
		RULE_OT_REVISION_DEFINITION,			//Implemented
		RULE_OT_AUGMENTS,						//Implemented
		RULE_OT_AUGMENTS_2,						//Implemented
		RULE_OT_AUGMENTS_MORE,					//Implemented
		RULE_OT_AUGMENTS_MORE_2,				//Implemented
		RULE_OT_AUGMENTS_MORE_3,				//Implemented
        RULE_OT_MAX_ACCESS,						//Implemented
        RULE_OT_MAX_ACCESS_2,					//Implemented
        RULE_OT_MAX_ACCESS_DEFINITION,			//Implemented
        RULE_OT_MAX_ACCESS_VALID_KEYWORDS,		//Implemented
		RULE_TEXTUAL_CONVENTION_DEF,			//Implemented
		RULE_TEXTUAL_CONVENTION_DEF_2,			//Implemented
		RULE_SD_OCTET_STRING,					//Implemented
		//New Version 012 fields
		RULE_OT_VARIABLES,						//Implemented
		RULE_OT_VARIABLES_2,					//Implemented
		RULE_VARIABLES_MORE,					//Implemented
		RULE_VARIABLES_MORE_2,					//Implemented
		RULE_VARIABLES_MORE_3,					//Implemented
		RULE_OT_ENTERPRISE,						//Implemented
		RULE_OT_ENTERPRISE_2,					//Implemented
		RULE_ENTERPRISE_MORE,					//Implemented
		RULE_ENTERPRISE_MORE_2,					//Implemented
		RULE_ENTERPRISE_MORE_3,					//Implemented
		RULE_TRAP_TYPE_DEF,						//Implemented
		RULE_TRAP_NUMBER_SET,					//Implemented
		RULE_TRAP_NUMBER_SET_2,					//Implemented
		//New Version 013 fields
		RULE_OT_UNITS,							//Implemented
		RULE_OT_UNITS_DEFINITION,				//Implemented
		RULE_OT_UNITS_TEXT,						//Implemented
		RULE_OT_OBJECTS,						//Implemented
		RULE_OT_OBJECTS_MORE,					//Implemented
		RULE_OT_OBJECTS_MORE_2,					//Implemented
		RULE_OT_OBJECTS_MORE_3,					//Implemented
		RULE_OT_MIN_ACCESS,						//Implemented
		RULE_OT_MIN_ACCESS_DEFINITION,			//Implemented
		RULE_OT_MIN_ACCESS_VALID_KEYWORDS,		//Implemented
		RULE_OT_OBJECT,							//Implemented
		RULE_OT_OBJECT_DEFINITION,				//Implemented
		RULE_OT_MODULE,							//Implemented
		RULE_OT_MODULE_MORE,					//Implemented
		RULE_OT_MODULE_MORE_2,					//Implemented
		RULE_OT_MODULE_MORE_3,					//Implemented
		RULE_OT_MANDATORY_GROUPS,				//Implemented
		RULE_OT_MANDATORY_GROUPS_MORE,			//Implemented
		RULE_OT_MANDATORY_GROUPS_MORE_2,		//Implemented
		RULE_OT_MANDATORY_GROUPS_MORE_3,		//Implemented
		RULE_OT_NOTIFICATIONS,					//Implemented
		RULE_OT_NOTIFICATIONS_MORE,				//Implemented
		RULE_OT_NOTIFICATIONS_MORE_2,			//Implemented
		RULE_OT_NOTIFICATIONS_MORE_3,			//Implemented
		//New Version 014 fields, added after 2002.01.30
		RULE_OID_5,
		RULE_MORE_PARENT_VALUES,
		RULE_NEXT_PARENT_VALUE,
		//Add any text tokens here <TEXT>, that are placeholders for actual strings (OIDs etc.)
		PLACEHOLDER_FOR_MIB_NAME,
		PLACEHOLDER_FOR_IMPORT_NAME,
		PLACEHOLDER_FOR_MIB_MODULE_NAME,
		PLACEHOLDER_FOR_OID_VALUE,
		PLACEHOLDER_FOR_PARENT_OID_NAME,			//Represents "mgmt" in "{ mgmt 1 }" string
		PLACEHOLDER_FOR_PARENT_OID_VALUE,			//Represents "1" in "{ mgmt 1 }" string
		PLACEHOLDER_FOR_ANY_OBJECT_TYPE,			//Represents "OBJECT-TYPE" in "sysDescr OBJECT-TYPE" string
		PLACEHOLDER_FOR_TABLE_OBJECT_IDENTIFIER,	//Represents "IpAddrEntry in "SYNTAX  SEQUENCE OF IpAddrEntry"
		PLACEHOLDER_FOR_INDEX_FIELD_NAME,
        //Any literal tokens here. These are expected to be found 'as are' in the file.
		//If they are not - the syntax of the file is invalid.
		LITERAL_DEFINITIONS,				//This stands for "DEFINITIONS" literal
		LITERAL_EQUAL,						//This stands for "::=" literal
		LITERAL_BEGIN,						//This stands for "BEGIN" literal
		LITERAL_END,						//This stands for "END" literal
		LITERAL_IMPORTS,					//This stands for "IMPORTS" literal
		LITERAL_COMMA,						//This stands for "," literal
		LITERAL_SEMICOLON,					//This stands for ";" literal
		LITERAL_OPEN_CURLY_BRACKET,			//This stands for "{" literal
		LITERAL_CLOSE_CURLY_BRACKET,		//This stands for "}" literal
		LITERAL_FROM,						//This stands for "FROM" literal
		LITERAL_IDENTIFIER,					//This stands for "IDENTIFIER" literal
		LITERAL_OBJECT,						//This stands for "OBJECT" literal
		LITERAL_OCTET,						//This stands for "OCTET" literal
		LITERAL_STRING,						//This stands for "STRING" literal
		LITERAL_STRING_2,					//This stands for "STRING" literal
		LITERAL_SEQUENCE,					//This stands for "SEQUENCE" literal
		LITERAL_SYNTAX,						//This stands for "SYNTAX" keyword
		LITERAL_ACCESS,						//This stands for "ACCESS" keyword
		LITERAL_STATUS,						//This stands for "STATUS" keyword
		LITERAL_DESCRIPTION,				//This stands for "DESCRIPTION" keyword
		LITERAL_LAST_UPDATED,				//This stands for "LAST-UPDATED" keyword
		LITERAL_INTEGER,					//This stands for "INTEGER" keyword
		LITERAL_OF,							//This stands for "OF" keyword
		LITERAL_INDEX,						//This stands for "INDEX" keyword
		LITERAL_MAX_ACCESS,					//This stands for "MAX-ACCESS" keyword
		LITERAL_TEXTUAL_CONVENTION,			//This stands for "TEXTUAL-CONVENTION" keyword
		LITERAL_ORGANIZATION,				//This stands for "ORGANIZATION" keyword
		LITERAL_CONTACT_INFO,				//This stands for "CONTACT-INFO" keyword
		LITERAL_DISPLAY_HINT,				//This stands for "DISPLAY-HINT" keyword
		LITERAL_REFERENCE,					//This stands for "REFERENCE" keyword
		LITERAL_DEFVAL,						//This stands for "DEFVAL" keyword
		LITERAL_AUGMENTS,					//This stands for "AUGMENTS" keyword
		LITERAL_REVISION,					//This stands for "REVISION" keyword
		LITERAL_ENTERPRISE,					//This stands for "ENTERPRISE" keyword
		LITERAL_VARIABLES,					//This stands for "VARIABLES" keyword
		LITERAL_TRAP_TYPE,					//This stands for "TRAP-TYPE" keyword
		LITERAL_UNITS,						//This stands for "UNITS" keyword
		LITERAL_OBJECTS,					//This stands for "OBJECTS" keyword
		LITERAL_MODULE,						//This stands for "MODULE" keyword
		LITERAL_MANDATORY_GROUPS,			//This stands for "MANDATORY-GROUPS" keyword
		LITERAL_MIN_ACCESS,					//This stands for "MIN-ACCESS" keyword
		LITERAL_NOTIFICATIONS,				//This stands for "NOTIFICATIONS" keyword

		//This is the end of the list of possible rules
		RULE_LAST_RULE
	} m_nRuleType; //enum
};



///////////////////////////////////////////////////////////////////////////////
//
// class CRuleList
//
///////////////////////////////////////////////////////////////////////////////

/**
*	@brief		CRule is used in conjunction with CRuleList.
*	@author		Artur Perwenis
*	@date		18-Jan-2002
*/
class CRuleList : public CPtrList
{

//////////////
// Methods
//////////////

public:
	/**
	*	@brief	Gets head rule.
	*	@return			The head rule.
	*/
	CRule *GetHead ();
	/**
	*	@brief	Gets tail rule.
	*	@return			The tail rule.
	*/
	CRule *GetTail ();
	/**
	*	@brief	Adds a rule to the list.
	*	@param	nRule	The type of the rule to be added to the list.
	*/
	void AddRule (CRule::ERuleTypes nRule);
	/**
	*	@brief	Removes the head rule from the list.
	*/
	void RemoveHeadRule ();
};





///////////////////////////////////////////////////////////////////////////////
//
// class CMIBElement
//
///////////////////////////////////////////////////////////////////////////////

/**
*	@brief		CMIBElement is used in conjunction wigh CMIBElementList.
*	@author		Artur Perwenis
*	@date		18-Jan-2002
*/
class CMIBElement : public CObject
{

//////////////
// Methods
//////////////

public:
	/**
	*	@brief	class constructor
	*/
	CMIBElement ();
	/**
	*	@brief	class destructor
	*/
	~CMIBElement ();
	/**
	*	@brief	Attaches the supplied list to the element as its subobject list.
	*	@param	*pSubobjectList		The list to be added.
	*/
	void AttachList (CMIBElementList *pSubobjectList);
	/**
	*	@brief	Attaches element to this parent element. Creates the m_pSubObjectList if necessary.
	*	@param	*pSubElement		The element to be added.
	*/
	void AttachElement (CMIBElement *pSubElement);
	/**
	*	@brief	Use this method to construct a fully qualified element value (numeric)
	*			in the form ".1.2.4.x".
	*	@return			Constructed Oid value.
	*/
	CString BuildFullElementValue ();
	/**
	*	@brief	Use this method to construct a fully qualified element value (numeric)
	*			in the form ".iso.org.dod.internet.xxx".
	*	@return			Constructed element value.
	*/
	CString BuildFullElementName ();


//////////////
// Members
//////////////

public:
	//One of enum eElementTypes values
	eElementTypes m_nElementType;
	CString m_ElementString;	//The name of this element (displayed in the tree).
	CString m_ElementValue;		//The value (usually numeric) assigned to this element.
								//In "mib-2 OBJECT IDENTIFIER ::= { mgmt 1 }", this would be "1"
	CString m_ParentOIDName;	//CString. The name of parent element. Empty string if no parent is defined.
	
	//New class elements as of 2001-10-16.
	CString m_ObjectType;		//CString. Informational field only. No usage so far. In "sysDescr OBJECT-TYPE" this designates "OBJECT-TYPE" which, from the point of this algorithm, is not meaningful.
	CString m_Syntax;			//CString. Equals to SYNTAX field in element definition.
	CString m_Access;			//CString. Equals to ACCESS field in element definition.
	CString m_Status;			//CString. Equals to STATUS field in element definition.
	CString m_Description;		//CString. Equals to DESCRIPTION field in element definition.
	CString m_Index;			//CString. Equals to INDEX field in element definition (in a table.)
	CString m_ExtraInfo;		//CString. All non-standard field contents goes into this field.
	

		/*
			When an object is a compound object, this item is non-zero
			and points to another list (CMIBElementList) of CMIBElement
			objects, which are subordinate to this object
		*/
	CMIBElementList *m_pSubobjectList;
	CMIBElement *m_pParentElement;		//CMIBElement *. Provide two way linkage for elements.
	HTREEITEM m_hTreeItem;				//Used for display in the main window.
	CMIBFile *m_pParentFile;			//Used for tracking object syntax.
};

///////////////////////////////////////////////////////////////////////////////
//
// CMIBElement is used in conjunction wigh CMIBElementList
//
///////////////////////////////////////////////////////////////////////////////
class CMIBElementList : public CPtrList
{
public:
//Methods
	CMIBElement *GetHead (void);
	CMIBElement *GetTail (void);
	void RemoveElement (CMIBElement *pElement);
	CMIBElement *GetNextElement (POSITION &pos);
	bool LinkToParent (CMIBFile *pMIBFile, CMIBElement *pChildElement);
	CMIBElement *FindElement (CString ElementName);
	CMIBElement *FindElementEx (CString ElementName, CString ParentName);
	void ValidateLinks (void);
};





///////////////////////////////////////////////////////////////////////////////
//
// CMIBFile is used in conjunction wigh CMIBFileList
//
///////////////////////////////////////////////////////////////////////////////
class CMIBFileList : public CPtrList
{
public:
//Methods
	~CMIBFileList ();
	CMIBFile *GetHead (void);
	CMIBFile *GetTail (void);
	CMIBFile *GetNextFile (POSITION &pos);
	CMIBFile *GetPrevFile (POSITION &pos);
	CMIBFile *FindElement (HTREEITEM hTreeItem);
	CMIBFile *FindElement (CString FileName);
};





///////////////////////////////////////////////////////////////////////////////
//
// CIndexEntryList is used with CString
//
///////////////////////////////////////////////////////////////////////////////
class CIndexEntryList : public CPtrList
{
public:
//Methods
	~CIndexEntryList ();
	CString *GetHead (void);
	CString *GetTail (void);
	void AddIndexEntry (CString IndexEntry);		//Adds specified index field name to the table of index entries for later validation in ValidateIndiceEntries ().
	CString *GetNextEntry (POSITION &pos);
};

#endif	//___MIB_FILE_H__
