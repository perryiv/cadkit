# Microsoft Developer Studio Project File - Name="SceneGraphCore" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=SceneGraphCore - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SceneGraphCore.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SceneGraphCore.mak" CFG="SceneGraphCore - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SceneGraphCore - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SceneGraphCore - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SceneGraphCore - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"SgPrecompiled.h" /FD /c
# ADD CPP /nologo /MD /W3 /WX /GR /GX /O2 /I "../../" /D "_MBCS" /D "_USRDLL" /D "_CADKIT_USE_PRECOMPILED_HEADERS" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_CADKIT_COMPILING_SCENEGRAPH_CORE_LIBRARY" /YX"SgPrecompiled.h" /FD /Zm200 /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"../../bin/CkSg.dll"

!ELSEIF  "$(CFG)" == "SceneGraphCore - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"SgPrecompiled.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GR /GX /Zi /Od /I "../../" /D "_USRDLL" /D "_MBCS" /D "_CADKIT_USE_PRECOMPILED_HEADERS" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_CADKIT_COMPILING_SCENEGRAPH_CORE_LIBRARY" /YX"SgPrecompiled.h" /FD /GZ /Zm200 /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"../../bin/CkSgd.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "SceneGraphCore - Win32 Release"
# Name "SceneGraphCore - Win32 Debug"
# Begin Group "Precompile"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SgClientHeaders.h
# End Source File
# Begin Source File

SOURCE=.\SgPrecompiled.cpp
# End Source File
# Begin Source File

SOURCE=.\SgPrecompiled.h
# End Source File
# End Group
# Begin Group "Source"

# PROP Default_Filter ""
# Begin Group "Nodes"

# PROP Default_Filter ""
# Begin Group "Attributes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SgAttribute.cpp
# End Source File
# Begin Source File

SOURCE=.\SgAttribute.h
# End Source File
# Begin Source File

SOURCE=.\SgBackground.cpp
# End Source File
# Begin Source File

SOURCE=.\SgBackground.h
# End Source File
# Begin Source File

SOURCE=.\SgBaseColor.cpp
# End Source File
# Begin Source File

SOURCE=.\SgBaseColor.h
# End Source File
# Begin Source File

SOURCE=.\SgDrawStyle.cpp
# End Source File
# Begin Source File

SOURCE=.\SgDrawStyle.h
# End Source File
# Begin Source File

SOURCE=.\SgMaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\SgMaterial.h
# End Source File
# Begin Source File

SOURCE=.\SgShading.cpp
# End Source File
# Begin Source File

SOURCE=.\SgShading.h
# End Source File
# End Group
# Begin Group "Shapes"

# PROP Default_Filter ""
# Begin Group "Primitives"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SgCircle.cpp
# End Source File
# Begin Source File

SOURCE=.\SgCircle.h
# End Source File
# Begin Source File

SOURCE=.\SgCone.cpp
# End Source File
# Begin Source File

SOURCE=.\SgCone.h
# End Source File
# Begin Source File

SOURCE=.\SgCube.cpp
# End Source File
# Begin Source File

SOURCE=.\SgCube.h
# End Source File
# Begin Source File

SOURCE=.\SgCylinder.cpp
# End Source File
# Begin Source File

SOURCE=.\SgCylinder.h
# End Source File
# Begin Source File

SOURCE=.\SgEllipse.cpp
# End Source File
# Begin Source File

SOURCE=.\SgEllipse.h
# End Source File
# Begin Source File

SOURCE=.\SgPrimitive.cpp
# End Source File
# Begin Source File

SOURCE=.\SgPrimitive.h
# End Source File
# Begin Source File

SOURCE=.\SgRectangle.cpp
# End Source File
# Begin Source File

SOURCE=.\SgRectangle.h
# End Source File
# Begin Source File

SOURCE=.\SgSphere.cpp
# End Source File
# Begin Source File

SOURCE=.\SgSphere.h
# End Source File
# Begin Source File

SOURCE=.\SgSquare.cpp
# End Source File
# Begin Source File

SOURCE=.\SgSquare.h
# End Source File
# Begin Source File

SOURCE=.\SgTriangle.cpp
# End Source File
# Begin Source File

SOURCE=.\SgTriangle.h
# End Source File
# End Group
# Begin Group "Vertex Sets"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SgLineSet.cpp
# End Source File
# Begin Source File

SOURCE=.\SgLineSet.h
# End Source File
# Begin Source File

SOURCE=.\SgPointSet.cpp
# End Source File
# Begin Source File

SOURCE=.\SgPointSet.h
# End Source File
# Begin Source File

SOURCE=.\SgQuadMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\SgQuadMesh.h
# End Source File
# Begin Source File

SOURCE=.\SgTriStrip.cpp
# End Source File
# Begin Source File

SOURCE=.\SgTriStrip.h
# End Source File
# Begin Source File

SOURCE=.\SgVertexSet.cpp
# End Source File
# Begin Source File

SOURCE=.\SgVertexSet.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\SgColorWheel.cpp
# End Source File
# Begin Source File

SOURCE=.\SgColorWheel.h
# End Source File
# Begin Source File

SOURCE=.\SgShape.cpp
# End Source File
# Begin Source File

SOURCE=.\SgShape.h
# End Source File
# Begin Source File

SOURCE=.\SgText.cpp
# End Source File
# Begin Source File

SOURCE=.\SgText.h
# End Source File
# End Group
# Begin Group "Groups"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SgGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\SgGroup.h
# End Source File
# Begin Source File

SOURCE=.\SgLevelOfDetail.cpp
# End Source File
# Begin Source File

