# Microsoft Developer Studio Project File - Name="DatabaseJt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=DatabaseJt - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DatabaseJt.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DatabaseJt.mak" CFG="DatabaseJt - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DatabaseJt - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DatabaseJt - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DatabaseJt - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"DbJtPrecompiled.h" /FD /c
# ADD CPP /nologo /MD /W3 /WX /GR /GX /O2 /I "../../" /I "$(DMDTK_DEV_PATH)/include" /D "_MBCS" /D "_USRDLL" /D "_CADKIT_USE_PRECOMPILED_HEADERS" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_CADKIT_COMPILING_DATABASE_JUPITER_LIBRARY" /YX"DbJtPrecompiled.h" /FD /c
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
# ADD LINK32 DMDataTk$(DMDTK_VERSION).lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"../../bin/CkDbJt.dll" /libpath:"$(DMDTK_DEV_PATH)/lib/Windows"

!ELSEIF  "$(CFG)" == "DatabaseJt - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"DbJtPrecompiled.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GR /GX /Zi /Od /I "../../" /I "$(DMDTK_DEV_PATH)/include" /D "_USRDLL" /D "_MBCS" /D "_CADKIT_USE_PRECOMPILED_HEADERS" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_CADKIT_COMPILING_DATABASE_JUPITER_LIBRARY" /YX"DbJtPrecompiled.h" /FD /GZ /c
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
# ADD LINK32 DMDataTk$(DMDTK_VERSION).lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"../../bin/CkDbJtd.dll" /pdbtype:sept /libpath:"$(DMDTK_DEV_PATH)/lib/Windows"

!ENDIF 

# Begin Target

# Name "DatabaseJt - Win32 Release"
# Name "DatabaseJt - Win32 Debug"
# Begin Group "Precompile"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DbJtClientHeaders.h
# End Source File
# Begin Source File

SOURCE=.\DbJtPrecompiled.cpp
# End Source File
# Begin Source File

SOURCE=.\DbJtPrecompiled.h
# End Source File
# End Group
# Begin Group "Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DbJtDatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\DbJtDatabase.h
# End Source File
# Begin Source File

SOURCE=.\DbJtFunctions.cpp
# End Source File
# Begin Source File

SOURCE=.\DbJtFunctions.h
# End Source File
# Begin Source File

SOURCE=.\DbJtInline.h
# End Source File
# Begin Source File

SOURCE=.\DbJtTraversalState.cpp
# End Source File
# Begin Source File

SOURCE=.\DbJtTraversalState.h
# End Source File
# Begin Source File

SOURCE=.\DbJtTraverser.cpp
# End Source File
# Begin Source File

SOURCE=.\DbJtTraverser.h
# End Source File
# Begin Source File

SOURCE=.\DbJtVisApi.cpp
# End Source File
# Begin Source File

SOURCE=.\DbJtVisApi.h
# End Source File
# Begin Source File

SOURCE=.\DbJtVisApiArray.h
# End Source File
# Begin Source File

SOURCE=.\DbJtVisApiHeaders.h
# End Source File
# Begin Source File

SOURCE=.\DbJtVisApiVersion.cpp
# End Source File
# Begin Source File

SOURCE=.\DbJtVisApiVersion.h
# End Source File
# End Group
# Begin Group "DLL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DbJtApi.h
# End Source File
# Begin Source File

SOURCE=.\DbJtDll.cpp
# End Source File
# End Group
# Begin Group "VisApi"

# PROP Default_Filter ""
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiAttrib.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiBoxSet.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiBrep.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiBundle.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiCADExporter.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiCADImporter.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiClientData.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiCoedge.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiConfig.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiCurve.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiCylinderSet.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiEdge.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiEntity.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiEntityFactory.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiEntityList.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiEntityStack.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiEntityTable.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiFace.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiGeometry.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiHarness.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiHierarchy.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiLight.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiLineStripSet.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiLoop.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiMotionExporter.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiNURBSCurve.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiNURBSSurface.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiParser.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiPartShepherd.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiPartTable.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiPMIStructs.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiPoint.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiPointSet.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiPolygonSet.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiPrim.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiProperty.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiPropertyFilter.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiPyramidSet.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiRegion.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiSegment.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiShape.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiShell.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiSphereSet.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiStack.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiStandard.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiSurface.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiTopology.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiTraverser.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiTriPrismSet.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiTriStripSet.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiUVCurve.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiVertex.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiWire.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiWrep.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiWrepWire.h"
# End Source File
# Begin Source File

SOURCE="$(DMDTK_DEV_PATH)\include\DMDataTk\eaiXYZCurve.h"
# End Source File
# End Group
# Begin Group "Notes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Notes\Database.txt
# End Source File
# Begin Source File

SOURCE=..\..\Notes\jupiter_database.txt
# End Source File
# End Group
# End Target
# End Project
