# Microsoft Developer Studio Project File - Name="TranslateJt2Pf" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=TranslateJt2Pf - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TranslateJt2Pf.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TranslateJt2Pf.mak" CFG="TranslateJt2Pf - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TranslateJt2Pf - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TranslateJt2Pf - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TranslateJt2Pf - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=fl32.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"TrJt2PfPrecompiled.h" /FD /c
# ADD CPP /nologo /MD /W3 /WX /GR /GX /O2 /I "../../" /I "$(DMDTK_DEV_PATH)/include" /I "$(PFROOT)/Include" /I "$(PFROOT)/Include/Performer" /D "_MBCS" /D "_USRDLL" /D "_CADKIT_USE_PRECOMPILED_HEADERS" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_CADKIT_COMPILING_TRANSLATOR_JUPITER_2_PERFORMER_LIBRARY" /YX"TrJt2PfPrecompiled.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 libpf.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"../../bin/CkTrJt2Pf.dll" /libpath:"$(DMDTK_DEV_PATH)/lib/Windows" /libpath:"$(PFROOT)/Lib"

!ELSEIF  "$(CFG)" == "TranslateJt2Pf - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=fl32.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"TrJt2PfPrecompiled.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GR /GX /Zi /Od /I "../../" /I "$(DMDTK_DEV_PATH)/include" /I "$(PFROOT)/Include" /I "$(PFROOT)/Include/Performer" /D "_USRDLL" /D "_MBCS" /D "_CADKIT_USE_PRECOMPILED_HEADERS" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_CADKIT_COMPILING_TRANSLATOR_JUPITER_2_PERFORMER_LIBRARY" /YX"TrJt2PfPrecompiled.h" /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libpf.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"../../bin/CkTrJt2Pfd.dll" /pdbtype:sept /libpath:"$(DMDTK_DEV_PATH)/lib/Windows" /libpath:"$(PFROOT)/Lib"

!ENDIF 

# Begin Target

# Name "TranslateJt2Pf - Win32 Release"
# Name "TranslateJt2Pf - Win32 Debug"
# Begin Group "Precompile"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TrJt2PfClientHeaders.h
# End Source File
# Begin Source File

SOURCE=.\TrJt2PfPrecompiled.cpp
# End Source File
# Begin Source File

SOURCE=.\TrJt2PfPrecompiled.h
# End Source File
# End Group
# Begin Group "Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TrJt2Pf.cpp
# End Source File
# Begin Source File

SOURCE=.\TrJt2Pf.h
# End Source File
# Begin Source File

SOURCE=.\TrJt2PfPerformer.h
# End Source File
# End Group
# Begin Group "DLL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TrJt2PfApi.h
# End Source File
# Begin Source File

SOURCE=.\TrJt2PfDll.cpp
# End Source File
# End Group
# End Target
# End Project
