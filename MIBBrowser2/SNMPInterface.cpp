/**
*	@file		SNMPInterface.cpp
*	@brief		Implementation file of classes that work as interface between
*				the application and the SNMP subsystem.
*	@author		Artur Perwenis
*	@date		11-Jan-2002
*
*	@history
*	@item		11-Oct-2001		Artur Perwenis		First version.
*/

#include "SNMPInterface.h"
//Define this in Project settings.
#ifdef USE_NEW_SNMP_INTERFACE

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//                 Class implementation                                      
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
const char* const CSNMPInterface::SR_MGR_CONF_FILE_PREFIX = "/etc/snmp/mgr";
const char* const CSNMPInterface::FN_SNMPINFO_DAT = "snmpinfo2.dat";
const char* const CSNMPInterface::FN_MGR_FILE = "mgr.cnf";
const char* const CSNMPInterface::SNMP_RESEARCH_INC_SOFT_VERSION = "@(#)SNMP Research Distribution version 15.1.1.1";
const char* const CSNMPInterface::SNMP_RESEARCH_INC_COPYRIGHT = "@(#)Copyright 1992-1998 SNMP Research, Incorporated";
const char* const CSNMPInterface::SR_SNMP_ENV_NAME = "SR_SNMP_TEST_PORT";
const MIB_OID* const CSNMPInterface::m_pOriginalMIBOidTable = NULL;
const CString const CSNMPInterface::m_TypeName_NULL				= "Null";
const CString const CSNMPInterface::m_TypeName_NONLEAF			= "nonLeaf";
const CString const CSNMPInterface::m_TypeName_AGGREGATE		= "Aggregate";
const CString const CSNMPInterface::m_TypeName_DISPLAYSTRING	= "DisplayString";
const CString const CSNMPInterface::m_TypeName_OCTETSTRING		= "OctetString";
const CString const CSNMPInterface::m_TypeName_PHYSADDRESS		= "PhysAddress";
const CString const CSNMPInterface::m_TypeName_NETWORKADDRESS	= "NetworkAddress";
const CString const CSNMPInterface::m_TypeName_IPADDRESS		= "IpAddress";
const CString const CSNMPInterface::m_TypeName_OBJECTID			= "ObjectID";
const CString const CSNMPInterface::m_TypeName_TIMETICKS		= "TimeTicks";
const CString const CSNMPInterface::m_TypeName_INTEGER			= "INTEGER";
const CString const CSNMPInterface::m_TypeName_GAUGE			= "Gauge";
const CString const CSNMPInterface::m_TypeName_COUNTER			= "Counter";
const CString const CSNMPInterface::m_TypeName_SERVICES			= "Services";
const CString const CSNMPInterface::m_TypeName_COUNTER32		= "Counter32";
const CString const CSNMPInterface::m_TypeName_GAUGE32			= "Gauge32";
const CString const CSNMPInterface::m_TypeName_COUNTER64		= "Counter64";
const CString const CSNMPInterface::m_TypeName_INTEGER32		= "Integer32";
const CString const CSNMPInterface::m_TypeName_BITS				= "Bits";
const CString const CSNMPInterface::m_TypeName_UNSIGNED32		= "Unsigned32";
const CString const CSNMPInterface::m_AccessName_READONLY		= "read-only";
const CString const CSNMPInterface::m_AccessName_WRITEONLY		= "write-only";
const CString const CSNMPInterface::m_AccessName_READWRITE		= "read-write";
const CString const CSNMPInterface::m_AccessName_READCREATE		= "read-create";
const CString const CSNMPInterface::m_AccessName_NOTACCESSIBLE	= "not-accessible";
const CString const CSNMPInterface::m_AccessName_ACCESSIBLEFORNOTIFY	= "accessible-for-notify";



//-------------------------------------------------------------------------------------
//
// Default constructor
//
//-------------------------------------------------------------------------------------
CSNMPInterface::CSNMPInterface () :
	m_bInterfaceInitialized (false),
	m_bInitNewOIDRoutines (false),
	m_nUDPSocket (0),
	m_pHostEntry (NULL),
	util_local_snmpID (NULL),
	v2_defaultContext (NULL),
	m_pDefaultMIBTree (NULL),
	m_pMIBOidTable (NULL),
	m_nSeconds (0),
	m_nRetries (0),
	m_nTRetries (0),
	m_nLocalIPAddress (0)
{
	m_sIPAddress.Empty ();
	m_sCommunity.Empty ();
	m_eCommunicationType = ECommunicationType::SNMP_COMMUNICATIONS;
	m_eUtilVersion = ESNMPVersion::SR_SNMPv1_VERSION;
}


//-------------------------------------------------------------------------------------
//
// Default destructor
//
//-------------------------------------------------------------------------------------
CSNMPInterface::~CSNMPInterface ()
{
}


//-------------------------------------------------------------------------------------
//
// bool CSNMPInterface::InitializeInterface ()
//
//-------------------------------------------------------------------------------------
bool CSNMPInterface::InitializeInterface ()
{
	CString sEnvUtilVersion;
	bool bRetVal;

	bRetVal = true;

	m_eUtilVersion = SR_SNMPv1_VERSION;
	m_eCommunicationType = SNMP_COMMUNICATIONS;
	m_nSeconds = SNMP::SECS;
	m_nRetries = SNMP::RETRIES;
	m_nTRetries = SNMP::RETRIES;

	sEnvUtilVersion = getenv ("SR_UTIL_SNMP_VERSION");
	if (! sEnvUtilVersion.IsEmpty ())
	{
		if (sEnvUtilVersion == "-v1")
		{
			m_eUtilVersion = SR_SNMPv1_VERSION;
		}
		else if (sEnvUtilVersion == "-v2c")
		{
			m_eUtilVersion = SR_SNMPv2c_VERSION;
		}
		else if (sEnvUtilVersion == "-v2")
		{
			m_eUtilVersion = SR_SNMPv2_VERSION;
		}
		else if (sEnvUtilVersion == "-v3")
		{
			m_eUtilVersion = SR_SNMPv3_VERSION;
		}
	}

	if (m_eCommunicationType == TRAP_RCV_COMMUNICATIONS)
	{
		//Set this to appropriate protocol version supported.
		m_eUtilVersion = SR_SNMPv2c_VERSION;
	}

	m_sDefaultContextSnmpID.Empty ();
	m_sDefaultContext.Empty ();
	m_sCommunity.Empty ();

	if ((m_eUtilVersion == SR_SNMPv1_VERSION) ||
		(m_eUtilVersion == SR_SNMPv2c_VERSION))
	{
		m_sCommunity = getenv ("SR_UTIL_COMMUNITY");
	}
	else if ((m_eUtilVersion == SR_SNMPv2_VERSION) ||
			(m_eUtilVersion == SR_SNMPv3_VERSION))
	{
		m_sCommunity = getenv ("SR_UTIL_USERNAME");
	}

	//Make sure util_handle is not empty here. If it is, default it to "public" community name.
	if (m_sCommunity.IsEmpty ())
	{
		m_sCommunity = "public";
	}

	/*
	*	9.
	*	Set the 'util_dest' variable to the IP address of the target component.
	*	::	m_sIPAddress;
	*	::	The caller should call SetTargetComponent () prior to  using any of the interface functions.
	*/

	InitUtilFileNames ();
	/*
	*	11.
	*	Create MIB tree from the snmpinfo.dat file.
	*/
	if (! NewMIBFromFile (m_sSNMPInfoFile))
	{
		//Failure in snmpinfo.dat.
		return false;
	}

	m_pMIBOidTable = (MIB_OID *)m_pOriginalMIBOidTable;

	if (m_eCommunicationType == SNMP_COMMUNICATIONS)
	{
		InitializeIO (m_sIPAddress);
	}

	//util_dest is the target IP address.
	if (m_eCommunicationType == TRAP_SEND_COMMUNICATIONS)
	{
		if (m_sIPAddress.IsEmpty ())
		{
			//This situation is illegal.
			return false;
		}
		//This will similarily change into m_sIPAddress as in InitializeIO ().
		InitializeTrapIO (m_sIPAddress);
	}

	if (m_eCommunicationType == TRAP_RCV_COMMUNICATIONS)
	{
		InitializeTrapRcvIO ();
	}

	InitTimeNow ();
	util_local_snmpID = SrGetSnmpID (SR_SNMPID_ALGORITHM_SIMPLE, SR_SNMPID_ALGORITHM_SIMPLE_MGR);
	if (util_local_snmpID == NULL)
	{
		printf ("Cannot initialize local snmpID.\n");
		return false;
	}

	InitV2CLU ();
//	InitV3CLU ();

//	ParseConfigFile (fn_mgr_file, util_rt);

	//[NOT YET SUPPORTED]
	if (m_eUtilVersion == SR_SNMPv2_VERSION)
	{
//		if (!InitializeUser (util_handle, communication_type))
//		{
//			printf ("Bad username %s\n", util_handle);
//			return false;
//		}
	}
	//[NOT YET SUPPORTED]
	if (m_eUtilVersion == SR_SNMPv3_VERSION)
	{
//		if (!InitializeV3User (util_handle, communication_type))
//		{
//			printf ("Bad username %s\n", util_handle);
//			return false;
//		}
	}

//	global_req_id = MakeReqId ();

	return true;
}


