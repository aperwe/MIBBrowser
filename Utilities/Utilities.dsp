# Microsoft Developer Studio Project File - Name="Utilities" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Utilities - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Utilities.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Utilities.mak" CFG="Utilities - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Utilities - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Utilities - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Utilities - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I ".\h" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "SR_TCP_IPC" /D "SR_CONFIG_FP" /D "SR_UNSECURABLE" /D "SR_SNMPv1_WRAPPER" /D "SR_SNMPv1_PACKET" /D "SR_SNMPv1" /D "SR_SNMPv2c_PACKET" /D "SR_SNMPv2_PDU" /D "SR_SNMPv2_PACKET" /D "PC" /D "SR_WINDOWS" /D "SR_WIN32" /YX /FD /c
# ADD BASE RSC /l 0x415 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Libs/Utilities.lib"

!ELSEIF  "$(CFG)" == "Utilities - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I ".\h" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "SR_TCP_IPC" /D "SR_CONFIG_FP" /D "SR_UNSECURABLE" /D "SR_SNMPv1_WRAPPER" /D "SR_SNMPv1_PACKET" /D "SR_SNMPv1" /D "SR_SNMPv2c_PACKET" /D "SR_SNMPv2_PDU" /D "SR_SNMPv2_PACKET" /D "PC" /D "SR_WINDOWS" /D "SR_WIN32" /D "SNMP_V2_PACKET_MIB" /D "SNMP_V1_PACKET_MIB" /D "SR_MIB_TABLE_HASHING" /D "USE_NEW_SNMP_INTERFACE" /YX /FD /GZ /c
# ADD BASE RSC /l 0x415 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Libs/Utilities.lib"

!ENDIF 

# Begin Target

# Name "Utilities - Win32 Release"
# Name "Utilities - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "seclib"

# PROP Default_Filter ""
# Begin Group "mibtree"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\seclib\mibtree\convrsn.c
# End Source File
# Begin Source File

SOURCE=.\src\seclib\mibtree\mib_lib.c
# End Source File
# Begin Source File

SOURCE=.\src\seclib\mibtree\mib_tree.c
# End Source File
# Begin Source File

SOURCE=.\src\seclib\mibtree\mibread.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\seclib\bld_pkt.c
# End Source File
# Begin Source File

SOURCE=.\src\seclib\findlen.c
# End Source File
# Begin Source File

SOURCE=.\src\seclib\free_lib.c
# End Source File
# Begin Source File

SOURCE=.\src\seclib\getport.c
# End Source File
# Begin Source File

SOURCE=.\src\seclib\gettime.c
# End Source File
# Begin Source File

SOURCE=.\src\seclib\k_fileio.c
# End Source File
# Begin Source File

SOURCE=.\src\seclib\lookup.c
# End Source File
# Begin Source File

SOURCE=.\src\seclib\make_lib.c
# End Source File
# Begin Source File

SOURCE=.\src\seclib\oid_lib.c
# End Source File
# Begin Source File

SOURCE=.\src\seclib\prse_pkt.c
# End Source File
# Begin Source File

SOURCE=.\src\seclib\scanfile.c
# End Source File
# Begin Source File

SOURCE=.\src\seclib\sigfunc.c
# End Source File
# Begin Source File

SOURCE=.\src\seclib\snmpid.c
# End Source File
# Begin Source File

SOURCE=.\src\seclib\snmpio.c
# End Source File
# Begin Source File

SOURCE=.\src\seclib\sr_boots.c
# End Source File
# Begin Source File

SOURCE=.\src\seclib\sr_msg.c
# End Source File
# Begin Source File

SOURCE=.\src\seclib\sr_vta.c
# End Source File
# Begin Source File

SOURCE=.\src\seclib\t_ip.c
# End Source File
# Begin Source File

SOURCE=.\src\seclib\userauth.c
# End Source File
# Begin Source File

SOURCE=.\src\seclib\v1_cmsg.c
# End Source File
# Begin Source File

SOURCE=.\src\seclib\v1_msg.c
# End Source File
# Begin Source File

SOURCE=.\src\seclib\v2_cmsg.c
# End Source File
# Begin Source File

SOURCE=.\src\seclib\v2_msg.c
# End Source File
# Begin Source File

SOURCE=.\src\seclib\v2c_cmsg.c
# End Source File
# Begin Source File

