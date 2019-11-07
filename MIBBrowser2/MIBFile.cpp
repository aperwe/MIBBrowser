/**
*	@file		MIBFile.cpp
*	@brief		Parser subsystem implementation file.
*	@author		Artur Perwenis
*	@date		11-Oct-2001
*
*	@history
*	@item		11-Oct-2001		Artur Perwenis		First version.
*	@item		11-Jan-2002		Artur Perwenis		Added comments.
*/

#include "MIBFile.h"



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// CMIBFile class method bodies
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////





///////////////////////////////////////////////////////////////////////////////
// CMIBFile::CMIBFile ()
///////////////////////////////////////////////////////////////////////////////
CMIBFile::CMIBFile ()
{
	//Initialize buffers etc.
	m_dwIOBufferPos = 0;
	m_dwLastValidByte = -1;
	m_bBufferValid = false;
	m_bLastByteOK = true;
	m_bUnreadChar = false;
	m_cUnreadChar = -1;
	m_pFlatElementList = new CMIBElementList;
	m_pTreeElementList = new CMIBElementList;
	m_pIndexEntryList = new CIndexEntryList;
	m_pImportElementList = NULL;
	m_ModuleName = "";
	m_pLastFlatElement = NULL;
}


///////////////////////////////////////////////////////////////////////////////
// CMIBFile::~CMIBFile ()
///////////////////////////////////////////////////////////////////////////////
CMIBFile::~CMIBFile ()
{
	ReleaseAllFlatElements ();
	if (m_pFlatElementList)
	{
		delete m_pFlatElementList;
		m_pFlatElementList = NULL;
	}

	if (m_pTreeElementList)
	{
		delete m_pTreeElementList;
		m_pTreeElementList = NULL;
	}

	if (m_pIndexEntryList)
	{
		delete m_pIndexEntryList;
		m_pIndexEntryList = NULL;
	}

	if (m_pImportElementList)
	{
		delete m_pImportElementList;
		m_pImportElementList = NULL;
	}

	if (m_pRuleList)
	{
		delete m_pRuleList;
		m_pRuleList = NULL;
	}
}


///////////////////////////////////////////////////////////////////////////////
// bool CMIBFile::ParseFile ()
///////////////////////////////////////////////////////////////////////////////
bool CMIBFile::ParseFile ()
{
	bool bRetVal;

	m_bFileIsOpen = Open (m_FileName, CFile::modeRead | CFile::shareDenyWrite);

	if (!m_bFileIsOpen)
	{
		//The file could not be opened for reading
		//Display an error message to the user
		return false;
	}

	//Initialize the rule list for processing
	m_pRuleList = new CRuleList;

	//Now, create the first rule object that represents the whole MIB file for parsing
	//And add the first rules to the rule list

	m_pRuleList->AddRule (CRule::RULE_END_OF_FILE);
	m_pRuleList->AddRule (CRule::RULE_MIB);

	m_pFlatElementList->RemoveAll ();
	
	//As the rule list is initialized and the file is open
	//we can start actual parsing of the file.

	bRetVal = Parse ();

	m_pFlatElementList->ValidateLinks ();
	ValidateIndiceEntries ();

	Close (); m_bFileIsOpen = false;

	return !bRetVal;
}