SOURCE=.\SgLevelOfDetail.h
# End Source File
# Begin Source File

SOURCE=.\SgSelection.cpp
# End Source File
# Begin Source File

SOURCE=.\SgSelection.h
# End Source File
# Begin Source File

SOURCE=.\SgSeparator.cpp
# End Source File
# Begin Source File

SOURCE=.\SgSeparator.h
# End Source File
# Begin Source File

SOURCE=.\SgSwitch.cpp
# End Source File
# Begin Source File

SOURCE=.\SgSwitch.h
# End Source File
# End Group
# Begin Group "Transformations"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SgBillboard.cpp
# End Source File
# Begin Source File

SOURCE=.\SgBillboard.h
# End Source File
# Begin Source File

SOURCE=.\SgIdentity.cpp
# End Source File
# Begin Source File

SOURCE=.\SgIdentity.h
# End Source File
# Begin Source File

SOURCE=.\SgRotation.cpp
# End Source File
# Begin Source File

SOURCE=.\SgRotation.h
# End Source File
# Begin Source File

SOURCE=.\SgScale.cpp
# End Source File
# Begin Source File

SOURCE=.\SgScale.h
# End Source File
# Begin Source File

SOURCE=.\SgTransform.cpp
# End Source File
# Begin Source File

SOURCE=.\SgTransform.h
# End Source File
# Begin Source File

SOURCE=.\SgTransformation.cpp
# End Source File
# Begin Source File

SOURCE=.\SgTransformation.h
# End Source File
# Begin Source File

SOURCE=.\SgTranslation.cpp
# End Source File
# Begin Source File

SOURCE=.\SgTranslation.h
# End Source File
# Begin Source File

SOURCE=.\SgUnScale.cpp
# End Source File
# Begin Source File

SOURCE=.\SgUnScale.h
# End Source File
# End Group
# Begin Group "Cameras"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SgCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\SgCamera.h
# End Source File
# Begin Source File

SOURCE=.\SgOrthographicCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\SgOrthographicCamera.h
# End Source File
# Begin Source File

SOURCE=.\SgPerspectiveCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\SgPerspectiveCamera.h
# End Source File
# End Group
# Begin Group "Coordinates/Normals"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SgCoordinate.cpp
# End Source File
# Begin Source File

SOURCE=.\SgCoordinate.h
# End Source File
# Begin Source File

SOURCE=.\SgCoordinate3.cpp
# End Source File
# Begin Source File

SOURCE=.\SgCoordinate3.h
# End Source File
# Begin Source File

SOURCE=.\SgCoordinate4.cpp
# End Source File
# Begin Source File

SOURCE=.\SgCoordinate4.h
# End Source File
# Begin Source File

SOURCE=.\SgNormal.cpp
# End Source File
# Begin Source File

SOURCE=.\SgNormal.h
# End Source File
# Begin Source File

SOURCE=.\SgVecSequence.h
# End Source File
# End Group
# Begin Group "Lights"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SgDirectionalLight.cpp
# End Source File
# Begin Source File

SOURCE=.\SgDirectionalLight.h
# End Source File
# Begin Source File

SOURCE=.\SgLight.cpp
# End Source File
# Begin Source File

SOURCE=.\SgLight.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\SgNode.cpp
# End Source File
# Begin Source File

SOURCE=.\SgNode.h
# End Source File
# Begin Source File

SOURCE=.\SgNodeMacros.h
# End Source File
# End Group
# Begin Group "Visitors"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SgGetBoundingBox.cpp
# End Source File
# Begin Source File

SOURCE=.\SgGetBoundingBox.h
# End Source File
# Begin Source File

SOURCE=.\SgGetCount.cpp
# End Source File
# Begin Source File

SOURCE=.\SgGetCount.h
# End Source File
# Begin Source File

SOURCE=.\SgGetRayIntersection.cpp
# End Source File
# Begin Source File

SOURCE=.\SgGetRayIntersection.h
# End Source File
# Begin Source File

SOURCE=.\SgOffScreenRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\SgOffScreenRenderer.h
# End Source File
# Begin Source File

SOURCE=.\SgRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\SgRenderer.h
# End Source File
# Begin Source File

SOURCE=.\SgScreenRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\SgScreenRenderer.h
# End Source File
# Begin Source File

SOURCE=.\SgVisitor.cpp
# End Source File
# Begin Source File

SOURCE=.\SgVisitor.h
# End Source File
# Begin Source File

SOURCE=.\SgVisitorMacros.h
# End Source File
# End Group
# Begin Group "Viewers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SgViewer.cpp
# End Source File
# Begin Source File

SOURCE=.\SgViewer.h
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SgAllNodes.h
# End Source File
# Begin Source File

SOURCE=.\SgConstants.h
# End Source File
# Begin Source File

SOURCE=.\SgDefine.h
# End Source File
# Begin Source File

SOURCE=.\SgDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\SgDraw.h
# End Source File
# Begin Source File

SOURCE=.\SgMessage.h
# End Source File
# Begin Source File

SOURCE=.\SgMessageIds.h
# End Source File
# Begin Source File

SOURCE=.\SgPath.cpp
# End Source File
# Begin Source File

SOURCE=.\SgPath.h
# End Source File
# Begin Source File

SOURCE=.\SgTypedefs.h
# End Source File
# End Group
# End Group
# Begin Group "DLL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SgApi.h
# End Source File
# Begin Source File

SOURCE=.\SgDll.cpp
# End Source File
# End Group
# Begin Group "Notes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Notes\SceneGraph.txt
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\Nurbs\Core\notes.txt
# End Source File
# End Target
# End Project