//-------------------------------------------------------------------------------------
//
// bool CSNMPInterface::SetTargetComponent ()
//
//-------------------------------------------------------------------------------------
void CSNMPInterface::SetTargetComponent (CString& p_sTargetComponent)
{
	m_sIPAddress = p_sTargetComponent;
}


//-------------------------------------------------------------------------------------
//
// bool CSNMPInterface::EntryPoint_GetOne ()
//
//-------------------------------------------------------------------------------------
bool CSNMPInterface::EntryPoint_GetOne (CString& p_csOutputBuffer, const CString& p_csLeaf)
{
	Pdu	*req_pdu_ptr;

	if (! InitializeUtility (SNMP_COMMUNICATIONS))
	{
		return false;
	}
	
	req_pdu_ptr = MakePdu (GET_REQUEST_TYPE, 0);
	if (! req_pdu_ptr)
	{
		return false;
	}
	
	return true;
}


//-------------------------------------------------------------------------------------
//
// bool CSNMPInterface::InitializeUtility ()
// Obsoleted. The code has been moved to InitializeInterface ().
//
//-------------------------------------------------------------------------------------
bool CSNMPInterface::InitializeUtility (ECommunicationType type)
{
	return true;
}



//-------------------------------------------------------------------------------------
//
// OctetString *CSNMPInterface::MakeOctetString ()
//
//-------------------------------------------------------------------------------------
OctetString *CSNMPInterface::MakeOctetString (const unsigned char *p_pString, int p_nLength)
{
	int nIterator;
	OctetString *pOctetStr;

	if (p_nLength > SNMP::MAX_OCTET_STRING_SIZE)
	{
		return NULL;
	}

	pOctetStr = new OctetString (p_nLength);

	if (p_pString)
	{
		for (nIterator = 0; nIterator < p_nLength; nIterator++)
		{
			pOctetStr->m_pOctetString[nIterator] = p_pString[nIterator];
		}
	}
	return pOctetStr;
}


//-------------------------------------------------------------------------------------
//
// OctetString *CSNMPInterface::SrGetSnmpID ()
//
//-------------------------------------------------------------------------------------
OctetString *CSNMPInterface::SrGetSnmpID (EAlgorithmType p_eAlgorithm, void *p_pData)
{
    OctetString *psnmpID;
    psnmpID = MakeOctetString (NULL, 12);

	if (! psnmpID == NULL)
	{
		//Error: CANNOT ALLOCATE snmpID/snmpEngineID.
		return NULL;
	}

	*(unsigned int *)psnmpID->m_pOctetString = htonl (SNMP::IANA_ENTERPRISE_NUMBER);

	switch (p_eAlgorithm)
	{
	case SR_SNMPID_ALGORITHM_SIMPLE:
		{
			psnmpID->m_pOctetString[4] = (unsigned char)p_eAlgorithm;

			if (p_pData == SR_SNMPID_ALGORITHM_SIMPLE_AGT)
			{
				psnmpID->m_pOctetString[5] = 0;
			}
			else if (p_pData == SR_SNMPID_ALGORITHM_SIMPLE_MGR)
			{
				psnmpID->m_pOctetString[5] = 1;
			}
			else
			{
				//Error: BAD ALGORITHM DATA
			}
			
			*(unsigned short *)(psnmpID->m_pOctetString + 6) = htons ((unsigned short)GetSNMPPort ());

			*(unsigned int *)(psnmpID->m_pOctetString + 8) = htonl (GetLocalIPAddress ());
		} break;

	default:
		{
			//Error: BAD ALGORITHM
		} break;
	} //switch

	return psnmpID;
}


//-------------------------------------------------------------------------------------
//
// Pdu *CSNMPInterface::MakePdu ()
//
//-------------------------------------------------------------------------------------
unsigned long CSNMPInterface::GetLocalIPAddress ()
{
	char sHostName [SNMP::MAX_HOSTNAME_SIZE];
	hostent *pHostEntry;

	if (m_nLocalIPAddress)
	{
		return m_nLocalIPAddress;
	}

	if (! gethostname (sHostName, SNMP::MAX_HOSTNAME_SIZE))
	{
//		m_nLocalIPAddress// TODO: Finish here.
	}
}


//-------------------------------------------------------------------------------------
//
// Pdu *CSNMPInterface::MakePdu ()
//
//-------------------------------------------------------------------------------------
Pdu *CSNMPInterface::MakePdu (EContexts p_Context, int p_RequestId)
{
	Pdu	*pPdu;

	pPdu = new Pdu;

	pPdu->type = p_Context;

	switch (p_Context)
	{
	case GET_REQUEST_TYPE:
	case GET_NEXT_REQUEST_TYPE:
	case GET_RESPONSE_TYPE:
    case SET_REQUEST_TYPE:
    case INFORM_REQUEST_TYPE:
    case SNMPv2_TRAP_TYPE:
    case REPORT_TYPE:
		{
			pPdu->u.normpdu.request_id = p_RequestId;
			pPdu->u.normpdu.error_status = 0;
			pPdu->u.normpdu.error_index = 0;
		}
		break;
	case GET_BULK_REQUEST_TYPE:
		{
			pPdu->u.bulkpdu.request_id = p_RequestId;
			pPdu->u.bulkpdu.non_repeaters = 0;
			pPdu->u.bulkpdu.max_repetitions = 0;
		}
		break;
	case TRAP_TYPE:
		{
			//Enterprise cannot be NULL;
			//Agent_addr cannot be NULL;
			ASSERT ("MakePdu: Enterprise and Agent_addr cannot be NULL");
//			pPdu->u.trappdu.enterprise = enterprise;
//			pPdu->u.trappdu.agent_addr = agent_addr;
//			pPdu->u.trappdu.generic_trap = generic_trap;
//			pPdu->u.trappdu.specific_trap = specific_trap;
//			pPdu->u.trappdu.time_ticks = time_ticks;
		}
		break;
	default:
		{
			ASSERT ("MakePdu: Illegal PDU type requested.");
///			FreePdu (pPdu);
			pPdu = NULL;
		}
		break;
	}
	return pPdu;
}


//-------------------------------------------------------------------------------------
//
// OID	*CSNMPInterface::MakeOIDFromDot ()
//
//-------------------------------------------------------------------------------------
OID	*CSNMPInterface::MakeOIDFromDot (const CString& p_sDot)
{
    OID *pOid;
	//Artur Perwenis: Insertion to avoid compiler warning messages
	pOid = NULL;

	pOid = MakeOIDFragFromDot (p_sDot);

    if (! pOid)
	{
		ASSERT ("MakeOICFromDot: MakeOIDFragFromDot () failed.");
		return NULL;
    }

    if (pOid->m_pnOid[0] >= 4)     /* we have a bogus OID */
	{
		ASSERT ("MakeOIDFromDot, illegal OID value.");
//		FreeOID (pOid);
		return NULL;
    }

	return pOid;
}


//-------------------------------------------------------------------------------------
//
// OID	*CSNMPInterface::MakeOIDFragFromDot ()
//
//-------------------------------------------------------------------------------------
OID	*CSNMPInterface::MakeOIDFragFromDot (const CString& p_sDot)
{
	OID *pOid = NULL;
//	MIB_OID *pOidHashTable;
//	int nHashValue;

	//Have the OID routines been initialized?
	if (m_bInitNewOIDRoutines == false)
	{
		InitNewOIDRoutines ();
	}

	return pOid;
}


