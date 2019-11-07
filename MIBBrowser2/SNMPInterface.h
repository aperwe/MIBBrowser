/**
*	@file		SNMPInterface.h
*	@brief		Header file of classes that work as interface between
*				the application and the SNMP subsystem.
*				The use of CSNMPInterface class consists of one step only.
*				All you have to do to prepare the class instance to use
*				is to construct it. The default constructor does all
*				the required initialization.
*				Helper classes defined in the file: OID, OctetString, UInt64,
*				ObjectSyntax, VarBind, BulkPdu, NormPdu, TrapPdu, Pdu,
*				SnmpV1Message, SnmpV2Message, SnmpV3Message, SnmpMessage,
*				ENumer, MibIndex, OID_TREE_ELEMENT, MIB_TREE, MIB_OID.
*	@author		Artur Perwenis
*	@date		11-Jan-2002
*
*	@history
*	@item		11-Oct-2001		Artur Perwenis		First version.
*	@item		21-Jan-2002		Artur Perwenis		Moved additional types from the original code.
*/

#ifndef ___SNMP_INTERFACE_H__
#define ___SNMP_INTERFACE_H__

#include "StdAfx.h"

/**
*	@brief		Constant values used by the SNMPInterface.
*	@author		Artur Perwenis
*	@date		27-Jan-2002
*/
namespace SNMP
{
	///////////////////////////////////////////
	/// SNMP Research porting
	///////////////////////////////////////////
	enum Constants
	{
		MAX_OID_SIZE				= 128,
		MAX_OCTET_STRING_SIZE		= 2048,
		SNMP_PORT					= 161,
		SECS						= 5,
		RETRIES						= 10l,
		NAME_OID_HASH_SIZE			= 128,
		MIB_HASH_TABLE_SIZE			= NAME_OID_HASH_SIZE,
		/**
		*	@brief		IANA_ENTERPRISE_NUMBER:  This is the enterprise number assigned your company
		*				by the Internet Assigned Numbers Authority. If you do not change this, the SNMP Research,
		*				Inc. number will be used (99). This affects seclib/snmpv2.c
		*	@author		Artur Perwenis
		*	@date		30-Jan-2002
		*/
		IANA_ENTERPRISE_NUMBER		= 99,
		MAX_HOSTNAME_SIZE			= 64
	};

	//Moved from snmpid.h
	/* Algorithms for determining snmpID/snmpEngineID */
	#define SR_SNMPID_ALGORITHM_SIMPLE_AGT	((void *)0)
	#define SR_SNMPID_ALGORITHM_SIMPLE_MGR	((void *)1)


	//Moved from sr_snmp.h - to be deleted
	#define ul_value    u.ulval
	#define sl_value    u.slval
	#define os_value    u.osval
	#define oid_value   u.oidval
	#define uint64_value   u.uint64val
}

/**************************************************************************************
///////////////////////////////////////////////////////////////////////////////////////
///
/// Classes that replace the original old SNMPResearch code.
///
///////////////////////////////////////////////////////////////////////////////////////
**************************************************************************************/

/*
typedef int SR_INT32
typedef unsigned int SR_UINT32
*/


