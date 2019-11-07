# Microsoft Developer Studio Project File - Name="MIBBrowser2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MIBBrowser2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MIBBrowser2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MIBBrowser2.mak" CFG="MIBBrowser2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MIBBrowser2 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MIBBrowser2 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MIBBrowser2 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "SR_TCP_IPC" /D "SR_CONFIG_FP" /D "SR_UNSECURABLE" /D "SR_SNMPv1_WRAPPER" /D "SR_SNMPv1_PACKET" /D "SR_SNMPv1" /D "SR_SNMPv2_PDU" /D "SR_SNMPv2c_PACKET" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x415 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 Kernel32.lib Utilities.lib Ws2_32.lib /nologo /subsystem:windows /machine:I386 /out:"Release/MIBBrowser.exe" /libpath:"..\Libs"

!ELSEIF  "$(CFG)" == "MIBBrowser2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "SR_TCP_IPC" /D "SR_CONFIG_FP" /D "SR_UNSECURABLE" /D "SR_SNMPv1_WRAPPER" /D "SR_SNMPv1_PACKET" /D "SR_SNMPv1" /D "SR_SNMPv2_PDU" /D "SR_SNMPv2c_PACKET" /D "USE_NEW_SNMP_INTERFACE" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x415 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Kernel32.lib Utilities.lib Ws2_32.lib /nologo /version:1.2 /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\Libs"

!ENDIF 

# Begin Target

# Name "MIBBrowser2 - Win32 Release"
# Name "MIBBrowser2 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AppClass.cpp
# End Source File
# Begin Source File

SOURCE=.\FindElementDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LookupElement.cpp
# End Source File
# Begin Source File

SOURCE=.\MainWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\MIBFile.cpp
# End Source File
# Begin Source File

SOURCE=.\SetEnvDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\SNMPInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\SplashThread.cpp
# End Source File
# Begin Source File

SOURCE=.\SplashWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\SyntaxDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\UtilitiesDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AppClass.h
# End Source File
# Begin Source File

SOURCE=.\CommonDefs.h
# End Source File
# Begin Source File

SOURCE=.\Externs.h
# End Source File
# Begin Source File

SOURCE=.\FindElementDlg.h
# End Source File
# Begin Source File

SOURCE=.\MainWnd.h
# End Source File
# Begin Source File

SOURCE=.\MIBFile.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\SetEnvDialog.h
# End Source File
# Begin Source File

SOURCE=.\SNMPInterface.h
# End Source File
# Begin Source File

SOURCE=.\SplashThread.h
# End Source File
# Begin Source File

SOURCE=.\SplashWnd.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SyntaxDlg.h
# End Source File
# Begin Source File

SOURCE=.\UtilitiesDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=".\Res\1 MIB root.bmp"
# End Source File
# Begin Source File

SOURCE=".\Res\10 textual conventions root.bmp"
# End Source File
# Begin Source File

SOURCE=".\Res\11 trap-type entry.bmp"
# End Source File
# Begin Source File

SOURCE=".\Res\12 notification type entry.bmp"
# End Source File
# Begin Source File

SOURCE=".\Res\13 table external index field.bmp"
# End Source File
# Begin Source File

SOURCE=".\Res\2 mgmt (root key).bmp"
# End Source File
# Begin Source File

SOURCE=".\Res\3 closed folder.bmp"
# End Source File
# Begin Source File

SOURCE=".\Res\4 open folder.bmp"
# End Source File
# Begin Source File

SOURCE=".\Res\5 read only entry.bmp"
# End Source File
# Begin Source File

SOURCE=".\Res\6 table root.bmp"
# End Source File
# Begin Source File

SOURCE=".\Res\7 table item.bmp"
# End Source File
# Begin Source File

SOURCE=".\Res\8 table index field.bmp"
# End Source File
# Begin Source File

SOURCE=".\Res\9 read-write entry.bmp"
# End Source File
# Begin Source File

SOURCE=".\Res\mib-app icon.ico"
# End Source File
# Begin Source File

SOURCE=.\MIBRes.rc
# End Source File
# Begin Source File

SOURCE=".\Res\Program name.bmp"
# End Source File
# Begin Source File

SOURCE=.\Res\ProgramLogo.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\RTSLogo.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\RTSLogo256.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Errors.txt
# End Source File
# Begin Source File

SOURCE=.\Grammar.txt
# End Source File
# Begin Source File

SOURCE=.\HungarianNotation.txt
# End Source File
# Begin Source File

SOURCE=".\To do.txt"
# End Source File
# End Target
# End Project
