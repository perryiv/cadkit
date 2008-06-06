# Microsoft Developer Studio Project File - Name="SceneGraphMfc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=SceneGraphMfc - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SceneGraphMfc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SceneGraphMfc.mak" CFG="SceneGraphMfc - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SceneGraphMfc - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SceneGraphMfc - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SceneGraphMfc - Win32 Release"

# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=fl32.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"SgMfcPrecompiled.h" /FD /c
# ADD CPP /nologo /MD /W3 /WX /GX /O2 /I "../../" /D "NDEBUG" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /D "_WINDOWS" /D "WIN32" /D "_CADKIT_COMPILING_SCENEGRAPH_MFC_WRAPPER_LIBRARY" /YX"SgMfcPrecompiled.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"../../bin/CkSgMfc.dll" /libpath:"$(MODELSPACE_HOME)\lib"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "SceneGraphMfc - Win32 Debug"

# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=fl32.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "$(LOCAL_INCLUDE_DIR)" /I "$(MODELSPACE_HOME)\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /YX"SgMfcPrecompiled.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /WX /GX /Zi /Od /I "../../" /D "_WINDLL" /D "_AFXEXT" /D "_AFXDLL" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_CADKIT_COMPILING_SCENEGRAPH_MFC_WRAPPER_LIBRARY" /YX"SgMfcPrecompiled.h" /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Sgd.lib Sld.lib glu32.lib opengl32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug/CadKitSgMfcd.dll" /pdbtype:sept /libpath:"$(MODELSPACE_HOME)\lib"
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"../../bin/CkSgMfcd.dll" /pdbtype:sept /libpath:"$(MODELSPACE_HOME)\lib"

!ENDIF 

# Begin Target

# Name "SceneGraphMfc - Win32 Release"
# Name "SceneGraphMfc - Win32 Debug"
# Begin Group "Precompile"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SgMfcClientHeaders.h
# End Source File
# Begin Source File

SOURCE=.\SgMfcPrecompiled.cpp
# End Source File
# Begin Source File

SOURCE=.\SgMfcPrecompiled.h
# End Source File
# End Group
# Begin Group "Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SgMfcConstants.h
# End Source File
# Begin Source File

SOURCE=.\SgMfcDefine.h
# End Source File
# Begin Source File

SOURCE=.\SgMfcFrameWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\SgMfcFrameWnd.h
# End Source File
# Begin Source File

SOURCE=.\SgMfcMdiChildWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\SgMfcMdiChildWnd.h
# End Source File
# Begin Source File

SOURCE=.\SgMfcView.cpp
# End Source File
# Begin Source File

SOURCE=.\SgMfcView.h
# End Source File
# End Group
# Begin Group "Resources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\res\contextMenuToolBar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\hand.cur
# End Source File
# Begin Source File

SOURCE=.\res\magnify.cur
# End Source File
# Begin Source File

SOURCE=.\res\nodrop.cur
# End Source File
# Begin Source File

SOURCE=.\res\point.cur
# End Source File
# Begin Source File

SOURCE=.\res\rotate.cur
# End Source File
# Begin Source File

SOURCE=.\res\seek.cur
# End Source File
# Begin Source File

SOURCE=.\SgMfcResource.h
# End Source File
# Begin Source File

SOURCE=.\SgMfcResource.rc
# End Source File
# End Group
# Begin Group "Dll"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SgMfcApi.h
# End Source File
# Begin Source File

SOURCE=.\SgMfcDll.cpp
# End Source File
# End Group
# Begin Group "Notes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Notes\SceneGraph.txt
# End Source File
# End Group
# End Target
# End Project