/**
*	@brief		A replacement class for typedef struct OID
*	@author		Artur Perwenis
*	@date		18-Jan-2002
*/
class OID
{
public:
	/**
	*	@brief		Original type: SR_INT32.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	int	m_nLength;
	/**
	*	@brief		Original type: SR_UINT32 *.
	*				This is a pointer to array of values of the OID, which make up
	*				the 1.2.1.4 construct.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	unsigned int *m_pnOid;
};


/**
*	@brief		A replacement class for typedef struct OctetString
*	@author		Artur Perwenis
*	@date		18-Jan-2002
*/
class OctetString
{
public:
	/**
	*	@brief	class constructor
	*/
	OctetString ();
	/**
	*	@brief	Overloaded class constructor
	*	@param	nLength		Length of the OctetString to produce.
	*/
	OctetString (int nLength);


public:
	/**
	*	@brief		The same.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	unsigned char	*m_pOctetString;
	/**
	*	@brief		Original type: SR_INT32.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	int				m_nLength;
};


/**
*	@brief		A replacement class for typedef struct UInt64
*	@author		Artur Perwenis
*	@date		18-Jan-2002
*/
class UInt64
{

public:
	/**
	*	@brief		Original type: SR_UINT32.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	unsigned int	big_end;
	/**
	*	@brief		Original type: SR_UINT32.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	unsigned int	little_end;
};


/**
*	@brief		A replacement class for typedef struct ObjectSyntax.
*	@author		Artur Perwenis
*	@date		18-Jan-2002
*/
class ObjectSyntax
{

public:
	/**
	*	@brief		The same.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	short type;
	/**
	*	@brief		Original type: union.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	union
	{
		/**
		*	@brief		Original type: SR_UINT32. Counter, Gauge, time_ticks.
		*	@author		Artur Perwenis
		*	@date		18-Jan-2002
		*/
		unsigned int ulval;
		/**
		*	@brief		Original type: SR_INT32. Simple num.
		*	@author		Artur Perwenis
		*	@date		18-Jan-2002
		*/
		int	sval;
		/**
		*	@brief		Original type: OctetString *. For OS, IpAddress, Opaque.
		*	@author		Artur Perwenis
		*	@date		18-Jan-2002
		*/
		class OctetString	*osval;
		/**
		*	@brief		Original type: OID *.
		*	@author		Artur Perwenis
		*	@date		18-Jan-2002
		*/
		class OID	*oidval;
		/**
		*	@brief		Original type: UInt64 *. Big unsigned integer, e.g. *Counter64.
		*	@author		Artur Perwenis
		*	@date		18-Jan-2002
		*/
		class UInt64	*uint64val;
	} u;
};


/**
*	@brief		A replacement class for typedef struct VarBind.
*	@author		Artur Perwenis
*	@date		18-Jan-2002
*/
class VarBind
{

public:
	/**
	*	@brief		Original type: SR_INT32.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	int	data_length;
	/**
	*	@brief		Original type: OID *.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	class OID	*name;
	/**
	*	@brief		The same.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	short	name_size;
	/**
	*	@brief		Original type: ObjectSyntax.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	class ObjectSyntax	value;
	/**
	*	@brief		The same.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	short	value_size;
	/**
	*	@brief		Original type: struct _VarBind *.
	*	@author		Artur Perwenis
	*	@date		18-Jan-2002
	*/
	class VarBind	*next_var;
};


/**
*	@brief		A replacement class for typedef struct BuldPdu
*	@author		Artur Perwenis
*	@date		21-Jan-2002
*/
class BulkPdu
{
public:
	/**
	*	@brief		Original type: SR_INT32.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	int	request_id;
	/**
	*	@brief		Original type: SR_INT32.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	int	non_repeaters;
	/**
	*	@brief		Original type: SR_INT32.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	int	max_repetitions;
};


/**
*	@brief		A replacement class for typedef struct NormPdu
*	@author		Artur Perwenis
*	@date		21-Jan-2002
*/
class NormPdu
{
public:
	/**
	*	@brief		Original type: SR_INT32.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	int	request_id;
	/**
	*	@brief		Original type: SR_INT32.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	int	error_status;
	/**
	*	@brief		Original type: SR_INT32.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	int	error_index;
};


/**
*	@brief		A replacement class for typedef struct TrapPdu
*	@author		Artur Perwenis
*	@date		21-Jan-2002
*/
class TrapPdu
{
public:
	/**
	*	@brief		Original type: OID *.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	OID	*enterprise;
	/**
	*	@brief		Original type: OctetString *.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	OctetString *agent_addr;
	/**
	*	@brief		Original type: SR_INT32.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	int	generic_trap;
	/**
	*	@brief		Original type: SR_INT32.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	int	specific_trap;
	/**
	*	@brief		Original type: SR_UINT32.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	unsigned int	time_ticks;
};


/**
*	@brief		A replacement class for typedef struct Pdu
*	@author		Artur Perwenis
*	@date		21-Jan-2002
*/
class Pdu
{
public:
	/**
	*	@brief		Original type: OctetString *.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	OctetString	*packlet;
	/**
	*	@brief		Original type: short.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	short	type;
	/**
	*	@brief		Original type: SR_INT32.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	int	num_varbinds;
	/**
	*	@brief		Original type: union.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	union
	{
		NormPdu	normpdu;
		TrapPdu	trappdu;
		BulkPdu	bulkpdu;
	} u;
	/**
	*	@brief		Original type: VarBind *.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	VarBind	*var_bind_list;
	/**
	*	@brief		Original type: VarBind *.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	VarBind *var_bind_end_ptr;
};


/**
*	@brief		A replacement class for typedef struct SnmpV1Message.
*	@author		Artur Perwenis
*	@date		21-Jan-2002
*/
class SnmpV1Message
{
public:
	/**
	*	@brief		Original type: OctetString *.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	OctetString	*community;
};


/**
*	@brief		A replacement class for typedef struct SnmpV2Message. V2c uses SnmpV1Message.
*	@author		Artur Perwenis
*	@date		21-Jan-2002
*/
class SnmpV2Message
{
public:
	/**
	*	@brief		Original type: SR_UINT32.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	unsigned int	mms;
	/**
	*	@brief		Original type: SR_UINT32.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	unsigned int	reportableFlag;
	/**
	*	@brief		Original type: SR_UINT32.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	unsigned int	spi;
	/**
	*	@brief		Original type: 
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	OctetString	*authSnmpId;
	/**
	*	@brief		Original type: 
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	OctetString	*identityName;
	/**
	*	@brief		Original type: 
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	int	authSnmpBoots;
	/**
	*	@brief		Original type: 
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	int	authSnmpTime;
	/**
	*	@brief		Original type: 
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	OctetString	*transportLabel;
	/**
	*	@brief		Original type: 
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	OctetString	*contextSnmpID;
	/**
	*	@brief		Original type: 
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	OctetString	*contextName;
	/**
	*	@brief		This member is optional depending on preprocessor constant definition. '#ifndef SR_UNSECURABLE'.
	*				Information from LCD.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	OctetString	*authKey;
	/**
	*	@brief		Original type: 
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	OctetString	*groupName;
};


/**
*	@brief		A replacement class for typedef struct SnmpV3Message.
*	@author		Artur Perwenis
*	@date		21-Jan-2002
*/
class SnmpV3Message
{
public:
	/**
	*	@brief		Original type: SR_UINT32.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	unsigned int	msgID;
	/**
	*	@brief		Original type: SR_UINT32.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	unsigned int	mms;
	/**
	*	@brief		Original type: SR_UINT32.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	unsigned int	reportableFlag;
	/**
	*	@brief		Original type: SR_UINT32.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	unsigned int	reportFlag;
	/**
	*	@brief		Original type: SR_UINT32.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	unsigned int	securityLevel;
	/**
	*	@brief		Original type: SR_UINT32.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	unsigned int	securityModel;
	/**
	*	@brief		Original type: 
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	OctetString	*authSnmpEngineID;
	/**
	*	@brief		Original type: 
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	int	authSnmpEngineBoots;
	/**
	*	@brief		Original type: 
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	int authSnmpEngineTime;
	/**
	*	@brief		Original type: 
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	OctetString	*userName;
	/**
	*	@brief		Original type: 
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	OctetString *userSecurityName;
	/**
	*	@brief		Original type: 
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	OctetString *contextSnmpEngineID;
	/**
	*	@brief		Original type: 
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	OctetString	*contextName;
	/**
	*	@brief		This member is optional depending on preprocessor constant definition. '#ifndef SR_UNSECURABLE'.
	*				Information from LCD.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	OctetString *authKey;
	/**
	*	@brief		This member is optional depending on preprocessor constant definition. '#ifndef SR_UNSECURABLE'.
	*				Information from LCD. Original type: SR_INT32.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	int	authProtocol;
	/**
	*	@brief		This member is optional depending on preprocessor constant definition. '#ifndef SR_UNSECURABLE'.
	*				Information from LCD. Original type: SR_INT32.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	int authLocalized;
};


/**
*	@brief		A message class that carries either of the three defined message versions.
*	@author		Artur Perwenis
*	@date		21-Jan-2002
*/
class SnmpMessage
{
	/**
	*	@brief		Original type: OctetString *.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	OctetString	*packlet;
	/**
	*	@brief		Original type: int.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	int	error_code;
	/**
	*	@brief		Original type: int.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	int	cached;
	/**
	*	@brief		Original type: SR_INT32.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	int	version;
	/**
	*	@brief		Original type: SR_INT32.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	int	wrapper_size;
	/**
	*	@brief		Original type: SR_INT32
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	int	max_vbl_size;
	/**
	*	@brief		A union holding either of the three defined message types.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	union
	{
		/**
		*	@brief		Also v2c.
		*	@author		Artur Perwenis
		*	@date		21-Jan-2002
		*/
		SnmpV1Message v1;
		SnmpV2Message v2;
		SnmpV3Message v3;
	} u;
	/**
	*	@brief		Original type: const unsigned char *.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	const unsigned char *wptr;
	/**
	*	@brief		Original type: const unsigned char *.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	const unsigned char *end_ptr;
	/**
	*	@brief		Original type: const unsigned char *.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	const unsigned char *new_end_ptr;
};


/**
*	@brief		Original type: typedef struct ENUMER.
*	@author		Artur Perwenis
*	@date		23-Jan-2002
*/
class ENumer
{
public:
	/**
	*	@brief	class constructor
	*/
	ENumer ();


public:
	/**
	*	@brief		Original variable: char *name.
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	CString	m_sName;
	/**
	*	@brief		Original variable: SR_INT32 number.
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	int	m_nNumber;
	/**
	*	@brief		Original type: struct enumer *next.
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	ENumer *m_pNext;
};


/**
*	@brief		A replacement for: typedef struct mib_index.
*	@author		Artur Perwenis
*	@date		23-Jan-2002
*/
class MibIndex
{
public:
	/**
	*	@brief	class constructor
	*/
	MibIndex ();


public:
	/**
	*	@brief		Original type: char *name.
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	CString m_sName;
	/**
	*	@brief		Original type: int implied.
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	bool	m_bImplied;
	/**
	*	@brief		Original type: struct mib_index *next.
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	MibIndex	*m_pNext;
};


/**
*	@brief		Original type: typedef struct OID_TREE_ELE
*	@author		Artur Perwenis
*	@date		22-Jan-2002
*/
class OID_TREE_ELEMENT
{
public:
	/**
	*	@brief	class constructor
	*/
	OID_TREE_ELEMENT ();


public:
	/**
	*	@brief		Original variable: SR_UINT32 sid_value. A branch from the parent.
	*	@author		Artur Perwenis
	*	@date		22-Jan-2002
	*/
	unsigned int	m_IDValue;
	/**
	*	@brief		Original variable: char *oid_name. English descriptor.
	*	@author		Artur Perwenis
	*	@date		22-Jan-2002
	*/
	CString		m_sOidName;
	/**
	*	@brief		Original variable: char *oid_number_str. Dotted form.
	*	@author		Artur Perwenis
	*	@date		22-Jan-2002
	*/
	CString		m_sOidNumberStr;
	/**
	*	@brief		Variable type.
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	short		m_nType;
	/**
	*	@brief		Access.
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	short		m_nAccess;
	/**
	*	@brief		Original variable: struct enumer *enumer. English<->enumer translations
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	ENumer		*m_pEnumer;
	/**
	*	@brief		Indices.
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	MibIndex	*m_pMibIndex;
	/**
	*	@brief		Valid sizes.
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	unsigned int	m_nSizeLower;
	/**
	*	@brief		Valid sizes.
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	unsigned int	m_nSizeUpper;
	/**
	*	@brief		Valid lengths
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	int	m_nRangeLower;
	/**
	*	@brief		Valid lengths
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	int m_nRangeUpper;
	/**
	*	@brief		Next member of hash collisions.
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	OID_TREE_ELEMENT	*m_pHashNext;
	/**
	*	@brief		Previous member of hash collisions.
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	OID_TREE_ELEMENT	*m_pHashPrev;
	/**
	*	@brief		Parent MIB variable.
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	OID_TREE_ELEMENT	*m_pParent;
	/**
	*	@brief		Next sibling.
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	OID_TREE_ELEMENT	*m_pNext;
	/**
	*	@brief		Previous sibling.
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	OID_TREE_ELEMENT	*m_pPrev;
	/**
	*	@brief		First child node.
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	OID_TREE_ELEMENT	*m_pFirstDescendent;
};



/**
*	@brief		Original type: typedef struct MIB_TREE.
*	@author		Artur Perwenis
*	@date		22-Jan-2002
*/
class MIB_TREE
{
public:
	/**
	*	@brief	class constructor
	*/
	MIB_TREE ();


public:
	/**
	*	@brief		Textual name of this MIB tree. Original name: char *name.
	*	@author		Artur Perwenis
	*	@date		22-Jan-2002
	*/
	CString	m_sTreeName;
	/**
	*	@brief		OIC prefix to the root of the tree. Original name: OID *prefix.
	*	@author		Artur Perwenis
	*	@date		22-Jan-2002
	*/
	OID	*m_pPrefix;
	/**
	*	@brief		Pointer to the next MIB tree.
	*	@author		Artur Perwenis
	*	@date		22-Jan-2002
	*/
	MIB_TREE	*m_pNext;
	/**
	*	@brief		Pointer to the previous MIB tree.
	*	@author		Artur Perwenis
	*	@date		22-Jan-2002
	*/
	MIB_TREE	*m_pPrev;
	/**
	*	@brief		Root OID of the tree.
	*	@author		Artur Perwenis
	*	@date		22-Jan-2002
	*/
	OID_TREE_ELEMENT	*m_pRootOid;
	/**
	*	@brief		Hashing table for this tree.
	*	@author		Artur Perwenis
	*	@date		22-Jan-2002
	*/
	OID_TREE_ELEMENT	*m_OidHashTable[SNMP::MIB_HASH_TABLE_SIZE];
};


/**
*	@brief		A replacement class for struct MIB_OID.
*	@author		Artur Perwenis
*	@date		23-Jan-2002
*/
class MIB_OID
{
public:
	/**
	*	@brief	class constructor
	*/
	MIB_OID ();


public:
	/**
	*	@brief		Original name: char *name.
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	CString	m_sName;
	/**
	*	@brief		Original name: char *number.
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	CString m_sNumber;
	/**
	*	@brief		Original name: struct MIB_OID *hash_next.
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	MIB_OID	*m_pHashNext;
	/**
	*	@brief		NEW FIELD. Added to fix an error in the code with hashing table.
	*	@author		Artur Perwenis
	*	@date		28-Jan-2002
	*/
	MIB_OID *m_pNext;
};


/**
*	@brief		A class holding the information about the configuration file.
*	@author		Artur Perwenis
*	@date		26-Jan-2002
*/
class CConfigFile : public CStdioFile
{
public:
	enum EValues
	{
		CF_BUFFERSIZE		= 1024,
		CF_FIELDSIZE		= 255,
		CF_SMALLFIELDSIZE	= 32
	};
	/**
	*	@brief		Line counter
	*	@author		Artur Perwenis
	*	@date		26-Jan-2002
	*/
	int		m_nLineCount;
	/**
	*	@brief		Part of the MIBFromFileInfoSt structure.
	*	@author		Artur Perwenis
	*	@date		27-Jan-2002
	*/
	CString m_sBuffer;
	char	m_sName[CF_FIELDSIZE];
	char	m_sNumber[CF_FIELDSIZE];
	char	m_sType[CF_SMALLFIELDSIZE];
	char	m_sAccess[CF_SMALLFIELDSIZE];
	char	m_sSizeRange[CF_FIELDSIZE];
	char	m_sEnumName[CF_FIELDSIZE];
	char	m_sIndexName[CF_FIELDSIZE];
};



/**
*	@brief		A class acting as an interface between the application and the SNMP subsystem.
*				InitializeInterface () member must be called prior to calling any of the EntryPoint_
*				methods.
*	@author		Artur Perwenis
*	@date		18-Jan-2002
*/
class CSNMPInterface : public CObject
{

//////////////
// Methods
//////////////

public:
	/**
	*	@brief	class constructor
	*/
	CSNMPInterface ();
	/**
	*	@brief	class destructor
	*/
	~CSNMPInterface ();