//-------------------------------------------------------------------------------------
//
// void CSNMPInterface::InitNewOIDRoutines ()
//
//-------------------------------------------------------------------------------------
void CSNMPInterface::InitNewOIDRoutines ()
{
	int nIterator;

	//Clear out any old info from hash table.
	for (nIterator = 0; nIterator < SNMP::NAME_OID_HASH_SIZE; nIterator++)
	{
		m_NameOidHashTable[nIterator] = NULL;
	}
	InitNameOidHashTable ();

	m_bInitNewOIDRoutines = true;
}


//-------------------------------------------------------------------------------------
//
// void CSNMPInterface::InitNameOidHashTable ()
//
//-------------------------------------------------------------------------------------
void CSNMPInterface::InitNameOidHashTable ()
{
	int nIterator;
	int nHashValue;
	MIB_OID *pTmpHashOid;
	//HERE IS THE QUESTION: HOW DOES THIS ROUTINE WORK?
	CreateMIBOidTable ();

//	for (nIterator = 0; m_pMIBOidTable
}


//-------------------------------------------------------------------------------------
//
// int CSNMPInterface::CalculateHashFromName ()
//
//-------------------------------------------------------------------------------------
bool CSNMPInterface::CreateMIBOidTable ()
{
	bool bRetVal;

	bRetVal = false;

	return bRetVal;
}


//-------------------------------------------------------------------------------------
//
// int CSNMPInterface::CalculateHashFromName ()
//
//-------------------------------------------------------------------------------------
int CSNMPInterface::CalculateHashFromName (const CString& p_sName)
{
	int nIterator;
	int nHashValue;

	if (p_sName.GetLength ())
	{
		nHashValue = 0;
		for (nIterator = 0; nIterator < p_sName.GetLength (); nIterator++)
		{
			nHashValue = (nHashValue + p_sName[nIterator]) % SNMP::NAME_OID_HASH_SIZE;
		}
	}
	else
	{
		//For zero-length strings the hash is 0.
		return 0;
	}
	return nHashValue;
}


//-------------------------------------------------------------------------------------
//
// bool CSNMPInterface::MakeDotFromOID ()
//
//-------------------------------------------------------------------------------------
bool CSNMPInterface::MakeDotFromOID (const OID *p_pOid, CString& p_sBuffer)
{
	p_sBuffer.Empty ();

	if ((p_pOid == NULL) || (p_pOid->m_nLength == 0) || (p_pOid->m_pnOid == NULL)) 
	{
		ASSERT ("MakeDotFromOID (): bad (NULL) OID.");
		return false;
	}

	if (! m_bInitNewOIDRoutines)
	{
///		InitNewOIDRoutines ();
	}

	if (! GetStringFromSubIds (p_sBuffer, p_pOid->m_pnOid, p_pOid->m_nLength)) 
	{
//		DPRINTF((APPACKET, "MakeDotFromOID, GetStringFromSubIds\n"));
		return false;
	}

	return true;
}


//-------------------------------------------------------------------------------------
//
// bool CSNMPInterface::GetStringFromSubIds ()
//
//-------------------------------------------------------------------------------------
bool CSNMPInterface::GetStringFromSubIds (CString& p_sBuffer, unsigned int *p_psid, int p_nSidCounter)
{
    int             i;
    char            temp_buffer[20];
    int             dot_count;
    int             best_i, best_len, test_len;

	//Build the test string ... put dots between each entry but not after the last one.
	for (i = 0; i < p_nSidCounter; i++) 
	{
		(void) sprintf (temp_buffer, "%lu", (unsigned long)p_psid[i]);

		if ((i + 1) != p_nSidCounter) 
		{
///			(void) strcat (string_buffer, temp_buffer);
///			(void) strcat (string_buffer, ".");
		}
		else
		{
///			(void) strcat (string_buffer, temp_buffer);
		}
	}

	/*
	 * Now, let us try to convert the dotted decimal header of the string in
	 * temp_buffer into a more reasonable ASCII string.
	 */
/* Can do a binary search here, since snmp-mib.h is sorted by OID */

	best_i = -1;
	best_len = -1;
	/* OK, now find the best fit to this string */
    for (i = 0; m_pMIBOidTable[i].m_sName.IsEmpty () == false; i++) 
	{
        test_len = 0;
        while ((m_pMIBOidTable[i].m_sNumber[test_len] == p_sBuffer[test_len]) &&
               (m_pMIBOidTable[i].m_sNumber[test_len] != '\0') &&
               (p_sBuffer[test_len] != '\0')) 
		{
            test_len++;
        }

		/* 
		 * test_len is a count of the number successfully matched
		 * i.e., mib_oid_table[i].number[test_len] is the first
		 * mismatch; i.e., mib_oid_table[i].number[test_len-1] is the
		 * last good match want to back up to the first character
		 * before the last . or null 
		 */

		/**
		 * this is what the equation works out to be just looking at it but i
		 * simply could not sleep with such a thing so i applied demorgans
		 * theorem and got the resulting code below which should be
		 * equivalent but run faster 
		 * while ((test_len > 0) 
		 *        && 
		 *        (!((mib_oid_table[i].number[test_len] == '.') ||
		 *           (mib_oid_table[i].number[test_len] == '\0')) 
		 *        ||
		 *        !((string_buffer[test_len] == '.') || 
		 *          (string_buffer[test_len] == '\0')) 
		 *        )
		 *       ) {
		 * 
		 **/

		while ((test_len > 0) &&
			   (((m_pMIBOidTable[i].m_sNumber[test_len] != '.') &&
				(m_pMIBOidTable[i].m_sNumber[test_len] != '\0')) ||
				((p_sBuffer[test_len] != '.') &&
				(p_sBuffer[test_len] != '\0')))) 
		{
			test_len--;
		}	/* while */

///		if ((test_len > best_len) && (test_len == strlen(m_mib_oid_table[i].m_sNumber))) 
///		{
///			best_i = i;
///			best_len = test_len;
///		}			/* if test_len > */
    }	/* for i */

	/*
	 * Did we get something?  If so, then replace dot-digit string with the
	 * name.
	 */

	if (best_i > -1) 
	{
		p_sBuffer = m_pMIBOidTable[best_i].m_sName;

		/* count the dots.  num +1 is the number of SIDs */
		dot_count = 0;
		for (i = 0; m_pMIBOidTable[best_i].m_sNumber[i] != '\0'; i++) 
		{
			if (m_pMIBOidTable[best_i].m_sNumber[i] == '.')
			{
				dot_count++;
			}
		}

		/* now flesh out from where the name leaves off */

		for (i = dot_count + 1; i < p_nSidCounter; i++) 
		{
///			(void) sprintf (temp_buffer, ".%lu", (unsigned long)sid[i]);
///			(void) strcat (string_buffer, temp_buffer);
		}
	}
	/* else, just return the dot string we already filled in */
	return (1);
}


//-------------------------------------------------------------------------------------
//
// bool CSNMPInterface::process_command_line_option ()
//
//-------------------------------------------------------------------------------------
bool CSNMPInterface::process_command_line_option ()
{
	//This function accepts the following switches:
	// -d    enables debug mode
	// -v1   sets the util version (version_supported) to SR_SNMPv1_VERSION
	// -v2c  sets the util version (version_supported) to SR_SNMPv2c_VERSION
	// -v2   sets the util version (version_supported) to SR_SNMPv2_VERSION
	// -v3   sets the util version (version_supported) to SR_SNMPv3_VERSION
	// -ctxid or -contextid
	//       sets the defaultContextSnmpID variable to the value of the argument following -ctxid or -contextid
	//       using a call to MakeCotetStringFromHex (argv[2]).
	//       Should be made into a separate protected method to be called. E.g.: SetDefCtxId (CString& ctxid).
	// -ctx or -context
	//       sets the defaultContext variable to the value of the argument following -ctx or -context
	//       using a call to MakeOctetStringFromText (argv[2]).
	// -t or -timeout
	//       sets seconds variable to the value of the argument following -t or -timeout.
	//       Should be made into a separate public method SetTimeout (int seconds).
	// -r or -retries
	//       sets number of retries (retries and tretries variables) to the value of the argument following
	//       -r or -retries.
	//       Should be made into a separate public method SetRetries (int numRetries).




    return true;
}


