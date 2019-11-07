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

/* Not currently supporting notifyTable */
#undef I_notifyConfirm
#undef I_notifyAuthSnmpID
#undef I_notifyTimeout
#undef I_notifyMaxRetry

/* Not currently supporting proxyTable */
#undef I_proxyIndex
#undef I_proxyType
#undef I_proxySPIIn
#undef I_proxyAuthSnmpIDIn
#undef I_proxyIdentityNameIn
#undef I_proxyContextSnmpIDIn
#undef I_proxyContextNameIn
#undef I_proxySPIOut
#undef I_proxyAuthSnmpIDOut
#undef I_proxyIdentityNameOut
#undef I_proxyTransportLabelOut
#undef I_proxyMemoryType
#undef I_proxyStatus

#ifndef SR_SNMPv1_PACKET_MIB
#undef I_srCommunitySpinLock
#undef I_srCommunityAuthSnmpID
#undef I_srCommunityName
#undef I_srCommunityGroupName
#undef I_srCommunityContextSnmpID
#undef I_srCommunityContextName
#undef I_srCommunityTransportLabel
#undef I_srCommunityMemoryType
#undef I_srCommunityStatus
#endif /* SR_SNMPv1_PACKET_MIB */

#ifndef SR_SNMPv2_PACKET_MIB
#undef I_snmpID
#undef I_snmpMaxMessageSize
#undef I_maxIdentityNameLength
#undef I_maxGroupNameLength
#undef I_maxV2ContextNameLength
#undef I_maxViewTreeNameLength
#undef I_maxTransportLabelLength
#undef I_v2AdminStatsUnknownSPIs
#undef I_v2AdminStatsUnknownContexts
#undef I_v2AdminStatsUnavailableContexts
#undef I_v2AdminStatsCacheMisses
#undef I_snmpBoots
#undef I_snmpTime
#undef I_usecStatsNotInTimeWindows
#undef I_usecStatsUnknownUserNames
#undef I_usecStatsWrongDigestValues
#undef I_usecStatsBadPrivacys
#undef I_userSpinLock
#undef I_maxUserNameLength
#undef I_userAuthSnmpID
#undef I_userName
#undef I_userGroupName
#undef I_userAuthChange
#undef I_userPrivChange
#undef I_userNovel
#undef I_userTransportLabel
#undef I_userCloneFrom
#undef I_userMemoryType
#undef I_userStatus
#endif /* SR_SNMPv2_PACKET_MIB */

#if (! ( defined(SR_SNMPv2_ADMIN) ) || ! ( defined(SR_SNMP_ADMIN_MIB) ))
#undef I_v2ContextSnmpID
#undef I_v2ContextName
#undef I_v2ContextLocalEntity
#undef I_v2ContextLocalTime
#undef I_v2ContextMemoryType
#undef I_v2ContextStatus
#undef I_viewTreeSpinLock
#undef I_viewTreeName
#undef I_viewTreeSubTree
#undef I_viewTreeMask
#undef I_viewTreeType
#undef I_viewTreeMemoryType
#undef I_viewTreeStatus
#undef I_acSpinLock
#undef I_acSPI
#undef I_acGroupName
#undef I_acContextName
#undef I_acContextNameMask
#undef I_acPrivs
#undef I_acReadViewName
#undef I_acWriteViewName
#undef I_acMemoryType
#undef I_acStatus
#undef I_notifySpinLock
#undef I_notifyIndex
#undef I_notifySPI
#undef I_notifyIdentityName
#undef I_notifyTransportLabel
#undef I_notifyContextName
#undef I_notifyViewName
#undef I_notifyMemoryType
#undef I_notifyStatus
#undef I_transportSpinLock
#undef I_transportLabel
#undef I_transportSubindex
#undef I_transportDomain
#undef I_transportAddress
#undef I_transportReceiveMask
#undef I_transportMMS
#undef I_transportMemoryType
#undef I_transportStatus
#endif	/* (! ( defined(SR_SNMPv2_ADMIN) ) || ! ( defined(SR_SNMP_ADMIN_MIB) )) */
