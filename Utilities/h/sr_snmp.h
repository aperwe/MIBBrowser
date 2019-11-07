#ifndef SR_SNMP_H
#define SR_SNMP_H

#ifdef  __cplusplus
extern "C" {
#endif	/* __cplusplus */

#include "sr_type.h"
#include "sr_proto.h"
#include "sr_time.h"

/* map old library names to new library names? */
#ifdef SR_OLDNAMES
#include "oldnames.h"
#endif /* SR_OLDNAMES */

/* This is a default value for maximum message size. */
#define SR_DEFAULT_MAX_MSG_SIZE 2048

/*
 * This is the size of the wrapper fields in a pdu, and is used to
 * determine the maximum size of a varbind list that can be included
 * in an snmp message.
 *
 * This is calculated for the worst case scenario for the
 * size of extra fields in the PDU:
 *
 *        4 (worst case pdu type/length)
 *      + 6 (worst case request id)
 *      + 3 (worst case error status)
 *      + 6 (worst case error index)
 *      + 4 (worst case varbindlist wrapper)
 *      ---
 *       23
 *
 * Note this is an overestimate because we know that it normally should be:
 *
 *        2 (worst case pdu type/length)
 *      + 6 (worst case request id)
 *      + 3 (worst case error status)
 *      + 3 (worst case error index)
 *      + 2 (worst case varbindlist wrapper)
 *      ---
 *       16
 *
 * We will have a few more silent drops -- but this is so pathological
 * because of the 484 rule that we will not invest any more resources to
 * make it perfect
 */
#define SR_PDU_WRAPPER_SIZE 23

/*
 * Define SR_SNMPv._PDU_PAYLOAD constants
 */
#define SR_SNMPv1_PDU_PAYLOAD 1
#define SR_SNMPv2_PDU_PAYLOAD 2

#define SR_SNMPv1_VERSION  0
#define SR_SNMPv2c_VERSION 1
#define SR_SNMPv2_VERSION  2
#define SR_SNMPv3_VERSION  3

/* Lifetime for v2* usec messages */
#define SR_USEC_LIFETIME_WINDOW 150

/* SPI values for SNMPv2* */
#define SR_SPI_SNMPv1        1
#define SR_SPI_SNMPv2c       2
#define SR_SPI_MAINT         3
#define SR_SPI_USEC_NOAUTH   4
#define SR_SPI_USM_NOAUTH    7
#ifndef SR_UNSECURABLE
#define SR_SPI_USEC_AUTH     5
#define SR_SPI_USM_AUTH      8
#endif /* SR_UNSECURABLE */

/* Security Models */
#define SR_SECURITY_MODEL_V1     1
#define SR_SECURITY_MODEL_V2C    2
#define SR_SECURITY_MODEL_V2STAR 25345
#define SR_SECURITY_MODEL_USM    3

/* Security Levels */
#define SR_SECURITY_LEVEL_NOAUTH 1
#ifndef SR_UNSECURABLE
#define SR_SECURITY_LEVEL_AUTH   2
#endif /* SR_UNSECURABLE */

/* Authentication protocols */
#define SR_USM_NOAUTH_PROTOCOL        0
#ifndef SR_UNSECURABLE
#define SR_USM_HMAC_MD5_AUTH_PROTOCOL 1
#endif /* SR_UNSECURABLE */

/* Privacy protocols */
#define SR_USM_NOPRIV_PROTOCOL   0
#ifndef SR_UNSECURABLE
#endif /* SR_UNSECURABLE */

/* choke on ANYTHING over this size (cause it will only get bigger later on). */
#define MAX_OCTET_STRING_SIZE 2048
#define MAX_OID_SIZE 128

#ifndef TRUE
#define TRUE 1
#endif				/* TRUE */

#ifndef FALSE
#define FALSE 0
#endif				/* FALSE */

/* Added 8/8/95 for subtree registration. -DLB */
#define ANY_TYPE   0xFF
#define ANY_STATUS SR_READ_WRITE
#define ANY_NOMINATOR 0
#define FULLY_QUALIFIED ((unsigned char) 128)
#define SR_ACCESS_MASK ((unsigned char) ~FULLY_QUALIFIED)

/*
 * Standard trap types (SNMPv1 definitions)
 */
#define COLD_START_TRAP         ((SR_INT32)0)
#define WARM_START_TRAP         ((SR_INT32)1)
#define LINK_DOWN_TRAP          ((SR_INT32)2)
#define LINK_UP_TRAP            ((SR_INT32)3)
#define AUTHEN_FAILURE_TRAP     ((SR_INT32)4)
#define EGP_NEIGHBOR_LOSS_TRAP  ((SR_INT32)5)
#define ENTERPRISE_TRAP         ((SR_INT32)6)

/* Universals */
#define INTEGER_TYPE           0x02
#define OCTET_PRIM_TYPE        0x04
#define NULL_TYPE              0x05
#define OBJECT_ID_TYPE         0x06
#define SEQUENCE_TYPE          0x30
#define INTEGER_32_TYPE        INTEGER_TYPE

/* Applications */
#define IP_ADDR_PRIM_TYPE      0x40
#define COUNTER_TYPE           0x41
#define GAUGE_TYPE             0x42
#define TIME_TICKS_TYPE        0x43
#define OPAQUE_PRIM_TYPE       0x44
#define COUNTER_32_TYPE        COUNTER_TYPE
#define	GAUGE_32_TYPE          GAUGE_TYPE
#define	UNSIGNED32_TYPE        GAUGE_TYPE
#define	COUNTER_64_TYPE        0x46

#define BITS_TYPE              OCTET_PRIM_TYPE

/* Type used in SNMPv2* */
#define V2_AUTH_MSG_TYPE       0xA9
#define USEC_AUTH_INFO_TYPE    0xA9

/* Exceptions in SNMPv2 PDUs (also contexts) */
#define NO_SUCH_OBJECT_EXCEPTION     0x80
#define NO_SUCH_INSTANCE_EXCEPTION   0x81
#define END_OF_MIB_VIEW_EXCEPTION    0x82

/* Error codes */
#ifdef SR_WIN32
/* NO_ERROR is 0L for NT to avoid a conflict with a file included by windows.h
 * which #defines NO_ERROR as 0L */
#define NO_ERROR            0L
#else /* SR_WIN32 */
#define NO_ERROR            0
#endif /* SR_WIN32 */
#define TOO_BIG_ERROR       1
#define NO_SUCH_NAME_ERROR  2
#define BAD_VALUE_ERROR     3
#define READ_ONLY_ERROR     4
#define GEN_ERROR           5

#ifndef	SR_SNMPv2_PDU
#define NO_ACCESS_ERROR             NO_SUCH_NAME_ERROR
#define WRONG_TYPE_ERROR            BAD_VALUE_ERROR
#define WRONG_LENGTH_ERROR          BAD_VALUE_ERROR
#define WRONG_ENCODING_ERROR        BAD_VALUE_ERROR
#define WRONG_VALUE_ERROR           BAD_VALUE_ERROR
#define NO_CREATION_ERROR           NO_SUCH_NAME_ERROR
#define INCONSISTENT_VALUE_ERROR    BAD_VALUE_ERROR
#define RESOURCE_UNAVAILABLE_ERROR  GEN_ERROR
#define COMMIT_FAILED_ERROR         GEN_ERROR
#define UNDO_FAILED_ERROR           GEN_ERROR
#define AUTHORIZATION_ERROR         NO_SUCH_NAME_ERROR
#define NOT_WRITABLE_ERROR          NO_SUCH_NAME_ERROR
#define INCONSISTENT_NAME_ERROR     BAD_VALUE_ERROR
#else				/* SR_SNMPv2_PDU */
#define NO_ACCESS_ERROR              6
#define WRONG_TYPE_ERROR             7
#define WRONG_LENGTH_ERROR           8
#define WRONG_ENCODING_ERROR         9
#define WRONG_VALUE_ERROR           10
#define NO_CREATION_ERROR           11
#define INCONSISTENT_VALUE_ERROR    12
#define RESOURCE_UNAVAILABLE_ERROR  13
#define COMMIT_FAILED_ERROR         14
#define UNDO_FAILED_ERROR           15
#define AUTHORIZATION_ERROR         16
#define NOT_WRITABLE_ERROR          17
#define INCONSISTENT_NAME_ERROR     18
#endif				/* SR_SNMPv2_PDU */

/* These can be returned from ParseAuthentication */
#define ASN_PARSE_ERROR          9
#define SNMP_BAD_VERSION_ERROR   10
#define UNKNOWN_SPI_ERROR         50
#define WRONG_DIGEST_VALUE_ERROR  51
#define UNKNOWN_USER_NAME_ERROR   52
#define NOT_IN_TIME_WINDOW_ERROR  53
#define BAD_PRIVACY_ERROR         54
#define UNKNOWN_CONTEXT_ERROR     55
#define UNAVAILABLE_CONTEXT_ERROR 56
#define SILENT_DROP_ERROR         58

#define SR_SNMP_BAD_COMMUNITY_NAMES 100

/* Error codes generated by MP */
#define SR_SNMP_UNKNOWN_SECURITY_MODELS    500
#define SR_SNMP_INVALID_MSGS               501
#define SR_SNMP_UNKNOWN_PDU_HANDLERS       504

/* Error codes generated by USM */
#define SR_USM_UNSUPPORTED_SEC_LEVELS      600
#define SR_USM_NOT_IN_TIME_WINDOWS         601
#define SR_USM_UNKNOWN_USER_NAMES          602
#define SR_USM_UNKNOWN_ENGINE_IDS          603
#define SR_USM_WRONG_DIGESTS               604
#define SR_USM_DECRYPTION_ERRORS           605

/* Security Levels (as stored in msgFlags) */
#define SR_SECURITY_LEVEL_BITS_NOAUTH_NOPRIV 0
#ifndef SR_UNSECURABLE
#define SR_SECURITY_LEVEL_BITS_AUTH_NOPRIV   1
#endif /* SR_UNSECURABLE */

/* ContextMatch values */
#define SR_CONTEXT_MATCH_EXACT  1
#define SR_CONTEXT_MATCH_PREFIX 2

/* Lifetime for USM messages */
#define SR_USM_LIFETIME_WINDOW 150

/* domains */
#define UNKNOWN_DOMAIN		      0
#define SNMPv1_DOMAIN                 1
#define SNMPv2_DOMAIN                 2

/* View types */
#define INCLUDED   1
#define EXCLUDED   2

/* StorageType values  */
#define OTHER 1
#define VOLATILE 2
#define NONVOLATILE 3
#define PERMANENT 4
#define READONLY 5

/* RowStatus types */
#define RS_UNINITIALIZED   0
#define RS_ACTIVE          1
#define RS_NOT_IN_SERVICE  2
#define RS_NOT_READY       3
#define RS_CREATE_AND_GO   4
#define RS_CREATE_AND_WAIT 5
#define RS_DESTROY         6

/* TruthValue types */
#define SR_LOCAL 1
#define SR_REMOTE 2

/* read flags (access and variable) */
#define SR_DISCARD        -1
#define SR_NONE            0
#define SR_READ_ONLY       0x20
#define SR_READ_WRITE      0x21
#define SR_OLD_READ_CREATE 0x22
#define SR_READ_CREATE     0x23
#define SR_NOT_ACCESSIBLE  0x24
#define SR_ACCESSIBLE_FOR_NOTIFY SR_NOT_ACCESSIBLE

#ifndef NONULL
#define NULLIT(pointer) pointer = NULL;
#else                           /* NONULL */
#define NULLIT(pointer)
#endif                          /* NONULL */

#define OIDSIZE(x) (sizeof(x)/sizeof(SR_UINT32))



#ifdef  __cplusplus
}
#endif	/* __cplusplus */

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif				/* SR_SNMP_H */