//-------------------------------------------------------------------------------------
//
// void CSNMPInterface::InitUtilFileNames ()
//
//-------------------------------------------------------------------------------------
void CSNMPInterface::InitUtilFileNames ()
{
	CString	csDirPrefix;

    csDirPrefix = getenv("SR_MGR_CONF_DIR");
	
	if (csDirPrefix.IsEmpty ())
	{
		csDirPrefix = SR_MGR_CONF_FILE_PREFIX;
    }

	csDirPrefix.TrimRight ('/');

	m_sSNMPInfoFile.Format ("%s/%s", (LPCSTR)csDirPrefix, FN_SNMPINFO_DAT);
	m_sMGRFile.Format ("%s/%s", (LPCSTR)csDirPrefix, FN_MGR_FILE);
}


//-------------------------------------------------------------------------------------
//
// bool CSNMPInterface::NewMIBFromFile ()
//
//-------------------------------------------------------------------------------------
bool CSNMPInterface::NewMIBFromFile (const CString& p_sFileName)
{
    OID oOid;
    unsigned int nSid;
	MIB_TREE *pKillTree;


	/*
		1.
		Write the FreeMIBTree () method.
		2.
		Write the CreateMIBTree () method.
		2.
		Write the MergeMIBFromFile () method.
	*/
	while (m_pDefaultMIBTree)
	{
		pKillTree = m_pDefaultMIBTree;
		m_pDefaultMIBTree = m_pDefaultMIBTree->m_pNext;
//		FreeMIBTree (pKillTree);
	}

	//What is it? Oid length?
	nSid = 1;
	//Root level.
	oOid.m_nLength = 1;
	oOid.m_pnOid = &nSid;

	m_pDefaultMIBTree = CreateMIBTree ("default", &oOid);

	if (! m_pDefaultMIBTree)
	{
		return false;
	}

	return MergeMIBFromFile (p_sFileName);
}


//-------------------------------------------------------------------------------------
//
// MIB_TREE *CSNMPInterface::CreateMIBTree ()
//
//-------------------------------------------------------------------------------------
bool CSNMPInterface::MergeMIBFromFile (const CString& p_sFileName)
{
	return ReadMIBFile (p_sFileName, NULL, ECollisionType::SR_COL_REPLACE);
}


//-------------------------------------------------------------------------------------
//
// MIB_TREE *CSNMPInterface::ReadMIBFile ()
//
//-------------------------------------------------------------------------------------
bool CSNMPInterface::ReadMIBFile (const CString& p_sFileName, MIB_TREE *p_pMIBTree, ECollisionType p_eCollisionType)
{
	OID_TREE_ELEMENT	*pOIDTreeElement;			//	otep
	OID_TREE_ELEMENT	*pLastOIDTreeElement;		//	last_otep
	OID_TREE_ELEMENT	*pFirstOIDTreeElement;		//	oteps
	OID_TREE_ELEMENT	*pTailOIDTreeElement;		//	oteps_tail
	CConfigFile		*pConfigFile;
	EThreeStatesBoolean eReadEntryStatus;
///	bool bRetVal;

	if (! p_pMIBTree)
	{
		p_pMIBTree = m_pDefaultMIBTree;
	}

	pConfigFile = OpenConfigFile (p_sFileName, EFileOpenMode::FM_READ);
	if (! pConfigFile)
	{
		return false;
	}

	pOIDTreeElement = NULL;
	pFirstOIDTreeElement = NULL;
	pLastOIDTreeElement = NULL;
	pTailOIDTreeElement = NULL;

	for (eReadEntryStatus = EThreeStatesBoolean::SUCCESS; eReadEntryStatus == EThreeStatesBoolean::SUCCESS; )
	{
		eReadEntryStatus = ReadMIBFileEntry (pConfigFile, &pOIDTreeElement);
		if (eReadEntryStatus == EThreeStatesBoolean::SUCCESS)
		{
			if (pOIDTreeElement != pLastOIDTreeElement)
			{
				if (pOIDTreeElement)
				{
					if (pFirstOIDTreeElement)
					{
						pTailOIDTreeElement->m_pNext = pOIDTreeElement;
						pTailOIDTreeElement = pOIDTreeElement;
					}
					else
					{
						pFirstOIDTreeElement = pOIDTreeElement;
						pTailOIDTreeElement = pOIDTreeElement;
					}
				}
				pLastOIDTreeElement = pOIDTreeElement;
			}
		}
	}

	//Close the file.
	delete pConfigFile;

	//If the file failed to be parsed correctly, free up all allocated objects
	if (eReadEntryStatus == EThreeStatesBoolean::FAILURE)
	{
		while (pFirstOIDTreeElement)
		{
			pOIDTreeElement = pFirstOIDTreeElement;
			pFirstOIDTreeElement = pFirstOIDTreeElement->m_pNext;
			//ReleaseOIDTreeElement (pOIDTreeElement);
		}
		return false;
	}

	if (p_eCollisionType == ECollisionType::SR_COL_REPLACE)
	{
		while (pFirstOIDTreeElement)
		{
			//Move the queue by one step and detach its head
			pOIDTreeElement = pFirstOIDTreeElement;
			pFirstOIDTreeElement = pFirstOIDTreeElement->m_pNext;
			pOIDTreeElement->m_pNext = NULL;
			if (AddOTEToMIBTree (p_pMIBTree, pOIDTreeElement, p_eCollisionType) != pOIDTreeElement)
			{
			}
		}
	}
	else
	{
	}

	return true;
}


//-------------------------------------------------------------------------------------
//
// OID_TREE_ELEMENT *CSNMPInterface::AddOTEToMIBTree ()
//
//-------------------------------------------------------------------------------------
OID_TREE_ELEMENT *CSNMPInterface::AddOTEToMIBTree (MIB_TREE *p_pMIBTree, OID_TREE_ELEMENT *p_pOIDTreeElement, ECollisionType p_eCollisionType)
{
	OID *pOid;

	if (! p_pMIBTree)
	{
		p_pMIBTree = m_pDefaultMIBTree;
	}

	pOid = MakeOIDFromDot (p_pOIDTreeElement->m_sOidNumberStr);

	return NULL;
}