	/**
	*	@brief		Communication types. GetOne uses SNMP_COMMUNICATIONS.
	*				This parameter type is passed to InitializeUtility ().
	*	@author		Artur Perwenis
	*	@date		19-Jan-2002
	*/
	enum ECommunicationType
	{
		SNMP_COMMUNICATIONS =	1,
		TRAP_SEND_COMMUNICATIONS,
		TRAP_RCV_COMMUNICATIONS
	};
	enum EAlgorithmType
	{
		SR_SNMPID_ALGORITHM_SIMPLE	= 0
	};
	/**
	*	@brief		Supported SNMP protocol versions.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	enum ESNMPVersion
	{
		SR_SNMPv1_VERSION = 0,	// "-v1" command line option
		SR_SNMPv2c_VERSION,		// "-v2c" command line option
		SR_SNMPv2_VERSION,		// "-v2" command line option
		SR_SNMPv3_VERSION		// "-v3" command line option
	};
	/**
	*	@brief		Moved from sr_snmp.h file.
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
	enum EContexts
	{
		GET_REQUEST_TYPE		= 0xA0,
		GET_NEXT_REQUEST_TYPE	= 0xA1,
		GET_RESPONSE_TYPE		= 0xA2,
		SET_REQUEST_TYPE		= 0xA3,
		TRAP_TYPE				= 0xA4,
		RESPONSE_TYPE			= GET_RESPONSE_TYPE,
		GET_BULK_REQUEST_TYPE	= 0xA5,
		INFORM_REQUEST_TYPE		= 0xA6,
		SNMPv2_TRAP_TYPE		= 0xA7,
		REPORT_TYPE				= 0xA8
	};
	/**
	*	@brief		Symbols for selecting collision behavior when loading/merging MIBs.
	*	@author		Artur Perwenis
	*	@date		26-Jan-2002
	*/
	enum ECollisionType
	{
		SR_COL_REPLACE,
		SR_COL_DISCARD
	};
	/**
	*	@brief		File open modes.
	*	@author		Artur Perwenis
	*	@date		26-Jan-2002
	*/
	enum EFileOpenMode
	{
		FM_READ			=	1,	// read the file
		FM_WRITE		=	2	// write the file
	};
	/**
	*	@brief		Symbols for controlling MIB tree traversal behaviour.
	*	@author		Artur Perwenis
	*	@date		26-Jan-2002
	*/
	enum ETraverseType
	{
		BEHAVIOUR_REJECT_PARENT,
		BEHAVIOUR_ACCEPT_LEAF_PARENT,
		BEHAVIOUR_ACCEPT_ANY_PARENT,
	};
	/**
	*	@brief		Three-state value type.
	*	@author		Artur Perwenis
	*	@date		26-Jan-2002
	*/
	enum EThreeStatesBoolean
	{
		SUCCESS,			//Command successful
		ERROR_NO_FAIL,		//Error indicator, but not failure
		FAILURE				//Command failed.
	};
	/**
	*	@brief		The parser state in ReadMIBFileEntry () method.
	*	@author		Artur Perwenis
	*	@date		26-Jan-2002
	*/
	enum EParserState
	{
		PS_NONE,
		PS_COMMENT,
		PS_ENUMER,
		PS_INDEX
	};
	/**
	*	@brief		Table used for conversion of type-strings from snmpinfo.dat.
	*	@author		Artur Perwenis
	*	@date		27-Jan-2002
	*/
	enum EOidType
	{
		UNKNOWN_TYPE				=	0,
		NULL_TYPE					=	0x05,
		NON_LEAF_TYPE				=	0x84,
		AGGREGATE_TYPE				=	0x85,
		DISPLAY_STRING_TYPE			=	0x80,
		OCTET_PRIM_TYPE				=	0x04,
		PHYS_ADDRESS_TYPE			=	0x81,
		NETWORK_ADDRESS_TYPE		=	0x82,
		IP_ADDR_PRIM_TYPE			=	0x40,
		OBJECT_ID_TYPE				=	0x06,
		TIME_TICKS_TYPE				=	0x43,
		INTEGER_TYPE				=	0x02,
		GAUGE_TYPE					=	0x42,
		COUNTER_TYPE				=	0x41,
		SERVICES_TYPE				=	0x83,

