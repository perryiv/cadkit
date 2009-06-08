# Microsoft Developer Studio Project File - Name="DatabaseBase" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=DatabaseBase - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DatabaseBase.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DatabaseBase.mak" CFG="DatabaseBase - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DatabaseBase - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DatabaseBase - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DatabaseBase - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"DbBasePrecompiled.h" /FD /c
# ADD CPP /nologo /MD /W3 /WX /GR /GX /O2 /I "../../" /D "_MBCS" /D "_USRDLL" /D "_CADKIT_USE_PRECOMPILED_HEADERS" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_CADKIT_COMPILING_DATABASE_BASE_LIBRARY" /YX"DbBasePrecompiled.h" /FD /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"../../bin/CkDbBase.dll"

!ELSEIF  "$(CFG)" == "DatabaseBase - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"DbBasePrecompiled.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GR /GX /Zi /Od /I "../../" /D "_USRDLL" /D "_MBCS" /D "_CADKIT_USE_PRECOMPILED_HEADERS" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_CADKIT_COMPILING_DATABASE_BASE_LIBRARY" /YX"DbBasePrecompiled.h" /FD /GZ /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"../../bin/CkDbBased.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "DatabaseBase - Win32 Release"
# Name "DatabaseBase - Win32 Debug"
# Begin Group "Precompile"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DbBasePrecompiled.cpp
# End Source File
# Begin Source File

SOURCE=.\DbBasePrecompiled.h
# End Source File
# End Group
# Begin Group "Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DbBaseAsciiWriter.h
# End Source File
# Begin Source File

SOURCE=.\DbBaseInline.h
# End Source File
# Begin Source File

SOURCE=.\DbBaseObject.cpp
# End Source File
# Begin Source File

SOURCE=.\DbBaseObject.h
# End Source File
# Begin Source File

SOURCE=.\DbBaseShapeData.h
# End Source File
# Begin Source File

SOURCE=.\DbBaseSource.cpp
# End Source File
# Begin Source File

SOURCE=.\DbBaseSource.h
# End Source File
# Begin Source File

SOURCE=.\DbBaseTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\DbBaseTarget.h
# End Source File
# End Group
# Begin Group "DLL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DbBaseApi.h
# End Source File
# Begin Source File

SOURCE=.\DbBaseDll.cpp
# End Source File
# End Group
# Begin Group "Notes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Notes\Database.txt
# End Source File
# End Group
# End Target
# End Project