//-------------------------------------------------------------------------------------
//
// CSNMPInterface::EThreeStatesBoolean CSNMPInterface::ReadMIBFileEntry ()
//
//-------------------------------------------------------------------------------------
CSNMPInterface::EThreeStatesBoolean CSNMPInterface::ReadMIBFileEntry (CConfigFile *p_pConfigFile, OID_TREE_ELEMENT **p_pOIDTreeElement)
{
	EParserState	eParserContext;
	bool bGotSize;
	bool bImplied;
	EOidType eType;
	EAccessType eAccess;
	int nFieldCount;
	OID_TREE_ELEMENT	*pOIDTreeElement;
	ENumer *pENumer;
	MibIndex	*pMibIndex;
	int	nEnumValue;

	//Variable initialization
	pOIDTreeElement = *p_pOIDTreeElement;
	pENumer = NULL;
	eParserContext = PS_NONE;

	//[MIBFromFileInfo initialization should go here]

	if (pOIDTreeElement)
	{
		//Find the last ENumer.
		pENumer = pOIDTreeElement->m_pEnumer;
		if (pENumer)
		{
			while (pENumer->m_pNext)
			{
				pENumer = pENumer->m_pNext;
			}
		}
		//Find the last index
		pMibIndex = pOIDTreeElement->m_pMibIndex;
		if (pMibIndex)
		{
			while (pMibIndex->m_pNext)
			{
				pMibIndex = pMibIndex->m_pNext;
			}
		}
	}

	//Get an alias of the buffer for quicker access.
	CString& sBuffer = p_pConfigFile->m_sBuffer;

	//Read one line of the file and analyse it.
	while (p_pConfigFile->ReadString (sBuffer))
	{
		p_pConfigFile->m_nLineCount++;

		switch (eParserContext)
		{
		case PS_NONE:
			{
				//Check for blank lines.
				if (sBuffer.SpanIncluding (" \t\n").GetLength () == sBuffer.GetLength ())
				{
					break;
				}
				//Check for beginning or ending of comment lines.
				if (sBuffer.Find ("/*") != -1)
				{
					eParserContext = PS_COMMENT;
					break;
				}
				//Check if this is an enumerations entry.
				if (sBuffer[0] == '(')
				{
					eParserContext = PS_ENUMER;
					break;
				}
				//Check if this is an index entry.
				if (sBuffer[0] == '{')
				{
					eParserContext = PS_INDEX;
					break;
				}
				//Check if this is a new variable entry.
				if (((sBuffer[0] >= 'a') && (sBuffer[0] <= 'z')) ||
					((sBuffer[0] >= 'A') && (sBuffer[0] <= 'Z')))
				{
					nFieldCount = sscanf((LPCSTR)sBuffer, "%s %s %[^( \n\f\t](%[^( \n\f\t])",
								p_pConfigFile->m_sName,
								p_pConfigFile->m_sNumber,
								p_pConfigFile->m_sType,
								p_pConfigFile->m_sSizeRange);
					if (nFieldCount == 4)
					{
						bGotSize = true;
					}
					else if (nFieldCount == 3)
					{
						bGotSize = false;
					}
					else
					{
						//The number of fields must be at least 3.
						//This is a warning: "Wrong number of fields".
						//Notify the user: p_pConfigFile->m_nLineCount; p_pConfigFile->GetFileName ();
						return EThreeStatesBoolean::SUCCESS;
					}
					eType = MapStringToOidType (p_pConfigFile->m_sType);
					if (eType == EOidType::UNKNOWN_TYPE)
					{
						//This is a warning: "Unknown type".
						//Notify the user: p_pConfigFile->m_nLineCount; p_pConfigFile->GetFileName ();
						return EThreeStatesBoolean::SUCCESS;
					}
					if (eType != EOidType::NON_LEAF_TYPE)
					{
						nFieldCount = sscanf((LPCSTR)sBuffer, "%*s %*s %*s %s", p_pConfigFile->m_sAccess);
						if (nFieldCount != 1)
						{
							//This is a warning: "Missing access specifier".
							//Notify the user: p_pConfigFile->m_nLineCount; p_pConfigFile->GetFileName ();
							return EThreeStatesBoolean::SUCCESS;
						}
						eAccess = MapStringToAccessType (p_pConfigFile->m_sAccess);
						if (eAccess == EAccessType::UNKNOWN_ACCESS)
						{
							//This is a warning: "Bad access specifier".
							//Notify the user: p_pConfigFile->m_nLineCount; p_pConfigFile->GetFileName ();
							return EThreeStatesBoolean::SUCCESS;
						}
					}
					else
					{
						eAccess = EAccessType::NOT_ACCESSIBLE_ACCESS;
					} //if (eType != EOidType::NON_LEAF_TYPE)
					
					pOIDTreeElement = new OID_TREE_ELEMENT;
					pOIDTreeElement->m_sOidName = p_pConfigFile->m_sName;
					pOIDTreeElement->m_sOidNumberStr = p_pConfigFile->m_sNumber;
					pOIDTreeElement->m_nType = eType;
					pOIDTreeElement->m_nAccess = eAccess;
					
					if (bGotSize)
					{
						switch (eType)
						{
						case EOidType::DISPLAY_STRING_TYPE:
						case EOidType::OCTET_PRIM_TYPE:
						case EOidType::OBJECT_ID_TYPE:
						case EOidType::TIME_TICKS_TYPE:
						case EOidType::GAUGE_TYPE:
						case EOidType::COUNTER_TYPE:
						case EOidType::SERVICES_TYPE:
							{
								unsigned long nUlong1, nUlong2;

								sscanf ((LPCSTR)sBuffer, "%lu..%lu)", &nUlong1, &nUlong2);
								pOIDTreeElement->m_nSizeLower = nUlong1;
								pOIDTreeElement->m_nSizeUpper = nUlong2;
							}
							break;

						case EOidType::INTEGER_TYPE:
							{
								long nLong1, nLong2;

								sscanf ((LPCSTR)sBuffer, "%ld..%ld)", &nLong1, &nLong2);
								pOIDTreeElement->m_nRangeLower = nLong1;
								pOIDTreeElement->m_nRangeUpper = nLong2;
							}
							break;

						case EOidType::COUNTER_64_TYPE:
							//This type is not supported on Intel platform.
							break;

						case EOidType::PHYS_ADDRESS_TYPE:
						case EOidType::NETWORK_ADDRESS_TYPE:
						case EOidType::IP_ADDR_PRIM_TYPE:
						case EOidType::NON_LEAF_TYPE:
						case EOidType::AGGREGATE_TYPE:
							//These types cannot have the size specifier.
						default:
							//This is an error: "Bad range specifier".
							//Notify the user: p_pConfigFile->m_nLineCount; p_pConfigFile->GetFileName ();
							return EThreeStatesBoolean::FAILURE;
						} //switch (eType)
					} //if (bGotSize)
					//Return the constructed OID structure to the caller.
					*p_pOIDTreeElement = pOIDTreeElement;
					return EThreeStatesBoolean::SUCCESS;
				} //Check if this is a new variable entry.

				//This is a warning: "Bad variable name. Must start with a-z, A-Z".
				//Notify the user: p_pConfigFile->m_nLineCount; p_pConfigFile->GetFileName ();
				return EThreeStatesBoolean::SUCCESS;

			} //case PS_NONE:
			break;

		case PS_COMMENT:
			{
				//Check for beginning or ending of comment lines.
				if (sBuffer.Find ("*/") != -1)
				{
					eParserContext = PS_NONE;
				}
			} //case PS_COMMENT:
			break;

		case PS_ENUMER:
			{
				if (sBuffer[0] == ')')
				{
					return EThreeStatesBoolean::SUCCESS;
				}
				if (! pOIDTreeElement)
				{
					//This is a warning: "Enumerations found before MIB variable declaration".
					//Notify the user: p_pConfigFile->m_nLineCount; p_pConfigFile->GetFileName ();
					return EThreeStatesBoolean::SUCCESS;
				}

				//Try to locate the enumerations field
				nFieldCount = sscanf ((LPCSTR)sBuffer, "%d %s", &nEnumValue, p_pConfigFile->m_sEnumName);
				if (nFieldCount != 2)
				{
					//This is a warning: "Bad enumeration entry".
					//Notify the user: p_pConfigFile->m_nLineCount; p_pConfigFile->GetFileName ();
					break;
				}

				if (! pENumer)
				{
					pENumer = new ENumer;
					pOIDTreeElement->m_pEnumer = pENumer;
				}
				else
				{
					pENumer->m_pNext = new ENumer;
					pENumer = pENumer->m_pNext;
				}
				pENumer->m_sName = p_pConfigFile->m_sEnumName;
			} //case PS_ENUMER:
			break;

		case PS_INDEX:
			{
				if (sBuffer[0] == '}')
				{
					return EThreeStatesBoolean::SUCCESS;
				}
				if (! pOIDTreeElement)
				{
					//This is a warning: "MIB indices found before MIB variable declaration".
					//Notify the user: p_pConfigFile->m_nLineCount; p_pConfigFile->GetFileName ();
					return EThreeStatesBoolean::SUCCESS;
				}

				//Try to locate the IMPLIED index field.
				bImplied = true;
				nFieldCount = sscanf ((LPCSTR)sBuffer, "IMPLIED %s", p_pConfigFile->m_sIndexName);
				if (nFieldCount != 1)
				{
					bImplied = false;
					nFieldCount = sscanf ((LPCSTR)sBuffer, "%s", p_pConfigFile->m_sIndexName);
					if (nFieldCount != 1)
					{
						//The index field was neither IMPLIED nor regular index.
						//This is a warning: "Bad index entry".
						//Notify the user: p_pConfigFile->m_nLineCount; p_pConfigFile->GetFileName ();
						break;
					}
				}

				if (! pMibIndex)
				{
					pMibIndex = new MibIndex;
					pOIDTreeElement->m_pMibIndex = pMibIndex;
				}
				else
				{
					pMibIndex->m_pNext = new MibIndex;
					pMibIndex = pMibIndex->m_pNext;
				}
				pMibIndex->m_sName = p_pConfigFile->m_sIndexName;
				pMibIndex->m_bImplied = bImplied;
			} //case PS_INDEX:
			break;

		} //switch (eParserContext)
	} //while (p_pConfigFile->ReadString (sBuffer))
	if (p_pConfigFile->GetPosition () >= p_pConfigFile->GetLength ())
	{
		//End of file reached - normal termination
		return EThreeStatesBoolean::ERROR_NO_FAIL;
	}
	//Possibly read error
	return EThreeStatesBoolean::FAILURE;
}