		COUNTER_32_TYPE				=	COUNTER_TYPE,
		GAUGE_32_TYPE				=	GAUGE_TYPE,
		COUNTER_64_TYPE				=	0x46,
//		INTEGER_TYPE				=	
//		OCTET_PRIM_TYPE				=	
//		GAUGE_32_TYPE				=
	};
	/**
	*	@brief		Table used for conversion of access-type-strings from snmpinfo.dat.
	*	@author		Artur Perwenis
	*	@date		27-Jan-2002
	*/
	enum EAccessType
	{
		UNKNOWN_ACCESS					=	0,
		READ_ONLY_ACCESS				=	0x01,
		READ_WRITE_ACCESS				=	0x02,
		READ_CREATE_ACCESS				=	0x03,
		NOT_ACCESSIBLE_ACCESS			=	0x04,
		ACCESSIBLE_FOR_NOTIFY_ACCESS	=	0x04
	};
	/**
	*	@brief	An entry point function to perform the GetOne functionality.
	*	@param	p_sOutputBuffer	Where to store command results.
	*	@param	p_sLeaf	Leaf name to get.
	*	@return				Command status.
	*/
	bool EntryPoint_GetOne (CString& p_sOutputBuffer, const CString& p_sLeaf);
	/**
	*	@brief	Sets the target component IP address.
	*	@param	p_sTargetComponent	The IP address to set.
	*/
	void SetTargetComponent (CString& p_sTargetComponent);
	/**
	*	@brief	This function initializes the interface data for operation. Called from the constructor.
	*	@return		boolean: True if interface successfully initialized. Otherwise false.
	*/
	bool InitializeInterface ();

protected:
	/**
	*	@brief	Routines to initialize the new data structures that will speed name to OID and OID to name
	*			translations at the expense of memory and initialization time.
	*/
	void InitNewOIDRoutines ();
	/**
	*	@brief	InitNameOidHashTable (). Called from within InitNewOIDRoutines ().
	*/
	void InitNameOidHashTable ();
	/**
	*	@brief	Calculates hash value from the supplied name.
	*	@param	p_sName		Name to calculate hash for.
	*	@return				Hash value for the supplied name.
	*/
	int CalculateHashFromName (const CString& p_sName);
	/**
	*	@brief	
	*   This routine is called to create a library form object identifier
	*   from an character string.  The string input is usually in the
	*   format "integer.integer.integer...."  (i.e.,
	*   "1.3.6.1.2.1.1.1.0"), but can be shortened by using the names as
	*   they appear in RFC 1157 (i.e., "sysDescr.0").  It returns a
	*   pointer to a malloc'ed data structure containing the internal
	*   library representation for an object identifier.  This identifier
	*   can then be used in calls to make_varbind() and MakePdu() (in the
	*   case of traps).  This malloc'ed data structure will be free'ed by
	*   calls to FreePdu() after the pointer has been used.
	*
	* The former name (pre-snmp12.1.0.0 release) was make_obj_id_from_dot().
	*
	*	@param	p_sDot		Dotted form string to be converted.
	*	@return				A pointer to data structure containing the internal
	*						library representation for an object identifier.
	*/
	OID	*MakeOIDFromDot (const CString& p_sDot);
	/**
	*	@brief	MakeOIDFragFromDot:
	*			This routine is called to create a library form object identifier from a character string.
	*	@param	p_sDot		The string input is usually in format "integer.integer.integer...."
	*						(i.e., "1.3.6.1.2.1.1.1.0"), but can be shortened by using the names as they appear
	*						in RFC 1157 (i.e., "sysDescr.0").
	*	@return				Pointer to data structure containing the internal library representation for an object
	*						identifier. This identifier can then be used in calls to make_varbind() and MakePdu()
	*						(in the case of traps). This data structure will be free'ed by calls to FreePdu()
	*						after the pointer has been used.
	*/
	OID	*MakeOIDFragFromDot (const CString& p_sDot);
	/**
	*	@brief	
	*	This routine is called to convert an Object Identifier library
	*   construct into a dot notation character string, usually for us in
	*   a human interface.  The dot-notation output is the usual form
	*   (1.2.3.4.1.2.1.1) with the a MIB name subsituted for the most
	*   possible sub-identifiers starting from the left
	*   (1.3.6.1.2.1.1.1.0 becomes sysDescr.0).  The MIB names included
	*   in the library are found in the mib_oid_table in snmp-mib.h.
	*   This include file is used in the compilation of oid_lib.c, part
	*   of the libsnmpuser.a library and need not be included in
	*   applications.
	*
	* The former name (pre-snmp12.1.0.0 release) was make_dot_from_obj_id().
	*
	*	@param	*p_pOid		<TODO>
	*	@param	p_sBuffer	<TODO>
	*	@return				<TODO>
	*/
	bool MakeDotFromOID (const OID *p_pOid, CString& p_sBuffer);
	/**
	*	@brief
	* The former name (pre-snmp12.1.0.0 release) was get_str_from_sub_ids().
	*	@param	p_sBuffer		Target string, which will contain the string.
	*	@param	*p_psid			<TODO>
	*	@param	p_nSidCounter	<TODO>
	*	@return					Operation status.
	*/
	bool GetStringFromSubIds (CString& p_sBuffer, unsigned int *p_psid, int p_nSidCounter);
	/**
	*	@brief	This function replaces the original TypeStringToShort ()
	*	@param	p_sType		Type string to be mapped.
	*	@return				Mapped value. Value EOidType::UNKNOWN_TYPE is returned when string is not recognized.
	*/
	CSNMPInterface::EOidType MapStringToOidType (const CString& p_sType);
	/**
	*	@brief	This function replaces the original AccessStringToShort ()
	*	@param	p_sAccess	Access type string to be mapped.
	*	@return				Mapped value. Value EAccessType::UNKNOWN_ACCESS is returned when string is not recognized.
	*/
	CSNMPInterface::EAccessType MapStringToAccessType (const CString& p_sAccess);


private:
	/**
	*	@brief	Function ported from SNMPResearch Inc. code.
	*			Here are the example:
	*		Function declaration:
	*	int InitializeUtility (ECommunicationType type, int *argc, char *argv[])
	*		Call to the function.
	*	if (InitializeUtility (SNMP_COMMUNICATIONS, &argc, argv, NULL, NULL, NULL))
	*
	*	@param	type		Type of communication we're preparing for.
	*	@return				Initialization status. true = success; false = failure.
	*/
	bool InitializeUtility (ECommunicationType type);
	/**
	*	@brief	SrGetSnmpID:  Determines an engine's snmpID or snmpEngineID value.
	*	@param	p_eAlgorithm	<TODO>
	*	@param	*data			<TODO>
	*	@return					<TODO>
	*/
	OctetString *SrGetSnmpID (EAlgorithmType p_eAlgorithm, void *data);
	/**
	*	@brief	This routine produces a library OctetString construct from a passed byte-string and length.
	*			This construct is usually passed to other library calls, such as MakeVarBindWithNull()
	*			or MakeAuthentication(), and linked into a larger library construct of an ASN.1 entity.
	*			FreeOctetString() recovers all memory malloc'ed by MakeOctetString, but should not be used if
	*			the octetstring is passed to another library routine. The free_*() counterparts to those routine
	*			free up all memory that has been linked to the higher level ASN.1 structure.
	*			DLB/DBL 11/2/95 - if string is null, fill with 0's; before we dumped core.
	*	@param	*p_pString	<TODO>
	*	@param	p_nLength	<TODO>
	*	@return				<TODO>
	*/
	OctetString *MakeOctetString (const unsigned char *p_pString, int p_nLength);
	/**
	*	@brief	This routine will attempt to determine the local IP address of a device, and return this value
	*			as an unsigned 32-bit integer. Returned in host byte order.
	*	@return				IP address.
	*/
	unsigned long GetLocalIPAddress ();
	/**
	*	@brief	
	*   This routine is called to create the initial header block for
	*   building the SNMP ASN.1 data structure, which upon completion is
	*   used to build the actual SNMP packet.  It returns a pointer to a
	*   malloc'ed data structure of type Pdu:
	*
	*   typedef struct _Pdu {
	*     OctetString *packlet;
	*     short type;
	*     union {
	*       NormPdu normpdu;
	*       TrapPdu trappdu;
	*     } u;
	*     VarBind *var_bind_list;
	*     VarBind *var_bind_end_ptr;
	*   } Pdu;
	*
	*   The type is one of GET_REQUEST_TYPE, GET_NEXT_REQUEST_TYPE,
	*   GET_RESPONSE_TYPE, SET_REQUEST_TYPE, GET_BULK_REQUEST,
	*   INFORM_REQUEST_TYPE, SNMPv2_TRAP_TYPE, or TRAP_TYPE.  The request
	*   ID is the id number assigned to the particular packet by the
	*   application.  Since the application is UDP based, retry is solely
	*   controlled by the network management application.  The
	*   error_status is set to other than 0 in GET_RESPONSE_TYPE only to
	*   indicate that this response is in reply to a bad request.  The
	*   error_index is used only by GET_RESPONSE_TYPE and points to the
	*   var-bind entry in the PDU that offends the agent.  The enterprise
	*   is used by the TRAP_TYPE pdu and is an object identifier
	*   associated with the trap generating entity.  The agent_addr is
	*   used by the TRAP_TYPE pdu and consists of an octet string
	*   containing the IP address of the trap generating entity.  The
	*   generic_trap and specific_trap are used by the TRAP_TYPE pdu and
	*   consist of integers that indicate which type of trap this PDU
	*   represents.  The time_ticks is the TRAP_TYPE emiting entity's
	*   sense of time since the agent has restarted.
	*
	*   This routine is called once for each packet to be generated.  The
	*   pdu pointer is then passed to the routine LinkVarBind repeatedly
	*   to string var_binds into the packet.  Build_pdu() is then called
	*   to perform the ASN.1 encoding of the pdu and place that result in
	*   the pdu pointer's *packlet field.  After the packlet has been
	*   wrapped in an authentication envelope, it is free'ed by passing
	*   the pointer to FreePdu().
	*
	*
	*   The former name (pre-snmp12.1.0.0 release) was make_pdu().
	*	@param	p_Context	<TODO>
	*	@param	p_RequestId	This should be passed a global request ID (unique?).
	*	@return				A pointer to data structure of type Pdu.
	*/
	Pdu *MakePdu (EContexts p_Context, int p_RequestId);
	/**
	*	@brief
	* This function will process a single command line option.  If a valid option
	* is found, it returns 1.  If there were no valid options, it returns 0.
	*	In the ported version this function does actually nothing and always returns true.
	*	@return				True or false.
	*/
	bool process_command_line_option ();
	/**
	*	@brief	
	* InitUtilFileNames: initialize the filenames with the directory from
	*            the environment variable SR_MGR_CONF_DIR.  If the variable
	*            is missing or "too long," then the prefix (and hence the
	*            directory) is set to /etc.  "Too long" is the
	*            FNBUFSIZ minus the maximum length of a config file
	*            name.  As of 3/3/93, the longest config file name was 16,
	*            so 17 was arbitrarily chosen.
	*
	* The former name (pre-snmp12.1.0.0 release) was init_util_fnames().
	*/
	void InitUtilFileNames ();
	/**
	*	@brief
	* NewMIBFromFile:
	*
	* This is provided for compatibility.  It first frees all MIB trees
	* attached to the default MIB tree, and then calls ReadMIBFile with
	* a NULL for mib_tree and SR_COL_REPLACE for collision.
	*	@param	p_sFileName	File name to load
	*	@return				Load status.
	*/
	bool NewMIBFromFile (const CString& p_sFileName);
	/**
	*	@brief	free tree
	*	@param	*p_pKillTree	MIB tree to be deleted.
	*/
	void FreeMIBTree (MIB_TREE *p_pKillTree);
	/**
	*	@brief
	*	CreateMIBTree:
	*		Creates a new MIB tree, with no nodes.  The name and prefix are cloned.
	*		Note that the pointer returned should be treated as a token, and the
	*		underlying data structure should be accessed only via API calls.
	*	@param	&p_TreeName		Name of the tree.
	*	@param	*p_pOidPrefix	Tree prefix.
	*	@return					Constructed tree.
	*/
	MIB_TREE *CreateMIBTree (const CString &p_TreeName, const OID *p_pOidPrefix);
	/**
	*	@brief	MergeMIBFromFile:
	*			This is provided for compatibility.  It calls ReadMIBFile with
	*			a NULL for mib_tree and SR_COL_REPLACE for collision.
	*	@param	p_sFileName	Name of the file to load.
	*	@return				Load status.
	*/
	bool MergeMIBFromFile (const CString& p_sFileName);
	/**
	*	@brief	Creates MIB_OID table and assigns it to the m_MIBOidTable member.
	*			This function should be called only after a data file has been read and parsed.
	*	@author		Artur Perwenis
	*	@date		28-Jan-2002
	*	@return				Returns true if successful, or false on failure.
	*/
	bool CreateMIBOidTable ();
	/**
	*	@brief ReadMIBFile:
	*	This function reads a file containing MIB information and merges this info into
	*	a MIB tree (specified by the mib_tree argument). If the mib_tree argument
	*	is NULL, the info is read into the default MIB tree. The collision argument
	*	can take one of two values. If it is SR_COL_REPLACE, entries in the file will
	*	replace entries in the tree on a collision. If it is SR_COL_DISCARD, entries
	*	in the file will be discarded on a collision.
	*	@param	p_sFileName			File name to load.
	*	@param	*p_pMIBTree			MIB tree to read the file into.
	*	@param	p_eCollisionType	Collision behaviour.
	*	@return						True on success, false on failure.
	*/
	bool ReadMIBFile (const CString& p_sFileName, MIB_TREE *p_pMIBTree, ECollisionType p_eCollisionType);
	/**
	*	@brief	ReadMIBFileEntry:
	*	This function reads entries from a MIB file, and returns an OID_TREE_ELE
	*	structure in the otepp parameter.  The otepp parameter should contain a
	*	pointer to the previously read entry, or NULL if this is the first
	*	call during processing of a particular file.
	*
	*	There are currently three types of entries which can be read.  The first
	*	type is a new MIB variable definition, in which case the context of otepp
	*	are discarded and a new OID_TREE_ELE structure is returned.  The second
	*	and third types of entries are lists of integer/english enumerations,
	*	and lists of index variables.  When these are encountered, they are
	*	simply filled into the current OID_TREE_ELE pointed to by otepp.
	*
	*	@param	*p_pConfigFile	<TODO>
	*	@param	**p_pOIDTreeElement	<TODO>
	*	@return		SUCCESS on success; ERROR_NO_FAIL on EOF or warning; FAILURE on failure.
	*/
	EThreeStatesBoolean ReadMIBFileEntry (CConfigFile *p_pConfigFile, OID_TREE_ELEMENT **p_pOIDTreeElement);
	/**
	*	@brief	AddOTEToMibTree:
	*	Adds an OID_TREE_ELE node to a MIB tree. If collision is set to SR_COL_DISCARD, the return value is
	*	the value of ote if the node was successfully inserted, and NULL if there was a collision.
	*	@param	*p_pMIBTree				MIBTree to add the element to.
	*	@param	*p_pOIDTreeElement		Element to be added.
	*	@param	p_eCollisionType		Type of collision to handle.
	*	@return			If collision is set to SR_COL_REPLACE, the return value is the value of ote if
	*					the node was inserted without any collision, and if there is a collision, the return
	*					value is the node with which the collision occured. In this case, the contents of the
	*					two nodes will have been swapped, and the caller should call ReleaseOID_TREE_ELE.
	*/
	OID_TREE_ELEMENT *AddOTEToMIBTree (MIB_TREE *p_pMIBTree, OID_TREE_ELEMENT *p_pOIDTreeElement, ECollisionType p_eCollisionType);
	/**
	*	@brief	Clone the OID pointed to by oid_ptr and return the pointer to
	*			the cloned OID.
	*	@param	p_Oid		The OID to be copied.
	*	@return				The OID copy.
	*/
	OID *CloneOID (const OID& p_Oid);
	/**
	*	@brief
	* MakeOID:	This routine produces a library Object Identifier construct from a passed 
	*			sub-identifier array and length. The sub-identifier array is an array
	*			of unsigned long integers, with each element of the array corresponding
	*			to the value of each integer at each position in the dot-notation display
	*			of an object identifier.
	*			For example, the Object Identifier 1.3.6.1.2.1.1 would have a value
	*			1 in sid_array[0], 3 in sid_array[1], etc. length is the number of 
	*			sub-identifier elements present in the array. The construct returned can
	*			be free'ed by passing the pointer to FreeOID(). This is usually
	*			unnecessary as the construct is most often passed to another library
	*			routine for inclusion in a large ASN.1 construct and that library routine's
	*			free'ing counterpart will do the memory recovery.
	*	@param	p_pOidArray[]	Array of OID values.
	*	@param	p_Length		Length of this OID value array.
	*	@return					OID object.
	*/
	OID *MakeOID (unsigned int p_pOidArray[], int p_Length);
	/**
	*	@brief	InitializeIO: does initialization and opens udp connection.
	*	@param	&p_sTargetHostName	This can be an IP address (xxx.xxx.xxx.xxx).
	*	@return				Initialization status.
	*/
	bool InitializeIO (const CString &p_sTargetHostName);
	/**
	*	@brief	InitializeTrapIO: does initialization and opens udp connection.
	*	@param	&p_sTargetHostName	This can be an IP address (xxx.xxx.xxx.xxx).
	*	@return				Initialization status.
	*/
	bool InitializeTrapIO (const CString &p_sTargetHostName);
	/**
	*	@brief	InitializeTrapRcvIO: does initialization and opens udp connection 
	*	@return				Initialization status.
	*/
	bool InitializeTrapRcvIO ();
	/**
	*	@brief	If the environment variable SR_SNMP_ENV_NAME is set, then use that as the port to send to
	*			when sending SNMP requests. Otherwise, if the function getservbyname(SR_SNMP_SVC_NAME, "udp")
	*			returns a valid port (from /etc/services or equivalent database), use that. Otherwise use the
	*			#define SNMP_PORT if it exists, otherwise, use 161.
	*	@return		Returns the port in HOST byte order.
	*/
	unsigned int GetSNMPPort ();
	/**
	*	@brief	Initializes the time with the current time.
	*/
	void InitTimeNow ();
	/**
	*	@brief	Gets the current absolute time.
	*	@return				Absolute time value.
	*/
	unsigned int GetAbsoluteTime ();
	/**
	*	@brief	Creates a VarBind.
	*	@param	p_sName		<TODO>
	*	@param	p_sType		<TODO>
	*	@param	p_sValue	<TODO>
	*	@return				Created VarBind structure.
	*/
	VarBind *CreateVarBind (CString& p_sName, CString& p_sType, CString& p_sValue);
	/**
	*	@brief	Either InitV2CLU or InitV3CLU should be available depending on which
	*			protocol version is supported (V2 is the default).
	*	@param	*rt[]		<TODO>
	*	@param	*rt_count	<TODO>
	*	@return				Initialization status.
	*/
//	int InitV2CLU ( const PARSER_RECORD_TYPE *rt[], int *rt_count);
	bool InitV2CLU ();
	/**
	*	@brief	parse configuration file
	*	@param	p_sFileName	Name of the config file.
	*	@param	*rt[]		<TODO>
	*	@return				Parsing status.
	*/
//	bool ParseConfigFile (CString& p_sFileName, const PARSER_RECORD_TYPE *rt[]);
	bool ParseConfigFile (CString& p_sFileName);
	/**
	*	@brief	This routine opens a config file and keeps information about it.
	*	@param	p_sFileName	Name of the configuration file.
	*	@param	p_eOpenMode		Desired open mode.
	*	@return					Constructed configuration file object.
	*/
	CConfigFile *OpenConfigFile (const CString& p_sFileName, EFileOpenMode p_eOpenMode);

//////////////
// Members
//////////////

public:

protected:
	/**
	*	@brief	Holds the interface initialization status.
	*/
	bool	m_bInterfaceInitialized;
	/**
	*	@brief	Holds the initialization status for OID building routines.
	*/
	bool	m_bInitNewOIDRoutines;

private:
	/**
	*	@brief		The IP address string used to locate the element when executing command.
	*				This member replaces the global util_dest. Usually this is
	*				the IP address of the target system where NE is located.
	*	@author		Artur Perwenis
	*	@date		19-Jan-2002
	*/
	CString		m_sIPAddress;
	/**
	*	@brief		This member holds the IP address of the local host in binary notation.
	*	@author		Artur Perwenis
	*	@date		30-Jan-2002
	*/
	unsigned long m_nLocalIPAddress;
	/**
	*	@brief		The Community address string used when executing command.
	*				This member replaces the global util_handle. Usually it holds
	*				the 'public' value representing the name of community if
	*				the SR_SNMPv1_VERSION protocol is used, or [THIS IS NOT YET SUPPORTED]
	*				it contains the user name when SR_SNMPv2_VERSION or above is used.
	*	@author		Artur Perwenis
	*	@date		19-Jan-2002
	*/
	CString		m_sCommunity;
	/**
	*	@brief		Default context ID. Originally the variable was defaultContextSnmpID and of type OctetString *.
	*	@author		Artur Perwenis
	*	@date		22-Jan-2002
	*/
	CString		m_sDefaultContextSnmpID;
	/**
	*	@brief		Default context. Originally the variable was defaultContext and of type OctetString *.
	*	@author		Artur Perwenis
	*	@date		22-Jan-2002
	*/
	CString		m_sDefaultContext;
	/**
	*	@brief		The structure used by old SNMPResearch Inc. code.
	*	@author		Artur Perwenis
	*	@date		19-Jan-2002
	*/
//	SnmpLcd snmp_lcd;
	/**
	*	@brief		Originally it was an extern struct of type SNMPLcd
	*	@author		Artur Perwenis
	*	@date		21-Jan-2002
	*/
//	struct
//	{
//		int version_supported;
//	} snmp_lcd;
	int m_nSeconds;
	int m_nRetries;
	int m_nTRetries;
	/**
	*	@brief		This parameter is set in InitializeUtility ().
	*	@author		Artur Perwenis
	*	@date		25-Jan-2002
	*/
	ECommunicationType m_eCommunicationType;
	/**
	*	@brief		Originally it was extern int variable of name util_version.
	*				This indicates which version of SNMP protocol the utility
	*				is capable of handling. This should always be equal to
	*				SR_SNMPv1_VERSION, because this is selected via command line switch.
	*	@author		Artur Perwenis
	*	@date		22-Jan-2002
	*/
	ESNMPVersion	m_eUtilVersion;
	/**
	*	@brief		Original variable name: fn_snmpinfo_dat.
	*	@author		Artur Perwenis
	*	@date		22-Jan-2002
	*/
	CString			m_sSNMPInfoFile;
	/**
	*	@brief		Original variable name: fn_mgr_file.
	*	@author		Artur Perwenis
	*	@date		22-Jan-2002
	*/
	CString			m_sMGRFile;
	/**
	*	@brief		This member is probably used when no hashing is used. [THEREFORE SHOULD BE REMOVED].
	*	@author		Artur Perwenis
	*	@date		27-Jan-2002
	*/
	MIB_OID	*m_pMIBOidTable;
	/**
	*	@brief		Hash table used for fast OID to name and name to OID translations.
	*	@author		Artur Perwenis
	*	@date		27-Jan-2002
	*/
	MIB_OID *m_NameOidHashTable[SNMP::NAME_OID_HASH_SIZE];
	/**
	*	@brief		Original name: int fd.
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	int	m_nUDPSocket;
	/**
	*	@brief		Socket address, internet style.
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	sockaddr_in m_SockAddrIn;
	/**
	*	@brief		Socket address, internet style.
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	sockaddr_in m_SockAddrFrom;
	/**
	*	@brief		Original variable name: hostent *hp; This struct is defined in winsock.h.
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	hostent	*m_pHostEntry;
	/**
	*	@brief		agtbasetime and basetime are initialized by InitTimeNow () member.
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	unsigned int	agtbasetime;
	/**
	*	@brief		agtbasetime and basetime are initialized by InitTimeNow () member.
	*	@author		Artur Perwenis
	*	@date		23-Jan-2002
	*/
	unsigned int	basetime;
	/**
	*	@brief		Initialized in InitializeUtility() by a call to 
	*				SrGetSnmpID (SR_SNMPID_ALGORITHM_SIMPLE, SR_SNMPID_ALGORITHM_SIMPLE_MGR);
	*	@author		Artur Perwenis
	*	@date		24-Jan-2002
	*/
	OctetString *util_local_snmpID;
	/**
	*	@brief		<TODO>
	*	@author		Artur Perwenis
	*	@date		24-Jan-2002
	*/
	OctetString *v2_defaultContext;
	/**
	*	@brief		This is initialized and released in NewMIBFromFile ().
	*				This member replaces the global 'default_mib_tree'.
	*	@author		Artur Perwenis
	*	@date		26-Jan-2002
	*/
	MIB_TREE	*m_pDefaultMIBTree;


////////////////////////
// Static const members
////////////////////////
public:
	/**
	*	@brief		Default name for config directory.
	*	@author		Artur Perwenis
	*	@date		22-Jan-2002
	*/
	static const char* const SR_MGR_CONF_FILE_PREFIX;
	/**
	*	@brief		Default name for config file.
	*	@author		Artur Perwenis
	*	@date		22-Jan-2002
	*/
	static const char* const FN_SNMPINFO_DAT;
	/**
	*	@brief		Default name for config file.
	*	@author		Artur Perwenis
	*	@date		22-Jan-2002
	*/
	static const char* const FN_MGR_FILE;
	static const char* const SR_SNMP_ENV_NAME;
	static const char* const SNMP_RESEARCH_INC_SOFT_VERSION;
	static const char* const SNMP_RESEARCH_INC_COPYRIGHT;
	static const MIB_OID* const m_pOriginalMIBOidTable;