///////////////////////////////////////////////////////////////////////////////
// bool CMIBFile::Parse ()
///////////////////////////////////////////////////////////////////////////////
bool CMIBFile::Parse ()
{
	CRule *pCurrentRule;
	bool bParseError = false;

	m_bTokenInvalid = true;

	//Parsing the file
	//When the loop exits, bParseError indicates whether or not an error occured
	//while parsing the file (ought to indicate syntax error).
	//TO DO: provide more detailed information about the cause of an error.
	while (!m_pRuleList->IsEmpty ())
	{
		//Get the top-most element from the list.
		pCurrentRule = m_pRuleList->GetHead ();

#ifdef _DEBUG
		if (!(m_Token.CompareNoCase ("entityGeneralGroup")))
			int a = 1;
#endif //_DEBUG

		switch (pCurrentRule->m_nRuleType)
		{
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_EMPTY_RULE:
			{
				//Remove the empty element from the list
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_MIB:
			{
				//001. RULE_MIB -> <MIB-NAME> RULE_MIB_2
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_MIB_2);
				m_pRuleList->AddRule (CRule::PLACEHOLDER_FOR_MIB_NAME);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_MIB_2:
			{
				//002. RULE_MIB_2 -> "DEFINITIONS" RULE_MIB_3
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_MIB_3);
				m_pRuleList->AddRule (CRule::LITERAL_DEFINITIONS);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_MIB_3:
			{
				//003. RULE_MIB_3 -> "::=" RULE_MIB_4
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_MIB_4);
				m_pRuleList->AddRule (CRule::LITERAL_EQUAL);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_MIB_4:
			{
				//004. RULE_MIB_4 -> "BEGIN" RULE_BODY "END"
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::LITERAL_END);
				m_pRuleList->AddRule (CRule::RULE_BODY);
				m_pRuleList->AddRule (CRule::LITERAL_BEGIN);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_BODY:
			{
				//005. RULE_BODY -> RULE_IMPORT RULE_BODY_2
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_BODY_2);
				m_pRuleList->AddRule (CRule::RULE_IMPORT);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_BODY_2:
			{
				//016. RULE_BODY_2 -> RULE_OBJECT RULE_BODY_3
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_BODY_3);
				m_pRuleList->AddRule (CRule::RULE_OBJECT);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_BODY_3:
			{
				//016a. RULE_BODY_3 -> [RULE_BODY_2 | {0}]
				//Recognized keywords here are
				//1. "END" -> End of MIB
				//2. PLACEHOLDER_FOR OID NAME -> Next object definition

				//Rules dispatch here.

				//Get token here
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("END"))
				{
					//"END" keyword found.
					//Remove this rule and assume {0} (RULE_EMPTY_RULE) applies.
					m_pRuleList->RemoveHeadRule ();
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: The above rule does not apply.
				//Assume RULE_BODY_2 applies
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_BODY_2);
				m_bTokenInvalid = false;		//re-use this token
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OBJECT:
			{
				//017. RULE_OBJECT -> <OidValue> [RULE_OBJECT_2]
				//OidValue is the name of an object.
				//This will be visible in the ListTreeView

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OBJECT_2);
				m_pRuleList->AddRule (CRule::PLACEHOLDER_FOR_OID_VALUE);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OBJECT_2:
			{
				//018. RULE_OBJECT_2 -> [RULE_OID | RULE_TYPEDEF | RULE_OBJECT_TYPE ]
				//Oid type of an object.
				//Object icon and information related to an object
				//depend upon this type
				//Recognized keywords here are:
				//1. "OBJECT" "IDENTIFIER"
				//2. "::=" 
				//3. <ANY-OBJECT-TYPE> name

				//Rules dispatch here.

				//Get token here
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("OBJECT"))
				{
					//'OBJECT' keyword found.
					//Remove this rule and assume RULE_OID applies.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OID);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("TRAP-TYPE"))
				{
					//'TRAP-TYPE' keyword found.
					//Remove this rule and assume RULE_TRAP_TYPE applies.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_TRAP_TYPE_DEF);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("::="))
				{
					//'::=' keyword found.
					//Remove this rule and assume RULE_TYPEDEF applies.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_TYPEDEF);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: None of the above rule dispatch options applies.
				//Remove this rule and assume RULE_OBJECT_TYPE applies.
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OBJECT_TYPE);
				m_bTokenInvalid = false;		//re-use this token
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OID:
			{
				//019. RULE_OID -> {"OBJECT" "IDENTIFIER"} RULE_OID_2	//FINISH HERE (?)
				//OidValue is the name of an object.
				//This will be visible in the ListTreeView

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OID_2);
				m_pRuleList->AddRule (CRule::LITERAL_IDENTIFIER);
				m_pRuleList->AddRule (CRule::LITERAL_OBJECT);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OID_2:
			{
				//023. RULE_OID_2 -> "::="  "{"  RULE_OID_4  "}"

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::LITERAL_CLOSE_CURLY_BRACKET);
//				m_pRuleList->AddRule (CRule::RULE_OID_4);
				m_pRuleList->AddRule (CRule::RULE_OID_5);		//[THIS IS A TEST]
				m_pRuleList->AddRule (CRule::LITERAL_OPEN_CURLY_BRACKET);
				m_pRuleList->AddRule (CRule::LITERAL_EQUAL);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_EQUAL:
			{
				//This rule gets RULE_OID_2
				//It is a version 012 update
				//It is a version 013 update

				//Rules dispatch here
				if (m_pLastFlatElement->m_nElementType == ETYPE_TRAP)
				{
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_TRAP_NUMBER_SET);
					break;
				}

				//It is a version 013 update
/*				if (m_pLastFlatElement->m_ObjectType.CompareNoCase ("NOTIFICATION-TYPE"))
				{
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::LITERAL_CLOSE_CURLY_BRACKET);
					m_pRuleList->AddRule (CRule::RULE_NOTIFICATION_PARENTS);
					m_pRuleList->AddRule (CRule::LITERAL_OPEN_CURLY_BRACKET);
					m_pRuleList->AddRule (CRule::LITERAL_EQUAL);
				}

*/				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OID_2);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
/*		case CRule::RULE_NOTIFICATION_PARENTS:
			{
				//::= { <ParentOID> <sub-parent-1> [<sub-parent-2> ...] <this-number> }

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::PLACEHOLDER_FOR_NOTIFICATION_PARENT_VALUES);
				m_pRuleList->AddRule (CRule::PLACEHOLDER_FOR_PARENT_OID_NAME);
			}
			break;
*/		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
/*		case CRule::PLACEHOLDER_FOR_NOTIFICATION_PARENT_VALUES:
			{
				//::= { <ParentOID> <sub-parent-1> [<sub-parent-2> ...] <this-number> }
				CString MultipleParentsString;
				CString LastParentString;

				while (m_Token.CompareNoCase ("}"))
				{
					NextSingleToken ();
					if (m_Token.CompareNoCase ("}"))
					{
						LastParentString = m_Token;

						if (MultipleParentString == "")
						{
							//First parent definition
							MultipleParentString = "NOTIFICATIONS: ";
							MultipleParentString += "m_Token";
						}
						else
						{
							//Another parent definition

					}
					else
					{
						m_bTokenInvalid = false;
						break;
					}
				}

				//Ensure the parent object is created
				m_pFlatElementList->LinkToParent (this, m_pLastFlatElement);

				this->m_bTokenInvalid = false;
				m_pRuleList->RemoveHeadRule ();
			}
			break;
*/		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OID_4:
			{
				//025. RULE_OID_4 -> <PARENT-OID-NAME> <PARENT-OID-VALUE>

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::PLACEHOLDER_FOR_PARENT_OID_VALUE);
				m_pRuleList->AddRule (CRule::PLACEHOLDER_FOR_PARENT_OID_NAME);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OID_5:
			{
				//025. RULE_OID_5 -> <PARENT-OID-NAME> <PARENT-OID-VALUE1> {<PARENT-OID-VALUE2> ...}

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_MORE_PARENT_VALUES);
				m_pRuleList->AddRule (CRule::PLACEHOLDER_FOR_PARENT_OID_VALUE);
				m_pRuleList->AddRule (CRule::PLACEHOLDER_FOR_PARENT_OID_NAME);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_MORE_PARENT_VALUES:
			{
				//Search if there are no more parent values in the notation before the closing "}"
				//Get token here
				NextSingleToken ();

				if (!m_Token.CompareNoCase ("}"))
				{
					//Ending '}' token found.
					//No more parent values here.
					//Remove this rule and proceed normal action.
					m_pRuleList->RemoveHeadRule ();
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Here an assumption is necessary, that the next parent value has been found.
				//This ought to be a numeric value.
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_MORE_PARENT_VALUES);
				m_pRuleList->AddRule (CRule::RULE_NEXT_PARENT_VALUE);
				m_bTokenInvalid = false;
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_NEXT_PARENT_VALUE:
			{
				//Get the value of the next parent
				//Get token here
				NextSingleToken ();

				if (m_pLastFlatElement)
				{
					m_pLastFlatElement->m_ElementValue += "." + m_Token;
				}
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_TYPEDEF:
			{
				//020. RULE_TYPEDEF -> "::=" RULE_TYPEDEF_2 {0}		//this must be an end of such a definition
				//TO DO: To check if this definition is correct, that is,
				//that it conforms to 020., we may replace RULE_EMPTY_RULE
				//with another new rule that will check the conformance.
				//For now, let's assume this is the only syntax possible.

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_TYPEDEF_2);
				m_pRuleList->AddRule (CRule::LITERAL_EQUAL);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_TYPEDEF_2:
			{
				//021. RULE_TYPEDEF_2 -> RULE_WELL_KNOWN_NAME		//FINISH HERE

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_WELL_KNOWN_NAME);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_WELL_KNOWN_NAME:
			{
				//022. RULE_WELL_KNOWN_NAME -> [ {"OCTET" "STRING"} | RULE_SEQUENCE | RULE_TEXTUAL_CONVENTION_DEF | other types here]

				//Recognized keywords here are:
				//1. "OCTET" "STRING"
				//2. "SEQUENCE" "{"
				//TO DO: Check if there are other possible keywords here

				//Rules dispatch here.

				//Get token here
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("OCTET"))
				{
					//'OBJECT' keyword found.
					//Remove this rule and assume LITERAL_OCTET applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_SD_MORE);
					m_pRuleList->AddRule (CRule::LITERAL_STRING);
					m_pRuleList->AddRule (CRule::LITERAL_OCTET);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("SEQUENCE"))
				{
					//'SEQUENCE' keyword found.
					//Remove this rule and assume RULE_SEQUENCE applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_SEQUENCE);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("TEXTUAL-CONVENTION"))
				{
					//'TEXTUAL-CONVENTION' keyword found.
					//Remove this rule and assume RULE_TEXTUAL_CONVENTION_DEF applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_TEXTUAL_CONVENTION_DEF);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: Here we assume that it is a textual convention
				//That has been previously either defined or not
				//(if not, we have to create an element for it, and define
				//that the next fields are syntax components)

				//Has this element been previously defined?
				CMIBElement *pTCElement;
				pTCElement = m_pFlatElementList->FindElement (m_pLastFlatElement->m_ElementString);

				if (pTCElement)
				{
					if (pTCElement->m_nElementType == ETYPE_TEXTUAL_CONV)
					{
						//There has already been defined an object
						//with this name and as ETYPE_TEXTUAL_CONV type.
						//If it is not the same element as m_pLastFlatElement,
						//we can delete the m_pLastFlatElement and continue
						//defining the ETYPE_TEXTUAL_CONV element.
						//Otherwise we continue defining the m_pLastFlatElement
						if (pTCElement == m_pLastFlatElement)
						{
							//m_pLastFlatElement is the correct element
							//to continue its definition in this place
							//This should never happen!
							assert (false);
							bParseError = true;
							return bParseError;
						}
						else
						{
							//m_pLastFlatElement is not the element we are
							//looking for.
							m_pFlatElementList->RemoveElement (m_pLastFlatElement);
							delete m_pLastFlatElement;
							m_pLastFlatElement = pTCElement;

							//The substitution has been made.
							//Proceed with definition
							m_pRuleList->RemoveHeadRule ();
							m_pRuleList->AddRule (CRule::RULE_SYNTAX_DEFINITION);
							m_bTokenInvalid = false;    //re-use this token
							break;
						}
					}
					else
					{
						//The element is not of ETYPE_TEXTUAL_CONV
						//Most probably this is m_pLastFlatElement
						if (pTCElement == m_pLastFlatElement)
						{
							//m_pLastFlatElement is the correct element
							//to continue its definition in this place
							m_pLastFlatElement->m_nElementType = ETYPE_TEXTUAL_CONV;
							m_pLastFlatElement->m_ParentOIDName = "TEXTUAL CONVENTIONS";
							m_pFlatElementList->LinkToParent (this, m_pLastFlatElement);

							m_pRuleList->RemoveHeadRule ();
							m_pRuleList->AddRule (CRule::RULE_SYNTAX_DEFINITION);
							m_bTokenInvalid = false;     //re-use this token
							break;
						}
						else
						{
							//m_pLastFlatElement is not the element we are
							//looking for.
							//This should never happen!
							assert (false);
							m_pFlatElementList->RemoveElement (m_pLastFlatElement);
							delete m_pLastFlatElement;
							m_pLastFlatElement = pTCElement;
							m_pLastFlatElement->m_nElementType = ETYPE_TEXTUAL_CONV;

							//The substitution has been made.
							//Proceed with definition
							m_pRuleList->RemoveHeadRule ();
							m_pRuleList->AddRule (CRule::RULE_SYNTAX_DEFINITION);
							m_bTokenInvalid = false;    //re-use this token
							break;
						}
					}
				}
				else
				{
					//This should never happen!
					//TO DO: Check if this is the case.
					assert (false);
					bParseError = true;
					return bParseError;
				}
				
				bParseError = true;
				return bParseError;
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_TEXTUAL_CONVENTION_DEF:
			{
				//083. RULE_TEXTUAL_CONVENTION_DEF -> "TEXTUAL-CONVENTION" RULE_TEXTUAL_CONVENTION_DEF_2
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_TEXTUAL_CONVENTION_DEF_2);
				m_pRuleList->AddRule (CRule::LITERAL_TEXTUAL_CONVENTION);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_TEXTUAL_CONVENTION_DEF_2:
			{
				//084. RULE_TEXTUAL_CONVENTION_DEF_2 -> RULE_OID_FIELDS
				//This rule is meant to mark that the element's parent
				//is "TEXTUAL CONVENTIONS" and its type is ETYPE_TEXTUAL_CONV
				m_pLastFlatElement->m_nElementType = ETYPE_TEXTUAL_CONV;
				m_pLastFlatElement->m_ParentOIDName = "TEXTUAL CONVENTIONS";
				m_pFlatElementList->LinkToParent (this, m_pLastFlatElement);
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OID_FIELDS);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_SEQUENCE:
			{
				//058. RULE_SEQUENCE -> "SEQUENCE"  RULE_SEQUENCE_2
				//EXAMPLE:
				//
				//ifEntry OBJECT-TYPE
				//    SYNTAX  IfEntry
				//    ACCESS  not-accessible
				//    STATUS  mandatory
				//    DESCRIPTION
				//            "An interface entry containing objects at the
				//            subnetwork layer and below for a particular
				//            interface."
				//    INDEX   { ifIndex }
				//    ::= { ifTable 1 }
				//
				//IfEntry ::=
				//    SEQUENCE {
				//        ifIndex
				//            INTEGER,
				//        ifDescr
				//            DisplayString,
				//        ifType
				//            INTEGER,
				//        ifMtu
				//            INTEGER,
				//        ifOperStatus
				//            INTEGER,
				//        ifLastChange
				//            TimeTicks,
				//        ifInOctets
				//            Counter,
				//        ifOutQLen
				//            Gauge,
				//        ifSpecific
				//           OBJECT IDENTIFIER
				//    }
				//On encountering this rule we can assume the object is a table
				//(defined in an earlier object) and it has sub-elements
				//TO DO: return to finishing this rule after "SEQUENCE" "OF" rule
				//has been defined.
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_SEQUENCE_2);
				m_pRuleList->AddRule (CRule::LITERAL_SEQUENCE);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_SEQUENCE_2:
			{
				//059. RULE_SEQUENCE_2 -> "{" RULE_SEQUENCE_3 "}"
				//EXAMPLE:
				//
				//IpAddrEntry ::=
				//    SEQUENCE {
				//        ipAdEntAddr
				//            IpAddress,
				//        ipAdEntIfIndex
				//            INTEGER,
				//        ipAdEntNetMask
				//            IpAddress,
				//        ipAdEntBcastAddr
				//            INTEGER,
				//        ipAdEntReasmMaxSize
				//            INTEGER (0..65535),
				//        ipRouteInfo
				//            OBJECT IDENTIFIER
				//    }
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::LITERAL_CLOSE_CURLY_BRACKET);
				m_pRuleList->AddRule (CRule::RULE_SEQUENCE_3);
				m_pRuleList->AddRule (CRule::LITERAL_OPEN_CURLY_BRACKET);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_SEQUENCE_3:
			{
				//006. RULE_IMPORT -> [RULE_IMPORT_2 | {0}]
				//Note: We are not interested at all in what is between the { and }
				//since this information is redundant and repeated
				//in the definitions of table sub-elements themselves.
				SkipCharactersUpToCloseCurlyBracket ();
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_IMPORT:
			{
				//006. RULE_IMPORT -> [RULE_IMPORT_2 | {0}]
				//Means: RULE_IMPORT_2 is optional
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_IMPORT_2);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_IMPORT_2:
			{
				//007. RULE_IMPORT_2 -> "IMPORTS" RULE_IMPORT_3
				//RULE_IMPORT_2 is optional
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_IMPORT_3);
				m_pRuleList->AddRule (CRule::LITERAL_IMPORTS);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_IMPORT_3:
			{
				//008. RULE_IMPORT_3 -> <IMPORT-NAME> RULE_IMPORT_4
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_IMPORT_4);
				m_pRuleList->AddRule (CRule::PLACEHOLDER_FOR_IMPORT_NAME);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_IMPORT_4:
			{
				//009. RULE_IMPORT_4 -> [RULE_IMPORT_5 | RULE_IMPORT_6]
				//Rules dispatch here.

				//Get token here
				NextSingleToken ();

				if (!m_Token.CompareNoCase (","))
				{
					//',' keyword found.
					//Remove this rule and assume RULE_IMPORT_5 applies.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_IMPORT_5);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("FROM"))
				{
					//'FROM' keyword found.
					//Remove this rule and assume RULE_IMPORT_6 applies.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_IMPORT_6);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: None of the rule dispatch options applies.
				//This is a syntax error.
				//TO DO: Provide some more detailed information
				//Syntax: "FROM" OR "," keyword expected.
				bParseError = true;
				return bParseError;
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_IMPORT_5:
			{
				//010. RULE_IMPORT_5 -> "," RULE_IMPORT_3      //RECURRING ITEM (IMPORT)
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_IMPORT_3);
				m_pRuleList->AddRule (CRule::LITERAL_COMMA);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_IMPORT_6:
			{
				//011. RULE_IMPORT_6 -> "FROM" RULE_IMPORT_7
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_IMPORT_7);
				m_pRuleList->AddRule (CRule::LITERAL_FROM);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_IMPORT_7:
			{
				//012. RULE_IMPORT_7 -> <MIB-MODULE-NAME> RULE_IMPORT_8     //Imported MIB module name
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_IMPORT_8);
				m_pRuleList->AddRule (CRule::PLACEHOLDER_FOR_MIB_MODULE_NAME);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_IMPORT_8:
			{
				//013. RULE_IMPORT_8 -> [RULE_IMPORT_9 | RULE_IMPORT_10]    //EITHER ; OR ANOTHER IMPORTS
				//Rules dispatch here.

				//Get token here
				//NextTokenWS ();
				NextSingleToken ();

				if (!m_Token.CompareNoCase (";"))
				{
					//',' keyword found.
					//Remove this rule and assume RULE_IMPORT_9 applies.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_IMPORT_9);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: Assume RULE_IMPORT_10 applies.
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_IMPORT_10);
				m_bTokenInvalid = false;			//re-use this token
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_IMPORT_9:
			{
				//014. RULE_IMPORT_9 -> ";" {0}
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::LITERAL_SEMICOLON);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_IMPORT_10:
			{
				//015. RULE_IMPORT_10 -> RULE_IMPORT_11 RULE_IMPORT_3		//another imports
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_IMPORT_3);
				m_pRuleList->AddRule (CRule::RULE_IMPORT_11);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_IMPORT_11:
			{
				//015a. RULE_IMPORT_11 -> {0}		//Version 005 update
				//Entering this rule means that another section of IMPORTS
				//exists in the file. In order to accomodate for this
				//situation, all we need to do in this rule is to
				//create new list for imported elements.
				//This is the same routine as is done in LITERAL_IMPORTS
				//rule
				m_pRuleList->RemoveHeadRule ();
				m_pImportElementList = new CMIBElementList;
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OBJECT_TYPE:
			{
				//026. RULE_OBJECT_TYPE -> RULE_OBJECT_TYPE ID   RULE_OID_FIELDS
				//Example:
				//sysDescr OBJECT-TYPE
				//    SYNTAX  DisplayString (SIZE (0..255))
				//
				// OBJECT-TYPE here is the name of an element
				// that is being imported from RFC-1212

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OID_FIELDS);
				m_pRuleList->AddRule (CRule::RULE_OBJECT_TYPE_ID);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_2:
			{
				//027. RULE_OT_2 -> RULE_OT_SYNTAX  RULE_OT_3		//FINISH HERE

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_3);
				m_pRuleList->AddRule (CRule::RULE_OT_SYNTAX);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_3:
			{
				//037. RULE_OT_3 -> RULE_OT_ACCESS  RULE_OT_4       //FINISH HERE

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_4);
				m_pRuleList->AddRule (CRule::RULE_OT_ACCESS);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_4:
			{
				//042. RULE_OT_4 -> RULE_OT_STATUS  RULE_OT_5		//FINISH HERE

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_5);
				m_pRuleList->AddRule (CRule::RULE_OT_STATUS);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_5:
			{
				//046. RULE_OT_5 -> RULE_OT_DESCRIPTION  RULE_OT_6	//FINISH HERE

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_6);
				m_pRuleList->AddRule (CRule::RULE_OT_DESCRIPTION);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_6:
			{
				//051. RULE_OT_6 -> RULE_OT_INDEX  RULE_OID_2         //RULE_OID_2 is defined above (023). This stands for "::= {parent parentVal}" sentence.

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OID_2);
				m_pRuleList->AddRule (CRule::RULE_OT_INDEX);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_INDEX:
			{
				//052. RULE_OT_INDEX -> [RULE_OT_INDEX_2 | {0}]          //OPTIONAL
				//NOTE: "INDEX" keyword is optional.
				//Recognized keywords here are:
				//1. "INDEX"
				//2. "::="
				//3. Any other keyword that this version is not aware of yet.

				//Rules dispatch here.

				//Get token here
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("INDEX"))
				{
					//'INDEX' keyword found.
					//Remove this rule and assume RULE_OT_INDEX_2 applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_INDEX_2);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: We assume that INDEX keyword is optional.
				//Remove this rule and assume RULE_EMPTY_RULE applies here.
				//Perhaps other keywords may come in hand here, but
				//this program version considers only "INDEX" as the allowed keyword here.

				m_bTokenInvalid = false;			//re-use this token
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_INDEX_2:
			{
				//053. RULE_OT_INDEX_2 -> "INDEX"  RULE_OT_INDEX_DEFINITION
				//NOTE: "INDEX" keyword is optional.
				m_pLastFlatElement->m_nElementType = ETYPE_TABLE_ITEM;
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_INDEX_DEFINITION);
				m_pRuleList->AddRule (CRule::LITERAL_INDEX);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_INDEX_DEFINITION:
			{
				//054. RULE_OT_INDEX_DEFINITION -> "{" RULE_OT_INDEX_FIELDS "}"
				//NOTE: "INDEX" keyword is optional.
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::LITERAL_CLOSE_CURLY_BRACKET);
				m_pRuleList->AddRule (CRule::RULE_OT_INDEX_FIELDS);
				m_pRuleList->AddRule (CRule::LITERAL_OPEN_CURLY_BRACKET);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_INDEX_FIELDS:
			{
				//055. RULE_OT_INDEX_FIELDS -> <INDEX-FIELD-NAME>  RULE_OT_INDEX_FIELDS_MORE
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_INDEX_FIELDS_MORE);
				m_pRuleList->AddRule (CRule::PLACEHOLDER_FOR_INDEX_FIELD_NAME);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_INDEX_FIELDS_MORE:
			{
				//056. RULE_OT_INDEX_FIELDS_MORE -> [ RULE_OT_INDEX_FIELDS_2 | {0} ]
				//Recognized keywords here are:
				//1. "}"
				//2. ","

				//Rules dispatch here.

				//Get token here
				NextSingleToken ();

				if (!m_Token.CompareNoCase ("}"))
				{
					//'}' keyword found.
					//Remove this rule and assume RULE_EMPTY_RULE applies here.
					m_pRuleList->RemoveHeadRule ();
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase (","))
				{
					//',' keyword found.
					//Remove this rule and assume RULE_OT_INDEX_FIELDS_2 applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_INDEX_FIELDS_2);
					m_pLastFlatElement->m_Index += ", "; //Next indexing field will be placed here
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: Anything else is not allowed here.
				//This is an erroneous situation.
				//TO DO: Provide some more detailed information
				//Syntax: Index definition invalid. "," or "}" expected.
				bParseError = true;
				return bParseError;

			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_INDEX_FIELDS_2:
			{
				//057. RULE_OT_INDEX_FIELDS_2 -> ","  RULE_OT_INDEX_FIELDS
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_INDEX_FIELDS);
				m_pRuleList->AddRule (CRule::LITERAL_COMMA);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_SYNTAX:
			{
				//028. RULE_OT_SYNTAX -> [RULE_OT_SYNTAX_2 | {0}]      //Optional?
				//Example:
				//sysDescr OBJECT-TYPE
				//    SYNTAX  DisplayString (SIZE (0..255))
				//
				// OBJECT-TYPE here is the name of an element
				// that is being imported from RFC-1212
				//TO DO: Make sure if "SYNTAX" keyword is optional.
				//Recognized keywords here are:
				//1. "SEQUENCE"
				//2. Anything else (any keyword or OID)

				//'SYNTAX' keyword found.
				//Remove this rule and assume RULE_OT_SYNTAX_2 applies here.
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_SYNTAX_2);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_SYNTAX_2:
			{
				//029. RULE_OT_SYNTAX_2 -> "SYNTAX" RULE_SYNTAX_DEFINITION
				//Example:
				//sysDescr OBJECT-TYPE
				//    SYNTAX  DisplayString (SIZE (0..255))
				//
				//SYNTAX-DEFINITON will be displayed to the user.
				//Syntax is either an object type name (one token) after which if:
				//there is no "{" then the definition goes to the end of line.
				//if there is a "{" then it is a table.
				//else, if the first keyword is "SEQUENCE" "OF" <ObjectID> then it is a table.

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_SYNTAX_DEFINITION);
				m_pRuleList->AddRule (CRule::LITERAL_SYNTAX);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_DESCRIPTION:
			{
				//047. RULE_OT_DESCRIPTION -> [RULE_OT_DESCRIPTION_2 | {0}]
				//Example:
				//sysDescr OBJECT-TYPE
				//    SYNTAX  DisplayString (SIZE (0..255))
				//    ACCESS  read-only
				//    STATUS  mandatory
				//    DESCRIPTION
				//            "A textual description of the entity.  This value
				//            should include the full name and version
				//            identification of the system's hardware type,
				//            software operating-system, and networking
				//            software.  It is mandatory that this only contain
				//            printable ASCII characters."
				//    ::= { system 1 }
				//
				//Recognized keywords here are:
				//1. "DESCRIPTION"
				//2. Anything else (any keyword or OID)

				//'DESCRIPTION' keyword found.
				//Remove this rule and assume RULE_OT_DESCRIPTION_2 applies here.
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_DESCRIPTION_2);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_DESCRIPTION_2:
			{
				//048. RULE_OT_DESCRIPTION_2 -> "DESCRIPTION"  RULE_OT_DESCRIPTION_DEFINITION
				//Example:
				//sysDescr OBJECT-TYPE
				//    SYNTAX  DisplayString (SIZE (0..255))
				//    ACCESS  read-only
				//    STATUS  mandatory
				//    DESCRIPTION
				//            "A textual description of the entity.  This value
				//            should include the full name and version
				//            identification of the system's hardware type,
				//            software operating-system, and networking
				//            software.  It is mandatory that this only contain
				//            printable ASCII characters."
				//    ::= { system 1 }
				//
				//DESCRIPTION-DEFINITON will be displayed to the user.
				//Description is anything between opening quote and closing quote (").

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_DESCRIPTION_DEFINITION);
				m_pRuleList->AddRule (CRule::LITERAL_DESCRIPTION);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_DESCRIPTION_DEFINITION:
			{
				//049. RULE_OT_DESCRIPTION_DEFINITION -> """  <RULE-OT-DESCRIPTION-TEXT> {0}
				//Example:
				//sysDescr OBJECT-TYPE
				//    SYNTAX  DisplayString (SIZE (0..255))
				//    ACCESS  read-only
				//    STATUS  mandatory
				//    DESCRIPTION
				//            "A textual description of the entity.  This value
				//            should include the full name and version
				//            identification of the system's hardware type,
				//            software operating-system, and networking
				//            software.  It is mandatory that this only contain
				//            printable ASCII characters."
				//    ::= { system 1 }
				//
				//DESCRIPTION-DEFINITON will be displayed to the user.
				//Description is anything between opening quote and closing quote (").
				//
				//Recognized keywords here are
				//1. """ (quotation mark)

				//Get token here
				NextSingleToken ();

				if (!m_Token.CompareNoCase ("\""))
				{
					//'"' keyword found.
					//Remove this rule and assume RULE_OT_DESCRIPTION_TEXT applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_DESCRIPTION_TEXT);
					this->m_bTokenInvalid = false;  //re-use this token
					break;
				}
				//Else: Description text was not found after DESCRIPTION keyword.
				//This is an erroneous situation.
				//TO DO: Provide some more detailed information
				//Syntax: Description text after "DESCRIPTION" keyword expected.
				bParseError = true;
				return bParseError;
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_DESCRIPTION_TEXT:
			{
				//049. RULE_OT_DESCRIPTION_DEFINITION -> """  <RULE-OT-DESCRIPTION-TEXT> {0}
				//Example:
				//sysDescr OBJECT-TYPE
				//    SYNTAX  DisplayString (SIZE (0..255))
				//    ACCESS  read-only
				//    STATUS  mandatory
				//    DESCRIPTION
				//            "A textual description of the entity.  This value
				//            should include the full name and version
				//            identification of the system's hardware type,
				//            software operating-system, and networking
				//            software.  It is mandatory that this only contain
				//            printable ASCII characters."
				//    ::= { system 1 }
				//
				//DESCRIPTION-DEFINITON will be displayed to the user.
				//Description is anything between opening quote and closing quote (").
				//At this point the opening quote has been found.
				//We need only to read in all characters up to the closing quote.
				NextSingleToken ();
				GetDescriptionString ();
				m_pLastFlatElement->m_Description = m_Token;

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_STATUS:
			{
				//043. RULE_OT_STATUS -> [RULE_OT_STATUS_2 | {0}]        //OPTIONAL?
				//Example:
				//sysDescr OBJECT-TYPE
				//    SYNTAX  DisplayString (SIZE (0..255))
				//    ACCESS  read-only
				//    STATUS  mandatory
				//
				//TO DO: Make sure if "STATUS" keyword is optional.
				//Recognized keywords here are:
				//1. "STATUS"
				//2. Anything else (any keyword or OID)

				//'STATUS' keyword found.
				//Remove this rule and assume RULE_OT_STATUS_2 applies here.
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_STATUS_2);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_STATUS_2:
			{
				//043. RULE_OT_STATUS_2 -> "STATUS" RULE_OT_STATUS_DEFINITION
				//Example:
				//sysDescr OBJECT-TYPE
				//    SYNTAX  DisplayString (SIZE (0..255))
				//    ACCESS  read-only
				//    STATUS  mandatory
				//
				//STATUS-DEFINITON will be displayed to the user.
				//Status is either 'mandatory', or 'deprecated' keyword.

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_STATUS_DEFINITION);
				m_pRuleList->AddRule (CRule::LITERAL_STATUS);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_STATUS_DEFINITION:
			{
				//044. RULE_OT_STATUS_DEFINITION -> RULE_OT_STATUS_VALID_KEYWORDS {0}
				//Example:
				//sysDescr OBJECT-TYPE
				//    SYNTAX  DisplayString (SIZE (0..255))
				//    ACCESS  read-only
				//    STATUS  mandatory
				//
				//Recognized keywords here are
				//1. "mandatory"
				//2. "deprecated"
				//3. "current"
				//4. "optional" //Added as of: 2002.01.21, to support newest MIBs
				//5. "obsolete" //Added as of: 2002.01.30, to support other MIBs.

				//Rules dispatch here.

				//Get token here
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("mandatory"))
				{
					//'mandatory' keyword found.
					//Remove this rule and assume RULE_OT_STATUS_VALID_KEYWORDS applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_STATUS_VALID_KEYWORDS);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("deprecated"))
				{
					//'deprecated' keyword found.
					//Remove this rule and assume RULE_OT_STATUS_VALID_KEYWORDS applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_STATUS_VALID_KEYWORDS);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("current"))
				{
					//'deprecated' keyword found.
					//Remove this rule and assume RULE_OT_STATUS_VALID_KEYWORDS applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_STATUS_VALID_KEYWORDS);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("optional"))
				{
					//'deprecated' keyword found.
					//Remove this rule and assume RULE_OT_STATUS_VALID_KEYWORDS applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_STATUS_VALID_KEYWORDS);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("obsolete"))
				{
					//'deprecated' keyword found.
					//Remove this rule and assume RULE_OT_STATUS_VALID_KEYWORDS applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_STATUS_VALID_KEYWORDS);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: status type is not recognized.
				//This is an erroneous situation.
				//TO DO: Provide some more detailed information
				//STATUS type unknown: <m_Token>.
				bParseError = true;
				return bParseError;
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_STATUS_VALID_KEYWORDS:
			{
				//045. RULE_OT_STATUS_VALID_KEYWORDS -> ["mandatory" | "deprecated"]
				//Example:
				//sysDescr OBJECT-TYPE
				//    SYNTAX  DisplayString (SIZE (0..255))
				//    ACCESS  read-only
				//    STATUS  mandatory
				//
				//Recognized keywords here are
				//1. "mandatory"
				//2. "deprecated"
				//
				//We assume here that only the valid STATUS type keywords pass to this rule
				//(the previous rule performs checking for known STATUS types - see RULE_OT_STATUS_DEFINITION).

				//Get token here
				NextTokenWS ();

				m_pLastFlatElement->m_Status = m_Token;

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_ACCESS:
			{
				//038. RULE_OT_ACCESS -> [RULE_OT_ACCESS_2 | {0}]      //OPTIONAL?
				//Example:
				//sysDescr OBJECT-TYPE
				//    SYNTAX  DisplayString (SIZE (0..255))
				//    ACCESS  read-only
				//
				//TO DO: Make sure if "ACCESS" keyword is optional.
				//Recognized keywords here are:
				//1. "ACCESS"
				//2. Anything else (any keyword or OID)

				//Rules dispatch here.

				//Get token here
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("ACCESS"))
				{
					//'ACCESS' keyword found.
					//Remove this rule and assume RULE_OT_ACCESS_2 applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_ACCESS_2);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: We assume that "ACCESS" keyword is optional

				m_bTokenInvalid = false;			//re-use this token
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_ACCESS_2:
			{
				//039. RULE_OT_ACCESS_2 -> "ACCESS"  RULE_OT_ACCESS_DEFINITION
				//Example:
				//sysDescr OBJECT-TYPE
				//    SYNTAX  DisplayString (SIZE (0..255))
				//    ACCESS  read-only
				//
				//ACCESS-DEFINITON will be displayed to the user.
				//Access is either 'read-only', or 'read-write', or 'not-accessible' keyword.

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_ACCESS_DEFINITION);
				m_pRuleList->AddRule (CRule::LITERAL_ACCESS);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_ACCESS_DEFINITION:
			{
				//040. RULE_OT_ACCESS_DEFINITION -> RULE_OT_ACCESS_VALID_KEYWORDS {0}
				//Example:
				//sysDescr OBJECT-TYPE
				//    SYNTAX  DisplayString (SIZE (0..255))
				//
				//Recognized keywords here are
				//1. "read-only"
				//2. "read-write"
				//3. "not-accessible"

				//Rules dispatch here.

				//Get token here
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("read-only"))
				{
					//'read-only' keyword found.
					//Remove this rule and assume RULE_OT_ACCESS_VALID_KEYWORDS applies here.
					if (m_pLastFlatElement->m_nElementType != ETYPE_TABLE_ROOT)
					{
						m_pLastFlatElement->m_nElementType = ETYPE_READ_ONLY_ENTRY;
					}
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_ACCESS_VALID_KEYWORDS);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("read-write"))
				{
					//'read-write' keyword found.
					//Remove this rule and assume RULE_OT_ACCESS_VALID_KEYWORDS applies here.
					if (m_pLastFlatElement->m_nElementType != ETYPE_TABLE_ROOT)
					{
						m_pLastFlatElement->m_nElementType = ETYPE_READ_WRITE_ENTRY;
					}
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_ACCESS_VALID_KEYWORDS);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("read-create"))
				{
					//'read-create' keyword found.
					//Remove this rule and assume RULE_OT_ACCESS_VALID_KEYWORDS applies here.
					if (m_pLastFlatElement->m_nElementType != ETYPE_TABLE_ROOT)
					{
						m_pLastFlatElement->m_nElementType = ETYPE_READ_WRITE_ENTRY;
					}
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_ACCESS_VALID_KEYWORDS);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("not-accessible"))
				{
					//'not-accessible' keyword found.
					//Remove this rule and assume RULE_OT_ACCESS_VALID_KEYWORDS applies here.
					if (m_pLastFlatElement->m_nElementType != ETYPE_TABLE_ROOT)
					{
						m_pLastFlatElement->m_nElementType = ETYPE_READ_ONLY_ENTRY;
					}
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_ACCESS_VALID_KEYWORDS);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: access type is not recognized.
				//This is an erroneous situation.
				//TO DO: Provide some more detailed information
				//ACCESS type unknown: <m_Token>.
				bParseError = true;
				return bParseError;
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_ACCESS_VALID_KEYWORDS:
			{
				//041. RULE_OT_ACCESS_VALID_KEYWORDS -> ["read-only" | "read-write" | "not-accessible"]
				//Example:
				//sysDescr OBJECT-TYPE
				//    SYNTAX  DisplayString (SIZE (0..255))
				//
				//Recognized keywords here are
				//1. "read-only"
				//2. "read-write"
				//2. "read-create"
				//3. "not-accessible"

				//
				//We assume here that only the valid ACCESS type keywords pass to this rule
				//(the previous rule performs checking for known ACCESS types - see RULE_OT_ACCESS_DEFINITION).

				//Get token here
				NextTokenWS ();

				m_pLastFlatElement->m_Access = m_Token;
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_SYNTAX_DEFINITION:
			{
				//030. RULE_SYNTAX_DEFINITION -> [ RULE_SYNTAX_SEQUENCE | RULE_SYNTAX_DEFINITION_2 ]
				//Example:
				//sysDescr OBJECT-TYPE
				//    SYNTAX  DisplayString (SIZE (0..255))
				//
				//SYNTAX-DEFINITON will be displayed to the user.
				//Syntax is either an object type name (one token) after which if:
				//there is no "{" then the definition goes to the end of line.
				//if there is a "{" then it is a table.
				//else, if the first keyword is "SEQUENCE" "OF" <ObjectID> then it is a table.
				//Recognized keywords here are
				//1. "OBJECT" "IDENTIFIER" //Leads to the same rule as <ANY-OBJECT-TYPE>
				//2. "INTEGER"				//Leads to the same rule as <ANY-OBJECT-TYPE>
				//3. "SEQUENCE" "OF" <TABLE-ENTRY-TYPE>
				//4. <ANY-OBJECT-TYPE>

				//Rules dispatch here.

				//Get token here
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("SEQUENCE"))
				{
					//'SEQUENCE' keyword found.
					//Remove this rule and assume RULE_SYNTAX_SEQUENCE applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_SYNTAX_SEQUENCE);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("OBJECT"))
				{
					//'OBJECT' keyword found.
					//Remove this rule and assume RULE_SD_MORE applies here.
					//Look at RULE_SYNTAX_DEFINITION_2. "OBJECT IDENTIFIER" is <SYNTAX-TYPE> in this case.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_SD_MORE);
					m_pRuleList->AddRule (CRule::RULE_SYNTAX_OBJECT_IDENTIFIER);
					m_pRuleList->AddRule (CRule::LITERAL_IDENTIFIER);
					m_pRuleList->AddRule (CRule::LITERAL_OBJECT);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("INTEGER"))
				{
					//'INTEGER' keyword found.
					//Remove this rule and assume RULE_SD_MORE applies here.
					//Look at RULE_SYNTAX_DEFINITION_2. "INTEGER" is <SYNTAX-TYPE> in this case.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_SD_MORE);
					m_pRuleList->AddRule (CRule::RULE_SYNTAX_OBJECT_INTEGER);
					m_pRuleList->AddRule (CRule::LITERAL_INTEGER);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("OCTET"))
				{
					//'OCTET' keyword found.
					//Remove this rule and assume RULE_SD_MORE applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_SD_MORE);
					m_pRuleList->AddRule (CRule::RULE_SD_OCTET_STRING);
					m_pRuleList->AddRule (CRule::LITERAL_STRING_2);
					m_pRuleList->AddRule (CRule::LITERAL_OCTET);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: We assume that this is <ANY-OBJECT-TYPE>

				m_bTokenInvalid = false;			//re-use this token
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_SYNTAX_DEFINITION_2);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_SYNTAX_OBJECT_INTEGER:
			{
				//Use this rule only to set the Syntax type of an object
				//to "INTEGER". Nothing more.

				m_pLastFlatElement->m_Syntax = "INTEGER";
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_SYNTAX_OBJECT_IDENTIFIER:
			{
				//Use this rule only to set the Syntax type of an object
				//to "INTEGER". Nothing more.

				m_pLastFlatElement->m_Syntax = "OBJECT IDENTIFIER";
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_SYNTAX_DEFINITION_2:
			{
				//030a. RULE_SYNTAX_DEFINITION_2 -> <SYNTAX-TYPE> RULE_SD_MORE
				//Example:
				//sysDescr OBJECT-TYPE
				//    SYNTAX  DisplayString (SIZE (0..255))
				//
				//SYNTAX-DEFINITON will be displayed to the user.
				//Syntax is either an object type name (one token) after which if:
				//there is no "{" then the definition goes to the end of line.
				//if there is a "{" then it is a table.
				//else, if the first keyword is "SEQUENCE" "OF" <ObjectID> then it is a table.
				//Recognized keywords here are
				//1. <ANY-OBJECT-TYPE> (compare with RULE_SYNTAX_DEFINITION)

				//Get token here
				NextTokenWS ();

				//TO DO: Perform checking if <ANY-OBJECT-TYPE> exists in the MIB or has been imported. This is not a critical error.
				m_pLastFlatElement->m_Syntax = m_Token;
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_SD_MORE);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_SD_MORE:
			{
				//031. RULE_SD_MORE -> [RULE_SD_MORE_2 | RULE_SD_MORE_3 | {0}]
				//Example:
				//sysDescr OBJECT-TYPE
				//    SYNTAX  DisplayString (SIZE (0..255))
				//
				//Recognized keywords here are
				//1. "{"
				//2. "("

				//Rules dispatch here.

				//Get token here
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("{"))
				{
					//'{' keyword found.
					//Remove this rule and assume RULE_SD_MORE_2 applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_SD_MORE_2);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("("))
				{
					//'(' keyword found.
					//Remove this rule and assume RULE_SD_MORE_3 applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_SD_MORE_3);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: We assume that rule {0} applies here

				m_bTokenInvalid = false;			//re-use this token
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_SD_MORE_2:
			{
				//032. RULE_SD_MORE_2 -> "{" RULE_SD_ENUM_DEF  "}"
				//Note: RULE_SD_ENUM_DEF is obsolete
				//Remember: watch out for opening/closing brackets
				//Example:
				//sysDescr OBJECT-TYPE
				//    SYNTAX  DisplayString (SIZE (0..255))
				//
				//This gets a little bit complicated.
				//What we do here is to append the "{" to the m_pLastFlatElement SYNTAX field
				//and then to fetch all characters up to the closing "}" bracket from the file,
				//and also append the characters to the SYNTAX field.
				NextTokenWS ();
				m_pLastFlatElement->m_Syntax += " {";
				GetEnumString ();
				m_pLastFlatElement->m_Syntax += m_Token;

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_SD_MORE_3:
			{
				//032a. RULE_SD_MORE_3 -> "(" RULE_SD_RANGE ")"
				//Remember: watch out for opening/closing brackets
				//Example:
				//sysDescr OBJECT-TYPE
				//    SYNTAX  DisplayString (SIZE (0..255))
				//
				//This gets a little bit complicated.
				//What we do here is to append the "(" to the m_pLastFlatElement SYNTAX field
				//and then to fetch all characters up to the closing ")" bracket from the file,
				//and also append the characters to the SYNTAX field.
				NextTokenWS ();
				m_pLastFlatElement->m_Syntax += " (";
				GetRangeString ();
				m_pLastFlatElement->m_Syntax += m_Token;

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OBJECT_TYPE_ID:
			{
				//026a. RULE_OBJECT_TYPE_ID -> <ANY-OBJECT-TYPE>
				//Example:
				//sysDescr OBJECT-TYPE
				//    SYNTAX  DisplayString (SIZE (0..255))
				//
				// OBJECT-TYPE here is the name of an element
				// that is being imported from RFC-1212

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::PLACEHOLDER_FOR_ANY_OBJECT_TYPE);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_SYNTAX_SEQUENCE:
			{
				//064. RULE_SYNTAX_SEQUENCE -> {"SEQUENCE" "OF"} RULE_SYNTAX_SEQUENCE_2
				//EXAMPLE:
				//ipAddrTable OBJECT-TYPE
				//    SYNTAX  SEQUENCE OF IpAddrEntry
				//    ACCESS  not-accessible
				//    STATUS  mandatory
				//    DESCRIPTION
				//            "The table of addressing information relevant to
				//            this entity's IP addresses."
				//    ::= { ip 20 }

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_SYNTAX_SEQUENCE_2);
				m_pRuleList->AddRule (CRule::LITERAL_OF);
				m_pRuleList->AddRule (CRule::LITERAL_SEQUENCE);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_SYNTAX_SEQUENCE_2:
			{
				//065. RULE_SYNTAX_SEQUENCE_2 -> <TABLE-OBJECT-IDENTIFIER> {0}
				//EXAMPLE:
				//ipAddrTable OBJECT-TYPE
				//    SYNTAX  SEQUENCE OF IpAddrEntry
				//    ACCESS  not-accessible
				//    STATUS  mandatory
				//    DESCRIPTION
				//            "The table of addressing information relevant to
				//            this entity's IP addresses."
				//    ::= { ip 20 }
				//At this point we know that the element currently being defined
				//is a table. We may prepare it for accepting
				//table entries.
				m_pLastFlatElement->m_nElementType = ETYPE_TABLE_ROOT;
				//In the RULE_SYNTAX_SEQUENCE_2 a table will be created
				//and its parent initialized, so that we know where to link the table

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::PLACEHOLDER_FOR_TABLE_OBJECT_IDENTIFIER);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OID_CUSTOM:
			{
				//066. RULE_OID_CUSTOM  ->  RULE_OID_LAST_UPDATED   RULE_OID_CUSTOM_2
				//		LAST-UPDATED "update time"
				//		ORGANIZATION "org name"
				//		CONTACT-INFO "contact information"
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OID_CUSTOM_2);
				m_pRuleList->AddRule (CRule::RULE_OID_LAST_UPDATED);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OID_LAST_UPDATED:
			{
				//067. RULE_OID_LAST_UPDATED  ->  [ RULE_OID_LAST_UPDATED_2 | {0} ]
				//		LAST-UPDATED "update time"
				//		ORGANIZATION "org name"
				//		CONTACT-INFO "contact information"
				//Recognized keywords here are:
				//1. "LAST-UPDATED"
				//2. <ANY-OTHER-IDENTIFIER>

				//Rules dispatch here

				//Get token here
				NextSingleToken ();

				if (!m_Token.CompareNoCase ("LAST-UPDATED"))
				{
					//'LAST-UPDATED' keyword found.
					//Remove this rule and assume RULE_OID_LAST_UPDATED_2 applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OID_LAST_UPDATED_2);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: "LAST_UPDATED" keyword is optional

				m_bTokenInvalid = false;			//re-use this token
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OID_FIELDS:
			{
				//078. RULE_OID_FIELDS -> [RULE_OT_SYNTAX  |
				//						 RULE_OT_ACCESS  |
				//						 RULE_OT_STATUS  |
				//						 RULE_OT_DESCRIPTION  |
				//						 RULE_OT_INDEX  |
				//						 RULE_OT_LAST_UPDATED  |
				//						 RULE_OT_ORGANIZATION  |
				//						 RULE_OT_CONTACT_INFO  |
				//						 RULE_OT_MAX_ACCESS  |
				//						 RULE_OT_DISPLAY_HINT  |
				//						 RULE_OT_EQUAL  |					//this is a "::=" token
				//						 RULE_OT_REFERENCE  |
				//						 RULE_OT_DEFVAL  |					//this is a "DEFVAL { <value> }"
				//						 RULE_OT_REVISION  |
				//						 RULE_OT_AUGMENTS                  //this is "AUGMENTS { <parentOid> }"
				//						 ]
				//						 [RULE_OID_FIELDS | {0} ]
				//Recognized keywords here are:
				//1.  SYNTAX
				//2.  ACCESS
				//3.  STATUS
				//4.  DESCRIPTION
				//5.  INDEX
				//6.  LAST-UPDATED
				//7.  ORGANIZATION
				//8.  CONTACT-INFO
				//9.  MAX-ACCESS
				//10. DISPLAY-HINT
				//11. ::=
				//12. REFERENCE
				//13. DEFVAL
				//14. REVISION
				//15. AUGMENTS
				//16. VARIABLES
				//17. ENTERPRISE
				//18. UNITS
				//19. OBJECTS
				//20. <ANY-OBJECT-NAME-BEING-DEFINED>		//this means: {0}; the end of rule

				//Rules dispatch here

				//Get token here
				NextSingleToken ();

				if (!m_Token.CompareNoCase ("SYNTAX"))
				{
					//'SYNTAX' keyword found.
					//Remove this rule and assume RULE_OT_SYNTAX applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OID_FIELDS);		//Repeat the rule
					m_pRuleList->AddRule (CRule::RULE_OT_SYNTAX);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("::="))
				{
					//'::=' keyword found.
					//Remove this rule and assume RULE_OT_EQUAL applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OID_FIELDS);		//Repeat the rule
					m_pRuleList->AddRule (CRule::RULE_OT_EQUAL);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("ACCESS"))
				{
					//'ACCESS' keyword found.
					//Remove this rule and assume RULE_OT_ACCESS applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OID_FIELDS);		//Repeat the rule
					m_pRuleList->AddRule (CRule::RULE_OT_ACCESS);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("STATUS"))
				{
					//'STATUS' keyword found.
					//Remove this rule and assume RULE_OT_STATUS applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OID_FIELDS);		//Repeat the rule
					m_pRuleList->AddRule (CRule::RULE_OT_STATUS);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("DESCRIPTION"))
				{
					//'DESCRIPTION' keyword found.
					//Remove this rule and assume RULE_OT_DESCRIPTION applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OID_FIELDS);		//Repeat the rule
					m_pRuleList->AddRule (CRule::RULE_OT_DESCRIPTION);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("OBJECT"))
				{
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OID_FIELDS);		//Repeat the rule
					m_pRuleList->AddRule (CRule::RULE_OT_OBJECT);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("INDEX"))
				{
					//'INDEX' keyword found.
					//Remove this rule and assume RULE_OT_INDEX applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OID_FIELDS);		//Repeat the rule
					m_pRuleList->AddRule (CRule::RULE_OT_INDEX);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("LAST-UPDATED"))
				{
					//'LAST-UPDATED' keyword found.
					//Remove this rule and assume RULE_OT_LAST_UPDATED applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OID_FIELDS);		//Repeat the rule
					m_pRuleList->AddRule (CRule::RULE_OT_LAST_UPDATED);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("ORGANIZATION"))
				{
					//'ORGANIZATION' keyword found.
					//Remove this rule and assume RULE_OT_ORGANIZATION applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OID_FIELDS);		//Repeat the rule
					m_pRuleList->AddRule (CRule::RULE_OT_ORGANIZATION);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("MAX-ACCESS"))
				{
					//'MAX-ACCESS' keyword found.
					//Remove this rule and assume RULE_OT_MAX_ACCESS applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OID_FIELDS);		//Repeat the rule
					m_pRuleList->AddRule (CRule::RULE_OT_MAX_ACCESS);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("MIN-ACCESS"))
				{
					//'MIN-ACCESS' keyword found.
					//Remove this rule and assume RULE_OT_MIN_ACCESS applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OID_FIELDS);		//Repeat the rule
					m_pRuleList->AddRule (CRule::RULE_OT_MIN_ACCESS);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("DISPLAY-HINT"))
				{
					//'DISPLAY-HINT' keyword found.
					//Remove this rule and assume RULE_OT_DISPLAY_HINT applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OID_FIELDS);		//Repeat the rule
					m_pRuleList->AddRule (CRule::RULE_OT_DISPLAY_HINT);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("REFERENCE"))
				{
					//'REFERENCE' keyword found.
					//Remove this rule and assume RULE_OT_REFERENCE applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OID_FIELDS);		//Repeat the rule
					m_pRuleList->AddRule (CRule::RULE_OT_REFERENCE);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("DEFVAL"))
				{
					//'DEFVAL' keyword found.
					//Remove this rule and assume RULE_OT_DEFVAL applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OID_FIELDS);		//Repeat the rule
					m_pRuleList->AddRule (CRule::RULE_OT_DEFVAL);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("REVISION"))
				{
					//'REVISION' keyword found.
					//Remove this rule and assume RULE_OT_REVISION applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OID_FIELDS);		//Repeat the rule
					m_pRuleList->AddRule (CRule::RULE_OT_REVISION);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("AUGMENTS"))
				{
					//'AUGMENTS' keyword found.
					//Remove this rule and assume RULE_OT_AUGMENTS applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OID_FIELDS);		//Repeat the rule
					m_pRuleList->AddRule (CRule::RULE_OT_AUGMENTS);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("ENTERPRISE"))
				{
					//'ENTERPRISE' keyword found.
					//Remove this rule and assume RULE_OT_AUGMENTS applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OID_FIELDS);		//Repeat the rule
					m_pRuleList->AddRule (CRule::RULE_OT_ENTERPRISE);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("VARIABLES"))
				{
					//'VARIABLES' keyword found.
					//Remove this rule and assume RULE_OT_ENTERPRISE applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OID_FIELDS);		//Repeat the rule
					m_pRuleList->AddRule (CRule::RULE_OT_VARIABLES);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("UNITS"))
				{
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OID_FIELDS);		//Repeat the rule
					m_pRuleList->AddRule (CRule::RULE_OT_UNITS);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("OBJECTS"))
				{
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OID_FIELDS);		//Repeat the rule
					m_pRuleList->AddRule (CRule::RULE_OT_OBJECTS);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("MODULE"))
				{
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OID_FIELDS);		//Repeat the rule
					m_pRuleList->AddRule (CRule::RULE_OT_MODULE);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("MANDATORY-GROUPS"))
				{
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OID_FIELDS);		//Repeat the rule
					m_pRuleList->AddRule (CRule::RULE_OT_MANDATORY_GROUPS);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("NOTIFICATIONS"))
				{
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OID_FIELDS);		//Repeat the rule
					m_pRuleList->AddRule (CRule::RULE_OT_NOTIFICATIONS);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("CONTACT-INFO"))
				{
					//'CONTACT-INFO' keyword found.
					//Remove this rule and assume RULE_OT_CONTACT_INFO applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OID_FIELDS);		//Repeat the rule
					m_pRuleList->AddRule (CRule::RULE_OT_CONTACT_INFO);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: End of object definition encountered
				m_bTokenInvalid = false;			//re-use this token
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_LAST_UPDATED:
			{
				//085. RULE_OT_LAST_UPDATED -> [RULE_OT_LAST_UPDATED_2 | {0}]
				//Recognized keywords here are:
				//1. "LAST-UPDATED"
				//2. Anything else (any keyword or OID)

				//Rules dispatch here.

				//Get token here
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("LAST-UPDATED"))
				{
					//'LAST-UPDATED' keyword found.
					//Remove this rule and assume RULE_OT_LAST_UPDATED_2 applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_LAST_UPDATED_2);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: "LAST-UPDATED" keyword is optional

				m_bTokenInvalid = false;			//re-use this token
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_LAST_UPDATED_2:
			{
				//086. RULE_OT_LAST_UPDATED_2 -> "LAST-UPDATED"  RULE_OT_LAST_UPDATED_DEFINITION
				//LAST-UPDATED will be displayed to the user as extra information.

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_LAST_UPDATED_DEFINITION);
				m_pRuleList->AddRule (CRule::LITERAL_LAST_UPDATED);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_LAST_UPDATED_DEFINITION:
			{
				//087. RULE_OT_LAST_UPDATED_DEFINITION -> """  RULE_OT_LAST_UPDATED_TEXT {0}
				//
				//Recognized keywords here are
				//1. """ (quotation mark)

				//Get token here
				NextSingleToken ();

				if (!m_Token.CompareNoCase ("\""))
				{
					//'"' keyword found.
					//Remove this rule and assume RULE_OT_LAST_UPDATED_TEXT applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_LAST_UPDATED_TEXT);
					this->m_bTokenInvalid = false;  //re-use this token
					break;
				}
				//Else: LAST-UPDATED text was not found after LAST-UPDATED keyword.
				//This is an erroneous situation.
				//TO DO: Provide some more detailed information
				//Syntax: Update time text after "LAST-UPDATED" keyword expected.
				bParseError = true;
				return bParseError;
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_LAST_UPDATED_TEXT:
			{
				//088. RULE_OT_LAST_UPDATED_TEXT -> """  RULE_ANYSTRING  """ {0}
				//Last-updated text is anything between opening quote and closing quote (").
				//At this point the opening quote has been found.
				//We need only to read in all characters up to the closing quote.
				NextSingleToken ();
				GetDescriptionString ();
				m_pLastFlatElement->m_ExtraInfo += "LAST-UPDATED: " + m_Token + "\n";
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_ORGANIZATION:
			{
				//089. RULE_OT_ORGANIZATION -> [RULE_OT_ORGANIZATION_2 | {0}]
				//Recognized keywords here are:
				//1. "ORGANIZATION"
				//2. Anything else (any keyword or OID)

				//Rules dispatch here.

				//Get token here
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("ORGANIZATION"))
				{
					//'ORGANIZATION' keyword found.
					//Remove this rule and assume RULE_OT_ORGANIZATION_2 applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_ORGANIZATION_2);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: "ORGANIZATION" keyword is optional

				m_bTokenInvalid = false;			//re-use this token
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_ORGANIZATION_2:
			{
				//090. RULE_OT_ORGANIZATION_2 -> "ORGANIZATION"  RULE_OT_ORGANIZATION_DEFINITION
				//ORGANIZATION will be displayed to the user as extra information.

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_ORGANIZATION_DEFINITION);
				m_pRuleList->AddRule (CRule::LITERAL_ORGANIZATION);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_ORGANIZATION_DEFINITION:
			{
				//091. RULE_OT_ORGANIZATION_DEFINITION -> """  RULE_OT_ORGANIZATION_TEXT {0}
				//
				//Recognized keywords here are
				//1. """ (quotation mark)

				//Get token here
				NextSingleToken ();

				if (!m_Token.CompareNoCase ("\""))
				{
					//'"' keyword found.
					//Remove this rule and assume RULE_OT_ORGANIZATION_TEXT applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_ORGANIZATION_TEXT);
					this->m_bTokenInvalid = false;  //re-use this token
					break;
				}
				//Else: ORGANIZATION text was not found after ORGANIZATION keyword.
				//This is an erroneous situation.
				//TO DO: Provide some more detailed information
				//Syntax: Organization name after "ORGANIZATION" keyword expected.
				bParseError = true;
				return bParseError;
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_ORGANIZATION_TEXT:
			{
				//092. RULE_OT_ORGANIZATION_TEXT -> """  RULE_ANYSTRING  """ {0}
				//Organization name is anything between opening quote and closing quote (").
				//At this point the opening quote has been found.
				//We need only to read in all characters up to the closing quote.
				NextSingleToken ();
				GetDescriptionString ();
				m_pLastFlatElement->m_ExtraInfo += "ORGANIZATION: " + m_Token + "\n";
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_CONTACT_INFO:
			{
				//089. RULE_OT_CONTACT_INFO -> [RULE_OT_CONTACT_INFO_2 | {0}]
				//Recognized keywords here are:
				//1. "CONTACT-INFO"
				//2. Anything else (any keyword or OID)

				//Rules dispatch here.

				//Get token here
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("CONTACT-INFO"))
				{
					//'CONTACT-INFO' keyword found.
					//Remove this rule and assume RULE_OT_CONTACT_INFO_2 applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_CONTACT_INFO_2);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: "CONTACT-INFO" keyword is optional

				m_bTokenInvalid = false;			//re-use this token
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_CONTACT_INFO_2:
			{
				//090. RULE_OT_CONTACT_INFO_2 -> "ORGANIZATION"  RULE_OT_CONTACT_INFO_DEFINITION
				//ORGANIZATION will be displayed to the user as extra information.

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_CONTACT_INFO_DEFINITION);
				m_pRuleList->AddRule (CRule::LITERAL_CONTACT_INFO);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_CONTACT_INFO_DEFINITION:
			{
				//091. RULE_OT_CONTACT_INFO_DEFINITION -> """  RULE_OT_CONTACT_INFO_TEXT {0}
				//
				//Recognized keywords here are
				//1. """ (quotation mark)

				//Get token here
				NextSingleToken ();

				if (!m_Token.CompareNoCase ("\""))
				{
					//'"' keyword found.
					//Remove this rule and assume RULE_OT_CONTACT_INFO_TEXT applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_CONTACT_INFO_TEXT);
					this->m_bTokenInvalid = false;  //re-use this token
					break;
				}
				bParseError = true;
				return bParseError;
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_CONTACT_INFO_TEXT:
			{
				//092. RULE_OT_CONTACT_INFO_TEXT -> """  RULE_ANYSTRING  """ {0}
				NextSingleToken ();
				GetDescriptionString ();
				m_pLastFlatElement->m_ExtraInfo += "CONTACT-INFO: " + m_Token + "\n";
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_DISPLAY_HINT:
			{
				//089. RULE_OT_DISPLAY_HINT -> [RULE_OT_CONTACT_INFO_2 | {0}]
				//Recognized keywords here are:
				//1. "CONTACT-INFO"
				//2. Anything else (any keyword or OID)

				//Rules dispatch here.

				//Get token here
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("DISPLAY-HINT"))
				{
					//'CONTACT-INFO' keyword found.
					//Remove this rule and assume RULE_OT_CONTACT_INFO_2 applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_DISPLAY_HINT_2);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: "CONTACT-INFO" keyword is optional

				m_bTokenInvalid = false;			//re-use this token
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_DISPLAY_HINT_2:
			{
				//090. RULE_OT_CONTACT_INFO_2 -> "ORGANIZATION"  RULE_OT_CONTACT_INFO_DEFINITION
				//ORGANIZATION will be displayed to the user as extra information.

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_DISPLAY_HINT_DEFINITION);
				m_pRuleList->AddRule (CRule::LITERAL_DISPLAY_HINT);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_DISPLAY_HINT_DEFINITION:
			{
				//091. RULE_OT_CONTACT_INFO_DEFINITION -> """  RULE_OT_CONTACT_INFO_TEXT {0}

				//Get token here
				NextSingleToken ();

				if (!m_Token.CompareNoCase ("\""))
				{
					//'"' keyword found.
					//Remove this rule and assume RULE_OT_CONTACT_INFO_TEXT applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_DISPLAY_HINT_TEXT);
					this->m_bTokenInvalid = false;  //re-use this token
					break;
				}
				bParseError = true;
				return bParseError;
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_DISPLAY_HINT_TEXT:
			{
				//092. RULE_OT_CONTACT_INFO_TEXT -> """  RULE_ANYSTRING  """ {0}
				NextSingleToken ();
				GetDescriptionString ();
				m_pLastFlatElement->m_ExtraInfo += "DISPLAY-HINT: " + m_Token + "\n";
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_REFERENCE:
			{
				//089. RULE_OT_REFERENCE -> [RULE_OT_CONTACT_INFO_2 | {0}]
				//Recognized keywords here are:
				//1. "CONTACT-INFO"
				//2. Anything else (any keyword or OID)

				//Rules dispatch here.

				//Get token here
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("REFERENCE"))
				{
					//'CONTACT-INFO' keyword found.
					//Remove this rule and assume RULE_OT_REFERENCE_2 applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_REFERENCE_2);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: "CONTACT-INFO" keyword is optional

				m_bTokenInvalid = false;			//re-use this token
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_REFERENCE_2:
			{
				//090. RULE_OT_REFERENCE_2 -> "ORGANIZATION"  RULE_OT_CONTACT_INFO_DEFINITION
				//ORGANIZATION will be displayed to the user as extra information.

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_REFERENCE_DEFINITION);
				m_pRuleList->AddRule (CRule::LITERAL_REFERENCE);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_REFERENCE_DEFINITION:
			{
				//091. RULE_OT_REFERENCE_DEFINITION -> """  RULE_OT_CONTACT_INFO_TEXT {0}

				//Get token here
				NextSingleToken ();

				if (!m_Token.CompareNoCase ("\""))
				{
					//'"' keyword found.
					//Remove this rule and assume RULE_OT_CONTACT_INFO_TEXT applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_REFERENCE_TEXT);
					this->m_bTokenInvalid = false;  //re-use this token
					break;
				}
				bParseError = true;
				return bParseError;
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_REFERENCE_TEXT:
			{
				//092. RULE_OT_REFERENCE_TEXT -> """  RULE_ANYSTRING  """ {0}
				NextSingleToken ();
				GetDescriptionString ();
				m_pLastFlatElement->m_ExtraInfo += "REFERENCE: " + m_Token + "\n";
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_REVISION:
			{
				//089. RULE_OT_REVISION -> [RULE_OT_REVISION_2 | {0}]
				//Recognized keywords here are:
				//1. "CONTACT-INFO"
				//2. Anything else (any keyword or OID)

				//Rules dispatch here.

				//Get token here
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("REVISION"))
				{
					//'CONTACT-INFO' keyword found.
					//Remove this rule and assume RULE_OT_REVISION_2 applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_REVISION_2);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: "CONTACT-INFO" keyword is optional

				m_bTokenInvalid = false;			//re-use this token
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_REVISION_2:
			{
				//090. RULE_OT_REFERENCE_2 -> "ORGANIZATION"  RULE_OT_CONTACT_INFO_DEFINITION
				//ORGANIZATION will be displayed to the user as extra information.

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_REVISION_DEFINITION);
				m_pRuleList->AddRule (CRule::LITERAL_REVISION);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_REVISION_DEFINITION:
			{
				//091. RULE_OT_REFERENCE_DEFINITION -> """  RULE_OT_CONTACT_INFO_TEXT {0}

				//Get token here
				NextSingleToken ();

				if (!m_Token.CompareNoCase ("\""))
				{
					//'"' keyword found.
					//Remove this rule and assume RULE_OT_CONTACT_INFO_TEXT applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_REVISION_TEXT);
					this->m_bTokenInvalid = false;  //re-use this token
					break;
				}
				bParseError = true;
				return bParseError;
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_REVISION_TEXT:
			{
				//092. RULE_OT_REFERENCE_TEXT -> """  RULE_ANYSTRING  """ {0}
				NextSingleToken ();
				GetDescriptionString ();
				m_pLastFlatElement->m_ExtraInfo += "REVISION: " + m_Token + "\n";
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_MAX_ACCESS:
			{
				//038. RULE_OT_ACCESS -> [RULE_OT_ACCESS_2 | {0}]      //OPTIONAL?
				//Example:
				//sysDescr OBJECT-TYPE
				//    SYNTAX  DisplayString (SIZE (0..255))
				//    ACCESS  read-only
				//
				//TO DO: Make sure if "ACCESS" keyword is optional.
				//Recognized keywords here are:
				//1. "ACCESS"
				//2. Anything else (any keyword or OID)

				//Rules dispatch here.

				//Get token here
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("MAX-ACCESS"))
				{
					//'MAX-ACCESS' keyword found.
					//Remove this rule and assume RULE_OT_ACCESS_2 applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_MAX_ACCESS_2);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: We assume that "ACCESS" keyword is optional

				m_bTokenInvalid = false;			//re-use this token
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_MAX_ACCESS_2:
			{
				//039. RULE_OT_ACCESS_2 -> "ACCESS"  RULE_OT_ACCESS_DEFINITION
				//Example:
				//sysDescr OBJECT-TYPE
				//    SYNTAX  DisplayString (SIZE (0..255))
				//    ACCESS  read-only
				//
				//ACCESS-DEFINITON will be displayed to the user.
				//Access is either 'read-only', or 'read-write', or 'not-accessible' keyword.

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_MAX_ACCESS_DEFINITION);
				m_pRuleList->AddRule (CRule::LITERAL_MAX_ACCESS);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_MAX_ACCESS_DEFINITION:
			{
				//040. RULE_OT_MAX_ACCESS_DEFINITION -> RULE_OT_ACCESS_VALID_KEYWORDS {0}
				//
				//Recognized keywords here are
				//1. "read-only"
				//2. "read-write"
				//3. "not-accessible"

				//Rules dispatch here.

				//Get token here
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("read-only"))
				{
					if (m_pLastFlatElement->m_nElementType != ETYPE_TABLE_ROOT)
					{
						m_pLastFlatElement->m_nElementType = ETYPE_READ_ONLY_ENTRY;
					}
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_MAX_ACCESS_VALID_KEYWORDS);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("read-write"))
				{
					if (m_pLastFlatElement->m_nElementType != ETYPE_TABLE_ROOT)
					{
						m_pLastFlatElement->m_nElementType = ETYPE_READ_WRITE_ENTRY;
					}
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_MAX_ACCESS_VALID_KEYWORDS);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("read-create"))
				{
					if (m_pLastFlatElement->m_nElementType != ETYPE_TABLE_ROOT)
					{
						m_pLastFlatElement->m_nElementType = ETYPE_READ_WRITE_ENTRY;
					}
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_MAX_ACCESS_VALID_KEYWORDS);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("not-accessible"))
				{
					if (m_pLastFlatElement->m_nElementType != ETYPE_TABLE_ROOT)
					{
						m_pLastFlatElement->m_nElementType = ETYPE_READ_ONLY_ENTRY;
					}
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_MAX_ACCESS_VALID_KEYWORDS);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("accessible-for-notify"))
				{
					if (m_pLastFlatElement->m_nElementType != ETYPE_TABLE_ROOT)
					{
						m_pLastFlatElement->m_nElementType = ETYPE_READ_ONLY_ENTRY;
					}
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_MAX_ACCESS_VALID_KEYWORDS);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: access type is not recognized.
				//This is an erroneous situation.
				//TO DO: Provide some more detailed information
				//ACCESS type unknown: <m_Token>.
				bParseError = true;
				return bParseError;
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_MAX_ACCESS_VALID_KEYWORDS:
			{
				//041. RULE_OT_MAX_ACCESS_VALID_KEYWORDS -> ["read-only" | "read-write" | "not-accessible"]
				//Get token here
				NextTokenWS ();

				m_pLastFlatElement->m_Access = m_Token;
				m_pLastFlatElement->m_ExtraInfo += "MAX-ACCESS: " + m_Token + "\n";
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_MIN_ACCESS:
			{
				//Example:
				//    MIN-ACCESS  read-only

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_MIN_ACCESS_DEFINITION);
				m_pRuleList->AddRule (CRule::LITERAL_MIN_ACCESS);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_MIN_ACCESS_DEFINITION:
			{
				//Rules dispatch here.

				//Get token here
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("read-only"))
				{
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_MIN_ACCESS_VALID_KEYWORDS);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("read-write"))
				{
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_MIN_ACCESS_VALID_KEYWORDS);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("read-create"))
				{
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_MIN_ACCESS_VALID_KEYWORDS);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("not-accessible"))
				{
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_MIN_ACCESS_VALID_KEYWORDS);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("accessible-for-notify"))
				{
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_MIN_ACCESS_VALID_KEYWORDS);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: access type is not recognized.
				//This is an erroneous situation.
				//TO DO: Provide some more detailed information
				//MIN-ACCESS type unknown: <m_Token>.
				bParseError = true;
				return bParseError;
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_MIN_ACCESS_VALID_KEYWORDS:
			{
				//Get token here
				NextTokenWS ();

				m_pLastFlatElement->m_ExtraInfo += "MIN-ACCESS: " + m_Token + "\n";
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_OBJECT:
			{
				//Example:
				//    OBJECT atmObjectID

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_OBJECT_DEFINITION);
				m_pRuleList->AddRule (CRule::LITERAL_OBJECT);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_OBJECT_DEFINITION:
			{
				//Get token here
				NextTokenWS ();

				m_pLastFlatElement->m_ExtraInfo += "OBJECT: " + m_Token + "\n";
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_DEFVAL:
			{
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("DEFVAL"))
				{
					//'DEFVAL' keyword found.
					//Remove this rule and assume RULE_OT_DEFVAL_2 applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_DEFVAL_2);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: We assume that "DEFVAL" keyword is optional

				m_bTokenInvalid = false;			//re-use this token
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_DEFVAL_2:
			{
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_DEFVAL_MORE);
				m_pRuleList->AddRule (CRule::LITERAL_DEFVAL);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_DEFVAL_MORE:
			{
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("{"))
				{
					//'{' keyword found.
					//Remove this rule and assume RULE_SD_MORE_2 applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_DEFVAL_MORE_2);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("("))
				{
					//'(' keyword found.
					//Remove this rule and assume RULE_SD_MORE_3 applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_DEFVAL_MORE_3);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: We assume that rule {0} applies here

				m_bTokenInvalid = false;			//re-use this token
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_DEFVAL_MORE_2:
			{
				NextTokenWS ();
				m_pLastFlatElement->m_ExtraInfo += "DEFVAL: {";
				GetEnumString ();
				m_pLastFlatElement->m_ExtraInfo += m_Token + "\n";
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_DEFVAL_MORE_3:
			{
				NextTokenWS ();
				m_pLastFlatElement->m_ExtraInfo += "DEFVAL: (";
				GetRangeString ();
				m_pLastFlatElement->m_ExtraInfo += m_Token + "\n";
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_AUGMENTS:
			{
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("AUGMENTS"))
				{
					//'AUGMENTS' keyword found.
					//Remove this rule and assume RULE_OT_AUGMENTS_2 applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_AUGMENTS_2);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: We assume that "AUGMENTS" keyword is optional

				m_bTokenInvalid = false;			//re-use this token
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_AUGMENTS_2:
			{
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_AUGMENTS_MORE);
				m_pRuleList->AddRule (CRule::LITERAL_AUGMENTS);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_AUGMENTS_MORE:
			{
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("{"))
				{
					//'{' keyword found.
					//Remove this rule and assume RULE_AUGMENTS_MORE_2 applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_AUGMENTS_MORE_2);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("("))
				{
					//'(' keyword found.
					//Remove this rule and assume RULE_AUGMENTS_MORE_3 applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_AUGMENTS_MORE_3);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: We assume that rule {0} applies here

				m_bTokenInvalid = false;			//re-use this token
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_AUGMENTS_MORE_2:
			{
				NextTokenWS ();
				m_pLastFlatElement->m_ExtraInfo += "AUGMENTS: {";
				GetEnumString ();
				m_pLastFlatElement->m_ExtraInfo += m_Token + "\n";
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_AUGMENTS_MORE_3:
			{
				NextTokenWS ();
				m_pLastFlatElement->m_ExtraInfo += "AUGMENTS: (";
				GetRangeString ();
				m_pLastFlatElement->m_ExtraInfo += m_Token + "\n";
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_SD_OCTET_STRING:
			{
				//This rule is only for setting the SYNTAX field of the element
				//to "OCTET STRING" type.
				m_pLastFlatElement->m_Syntax = "OCTET_STRING";
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_VARIABLES:
			{
				//028. RULE_OT_VARIABLES -> [RULE_OT_VARIABLES_2 | {0}]
				//Example:
				//	 -- Alarm Trap 
				//	 xldv20TrLos TRAP-TYPE
				//	    ENTERPRISE xldv20Traps 
				//	    VARIABLES { xldv20TvTimeAndDate,
				//			xldv20TvRepEntity,
				//			xldv20TvRepSource,
				//			xldv20TvAlmSpecProblem,
				//			xldv20TvAlmNatureOfAlarm,
				//			xldv20TvAlmFailedComponent,
				//			xldv20TvAlmSpecProblemText }
				//	    DESCRIPTION
				//		 " Loss of signal. "
				//	    ::= 6
				//Recognized keywords here are:
				//1. "VARIABLES"
				//2. Anything else (any keyword or OID)

				//Rules dispatch here.

				//Get token here
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("VARIABLES"))
				{
					//'VARIABLES' keyword found.
					//Remove this rule and assume RULE_OT_VARIABLES_2 applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_VARIABLES_2);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: We assume that "VARIABLES" keyword is optional

				m_bTokenInvalid = false;			//re-use this token
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_VARIABLES_2:
			{
				//029. RULE_OT_VARIABLES_2 -> "VARIABLES" RULE_VARIABLES_DEFINITION
				//Variables is either an object type name (one token) after which if:
				//there is no "{" then the definition goes to the end of line.
				//if there is a "{" then it is a table.
				//else, if the first keyword is "SEQUENCE" "OF" <ObjectID> then it is a table.

				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_VARIABLES_MORE);
				m_pRuleList->AddRule (CRule::LITERAL_VARIABLES);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_VARIABLES_MORE:
			{
				//031. RULE_VARIABLES_MORE -> [RULE_VARIABLES_MORE_2 | RULE_VARIABLES_MORE_3 | {0}]
				//
				//Recognized keywords here are
				//1. "{"
				//2. "("

				//Rules dispatch here.

				//Get token here
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("{"))
				{
					//'{' keyword found.
					//Remove this rule and assume RULE_VARIABLES_MORE_2 applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_VARIABLES_MORE_2);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("("))
				{
					//This should be considered erroneous, but we allow such a construct
					//to be parsed correctly
					//'(' keyword found.
					//Remove this rule and assume RULE_VARIABLES_MORE_3 applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_VARIABLES_MORE_3);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: We assume that rule {0} applies here

				m_bTokenInvalid = false;			//re-use this token
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_VARIABLES_MORE_2:
			{
				//032. RULE_VARIABLES_MORE_2 -> "{" RULE_VARIABLES_ENUM_DEF  "}"
				//Note: RULE_VARIABLES_ENUM_DEF is obsolete
				//Remember: watch out for opening/closing brackets
				NextTokenWS ();
				m_pLastFlatElement->m_ExtraInfo += "VARIABLES:\n{";
				GetEnumString ();
				m_pLastFlatElement->m_ExtraInfo += m_Token + "\n";

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_VARIABLES_MORE_3:
			{
				//032a. RULE_VARIABLES_MORE_3 -> "(" RULE_VARIABLES_RANGE ")"
				//Remember: watch out for opening/closing brackets
				NextTokenWS ();
				m_pLastFlatElement->m_ExtraInfo += "VARIABLES:\n(";
				GetRangeString ();
				m_pLastFlatElement->m_ExtraInfo += m_Token + "\n";

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_ENTERPRISE:
			{
				//Get token here
				NextTokenWS ();

				//TO DO: Perform checking if <ANY-OBJECT-TYPE> exists in the MIB or has been imported. This is not a critical error.
				m_pLastFlatElement->m_ExtraInfo = "ENTERPRISE: ";
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_ENTERPRISE_2);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_ENTERPRISE_2:
			{
				//Get token here
				NextTokenWS ();

				//TO DO: Perform checking if <ANY-OBJECT-TYPE> exists in the MIB or has been imported. This is not a critical error.
				m_pLastFlatElement->m_ExtraInfo += m_Token;
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_ENTERPRISE_MORE);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_ENTERPRISE_MORE:
			{
				//031. RULE_ENTERPRISE_MORE -> [RULE_ENTERPRISE_MORE_2 | RULE_ENTERPRISE_MORE_3 | {0}]
				//Recognized keywords here are
				//1. "{"
				//2. "("

				//Rules dispatch here.

				//Get token here
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("{"))
				{
					//'{' keyword found.
					//Remove this rule and assume RULE_ENTERPRISE_MORE_2 applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_ENTERPRISE_MORE_2);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("("))
				{
					//'(' keyword found.
					//Remove this rule and assume RULE_SD_MORE_3 applies here.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_ENTERPRISE_MORE_3);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: We assume that rule {0} applies here
				m_pLastFlatElement->m_ExtraInfo += "\n";
				m_bTokenInvalid = false;			//re-use this token
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_ENTERPRISE_MORE_2:
			{
				//032. RULE_ENTERPRISE_MORE_2 -> "{" RULE_ENTERPRISE_ENUM_DEF  "}"
				NextTokenWS ();
				m_pLastFlatElement->m_ExtraInfo += " {";
				GetEnumString ();
				m_pLastFlatElement->m_ExtraInfo += m_Token + "\n";

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_ENTERPRISE_MORE_3:
			{
				//032a. RULE_ENTERPRISE_MORE_3 -> "(" RULE_ENTERPRISE_RANGE ")"
				//Remember: watch out for opening/closing brackets
				NextTokenWS ();
				m_pLastFlatElement->m_ExtraInfo += " (";
				GetRangeString ();
				m_pLastFlatElement->m_ExtraInfo += m_Token + "\n";

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_TRAP_TYPE_DEF:
			{
				//083. RULE_TRAP_TYPE_DEF -> "TRAP-TYPE" RULE_OID_FIELDS
				NextSingleToken ();

				m_pLastFlatElement->m_nElementType = ETYPE_TRAP;
				m_pLastFlatElement->m_ObjectType = "TRAP-TYPE";
				m_pLastFlatElement->m_ParentOIDName = "TRAPS";
				m_pFlatElementList->LinkToParent (this, m_pLastFlatElement);
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OID_FIELDS);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_TRAP_NUMBER_SET:
			{
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_TRAP_NUMBER_SET_2);
				m_pRuleList->AddRule (CRule::LITERAL_EQUAL);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_TRAP_NUMBER_SET_2:
			{
				//In trap-type elements, their respective trap number is defined as:
				// ::= <number>

				NextSingleToken ();
				m_pLastFlatElement->m_ExtraInfo += "TRAP-NUMBER: " + m_Token + ".\n";
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_UNITS:
			{
				//UNITS "units string"
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_UNITS_DEFINITION);
				m_pRuleList->AddRule (CRule::LITERAL_UNITS);
			}
			break;
		case CRule::RULE_OT_UNITS_DEFINITION:
			{
				//Get token here
				NextSingleToken ();

				if (!m_Token.CompareNoCase ("\""))
				{
					//'"' keyword found.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_UNITS_TEXT);
					this->m_bTokenInvalid = false;  //re-use this token
					break;
				}
				bParseError = true;
				return bParseError;
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_UNITS_TEXT:
			{
				NextSingleToken ();
				GetDescriptionString ();
				m_pLastFlatElement->m_ExtraInfo += "UNITS: " + m_Token + "\n";
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_OBJECTS:
			{
				//OBJECTS {<object1> [, <object2> [...]]}
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_OBJECTS_MORE);
				m_pRuleList->AddRule (CRule::LITERAL_OBJECTS);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_OBJECTS_MORE:
			{
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("{"))
				{
					//'{' keyword found.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_OBJECTS_MORE_2);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("("))
				{
					//'(' keyword found.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_OBJECTS_MORE_3);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				m_bTokenInvalid = false;			//re-use this token
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_OBJECTS_MORE_2:
			{
				NextTokenWS ();
				m_pLastFlatElement->m_ExtraInfo += "OBJECTS: {";
				GetEnumString ();
				m_pLastFlatElement->m_ExtraInfo += m_Token + "\n";
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_OBJECTS_MORE_3:
			{
				NextTokenWS ();
				m_pLastFlatElement->m_ExtraInfo += "OBJECTS: (";
				GetRangeString ();
				m_pLastFlatElement->m_ExtraInfo += m_Token + "\n";
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_NOTIFICATIONS:
			{
				//OBJECTS {<object1> [, <object2> [...]]}
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_NOTIFICATIONS_MORE);
				m_pRuleList->AddRule (CRule::LITERAL_NOTIFICATIONS);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_NOTIFICATIONS_MORE:
			{
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("{"))
				{
					//'{' keyword found.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_NOTIFICATIONS_MORE_2);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("("))
				{
					//'(' keyword found.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_NOTIFICATIONS_MORE_3);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				m_bTokenInvalid = false;			//re-use this token
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_NOTIFICATIONS_MORE_2:
			{
				NextTokenWS ();
				m_pLastFlatElement->m_ExtraInfo += "NOTIFICATIONS: {";
				GetEnumString ();
				m_pLastFlatElement->m_ExtraInfo += m_Token + "\n";
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_NOTIFICATIONS_MORE_3:
			{
				NextTokenWS ();
				m_pLastFlatElement->m_ExtraInfo += "NOTIFICATIONS: (";
				GetRangeString ();
				m_pLastFlatElement->m_ExtraInfo += m_Token + "\n";
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_MODULE:
			{
				//MODULE [<MODULE-NAME> | {0}
				//Module name after "MODULE" keyword is optional
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_MODULE_MORE);
				m_pRuleList->AddRule (CRule::LITERAL_MODULE);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_MODULE_MORE:
			{
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("{"))
				{
					//'{' keyword found.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_MODULE_MORE_2);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("("))
				{
					//'(' keyword found.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_MODULE_MORE_3);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//The module name is optional
				m_pLastFlatElement->m_ExtraInfo += "MODULE: [this module]\n";
				m_bTokenInvalid = false;			//re-use this token
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_MODULE_MORE_2:
			{
				NextTokenWS ();
				m_pLastFlatElement->m_ExtraInfo += "MODULE: {";
				GetEnumString ();
				m_pLastFlatElement->m_ExtraInfo += m_Token + "\n";
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_MODULE_MORE_3:
			{
				NextTokenWS ();
				m_pLastFlatElement->m_ExtraInfo += "MODULE: (";
				GetRangeString ();
				m_pLastFlatElement->m_ExtraInfo += m_Token + "\n";
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_MANDATORY_GROUPS:
			{
				//MODULE [<MODULE-NAME> | {0}
				//Module name after "MODULE" keyword is optional
				m_pRuleList->RemoveHeadRule ();
				m_pRuleList->AddRule (CRule::RULE_OT_MANDATORY_GROUPS_MORE);
				m_pRuleList->AddRule (CRule::LITERAL_MANDATORY_GROUPS);
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_MANDATORY_GROUPS_MORE:
			{
				NextTokenWS ();

				if (!m_Token.CompareNoCase ("{"))
				{
					//'{' keyword found.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_MANDATORY_GROUPS_MORE_2);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				if (!m_Token.CompareNoCase ("("))
				{
					//'(' keyword found.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->AddRule (CRule::RULE_OT_MANDATORY_GROUPS_MORE_3);
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//The mandatory group names are optional
				m_pLastFlatElement->m_ExtraInfo += "MANDATORY-GROUPS: none\n";
				m_bTokenInvalid = false;			//re-use this token
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_MANDATORY_GROUPS_MORE_2:
			{
				NextTokenWS ();
				m_pLastFlatElement->m_ExtraInfo += "MANDATORY-GROUPS: {";
				GetEnumString ();
				m_pLastFlatElement->m_ExtraInfo += m_Token + "\n";
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::RULE_OT_MANDATORY_GROUPS_MORE_3:
			{
				NextTokenWS ();
				m_pLastFlatElement->m_ExtraInfo += "MANDATORY-GROUPS: (";
				GetRangeString ();
				m_pLastFlatElement->m_ExtraInfo += m_Token + "\n";
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		/////////// P L A C E H O L D E R   R U L E S ///////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::PLACEHOLDER_FOR_INDEX_FIELD_NAME:
			{
				//PLACEHOLDER_FOR_INDEX_FIELD_NAME


				//Get token here
				NextSingleToken ();
				m_pLastFlatElement->m_Index += m_Token;
				m_pIndexEntryList->AddIndexEntry (m_Token);

				//Modification as of 2001-11-09:
				//Since it has been reported that some index fields may be declared
				//without the following definition, we need to create a fake element
				//which will be eventually modified at a later stage.
				CMIBElement *pIndexElement;

				pIndexElement = new CMIBElement;

				pIndexElement->m_ElementString = m_Token;
				pIndexElement->m_pParentFile = this;
				pIndexElement->m_nElementType = ETYPE_TABLE_EXTERNAL_INDEX_FIELD;
				pIndexElement->m_ParentOIDName = m_pLastFlatElement->m_ElementString;
				pIndexElement->m_ElementValue = "EXT";

				m_pFlatElementList->AddTail (pIndexElement);
				m_pFlatElementList->LinkToParent (this, pIndexElement);

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::PLACEHOLDER_FOR_TABLE_OBJECT_IDENTIFIER:
			{
				//065. RULE_SYNTAX_SEQUENCE_2 -> <TABLE-OBJECT-IDENTIFIER> {0}
				//EXAMPLE:
				//ipAddrTable OBJECT-TYPE
				//    SYNTAX  SEQUENCE OF IpAddrEntry
				//    ACCESS  not-accessible
				//    STATUS  mandatory
				//    DESCRIPTION
				//            "The table of addressing information relevant to
				//            this entity's IP addresses."
				//    ::= { ip 20 }
				//At this point we know that the element currently being defined
				//is a table. We may prepare it for accepting
				//table entries.
				//The type of SEQUENCE OF object doesn't matter.
				//We just skip it. It will be defined otherwise.

				//Get token here
				NextSingleToken ();
				//TO DO: Check if this works correctly
				m_pLastFlatElement->m_ExtraInfo += "Table of: " + m_Token + ".\n";

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::PLACEHOLDER_FOR_ANY_OBJECT_TYPE:
			{
				//026a. RULE_OBJECT_TYPE_ID -> <ANY-OBJECT-TYPE>
				//Example:
				//sysDescr OBJECT-TYPE
				//    SYNTAX  DisplayString (SIZE (0..255))
				//
				// OBJECT-TYPE here is the name of an element
				// that is being imported from RFC-1212
				//
				//Use this rule to mark that the object is of <ANY-OBJECT-TYPE> type.
				//This doesn't affect element handling in any way.

				//Get token here
				NextSingleToken ();

				m_pLastFlatElement->m_ObjectType = m_Token;

				if (m_pFlatElementList->FindElement (m_Token))
				{
					//The element type has been either defined in the file
					//or it is being imported from other MIBs. This is the proper situation.
					//Nothing to do for the moment.
				}
				else
				{
					//The element type could not be found in the file.
					//It is neither defined in the file, nor it is being imported.
					//This is wrong.
					//TO DO: Provide some more detailed information
					//Definition error: <m_Token> is not a defined object type.
					bParseError = true;
					return bParseError;
				}

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::PLACEHOLDER_FOR_MIB_NAME:
			{
				//001. RULE_MIB -> <MIB-NAME> RULE_MIB_2
				CMIBElement *pMIBElement;

				//Get token here
				NextSingleToken ();

				//Based on this token an element in the flat list should be created
				pMIBElement = new CMIBElement;
				pMIBElement->m_ElementString = m_Token;
				m_ModuleName = m_Token;	//Remember the module name
				pMIBElement->m_nElementType = ETYPE_MIB_ROOT;
				pMIBElement->m_pParentFile = this;
				
				m_pFlatElementList->AddTail (pMIBElement);
				m_pTreeElementList->AddTail (pMIBElement);
				m_pLastFlatElement = pMIBElement;


				//Remove this rule from the list
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::PLACEHOLDER_FOR_IMPORT_NAME:
			{
				//In the below example "NetworkAddress" is the import name.
				//IMPORTS
				//		mgmt, NetworkAddress, IpAddress, Counter, Gauge,
				//		TimeTicks
				//			FROM RFC1155-SMI
				CMIBElement *pMIBElement;

				//Get token here
				NextSingleToken ();

				//Based on this token an element in the imports list should be created
				pMIBElement = new CMIBElement;
				pMIBElement->m_ElementString = m_Token;
				pMIBElement->m_nElementType = ETYPE_IMPORTED_ELEMENT;
				pMIBElement->m_pParentFile = this;
				
				m_pImportElementList->AddHead (pMIBElement);
				m_pFlatElementList->AddTail (pMIBElement);	//Required for object searching


				//Remove this rule from the list
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::PLACEHOLDER_FOR_MIB_MODULE_NAME:
			{
				//012. RULE_IMPORT_7 -> <MIB-MODULE-NAME> RULE_IMPORT_8     //Imported MIB module name
				//In the below example "RFC1155-SMI" is the MIB module name.
				//IMPORTS
				//		mgmt, NetworkAddress, IpAddress, Counter, Gauge,
				//		TimeTicks
				//			FROM RFC1155-SMI

				
				//Here we create a new element, attach the m_pImportElementList to it,
				//and add the new element to the m_pFlatElementList
				CMIBElement *pMIBElement;

				//Get token here
				NextSingleToken ();

				//Create new element
				pMIBElement = new CMIBElement;
				pMIBElement->m_ElementString = m_Token;
				pMIBElement->m_nElementType = ETYPE_IMPORT_MODULE;
				pMIBElement->m_ParentOIDName = "IMPORTS";
				pMIBElement->m_pParentFile = this;

				//Attach the m_pImportElementList to this element
				pMIBElement->AttachList (m_pImportElementList);
				//Make sure no more elements can be added to the list
				m_pImportElementList = NULL;
				
				//Attach this element to the flat list
				m_pFlatElementList->AddTail (pMIBElement);
				m_pLastFlatElement = pMIBElement;

				//And finally link this element to the "IMPORTS" element
				m_pFlatElementList->LinkToParent (this, pMIBElement);

				//Remove this rule from the list
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::PLACEHOLDER_FOR_OID_VALUE:
			{
				//017. RULE_OBJECT -> <OidValue> [RULE_OBJECT_2]
				//Here we create a new element and add it to the FlatElementList
				CMIBElement *pMIBElement;

				//Get token here
				NextSingleToken ();

				//Create new element
				pMIBElement = new CMIBElement;
				pMIBElement->m_ElementString = m_Token;
				pMIBElement->m_pParentFile = this;
				//Type is not known yet, so we leave it as is.
				//This will be set later on when we know the element type.
				//pMIBElement->m_nElementType = ETYPE_IMPORT_MODULE;

				//Attach this element to the flat list
				m_pFlatElementList->AddTail (pMIBElement);
				m_pLastFlatElement = pMIBElement;

				//Remove this rule from the list
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::PLACEHOLDER_FOR_PARENT_OID_NAME:
			{
				//025. RULE_OID_4 -> <PARENT-OID-NAME> <PARENT-OID-VALUE>
				//What we need to do here is to attach the read token to
				//the m_pLastFlatElement, which will indicate its parent object.
				//TO DO: Implement searching for parent OID object and linking
				//       with m_pLastFlatElement

				assert (m_pLastFlatElement); //ensure we have the element existent

				NextSingleToken ();

				//Set the parent OID name in the element
				m_pLastFlatElement->m_ParentOIDName = m_Token;

				//Remove this rule from the list
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::PLACEHOLDER_FOR_PARENT_OID_VALUE:
			{
				//025. RULE_OID_4 -> <PARENT-OID-NAME> <PARENT-OID-VALUE>
				//What we need to do here is to attach the read token to
				//the m_pLastFlatElement, which will indicate its value.
				//TO DO: Implement searching for parent OID object and linking
				//       with m_pLastFlatElement

				assert (m_pLastFlatElement); //ensure we have the element existent

				NextSingleToken ();

				//Set the parent OID name in the element
				m_pLastFlatElement->m_ElementValue = m_Token;

				//IMPLEMENTATION:
				//We call a method of the m_pFlatElementList to attach the m_pLastFlatElement
				//to it together with searching for its parent object and linking to it.
				if (m_pFlatElementList->LinkToParent (this, m_pLastFlatElement))
				{
					//The element has been successfully stored in the flat list
					//and attached to its parent.
					if (m_pLastFlatElement->m_pParentElement)
					{
						//Check if parent is of ETYPE_TABLE_ROOT type.
						//If this is the case, this item must become ETYPE_TABLE_ITEM
						if (m_pLastFlatElement->m_pParentElement->m_nElementType == ETYPE_TABLE_ROOT)
						{
							m_pLastFlatElement->m_nElementType = ETYPE_TABLE_ITEM;
						}
					}
				}
				//Note - forward-declarations are allowed.

				//Remove this rule from the list
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		/////////// L I T E R A L S   R U L E S /////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_IMPORTS:
			{
				//Expecting "IMPORTS" literal.
				//NOTE: This keyword is optional
				CMIBElement *pMIBElement;

				//Get token here
				NextSingleToken ();
				if (m_Token.CompareNoCase ("IMPORTS"))
				{
					//IMPORTS keyword not found.
					//Remove this rule and the next (RULE_IMPORT_3) which does not appy in this case
					//This section does not exist.
					//This is not an error.
					m_pRuleList->RemoveHeadRule ();
					m_pRuleList->RemoveHeadRule ();
					m_bTokenInvalid = false;		//re-use this token
					break;
				}
				//Else: The IMPORTS section has been found.
				//Since IMPORTS section exists, we need to prepare a list header
				//for imported elements. This is the place to do so.
				m_pImportElementList = new CMIBElementList;

				//Create new element
				pMIBElement = new CMIBElement;
				pMIBElement->m_ElementString = m_Token;	//"IMPORTS"
				pMIBElement->m_nElementType = ETYPE_IMPORTS_SECTION;
				pMIBElement->m_ParentOIDName = m_ModuleName;
				pMIBElement->m_pParentFile = this;

				//Attach this element to the flat list
				m_pFlatElementList->AddTail (pMIBElement);
				m_pLastFlatElement = pMIBElement;
				
				//Add this element also to the TreeElementList, under Root element
				//This should work the same as:
				m_pFlatElementList->LinkToParent (this, pMIBElement);

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_TEXTUAL_CONVENTION:
			{
				//Expecting "TEXTUAL-CONVENTION" literal.

				//Get token here
				NextSingleToken ();

#ifdef _DEBUG
				if (m_Token.CompareNoCase ("TEXTUAL-CONVENTION"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "TEXTUAL-CONVENTION" keyword expected.
					bParseError = true;
					return bParseError;
				}
#endif //_DEBUG
				//Else: do nothing - the object is defined. Nothing changes

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_DEFINITIONS:
			{
				//Expecting "DEFINITIONS" literal.

				//Get token here
				NextSingleToken ();

#ifdef _DEBUG
				if (m_Token.CompareNoCase ("DEFINITIONS"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "DEFINITIONS" keyword expected.
					bParseError = true;
					return bParseError;
				}
#endif //_DEBUG
				//Else: do nothing - the object is defined. Nothing changes

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_INDEX:
			{
				//Expecting "INDEX" literal.

				//Get token here
				NextSingleToken ();

#ifdef _DEBUG
				if (m_Token.CompareNoCase ("INDEX"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "INDEX" keyword expected.
					bParseError = true;
					return bParseError;
				}
#endif //_DEBUG
				//Else: do nothing - the object is defined. Nothing changes

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_SEQUENCE:
			{
				//Expecting "SEQUENCE" literal.

				//Get token here
				NextSingleToken ();

				if (m_Token.CompareNoCase ("SEQUENCE"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "SEQUENCE" keyword expected.
					bParseError = true;
					return bParseError;
				}
				//Else: do nothing - the object is defined. Nothing changes

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_OF:
			{
				//Expecting "OF" literal.

				//Get token here
				NextSingleToken ();

				if (m_Token.CompareNoCase ("OF"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "OF" keyword expected.
					bParseError = true;
					return bParseError;
				}
				//Else: do nothing - the object is defined. Nothing changes

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_OBJECT:
			{
				//Expecting "OBJECT" literal.

				//Get token here
				NextSingleToken ();

				if (m_Token.CompareNoCase ("OBJECT"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "OBJECT" keyword expected.
					bParseError = true;
					return bParseError;
				}
				//Else: do nothing - the object is defined. Nothing changes

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_IDENTIFIER:
			{
				//Expecting "IDENTIFIER" literal.

				//Get token here
				NextSingleToken ();

				if (m_Token.CompareNoCase ("IDENTIFIER"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "IDENTIFIER" keyword expected.
					bParseError = true;
					return bParseError;
				}
				//Else: do nothing - the object is defined. Nothing changes

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_EQUAL:
			{
				//Expecting "::=" literal.

				//Get token here
				NextTokenWS ();

				if (m_Token.CompareNoCase ("::="))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "::=" expected.
					bParseError = true;
					return bParseError;
				}
				//Else: do nothing - the object is defined. Nothing changes

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_OPEN_CURLY_BRACKET:
			{
				//Expecting "{" literal.

				//Get token here
				NextTokenWS ();

				if (m_Token.CompareNoCase ("{"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "{" expected.
					bParseError = true;
					return bParseError;
				}
				//Else: do nothing - the object is defined. Nothing changes

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_CLOSE_CURLY_BRACKET:
			{
				//Expecting "}" literal.

				//Get token here
				NextTokenWS ();

#ifdef _DEBUG
				if (m_Token.CompareNoCase ("}"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "}" expected.
					bParseError = true;
					return bParseError;
				}
#endif //_DEBUG
				//Else: do nothing - the object is defined. Nothing changes

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_BEGIN:
			{
				//Expecting "BEGIN" literal.

				//Get token here
				NextSingleToken ();

				if (m_Token.CompareNoCase ("BEGIN"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "BEGIN" keyword expected.
					bParseError = true;
					return bParseError;
				}
				//Else: do nothing - the object is defined. Nothing changes

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_END:
			{
				//Expecting "END" literal.

				//Get token here
				NextSingleToken ();

				if (m_Token.CompareNoCase ("END"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "END" keyword expected.
					bParseError = true;
					return bParseError;
				}
				//Else: do nothing - the object is defined. Nothing changes

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_COMMA:
			{
				//Expecting "," literal.

				//Get token here
				NextTokenWS ();

				if (m_Token.CompareNoCase (","))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "," expected.
					bParseError = true;
					return bParseError;
				}
				//Else: do nothing - the object is defined. Nothing changes

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_SEMICOLON:
			{
				//Expecting ";" literal.

				//Get token here
				NextTokenWS ();

				if (m_Token.CompareNoCase (";"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: ";" expected.
					bParseError = true;
					return bParseError;
				}
				//Else: do nothing - the object is defined. Nothing changes

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_FROM:
			{
				//Expecting "FROM" literal.

				//Get token here
				NextSingleToken ();

				if (m_Token.CompareNoCase ("FROM"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "FROM" keyword expected.
					bParseError = true;
					return bParseError;
				}
				//Else: do nothing - the object is defined. Nothing changes

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_OCTET:
			{
				//Expecting "OCTET" literal.

				//Get token here
				NextSingleToken ();

				if (m_Token.CompareNoCase ("OCTET"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "OCTET" keyword expected.
					bParseError = true;
					return bParseError;
				}
				//Else: do nothing - the object is defined. Nothing changes

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_INTEGER:
			{
				//Expecting "INTEGER" literal.

				//Get token here
				NextSingleToken ();

#ifdef _DEBUG
				if (m_Token.CompareNoCase ("INTEGER"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "INTEGER" keyword expected.
					bParseError = true;
					return bParseError;
				}
#endif //_DEBUG
				//Else: do nothing - the object is defined. Nothing changes

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_SYNTAX:
			{
				//Expecting "SYNTAX" literal.

				//Get token here
				NextSingleToken ();

#ifdef _DEBUG
				if (m_Token.CompareNoCase ("SYNTAX"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "SYNTAX" keyword expected.
					bParseError = true;
					return bParseError;
				}
#endif //_DEBUG
				//Else: do nothing - the object is defined. Nothing changes

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_ACCESS:
			{
				//Expecting "ACCESS" literal.

				//Get token here
				NextSingleToken ();

#ifdef _DEBUG
				if (m_Token.CompareNoCase ("ACCESS"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "ACCESS" keyword expected.
					bParseError = true;
					return bParseError;
				}
#endif //_DEBUG
				//Else: do nothing - the object is defined. Nothing changes

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_MAX_ACCESS:
			{
				//Expecting "MAX-ACCESS" literal.

				//Get token here
				NextSingleToken ();

#ifdef _DEBUG
				if (m_Token.CompareNoCase ("MAX-ACCESS"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "MAX-ACCESS" keyword expected.
					bParseError = true;
					return bParseError;
				}
#endif //_DEBUG
				//Else: do nothing - the object is defined. Nothing changes

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_STATUS:
			{
				//Expecting "STATUS" literal.

				//Get token here
				NextSingleToken ();

#ifdef _DEBUG
				if (m_Token.CompareNoCase ("STATUS"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "STATUS" keyword expected.
					bParseError = true;
					return bParseError;
				}
#endif //_DEBUG
				//Else: do nothing - the object is defined. Nothing changes

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_DESCRIPTION:
			{
				//Expecting "DESCRIPTION" literal.

				//Get token here
				NextSingleToken ();

#ifdef _DEBUG
				if (m_Token.CompareNoCase ("DESCRIPTION"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "DESCRIPTION" keyword expected.
					bParseError = true;
					return bParseError;
				}
#endif //_DEBUG
				//Else: do nothing - the object is defined. Nothing changes

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_LAST_UPDATED:
			{
				//Expecting "LAST-UPDATED" literal.

				//Get token here
				NextSingleToken ();

				if (m_Token.CompareNoCase ("LAST-UPDATED"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "LAST-UPDATED" keyword expected.
					bParseError = true;
					return bParseError;
				}
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_STRING:
			{
				//Expecting "STRING" literal.
				//If this literal is found, it is assumed it comes from 
				//the OCTET STRING sequence.
				//In such a case it indicates the m_pLastFlatElement
				//is of TEXTUAL CONVENTIONS type.

				//Get token here
				NextSingleToken ();

				if (m_Token.CompareNoCase ("STRING"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "STRING keyword expected.
					bParseError = true;
					return bParseError;
				}
				//Else: The object is of OCTET STRING type.
				//Define it as a child of a TEXTUAL CONVENTIONS
				//element and link it to the element.
				//The just defined element is of OCTET STRING 
				//is kept in m_pLastFlatElement.
				m_pLastFlatElement->m_nElementType = ETYPE_TEXTUAL_CONV;
				m_pLastFlatElement->m_ParentOIDName = "TEXTUAL CONVENTIONS";
				m_pLastFlatElement->m_Syntax = "OCTET STRING";

				//Add this element also to the TreeElementList, under Root element
				m_pFlatElementList->LinkToParent (this, m_pLastFlatElement);

				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_STRING_2:
			{
				NextSingleToken ();
				if (m_Token.CompareNoCase ("STRING"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "STRING" keyword expected.
					bParseError = true;
					return bParseError;
				}
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_ORGANIZATION:
			{
				//Expecting "ORGANIZATION" literal.
				NextSingleToken ();
				if (m_Token.CompareNoCase ("ORGANIZATION"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "ORGANIZATION" keyword expected.
					bParseError = true;
					return bParseError;
				}
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_CONTACT_INFO:
			{
				NextSingleToken ();
				if (m_Token.CompareNoCase ("CONTACT-INFO"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "CONTACT-INFO" keyword expected.
					bParseError = true;
					return bParseError;
				}
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_DISPLAY_HINT:
			{
				NextSingleToken ();
				if (m_Token.CompareNoCase ("DISPLAY-HINT"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "DISPLAY_HINT" keyword expected.
					bParseError = true;
					return bParseError;
				}
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_REFERENCE:
			{
				NextSingleToken ();
				if (m_Token.CompareNoCase ("REFERENCE"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "REFERENCE" keyword expected.
					bParseError = true;
					return bParseError;
				}
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_DEFVAL:
			{
				NextSingleToken ();
				if (m_Token.CompareNoCase ("DEFVAL"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "DEFVAL" keyword expected.
					bParseError = true;
					return bParseError;
				}
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_AUGMENTS:
			{
				NextSingleToken ();
				if (m_Token.CompareNoCase ("AUGMENTS"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "AUGMENTS" keyword expected.
					bParseError = true;
					return bParseError;
				}
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_REVISION:
			{
				NextSingleToken ();
				if (m_Token.CompareNoCase ("REVISION"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "REVISION" keyword expected.
					bParseError = true;
					return bParseError;
				}
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_VARIABLES:
			{
				NextSingleToken ();
				if (m_Token.CompareNoCase ("VARIABLES"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "VARIABLES" keyword expected.
					bParseError = true;
					return bParseError;
				}
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_ENTERPRISE:
			{
				NextSingleToken ();
				if (m_Token.CompareNoCase ("ENTERPRISE"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "ENTERPRISE" keyword expected.
					bParseError = true;
					return bParseError;
				}
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_TRAP_TYPE:
			{
				NextSingleToken ();
				if (m_Token.CompareNoCase ("TRAP-TYPE"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "TRAP-TYPE" keyword expected.
					bParseError = true;
					return bParseError;
				}
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_UNITS:
			{
				NextSingleToken ();
				if (m_Token.CompareNoCase ("UNITS"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "UNITS" keyword expected.
					bParseError = true;
					return bParseError;
				}
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_OBJECTS:
			{
				NextSingleToken ();
				if (m_Token.CompareNoCase ("OBJECTS"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "OBJECTS" keyword expected.
					bParseError = true;
					return bParseError;
				}
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_MIN_ACCESS:
			{
				NextSingleToken ();
				if (m_Token.CompareNoCase ("MIN-ACCESS"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "MIN-ACCESS" keyword expected.
					bParseError = true;
					return bParseError;
				}
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_MODULE:
			{
				NextSingleToken ();
				if (m_Token.CompareNoCase ("MODULE"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "MODULE" keyword expected.
					bParseError = true;
					return bParseError;
				}
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_MANDATORY_GROUPS:
			{
				NextSingleToken ();
				if (m_Token.CompareNoCase ("MANDATORY-GROUPS"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "MANDATORY-GROUPS" keyword expected.
					bParseError = true;
					return bParseError;
				}
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case CRule::LITERAL_NOTIFICATIONS:
			{
				NextSingleToken ();
				if (m_Token.CompareNoCase ("NOTIFICATIONS"))
				{
					//TO DO: Provide some more detailed information
					//Syntax: "NOTIFICATIONS" keyword expected.
					bParseError = true;
					return bParseError;
				}
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		default:
			{
				m_pRuleList->RemoveHeadRule ();
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		}
	}

	return bParseError;
} //bool CMIBFile::Parse (void)






/////////////////////////////////////////////////////////////////////////////////
/// void CMIBFile::NextSingleToken ()
/////////////////////////////////////////////////////////////////////////////////
void CMIBFile::NextSingleToken ()
{
	if (m_bTokenInvalid)
	{
		m_Token = GetFileSingleToken ();
	}
	m_bTokenInvalid = true; //Ensure the token checking routines will fetch next token unless this value is manually set to false.
							//For proper buffered token handling, use the following construct
							//if (m_bTokenInvalid) Token = GetFileSingleToken ().
}


/////////////////////////////////////////////////////////////////////////////////
/// void CMIBFile::NextTokenWS ()
/////////////////////////////////////////////////////////////////////////////////
void CMIBFile::NextTokenWS ()
{
	if (m_bTokenInvalid)
	{
		m_Token = GetFileTokenWS ();
	}
	m_bTokenInvalid = true; //Ensure the token checking routines will fetch next token unless this value is manually set to false.
							//For proper buffered token handling, use the following construct
							//if (m_bTokenInvalid) Token = GetFileSingleToken ().
}



/////////////////////////////////////////////////////////////////////////////////
/// CString CMIBFile::GetFileSingleToken ()
/////////////////////////////////////////////////////////////////////////////////
CString CMIBFile::GetFileSingleToken ()
{
	CString pToken;
	char cTokenChar;
	bool bIsOID;

	cTokenChar = GetFirstNonWhiteSpace ();

	pToken.Insert (STR_APPEND, cTokenChar);

	//If it is a delimiter, it means the keyword is a delimiter
	if (IsDelimiter (cTokenChar)) return pToken;

	bIsOID = (cTokenChar != ':');

	for (;;)
	{
		cTokenChar = ReadFileByte ();
		if (!m_bLastByteOK)
		{
			//Add any special processing for end-of-file.
			break;
		}
		if (IsWhiteSpace (cTokenChar))
		{
			//Add any special processing at the end of the token string.
			break;
		}

		if (IsDelimiter (cTokenChar))
		{
			//Add any special processing on detection of delimiter char
			UnReadFileByte (cTokenChar);
			break;
		}

		//Modification date: 2001-10-23
		if (bIsOID)
		{
			if (cTokenChar == ':')
			{
				UnReadFileByte (cTokenChar);
				break;
			}
		}
		//Add any special character recognition that should break the OID recognition below.

		pToken.Insert (STR_APPEND, cTokenChar);
	}

	return pToken;
}



/////////////////////////////////////////////////////////////////////////////////
/// CString CMIBFile::GetFileTokenWS ()
/////////////////////////////////////////////////////////////////////////////////
CString CMIBFile::GetFileTokenWS ()
{
	CString pToken;
	char cTokenChar;
	bool bIsOID;

	cTokenChar = GetFirstNonWhiteSpace ();

	pToken.Insert (STR_APPEND, cTokenChar);

	//If it is a delimiter, it means the keyword is a delimiter
	if (IsDelimiter (cTokenChar)) return pToken;

	bIsOID = (cTokenChar != ':');

	for (;;)
	{
		cTokenChar = ReadFileByte ();
		if (!m_bLastByteOK)
		{
			//Add any special processing for end-of-file.
			break;
		}
		if (IsWhiteSpace (cTokenChar))
		{
			//Add any special processing at the end of the token string.
			break;
		}

		//Modification date: 2001-10-23
		if (bIsOID)
		{
			if (cTokenChar == ':')
			{
				UnReadFileByte (cTokenChar);
				break;
			}
		}
		//Add any special character recognition that should break the OID recognition below.

		pToken.Insert (STR_APPEND, cTokenChar);
	}

	return pToken;
}



/////////////////////////////////////////////////////////////////////////////////
/// GetFirstNonWhiteSpace ()
/////////////////////////////////////////////////////////////////////////////////
char CMIBFile::GetFirstNonWhiteSpace ()
{
	char cRetVal;

	while (m_bLastByteOK)
	{
		cRetVal = ReadFileByte ();
		if (!IsWhiteSpace (cRetVal))
		{
			//Check if this is a comment
			if (cRetVal == '-')
			{
				//This is MOST PROBABLY a comment.
				//We assume here that no keyword or OID may begin with minus ('-') character.
				//If this is not the case, we need to rewrite this routine.
				cRetVal = ReadFileByte ();
				
				if (!m_bLastByteOK)
				{
					//This is an erroneous situation.
					//The last byte in file was a single minus character then EOF was reached.
					//TO DO: Provide more detailed error information to the user.
					return cRetVal;
				}

				if (cRetVal == '-')
				{
					//This is a comment.
					//Skip all characters to the end of line.
					while (m_bLastByteOK)
					{
						cRetVal = ReadFileByte ();
						if ((cRetVal == 13) || (cRetVal == 10))
						{
							//End of line means: end-of-comment.
							break;
						}
					}
				}
				else
				{
					//This is an errorneous situation.
					//A minus ('-') character was encountered alone. This is not valid.
					//TO DO: Provide more detailed error information to the user.
				}


			}
			else
			{
				return cRetVal;
			}
		}

	}

	return cRetVal;
}


/////////////////////////////////////////////////////////////////////////////////
/// void CMIBFile::GetRangeString ()
/////////////////////////////////////////////////////////////////////////////////
void CMIBFile::GetRangeString ()
{
	CString RangeString;
	char cFileChar;
	int nOpenBracketCount;

	//We assume that we are already within the range string,
	//which means we just passed one opening bracket.
	//The string ends when nOpenBracketCount == 0
	nOpenBracketCount = 1;

	while (m_bLastByteOK && (nOpenBracketCount > 0))
	{
		cFileChar = ReadFileByte ();
		if (cFileChar == ')')
		{
			RangeString += cFileChar;
			nOpenBracketCount--;
			continue;
		}
		if (cFileChar == '(')
		{
			RangeString += cFileChar;
			nOpenBracketCount++;
			continue;
		}
		if (cFileChar == '-')
		{
			//might be a comment
			//Put this back to file...
			UnReadFileByte (cFileChar);
			//and get first character that is neither a white space nor a comment...
			cFileChar = GetFirstNonWhiteSpace ();
			//and again put this character back to file for analysis 
			//in the next pass of this loop
			UnReadFileByte (cFileChar);
			continue;
		}
		if (IsWhiteSpace (cFileChar))
		{
			//We do not want more than one whitespace character in a row,
			//so we skip all the next whitespaces up to the next non-whitespace.
			RangeString += ' ';
			cFileChar = GetFirstNonWhiteSpace ();
			UnReadFileByte (cFileChar);
			continue;
		}
		//Here only those characters pass, that are valid range description characters.
		//Add them to the string
		RangeString += cFileChar;
	}

	m_Token = RangeString;
}




/////////////////////////////////////////////////////////////////////////////////
/// void CMIBFile::GetEnumString ()
/////////////////////////////////////////////////////////////////////////////////
void CMIBFile::GetEnumString ()
{
	CString EnumString;
	char cFileChar;
	int nOpenBracketCount;

	//We assume that we are already within the enum string,
	//which means we just passed one opening bracket.
	//The string ends when nOpenBracketCount == 0
	nOpenBracketCount = 1;

	while (m_bLastByteOK && (nOpenBracketCount > 0))
	{
		cFileChar = ReadFileByte ();
		if (cFileChar == '}')
		{
			EnumString += cFileChar;
			nOpenBracketCount--;
			continue;
		}
		if (cFileChar == '{')
		{
			EnumString += cFileChar;
			nOpenBracketCount++;
			continue;
		}
		if (cFileChar == '-')
		{
			//might be a comment
			//Put this back to file...
			UnReadFileByte (cFileChar);
			//and get first character that is neither a white space nor a comment...
			cFileChar = GetFirstNonWhiteSpace ();
			//and again put this character back to file for analysis 
			//in the next pass of this loop
			UnReadFileByte (cFileChar);
			continue;
		}
		if (IsWhiteSpace (cFileChar))
		{
			//We do not want more than one whitespace character in a row,
			//so we skip all the next whitespaces up to the next non-whitespace.
			EnumString += ' ';
			cFileChar = GetFirstNonWhiteSpace ();
			UnReadFileByte (cFileChar);
			continue;
		}
		//Here only those characters pass, that are valid enum description characters.
		//Add them to the string
		EnumString += cFileChar;
	}

	m_Token = EnumString;
}




/////////////////////////////////////////////////////////////////////////////////
/// void CMIBFile::GetDescriptionString ()
/////////////////////////////////////////////////////////////////////////////////
void CMIBFile::GetDescriptionString ()
{
	CString DescriptionString;
	char cFileChar;

	//We assume that we are already within the description string,
	//which means we just passed one opening quote.
	//The string ends when next quote is encountered.
	//It is assumed no comments are interpreted within the description;
	//that means the whole string in quotes is a description.

	while (m_bLastByteOK)
	{
		cFileChar = ReadFileByte ();
		if (cFileChar == '\"')
		{
			//This is the end of the description string.
			break;
		}
		if (IsWhiteSpace (cFileChar))
		{
			//We do not want more than one whitespace character in a row,
			//so we skip all the next whitespaces up to the next non-whitespace.
			DescriptionString += ' ';
			cFileChar = GetFirstNonWhiteSpace ();
			UnReadFileByte (cFileChar);
			continue;
		}
		//Here only those characters pass, that are valid range description characters.
		//Add them to the string
		DescriptionString += cFileChar;
	}

	m_Token = DescriptionString;
}



/////////////////////////////////////////////////////////////////////////////////
/// void CMIBFile::SkipCharactersUpToCloseCurlyBracket ()
/////////////////////////////////////////////////////////////////////////////////
void CMIBFile::SkipCharactersUpToCloseCurlyBracket ()
{
	char cFileChar;

	while (m_bLastByteOK)
	{
		cFileChar = ReadFileByte ();
		if (cFileChar == '}')
		{
			//This is what we are looking for
			UnReadFileByte (cFileChar);
			break;
		}
		if (IsWhiteSpace (cFileChar))
		{
			//This is a trick to detect and skip comments.
			cFileChar = GetFirstNonWhiteSpace ();
			UnReadFileByte (cFileChar);
			continue;
		}
	}
}



/////////////////////////////////////////////////////////////////////////////////
/// bool CMIBFile::IsWhiteSpace (const char cChar)
/////////////////////////////////////////////////////////////////////////////////
bool CMIBFile::IsWhiteSpace (const char cChar)
{
	const char acWS [] = {13,
		10,
		' ',
		'\t'
	};
	int i;

	//Iterate through the array of recognized whitespaces
	for (i = 0; i < 4; i++)
	{
		if (cChar == acWS[i])
			return true;
	}
	return false;
}



/////////////////////////////////////////////////////////////////////////////////
/// bool CMIBFile::IsWhiteSpace (const char cChar)
/////////////////////////////////////////////////////////////////////////////////
bool CMIBFile::IsDelimiter (const char cChar)
{
	const char acDelim [] = {/*':',*/	//We exclude ':' from the delimiters
		';',
		',',
		'\"',
		'{',
		'}',
		'(',
		')'
	};
	int i;

	//Iterate through the array of recognized delimiters
	for (i = 0; i < 7; i++)
	{
		if (cChar == acDelim[i])
			return true;
	}
	return false;
}



/////////////////////////////////////////////////////////////////////////////////
/// char CMIBFile::ReadFileByte ()
/////////////////////////////////////////////////////////////////////////////////
char CMIBFile::ReadFileByte ()
{
	char cRetVal;

	//Handling of unread characters
	if (m_bUnreadChar)
	{
		m_bUnreadChar = false;
		cRetVal = m_cUnreadChar;
		return cRetVal;
	}

	//Regular ReadFileByte () routine. Cut off the above code
	//to get rid of 'unreading' capability.

	if (!m_bBufferValid)
	{
		m_dwLastValidByte = Read (m_acIOBuffer, MIB_FILE_BUFFER_SIZE) - 1;

		//if m_dwLastValidByte == -1 (0 file bytes minus 1), this means 
		//that end of file has been reached.
		m_dwIOBufferPos = 0;
		if (m_dwLastValidByte == -1)
		{
			m_bLastByteOK = false;	//operation was not successful
			return -1;
		}
		m_bBufferValid = true;
	}
	//At this point buffer contents is valid, and m_dwIOBufferPos points
	//to the first byte waiting to be read
	cRetVal = m_acIOBuffer[m_dwIOBufferPos++];

	//Now let's perform post-read buffer validation (preparation for the next read.
	if (m_dwIOBufferPos > m_dwLastValidByte)
	{
		//Are we beyond the end of buffer?
		m_bBufferValid = false;			//Invalidate buffer
	}
	m_bLastByteOK = true;

	return cRetVal;
}



/////////////////////////////////////////////////////////////////////////////////
/// bool CMIBFile::UnReadFileByte (char cChar)
/////////////////////////////////////////////////////////////////////////////////
bool CMIBFile::UnReadFileByte (char cChar)
{
	if (m_bUnreadChar)
	{
		return false;
	}
	
	m_bUnreadChar = true;
	m_cUnreadChar = cChar;
	return true;
}



/////////////////////////////////////////////////////////////////////////////////
/// void CMIBFile::ValidateIndiceEntries ()
/////////////////////////////////////////////////////////////////////////////////
void CMIBFile::ValidateIndiceEntries ()
{
	CString *pIndexEntry;
	CMIBElement *pElement;
	POSITION pos;

	pos = m_pIndexEntryList->GetHeadPosition ();

	while (pos)
	{
		pIndexEntry = m_pIndexEntryList->GetNextEntry (pos);
		if (pIndexEntry->Compare (""))
		{
			//Find element with such a name and mark it as ETYPE_TABLE_INDEX_FIELD
			if (pElement = m_pFlatElementList->FindElement (*pIndexEntry))
			{
				//Element has been found.
				//If its parent is of ETYPE_TABLE_ITEM type
				//we can set this element type.
				if (pElement->m_pParentElement)
				{
					if (pElement->m_pParentElement->m_nElementType == ETYPE_TABLE_ITEM)
					{
						pElement->m_nElementType = ETYPE_TABLE_INDEX_FIELD;
					}
				}
			}
		}
	}
}



/////////////////////////////////////////////////////////////////////////////////
/// void CMIBFile::ReleaseAllFlatElements ()
/////////////////////////////////////////////////////////////////////////////////
void CMIBFile::ReleaseAllFlatElements ()
{
	CMIBElement *pElement;
	POSITION pos;

	pos = m_pFlatElementList->GetHeadPosition ();

	while (pos)
	{
		pElement = m_pFlatElementList->GetNextElement (pos);
		delete pElement;
	}
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//List element handling methods
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////
/// CMIBElement::CMIBElement ()
/////////////////////////////////////////////////////////////////////////////////
CMIBElement::CMIBElement ()
{
	m_nElementType = ETYPE_OID;
	m_ElementString = "";
	m_ElementValue = "";
	m_ParentOIDName = "";
	m_pSubobjectList = NULL;
	m_pParentElement = NULL;
	m_pParentFile = NULL;
}


/////////////////////////////////////////////////////////////////////////////////
/// CMIBElement::~CMIBElement ()
/////////////////////////////////////////////////////////////////////////////////
CMIBElement::~CMIBElement ()
{
	//Cleanup the subobject list
	if (m_pSubobjectList)
	{
		delete m_pSubobjectList;
	}
}



/////////////////////////////////////////////////////////////////////////////////
/// void CMIBElement::AttachList (CMIBElementList *pSubobjectList)
/////////////////////////////////////////////////////////////////////////////////
void CMIBElement::AttachList (CMIBElementList *pSubobjectList)
{
	m_pSubobjectList = pSubobjectList;
}



/////////////////////////////////////////////////////////////////////////////////
/// void CMIBElement::AttachElement (CMIBElement *pSubElement)
/////////////////////////////////////////////////////////////////////////////////
void CMIBElement::AttachElement (CMIBElement *pSubElement)
{
	if (!m_pSubobjectList)
	{
		//List does not exist yet
		m_pSubobjectList = new CMIBElementList;
	}

	//Modification date: 2001-10-19
	//Modification description:
	//Added checking for existence of the pSubElement in the list of children objects
	//to avoid duplication in case an element is added to its parent twice.
	//This especially applies to forward-declarated parents
	if (m_pSubobjectList->FindElement (pSubElement->m_ElementString))
	{
		//The element already exists in the children list.
		return;
	}

	m_pSubobjectList->AddTail (pSubElement);
	pSubElement->m_pParentElement = this;
}



/////////////////////////////////////////////////////////////////////////////////
/// CString CMIBElement::BuildFullElementValue ()
/////////////////////////////////////////////////////////////////////////////////
CString CMIBElement::BuildFullElementValue ()
{
	CString FullElementValue;

	FullElementValue = m_ElementValue;

	if (m_pParentElement)
	{
		FullElementValue = m_pParentElement->BuildFullElementValue () + "." + FullElementValue;
	}
	return FullElementValue;
}



/////////////////////////////////////////////////////////////////////////////////
/// CString CMIBElement::BuildFullElementValue ()
/////////////////////////////////////////////////////////////////////////////////
CString CMIBElement::BuildFullElementName ()
{
	CString FullElementName;

	if (m_nElementType == ETYPE_MIB_ROOT)
	{
		//For module file names do not return any name.
		return "";
	}

	FullElementName = m_ElementString;

	if (m_pParentElement)
	{
		FullElementName = m_pParentElement->BuildFullElementName () + "." + FullElementName;
	}
	return FullElementName;
}



///////////////////////////////////////////////////////////////////////////////
//Rule list handling methods
///////////////////////////////////////////////////////////////////////////////

CRule *CRuleList::GetHead ()
{
	return (CRule *)CPtrList::GetHead ();
}

void CRuleList::RemoveHeadRule (void)
{
	CRule *pRule;

	pRule = (CRule *)RemoveHead ();

	delete pRule;
}


CRule *CRuleList::GetTail ()
{
	return (CRule *)CPtrList::GetTail ();
}


/*
	Use this function to simplify new rule addition to the list.
	The new rule of type nRule is added to the head of the list.
*/
void CRuleList::AddRule (CRule::ERuleTypes nRule)
{
	CRule *pRule;

	pRule = new CRule;
	pRule->m_nRuleType = nRule;
	AddHead (pRule);
}



///////////////////////////////////////////////////////////////////////////////
//Element list handling methods
///////////////////////////////////////////////////////////////////////////////


CMIBElement *CMIBElementList::GetHead ()
{
	return (CMIBElement *)CPtrList::GetHead ();
}

CMIBElement *CMIBElementList::GetTail ()
{
	return (CMIBElement *)CPtrList::GetTail ();
}

CMIBElement *CMIBElementList::GetNextElement (POSITION &pos)
{
	return (CMIBElement *)CPtrList::GetNext (pos);
}



//Iterates through the list and verifies if all child objects are linked
//to their parents. If not, this method tries to re-link the element to its parent.
void CMIBElementList::ValidateLinks ()
{
	CMIBElement *pElement, *pParentElement;
	POSITION pos;

	pos = GetHeadPosition ();

	while (pos)
	{
		pElement = GetNextElement (pos);
		if (pElement->m_pParentElement)
		{
			//Nothing to do. Proceed with iteration.
			//The object is already linked.
		}
		else
		{
			//This element does not have a parent linked.
			//If a parent name is present, try to re-link.
			if (pElement->m_ParentOIDName != "")
			{
				pParentElement = FindElement (pElement->m_ParentOIDName);
				if (pParentElement)
				{
					pParentElement->AttachElement (pElement);
				}
				else
				{
					//The parent element has been forward-declared, but it
					//is nowhere defined in the file.
					//This is an erroneous situation.
					//Is it a well-known name that we do not know?
				}
			}
		}
	}
}


/*
	Adds element to the list (tree).
	If the parent is a well-known object and it is not in this list
	it will be created and attached to the list as well.
	This function returns false if the parent element is not
	a well-known object and it is not in the list.
	Otherwise this function returns true on successful linkage.

	Creation date: 2001-10-14
	Created by: Artur Perwenis

	Last modification date: 2001-11-09
	Last modified by: Artur Perwenis

	Modification history (most recent first):
------	
	Modification date: 2001-11-12
	Modification description:
		Modification to handle externally declared index fields.
		The inserted code is placed to handle specifically the external index fields only.

	Modification date: 2001-10-14
	Modification description:
		First test version.
------
*/
bool CMIBElementList::LinkToParent (CMIBFile *pMIBFile, CMIBElement *pChildElement)
{
	CMIBElement *pParentElement;
	CString ParentName;
	int i;
	char *lpszWellKnownNames [] = {"root",
		"ccitt",
		"iso",
		"joint-iso-ccitt",
		"org",
		"dod",
		"internet",
		"mgmt",
		"experimental",
		"private",
		"snmpv2",
		"mib",
		"enterprises",
		"sni",
		"siemensUnits",
		"oenProductMibs",
		"an",
		"xld",
		"onu",
		"TEXTUAL CONVENTIONS",
		"TRAPS",
		NULL
	};

	char *lpszWellKnownValues [] = {"0",
		"0",
		"1",
		"2",
		"1.3",
		"1.3.6",
		"1.3.6.1",
		"1.3.6.1.2",
		"1.3.6.1.3",
		"1.3.6.1.4",
		"1.3.6.1.6",
		"1.3.6.1.2.1",
		"1.3.6.1.4.1",
		"1.3.6.1.4.1.231",
		"1.3.6.1.4.1.231.7",
		"1.3.6.1.4.1.231.7.1",
		"1.3.6.1.4.1.231.7.1.2",
		"1.3.6.1.4.1.231.7.1.2.1",
		"1.3.6.1.4.1.231.7.1.2.1.1",
		"",	//TEXTUAL CONVENTIONS
		"", //TRAPS
		NULL
	};

	char *lpszWellKnownDisplayNames [] = {"root",
		"ccitt",
		"iso",
		"joint-iso-ccitt",
		"iso.org",
		"iso.org.dod",
		"iso.org.dod.internet",
		"iso.org.dod.internet.mgmt",
		"iso.org.dod.internet.experimental",
		"iso.org.dod.internet.private",
		"iso.org.dod.internet.snmpv2",
		"iso.org.dod.internet.mgmt.mib",
		"iso.org.dod.internet.private.enterprises",
		"iso.org.dod.internet.private.enterprises.sni",
		"iso.org.dod.internet.private.enterprises.sni.siemensUnits",
		"iso.org.dod.internet.private.enterprises.sni.siemensUnits.oenProductMibs",
		"iso.org.dod.internet.private.enterprises.sni.siemensUnits.oenProductMibs.an",
		"iso.org.dod.internet.private.enterprises.sni.siemensUnits.oenProductMibs.an.xld",
		"iso.org.dod.internet.private.enterprises.sni.siemensUnits.oenProductMibs.an.xld.onu",
		"TEXTUAL CONVENTIONS",
		"TRAPS",
		NULL
	};



	assert (pChildElement);

	ParentName = pChildElement->m_ParentOIDName;

	if (ParentName == "")
	{
		//This element has no parent defined
		//This is illegal.
		return false;
	}

	//Update as of 2001-11-12
	//UPDATE_BEGIN:
	//Implementation of handling externally defined index fields
	{
		CMIBElement *pExtIndexElement;
		
		pExtIndexElement = FindElementEx (pChildElement->m_ElementString, ParentName);
		if (pExtIndexElement)
		{
			//Clean-up a little by removing the first duplicate.
			if (pExtIndexElement != pChildElement)
			{
				RemoveElement (pExtIndexElement);
				if (pExtIndexElement->m_pParentElement)
				{
					pExtIndexElement->m_pParentElement->m_pSubobjectList->RemoveElement (pExtIndexElement);
				}
			}
		}
	}

	//UPDATE_END:

	pParentElement = FindElement (ParentName);

	//Check if the parent is ETYPE_IMPORTED_ELEMENT.
	//Note: This is due to the fact that well-known names take
	//precedence over imported elements, and it is known
	//that well-known names can also appear as imports in MIBs.
	if ((!pParentElement) || (pParentElement->m_nElementType == ETYPE_IMPORTED_ELEMENT))
	{
		//The parent hasn't been found in the list.
		//Check if it is one of the well-known OIDs.
		//If so, create an element for it.
		//ParentName is the name we look for.
		i = 0;
		while (lpszWellKnownNames[i])
		{
			if (ParentName.Compare (lpszWellKnownNames[i]))
			{
				//This is not the name.
				i++;
			}
			else
			{
				//Parent object name has been found
				break;
			}
		}

		if (lpszWellKnownNames[i])
		{
			pParentElement = new CMIBElement;
			pParentElement->m_ElementString = lpszWellKnownDisplayNames[i];
			pParentElement->m_ElementValue = lpszWellKnownValues[i];
			pParentElement->m_nElementType = ETYPE_MIB_ROOT_KEY;
			pParentElement->m_ParentOIDName = pMIBFile->m_ModuleName;
			pParentElement->m_pParentElement = NULL;
			pParentElement->m_pParentFile = pMIBFile;
			AddHead (pParentElement);
			//Every well-known name should be added to the MIB root element.
			LinkToParent (pMIBFile, pParentElement);
		}

	}

	if (!pParentElement)
	{
		//There is neither a parent element already in the list
		//not any well-known object does not match pChildElement's parent
		//this is not valid.
		return false;
	}

	//At this point we have the parent element, whose pointer is
	//in pParentElement. Link the pChildElement to pParentElement.
	pParentElement->AttachElement (pChildElement);
	return true;
}



//Iterates through the list to find the first occurence of
//an object, whose m_ElementString equals to ElementName
//Returns a pointer to such an object or NULL if no object
//could be found.
CMIBElement *CMIBElementList::FindElement (CString ElementName)
{
	CMIBElement *pElement;
	POSITION pos;

	pos = GetHeadPosition ();

	while (pos)
	{
		pElement = GetNextElement (pos);
		if (pElement->m_ElementString.Compare (ElementName))
		{
			//Nothing to do. Proceed with iteration
		}
		else
		{
			//This is the element!
			return pElement;
		}
	}

	return NULL;
}



//Iterates through the list to find the first occurence of
//an object, whose m_ElementString equals to ElementName and m_ParentOIDName
//equals to ParentName.
//Returns a pointer to such an object or NULL if no object
//could be found.
//Use this is the same child name can be used across different parents.
CMIBElement *CMIBElementList::FindElementEx (CString ElementName, CString ParentName)
{
	CMIBElement *pElement;
	POSITION pos;

	pos = GetHeadPosition ();

	while (pos)
	{
		pElement = GetNextElement (pos);
		if (pElement->m_ElementString.Compare (ElementName))
		{
			//Nothing to do. Proceed with iteration
		}
		else
		{
			//This is an element with the correct name.
			//Check if its parent matches our search.
			if (pElement->m_ParentOIDName.Compare (ParentName))
			{
				//Nothing to do. Proceed with iteration
			}
			else
			{
				//This is the element!
				return pElement;
			}
		}
	}

	return NULL;
}



//Deletes an element from the list. Does not free its memory.
void CMIBElementList::RemoveElement (CMIBElement *pElement)
{
	POSITION pos;

	pos = Find (pElement);
	if (pos)
	{
		RemoveAt (pos);
	}
}



///////////////////////////////////////////////////////////////////////////////
//File list handling methods
///////////////////////////////////////////////////////////////////////////////
CMIBFileList::~CMIBFileList ()
{
	CMIBFile *pFile;
	POSITION pos;

	pos = GetHeadPosition ();

	while (pos)
	{
		pFile = GetNextFile (pos);
		delete pFile;
	}
}

CMIBFile *CMIBFileList::GetHead ()
{
	return (CMIBFile *)CPtrList::GetHead ();
}

CMIBFile *CMIBFileList::GetTail ()
{
	return (CMIBFile *)CPtrList::GetTail ();
}

CMIBFile *CMIBFileList::GetNextFile (POSITION &pos)
{
	return (CMIBFile *)CPtrList::GetNext (pos);
}

CMIBFile *CMIBFileList::GetPrevFile (POSITION &pos)
{
	return (CMIBFile *)CPtrList::GetPrev (pos);
}



//Iterates through the list to find the first occurence of
//an object, whose m_hTreeItem equals to hTreeItem
//Returns a pointer to such an object or NULL if no object
//could be found.
CMIBFile *CMIBFileList::FindElement (HTREEITEM hTreeItem)
{
	CMIBFile *pFile;
	POSITION pos;

	pos = GetHeadPosition ();

	while (pos)
	{
		pFile = GetNextFile (pos);
		if (pFile->m_hTreeItem == hTreeItem)
		{
			//This is the element!
			return pFile;
		}
	}

	return NULL;
}


//Iterates through the list to find the first occurence of
//an object, whose m_FileName equals to FileName
//Returns a pointer to such an object or NULL if no object
//could be found.
CMIBFile *CMIBFileList::FindElement (CString FileName)
{
	CMIBFile *pFile;
	POSITION pos;

	pos = GetHeadPosition ();

	while (pos)
	{
		pFile = GetNextFile (pos);
		if (pFile->m_FileName.CompareNoCase (FileName))
		{
			//Do nothing. This is not the element.
		}
		else
		{
			//This is the element!
			return pFile;
		}
	}

	return NULL;
}




///////////////////////////////////////////////////////////////////////////////
//Index entry list handling methods
///////////////////////////////////////////////////////////////////////////////
CString *CIndexEntryList::GetHead ()
{
	return (CString *)CPtrList::GetHead ();
}

CString *CIndexEntryList::GetTail ()
{
	return (CString *)CPtrList::GetTail ();
}



//Adds specified index field name to the table of index entries 
//for later validation in ValidateIndiceEntries ().
void CIndexEntryList::AddIndexEntry (CString IndexEntry)
{
	CString *pIndexEntry;

	pIndexEntry = new CString;
	*pIndexEntry = IndexEntry;
	AddHead (pIndexEntry);
}

//Returns next index entry from the list.
CString *CIndexEntryList::GetNextEntry (POSITION &pos)
{
	return (CString *)CPtrList::GetNext (pos);
}



CIndexEntryList::~CIndexEntryList ()
{
	CString *pIndex;
	POSITION pos;

	pos = GetHeadPosition ();

	while (pos)
	{
		pIndex = GetNextEntry (pos);
		delete pIndex;
	}
}