//-------------------------------------------------------------------------------------
//
// CSNMPInterface::EOidType CSNMPInterface::MapStringToOidType ()
//
//-------------------------------------------------------------------------------------
CSNMPInterface::EOidType CSNMPInterface::MapStringToOidType (const CString& p_sType)
{
	if (p_sType == m_TypeName_NULL)						return EOidType::NULL_TYPE;
	if (p_sType == m_TypeName_NONLEAF)					return EOidType::NON_LEAF_TYPE;
	if (p_sType == m_TypeName_AGGREGATE)				return EOidType::AGGREGATE_TYPE;
	if (p_sType == m_TypeName_DISPLAYSTRING)			return EOidType::DISPLAY_STRING_TYPE;
	if (p_sType == m_TypeName_OCTETSTRING)				return EOidType::OCTET_PRIM_TYPE;
	if (p_sType == m_TypeName_PHYSADDRESS)				return EOidType::PHYS_ADDRESS_TYPE;
	if (p_sType == m_TypeName_NETWORKADDRESS)			return EOidType::NETWORK_ADDRESS_TYPE;
	if (p_sType == m_TypeName_IPADDRESS)				return EOidType::IP_ADDR_PRIM_TYPE;
	if (p_sType == m_TypeName_OBJECTID)					return EOidType::OBJECT_ID_TYPE;
	if (p_sType == m_TypeName_TIMETICKS)				return EOidType::TIME_TICKS_TYPE;
	if (p_sType == m_TypeName_INTEGER)					return EOidType::INTEGER_TYPE;
	if (p_sType == m_TypeName_GAUGE)					return EOidType::GAUGE_TYPE;
	if (p_sType == m_TypeName_COUNTER)					return EOidType::COUNTER_TYPE;
	if (p_sType == m_TypeName_SERVICES)					return EOidType::SERVICES_TYPE;
	if (p_sType == m_TypeName_COUNTER32)				return EOidType::COUNTER_32_TYPE;
	if (p_sType == m_TypeName_GAUGE32)					return EOidType::GAUGE_32_TYPE;
	if (p_sType == m_TypeName_COUNTER64)				return EOidType::COUNTER_64_TYPE;
	if (p_sType == m_TypeName_INTEGER32)				return EOidType::INTEGER_TYPE;
	if (p_sType == m_TypeName_BITS)						return EOidType::OCTET_PRIM_TYPE;
	if (p_sType == m_TypeName_UNSIGNED32)				return EOidType::GAUGE_32_TYPE;
	return EOidType::UNKNOWN_TYPE;
}


//-------------------------------------------------------------------------------------
//
// CSNMPInterface::EAccessType CSNMPInterface::MapStringToAccessType ()
//
//-------------------------------------------------------------------------------------
CSNMPInterface::EAccessType CSNMPInterface::MapStringToAccessType (const CString& p_sAccess)
{
	if (p_sAccess == m_AccessName_READONLY)				return EAccessType::READ_ONLY_ACCESS;
	if (p_sAccess == m_AccessName_WRITEONLY)			return EAccessType::READ_WRITE_ACCESS;
	if (p_sAccess == m_AccessName_READWRITE)			return EAccessType::READ_WRITE_ACCESS;
	if (p_sAccess == m_AccessName_READCREATE)			return EAccessType::READ_CREATE_ACCESS;
	if (p_sAccess == m_AccessName_NOTACCESSIBLE)		return EAccessType::NOT_ACCESSIBLE_ACCESS;
	if (p_sAccess == m_AccessName_ACCESSIBLEFORNOTIFY)	return EAccessType::ACCESSIBLE_FOR_NOTIFY_ACCESS;
	return EAccessType::UNKNOWN_ACCESS;
}


//-------------------------------------------------------------------------------------
//
// CConfigFile *CSNMPInterface::OpenConfigFile ()
//
//-------------------------------------------------------------------------------------
CConfigFile *CSNMPInterface::OpenConfigFile (const CString& p_sFileName, EFileOpenMode p_eOpenMode)
{
	CConfigFile	*pConfigFile;
	bool bRetVal;

	bRetVal = false;

	pConfigFile = new CConfigFile;
	pConfigFile->m_nLineCount = 0;
	switch (p_eOpenMode)
	{
	case EFileOpenMode::FM_READ:
		bRetVal = pConfigFile->Open (p_sFileName, CFile::modeRead);
		break;
	case EFileOpenMode::FM_WRITE:
		bRetVal = pConfigFile->Open (p_sFileName, CFile::modeWrite);
		break;
	}

	if (bRetVal)
	{
		return pConfigFile;
	}
	else
	{
		delete pConfigFile;
		return NULL;
	}
}

//-------------------------------------------------------------------------------------
//
// MIB_TREE *CSNMPInterface::CreateMIBTree ()
//
//-------------------------------------------------------------------------------------
MIB_TREE *CSNMPInterface::CreateMIBTree (const CString &p_TreeName, const OID *p_pOidPrefix)
{
	MIB_TREE *pNewMIBTree;

	pNewMIBTree = new MIB_TREE;
	pNewMIBTree->m_sTreeName = p_TreeName;
	//Copy the prefix
	pNewMIBTree->m_pPrefix = CloneOID (*p_pOidPrefix);
	if (! pNewMIBTree->m_pPrefix)
	{
		//Error! The OID copy could not be created.
		delete pNewMIBTree;
		return NULL;
	}
	return pNewMIBTree;
}


//-------------------------------------------------------------------------------------
//
// OID *CSNMPInterface::CloneOID ()
//
//-------------------------------------------------------------------------------------
OID *CSNMPInterface::CloneOID (const OID& p_Oid)
{
	return MakeOID (p_Oid.m_pnOid, p_Oid.m_nLength);
}


//-------------------------------------------------------------------------------------
//
// OID *CSNMPInterface::MakeOID ()
//
//-------------------------------------------------------------------------------------
OID *CSNMPInterface::MakeOID (unsigned int p_pOidArray[], int p_Length)
{
	OID	*pOid;
	int nIterator;	//replaces i

	if (p_Length > SNMP::MAX_OID_SIZE)
	{
		return NULL;
	}

	pOid = new OID;

	pOid->m_pnOid = new unsigned int [p_Length];
	if (p_pOidArray)
	{
		//Is the OID value array supplied?
		for (nIterator = 0; nIterator < p_Length; nIterator++)
		{
			pOid->m_pnOid[nIterator] = p_pOidArray[nIterator];
		}
	}
	else
	{
		for (nIterator = 0; nIterator < p_Length; nIterator++)
		{
			pOid->m_pnOid[nIterator] = 0;
		}
	}
	pOid->m_nLength = p_Length;

	return pOid;
}

//-------------------------------------------------------------------------------------
//
// bool CSNMPInterface::InitializeIO ()
//
//-------------------------------------------------------------------------------------
bool CSNMPInterface::InitializeIO (const CString &p_sTargetHostName)
{
    WSADATA wsadata;

    if (WSAStartup (257, &wsadata) != 0)
	{
		//WSAStartup failed.
        return false;
    }

    /* now, set up UDP connection */
	m_nUDPSocket = socket (AF_INET, SOCK_DGRAM, 0);
    if (m_nUDPSocket < 0)
	{
		//Unable to connect to socket.
		return false;
    }

	m_SockAddrIn.sin_addr.S_un.S_addr = inet_addr((LPCSTR)p_sTargetHostName);

    if (m_SockAddrIn.sin_addr.s_addr == -1)
	{
		m_pHostEntry = gethostbyname ((LPCSTR)p_sTargetHostName);
		if (m_pHostEntry)
		{
			memcpy((char *) &m_SockAddrIn.sin_addr, (char *) m_pHostEntry->h_addr, m_pHostEntry->h_length);
		}
		else
		{
			//Unknown host
			return false;
		}
    }
	m_SockAddrIn.sin_family = AF_INET;
	m_SockAddrIn.sin_port = htons ((short) GetSNMPPort ());
	return true;
}