	static const CString const m_TypeName_NULL;
	static const CString const m_TypeName_NONLEAF;
	static const CString const m_TypeName_AGGREGATE;
	static const CString const m_TypeName_DISPLAYSTRING;
	static const CString const m_TypeName_OCTETSTRING;
	static const CString const m_TypeName_PHYSADDRESS;
	static const CString const m_TypeName_NETWORKADDRESS;
	static const CString const m_TypeName_IPADDRESS;
	static const CString const m_TypeName_OBJECTID;
	static const CString const m_TypeName_TIMETICKS;
	static const CString const m_TypeName_INTEGER;
	static const CString const m_TypeName_GAUGE;
	static const CString const m_TypeName_COUNTER;
	static const CString const m_TypeName_SERVICES;
	static const CString const m_TypeName_COUNTER32;
	static const CString const m_TypeName_GAUGE32;
	static const CString const m_TypeName_COUNTER64;
	static const CString const m_TypeName_INTEGER32;
	static const CString const m_TypeName_BITS;
	static const CString const m_TypeName_UNSIGNED32;

	static const CString const m_AccessName_READONLY;
	static const CString const m_AccessName_WRITEONLY;
	static const CString const m_AccessName_READWRITE;
	static const CString const m_AccessName_READCREATE;
	static const CString const m_AccessName_NOTACCESSIBLE;
	static const CString const m_AccessName_ACCESSIBLEFORNOTIFY;
};





#endif //___SNMP_INTERFACE_H__