SOURCE=.\src\seclib\v2suport.c
# End Source File
# Begin Source File

SOURCE=.\src\seclib\v2table.c
# End Source File
# End Group
# Begin Group "mibtable"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\mibtable\mib_tabl.c
# End Source File
# Begin Source File

SOURCE=.\src\mibtable\oidtran.c
# End Source File
# Begin Source File

SOURCE=.\src\mibtable\prse_sid.c
# End Source File
# End Group
# Begin Group "snmpv2s"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\h\snmpv2s\i_adms.c
# End Source File
# Begin Source File

SOURCE=.\h\snmpv2s\i_comm.c
# End Source File
# Begin Source File

SOURCE=.\h\snmpv2s\i_usec.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\common.c
# End Source File
# Begin Source File

SOURCE=.\src\getbulk.c
# End Source File
# Begin Source File

SOURCE=.\src\getid.c
# End Source File
# Begin Source File

SOURCE=.\src\getmany.c
# End Source File
# Begin Source File

SOURCE=.\src\getmet.c
# End Source File
# Begin Source File

SOURCE=.\src\getnext.c
# End Source File
# Begin Source File

SOURCE=.\src\getone.c
# End Source File
# Begin Source File

SOURCE=.\src\getroute.c
# End Source File
# Begin Source File

SOURCE=.\src\getsub.c
# End Source File
# Begin Source File

SOURCE=.\src\gettab.c
# End Source File
# Begin Source File

SOURCE=.\src\globals.c
# End Source File
# Begin Source File

SOURCE=.\src\inform.c
# End Source File
# Begin Source File

SOURCE=.\src\prnt_lib.c
# End Source File
# Begin Source File

SOURCE=.\src\prnt_rt.c
# End Source File
# Begin Source File

SOURCE=.\src\setany.c
# End Source File
# Begin Source File

SOURCE=.\src\traprcv.c
# End Source File
# Begin Source File

SOURCE=.\src\trapsend.c
# End Source File
# Begin Source File

SOURCE=.\src\util_v2s.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\h\ct_old.h
# End Source File
# Begin Source File

SOURCE=.\h\diag.h
# End Source File
# Begin Source File

SOURCE=.\h\lookup.h
# End Source File
# Begin Source File

SOURCE=.\h\mib_tree.h
# End Source File
# Begin Source File

SOURCE=.\h\oid_lib.h
# End Source File
# Begin Source File

SOURCE=.\h\prnt_lib.h
# End Source File
# Begin Source File

SOURCE=.\h\scan.h
# End Source File
# Begin Source File

SOURCE=.\h\snmpid.h
# End Source File
# Begin Source File

SOURCE=.\h\snmpio.bsd
# End Source File
# Begin Source File

SOURCE=.\h\snmpio.h
# End Source File
# Begin Source File

SOURCE=.\h\snmpv2.h
# End Source File
# Begin Source File

SOURCE=.\h\sr_ad.h
# End Source File
# Begin Source File

SOURCE=.\h\sr_boots.h
# End Source File
# Begin Source File

SOURCE=.\h\sr_cfg.h
# End Source File
# Begin Source File

SOURCE=.\h\sr_conf.h
# End Source File
# Begin Source File

SOURCE=.\h\sr_featr.h
# End Source File
# Begin Source File

SOURCE=.\h\sr_msg.h
# End Source File
# Begin Source File

SOURCE=.\h\sr_proto.h
# End Source File
# Begin Source File

SOURCE=.\h\sr_snmp.h
# End Source File
# Begin Source File

SOURCE=.\h\sr_time.h
# End Source File
# Begin Source File

SOURCE=.\h\sr_trans.h
# End Source File
# Begin Source File

SOURCE=.\h\sr_type.h
# End Source File
# Begin Source File

SOURCE=.\h\sr_user.h
# End Source File
# Begin Source File

SOURCE=.\h\sr_utils.h
# End Source File
# Begin Source File

SOURCE=.\h\UExterns.h
# End Source File
# Begin Source File

SOURCE=.\h\util_def.h
# End Source File
# Begin Source File

SOURCE=.\h\v1_msg.h
# End Source File
# Begin Source File

SOURCE=.\h\v2_msg.h
# End Source File
# Begin Source File

SOURCE=.\h\v2table.h
# End Source File
# End Group
# End Target
# End Project