//-------------------------------------------------------------------------------------
//
// bool CSNMPInterface::InitializeTrapIO ()
//
//-------------------------------------------------------------------------------------
bool CSNMPInterface::InitializeTrapIO (const CString &p_sTargetHostName)
{
    WSADATA wsadata;

	if (WSAStartup (257, &wsadata) != 0)
	{
		//WSAStartup failed
		return false;
	}

    /* now, set up UDP connection */
	m_nUDPSocket = socket (AF_INET, SOCK_DGRAM, 0);
	if (m_nUDPSocket < 0)
	{
		//Unable to connect to socket.
		return false;
    }

	m_SockAddrIn.sin_addr.S_un.S_addr = inet_addr ((LPCSTR)p_sTargetHostName);

	if (m_SockAddrIn.sin_addr.S_un.S_addr == -1)
	{
		m_pHostEntry = gethostbyname ((LPCSTR)p_sTargetHostName);
		if (m_pHostEntry)
		{
			memcpy((char *) &m_SockAddrIn.sin_addr, (char *) m_pHostEntry->h_addr, m_pHostEntry->h_length);
		}
		else
		{
			//Host unknown
			return false;
		}
	}

	m_SockAddrIn.sin_family = AF_INET;
///	m_SockAddrIn.sin_port = htons((short) GetSNMPTrapPort());
	return true;
}


//-------------------------------------------------------------------------------------
//
/// unsigned int CSNMPInterface::GetSNMPPort ()
//
//-------------------------------------------------------------------------------------
unsigned int CSNMPInterface::GetSNMPPort ()
{
	static bool bCalled;
	static unsigned int nResult;
	CString	sSnmpPort;

	if (! bCalled)
	{
		bCalled = true;
		sSnmpPort = getenv (SR_SNMP_ENV_NAME);
		if (! sSnmpPort.IsEmpty ())
		{
			nResult = atoi ((char *)(LPCSTR)sSnmpPort);
		}
		else
		{
			nResult = SNMP::SNMP_PORT;
		}
	}
	return nResult;
}


//-------------------------------------------------------------------------------------
//
/// bool CSNMPInterface::InitializeTrapRcvIO ()
//
//-------------------------------------------------------------------------------------
bool CSNMPInterface::InitializeTrapRcvIO ()
{
	int i;
	WSADATA wsadata;

	if (WSAStartup (257, &wsadata) != 0)
	{
		//WSAStartup failed
		return false;
	}

    /* now, set up UDP connection */
	m_nUDPSocket = socket (AF_INET, SOCK_DGRAM, 0);
	if (m_nUDPSocket < 0)
	{
		//Unable to connect to socket.
		return false;
    }

	/* clear out and initialize the socket's address */
	memset (&m_SockAddrIn, 0, sizeof(m_SockAddrIn));
	
	//This will be an internet socket
	m_SockAddrIn.sin_family = AF_INET;

///	m_SockAddrIn.sin_port = htons ((short) GetSNMPTrapPort ());
	
	for (i = 0; ((bind (m_nUDPSocket, (struct sockaddr *)&m_SockAddrIn, sizeof (m_SockAddrIn)) < 0) && (i < 5)); i++)
	{
		//Bind error on traprcv.
	}

	//Normal return.
	return true;
}


//-------------------------------------------------------------------------------------
//
// void CSNMPInterface::InitTimeNow ()
//
//-------------------------------------------------------------------------------------
void CSNMPInterface::InitTimeNow ()
{
	agtbasetime = basetime = GetAbsoluteTime ();
///	(void) GetTOD(&bootTOD);
}


//-------------------------------------------------------------------------------------
//
// unsigned int CSNMPInterface::GetAbsoluteTime ()
//
//-------------------------------------------------------------------------------------
unsigned int CSNMPInterface::GetAbsoluteTime ()
{
///	struct _timeb t;

///	_ftime (&t);
///	return ((unsigned int) ((t.time * 100) + (t.millitm / 10)));
	return 1;
}


//-------------------------------------------------------------------------------------
//
// VarBind *CSNMPInterface::CreateVarBind ()
//
//-------------------------------------------------------------------------------------
VarBind *CSNMPInterface::CreateVarBind (CString& p_sName, CString& p_sType, CString& p_sValue)
{
	/*
	OID            *oid_ptr, *oid_ptr1;
	VarBind        *vb_ptr;
	SR_INT32        sl;
	SR_UINT32       ul;
	OctetString    *os_ptr;
	unsigned char   ip_addr_buffer[4];
	unsigned char   oidtype;
	SR_UINT32       temp_ip_addr;

	sl = (SR_INT32)0;
	ul = (SR_UINT32)0;

	oid_ptr = MakeOIDFromDot (name);
	if (oid_ptr == NULL)
	{
		DPRINTF((APWARN, "Cannot translate variable class: %s\n", name));
		exit(1);
	}

	if (strcmp(type, "-i") == 0) {
		
		if ((sscanf(value, INT32_HEX_FORMAT, &sl)) != 1) {
			if ((sscanf(value, INT32_FORMAT, &sl)) != 1) {
				DPRINTF((APTRACE, "CreateVarBind: unable to parse given arguments\n"));
				return NULL;
			}
		}
		
		oidtype = INTEGER_TYPE;
		vb_ptr = MakeVarBindWithValue(oid_ptr, (OID *) NULL, oidtype, (void *) &sl);
		FreeOID(oid_ptr);
		oid_ptr = NULL;
		return (vb_ptr);
	}

	if (strcmp(type, "-o") == 0) {
		if ((os_ptr = MakeOctetStringFromHex(value)) == NULL) {
			if ((os_ptr = MakeOctetStringFromText(value)) == NULL) {
				return NULL;
			}
		}
		oidtype = OCTET_PRIM_TYPE;
		vb_ptr = MakeVarBindWithValue(oid_ptr, (OID *) NULL, oidtype,
			(void *) os_ptr);
		FreeOID(oid_ptr);
		oid_ptr = NULL;
		os_ptr = NULL;		// do not free the octetstring, it is part of the vb.
		//end bug fix
		return (vb_ptr);
	}

	if (strcmp(type, "-D") == 0) {
		os_ptr = MakeOctetString((unsigned char *) value, (SR_INT32) strlen(value));
		if (os_ptr == NULL) {
			return NULL;
		}
		oidtype = OCTET_PRIM_TYPE;
		vb_ptr = MakeVarBindWithValue(oid_ptr, (OID *) NULL, oidtype,
			(void *) os_ptr);
		FreeOID(oid_ptr);
		oid_ptr = NULL;
		os_ptr = NULL;
		return (vb_ptr);
	}

	if (strcmp(type, "-d") == 0) {
		if ((oid_ptr1 = MakeOIDFromDot(value)) == NULL) {
			return NULL;
		}
		oidtype = OBJECT_ID_TYPE;
		vb_ptr = MakeVarBindWithValue(oid_ptr, (OID *) NULL, oidtype,
			(void *) oid_ptr1);
		FreeOID(oid_ptr);
		oid_ptr = NULL;
		oid_ptr1 = NULL;
		return (vb_ptr);
	}

	if (strcmp(type, "-a") == 0) {
		temp_ip_addr = ntohl(inet_addr(value));
		ip_addr_buffer[0] = (unsigned char) ((temp_ip_addr >> 24) & 0xff);
		ip_addr_buffer[1] = (unsigned char) ((temp_ip_addr >> 16) & 0xff);
		ip_addr_buffer[2] = (unsigned char) ((temp_ip_addr >> 8) & 0xff);
		ip_addr_buffer[3] = (unsigned char) (temp_ip_addr & 0xff);
		
		os_ptr = MakeOctetString(ip_addr_buffer, ((SR_INT32)4));
		
		oidtype = IP_ADDR_PRIM_TYPE;
		vb_ptr = MakeVarBindWithValue(oid_ptr, (OID *) NULL, oidtype,
			(void *) os_ptr);
		FreeOID(oid_ptr);
		oid_ptr = NULL;
		os_ptr = NULL;		// do not free the octetstring, it is part of the vb.
		// end bug fix
		return (vb_ptr);
	}

	if (strcmp(type, "-c") == 0) {
		if ((sscanf(value, UINT32_FORMAT, &ul)) != 1) {
			DPRINTF((APTRACE, "CreateVarBind: unable to parse given arguments\n"));
			return NULL;
		}
		oidtype = COUNTER_TYPE;
		vb_ptr = MakeVarBindWithValue(oid_ptr, (OID *) NULL, oidtype, (void *) &ul);
		FreeOID(oid_ptr);
		oid_ptr = NULL;
		return (vb_ptr);
	}

	if (strcmp(type, "-g") == 0) {
		if ((sscanf(value, UINT32_FORMAT, &ul)) != 1) {
			DPRINTF((APTRACE, "CreateVarBind: unable to parse given arguments\n"));
			return NULL;
		}
		oidtype = GAUGE_TYPE;
		vb_ptr = MakeVarBindWithValue(oid_ptr, (OID *) NULL, oidtype, (void *) &ul);
		FreeOID(oid_ptr);
		oid_ptr = NULL;
		return (vb_ptr);
	}

	if (strcmp(type, "-t") == 0) {
		
		if ((sscanf(value, UINT32_FORMAT, &ul)) != 1) {
			DPRINTF((APTRACE, "CreateVarBind: unable to parse given arguments\n"));
			return NULL;
		}
		
		oidtype = TIME_TICKS_TYPE;
		vb_ptr = MakeVarBindWithValue(oid_ptr, (OID *) NULL, oidtype, (void *) &ul);
		FreeOID(oid_ptr);
		oid_ptr = NULL;
		return (vb_ptr);
	}

	if (strcmp(type, "-N") == 0) {
		
		if ((sscanf(value, INT32_FORMAT, &sl)) != 1) {
			DPRINTF((APTRACE, "CreateVarBind: unable to parse given arguments\n"));
			return NULL;
		}
		
		oidtype = NULL_TYPE;
		vb_ptr = MakeVarBindWithValue(oid_ptr, (OID *) NULL, oidtype, (void *) NULL);
		FreeOID(oid_ptr);
		oid_ptr = NULL;
		return (vb_ptr);
	}
*/
	return (NULL);
}				/* CreateVarBind() */


//-------------------------------------------------------------------------------------
//
// bool CSNMPInterface::InitV2CLU ( const PARSER_RECORD_TYPE *rt[], int *rt_count) -- deprecated
// bool CSNMPInterface::InitV2CLU ()
//
//-------------------------------------------------------------------------------------
bool CSNMPInterface::InitV2CLU ()
{
	return true;
}


//-------------------------------------------------------------------------------------
//
// bool CSNMPInterface::ParseConfigFile ()
//
//-------------------------------------------------------------------------------------
bool CSNMPInterface::ParseConfigFile (CString& p_sFileName)
{
	/*
	int status = 0;
	CConfigFile *cfgp;
	char *cfgline = NULL, *cfgdata;
	int rti;
	int st;
	ValType *cfg_vals;
	int i, max_cfg_vals;
	int len;

	//Determine what the size of cfg_vals should be
	max_cfg_vals = 0;
	for (rti = 0; rt[rti] != NULL; rti++)
	{
		for (i=0; rt[rti]->type_table[i].type != -1; i++);
		if (i > max_cfg_vals)
		{
			max_cfg_vals = i;
		}
	}

	//allocate space for the configuration information
	cfg_vals = (ValType *) malloc(max_cfg_vals * sizeof(ValType));
	if (cfg_vals == NULL)
	{
		goto fail;
	}

	cfgline = (char *)malloc(CFGLINESIZE);
	if (cfgline == NULL)
	{
		goto fail;
	}

	//Open config file
	if (OpenConfigFile(filename, FM_READ, &cfgp) != FS_OK)
	{
		goto fail;
	}

	//Read records from config file
	while (!sr_feof(cfgp))
	{
		//Read a record
		if (!ReadConfigLine(cfgp, cfgline, CFGLINESIZE))
		{
			continue;
		}

		//Process it
		for (rti = 0; rt[rti] != NULL; rti++)
		{
			if (!strncmp(cfgline, rt[rti]->label, strlen(rt[rti]->label)))
			{
				break;
			}
		}
		if (rt[rti] == NULL)
		{
			DPRINTF((APWARN, "Ignoring unknown record type: %s\n", cfgline));
			status++;
			continue;
		}
		cfgdata = cfgline + strlen(rt[rti]->label);
		len = strlen(cfgline) - 1;
		if ((rt[rti]->type & 0x00f0) == PARSER_FORCE_INDEX_ORDER)
		{
			st = ProcessConfigRecordForcingIndexOrder(cfgp->linecount,
													  cfgdata,
													  rt[rti]->type_table,
													  rt[rti]->converters,
													  cfg_vals);
		}
		else
		{
			st = ProcessConfigRecord(cfgp->linecount,
									 cfgdata,
									 rt[rti]->type_table,
									 rt[rti]->converters,
									 cfg_vals);
		}
		for (;len >= 0; len--)
		{
			if (cfgline[len] == '\0')
			{
				cfgline[len] = ' ';
			}
		}
		if (st)
		{
			status++;
		}
		if (st == -1)
		{
			continue;
		}

		if ((rt[rti]->type & 0x000f) == PARSER_SCALAR)
		{
			FillInScalarVar(rt[rti], cfg_vals);
		}
		else if ((rt[rti]->type & 0x000f) == PARSER_TABLE)
		{
			if (CreateTableEntry(rt[rti], cfg_vals))
			{
				status++;
				FreeValTypes(rt[rti]->type_table, cfg_vals, -1);
			}
		}
		else
		{
			DPRINTF((APERROR, "%s: Bad PARSER_RECORD_TYPE\n", Fname));
			FreeValTypes(rt[rti]->type_table, cfg_vals, -1);
		}
	}

	//Close config file
	CloseConfigFile(cfgp);
	free(cfgline);
	free(cfg_vals);
	return true;

	fail:

	if (cfg_vals)
	{
		free(cfg_vals);
	}
	if (cfgline != NULL)
	{
		free(cfgline);
	}
	return false;
	*/
	return true;
}




//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//                 OID_TREE_ELEMENT class implementation                    
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------




//-------------------------------------------------------------------------------------
//
// Default constructor
//
//-------------------------------------------------------------------------------------
OID_TREE_ELEMENT::OID_TREE_ELEMENT ()
{
	m_IDValue = 0;
	m_sOidName.Empty ();
	m_sOidNumberStr.Empty ();
	m_nType = 0;
	m_nAccess = 0;
	m_pEnumer = NULL;
	m_pMibIndex = NULL;
	m_nRangeLower = -0x7fffffff;
	m_nRangeUpper = 0x7fffffff;
	m_nSizeLower = 0;
	m_nSizeUpper = 0xffffffff;
	m_pHashNext = NULL;
	m_pHashPrev = NULL;
	m_pParent = NULL;
	m_pNext = NULL;
	m_pPrev = NULL;
	m_pFirstDescendent = NULL;
}




//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//                 MibIndex class implementation                    
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------




//-------------------------------------------------------------------------------------
//
// Default constructor
//
//-------------------------------------------------------------------------------------
MibIndex::MibIndex ()
{
	m_sName.Empty ();
	m_bImplied = false;
	m_pNext = NULL;
}




//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//                 MIB_OID class implementation                    
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------




//-------------------------------------------------------------------------------------
//
// Default constructor
//
//-------------------------------------------------------------------------------------
MIB_OID::MIB_OID ()
{
	m_sName.Empty ();
	m_sNumber.Empty ();
	m_pHashNext = NULL;
	m_pNext = NULL;
}




//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//                 MIB_TREE class implementation                    
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------




//-------------------------------------------------------------------------------------
//
// Default constructor
//
//-------------------------------------------------------------------------------------
MIB_TREE::MIB_TREE ()
{
	int nIterator;

	m_sTreeName.Empty ();
	m_pPrefix = NULL;
	m_pNext = NULL;
	m_pPrev = NULL;
	m_pRootOid = NULL;

	for (nIterator = 0; nIterator < SNMP::MIB_HASH_TABLE_SIZE; nIterator++)
	{
		m_OidHashTable[nIterator] = NULL;
	}
}





//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//                 ENumer class implementation                    
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------




//-------------------------------------------------------------------------------------
//
// Default constructor
//
//-------------------------------------------------------------------------------------
ENumer::ENumer ()
{
	m_sName.Empty ();
	m_nNumber = 0;
	m_pNext = NULL;
}





//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//                 OctetString class implementation                    
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------




//-------------------------------------------------------------------------------------
//
// Default constructor
//
//-------------------------------------------------------------------------------------
OctetString::OctetString ()
{
	m_pOctetString = NULL;
	m_nLength = 0;
}


//-------------------------------------------------------------------------------------
//
// Overloaded class constructor
//
//-------------------------------------------------------------------------------------
OctetString::OctetString (int nLength)
{
	if (nLength > 0)
	{
		m_pOctetString = (unsigned char *)malloc (nLength * sizeof (unsigned char));
		m_nLength = nLength;
	}
	else
	{
		m_pOctetString = NULL;
		m_nLength = 0;
	}
}


#endif //USE_NEW_SNMP_INTERFACE
