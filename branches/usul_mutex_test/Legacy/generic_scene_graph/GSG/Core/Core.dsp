# Microsoft Developer Studio Project File - Name="Core" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Core - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Core.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Core.mak" CFG="Core - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Core - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Core - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Core - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /WX /GR /GX /O2 /I "../../" /I "$(BOOST_ROOT_DIR)" /D "COMPILING_GSG_CORE" /D "_MBCS" /D "_USRDLL" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /YX"Precompiled.h" /FD /Zm200 /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"../../bin/gsg_core.dll"

!ELSEIF  "$(CFG)" == "Core - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /WX /Gm /GR /GX /Zi /Od /I "../../" /I "$(BOOST_ROOT_DIR)" /D "COMPILING_GSG_CORE" /D "_USRDLL" /D "_MBCS" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX"Precompiled.h" /FD /GZ /Zm200 /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"../../bin/gsg_core_d.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Core - Win32 Release"
# Name "Core - Win32 Debug"
# Begin Group "Precompiled"

# PROP Default_Filter "*.cpp;*.h"
# Begin Source File

SOURCE=.\Precompiled.cpp
# End Source File
# Begin Source File

SOURCE=.\Precompiled.h
# End Source File
# End Group
# Begin Group "Source"

# PROP Default_Filter ""
# Begin Group "Nodes"

# PROP Default_Filter ""
# Begin Group "Group"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Camera.cpp
# End Source File
# Begin Source File

SOURCE=.\Camera.h
# End Source File
# Begin Source File

SOURCE=.\Group.cpp
# End Source File
# Begin Source File

SOURCE=.\Group.h
# End Source File
# Begin Source File

SOURCE=.\Lod.cpp
# End Source File
# Begin Source File

SOURCE=.\Lod.h
# End Source File
# Begin Source File

SOURCE=.\Transform.cpp
# End Source File
# Begin Source File

SOURCE=.\Transform.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Node.cpp
# End Source File
# Begin Source File

SOURCE=.\Node.h
# End Source File
# Begin Source File

SOURCE=.\Shape.cpp
# End Source File
# Begin Source File

SOURCE=.\Shape.h
# End Source File
# End Group
# Begin Group "Primitives"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Primitive.cpp
# End Source File
# Begin Source File

SOURCE=.\Primitive.h
# End Source File
# Begin Source File

SOURCE=.\PrimitiveSet.cpp
# End Source File
# Begin Source File

SOURCE=.\PrimitiveSet.h
# End Source File
# End Group
# Begin Group "Visitors"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BinBuilder.cpp
# End Source File
# Begin Source File

SOURCE=.\BinBuilder.h
# End Source File
# Begin Source File

SOURCE=.\Draw.cpp
# End Source File
# Begin Source File

SOURCE=.\Draw.h
# End Source File
# Begin Source File

SOURCE=.\FrustumCull.cpp
# End Source File
# Begin Source File

SOURCE=.\FrustumCull.h
# End Source File
# Begin Source File

SOURCE=.\Renderer.cpp
# End Source File
# Begin Source File

SOURCE=.\Renderer.h
# End Source File
# Begin Source File

SOURCE=.\Sort.cpp
# End Source File
# Begin Source File

SOURCE=.\Sort.h
# End Source File
# Begin Source File

SOURCE=.\Update.cpp
# End Source File
# Begin Source File

SOURCE=.\Update.h
# End Source File
# Begin Source File

SOURCE=.\Visitor.cpp
# End Source File
# Begin Source File

SOURCE=.\Visitor.h
# End Source File
# End Group
# Begin Group "Attributes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Attribute.cpp
# End Source File
# Begin Source File

SOURCE=.\Attribute.h
# End Source File
# Begin Source File

SOURCE=.\AttributeSet.cpp
# End Source File
# Begin Source File

SOURCE=.\AttributeSet.h
# End Source File
# Begin Source File

SOURCE=.\Color.h
# End Source File
# Begin Source File

SOURCE=.\Defaults.h
# End Source File
# Begin Source File

SOURCE=.\Material.cpp
# End Source File
# Begin Source File

SOURCE=.\Material.h
# End Source File
# Begin Source File

SOURCE=.\PolygonMode.cpp
# End Source File
# Begin Source File

SOURCE=.\PolygonMode.h
# End Source File
# End Group
# Begin Group "Pools"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\InterleavedPool.cpp
# End Source File
# Begin Source File

SOURCE=.\InterleavedPool.h
# End Source File
# Begin Source File

SOURCE=.\Vec2Pool.cpp
# End Source File
# Begin Source File

SOURCE=.\Vec2Pool.h
# End Source File
# Begin Source File

SOURCE=.\Vec3Pool.cpp
# End Source File
# Begin Source File

SOURCE=.\Vec3Pool.h
# End Source File
# Begin Source File

SOURCE=.\Vec4Pool.cpp
# End Source File
# Begin Source File

SOURCE=.\Vec4Pool.h
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Assert.h
# End Source File
# Begin Source File

SOURCE=.\Base.h
# End Source File
# Begin Source File

SOURCE=.\Bits.h
# End Source File
# Begin Source File

SOURCE=.\BoundingBox.h
# End Source File
# Begin Source File

SOURCE=.\BoundingSphere.h
# End Source File
# Begin Source File

SOURCE=.\Factory.cpp
# End Source File
# Begin Source File

SOURCE=.\Factory.h
# End Source File
# Begin Source File

SOURCE=.\Interaction.cpp
# End Source File
# Begin Source File

SOURCE=.\Interaction.h
# End Source File
# Begin Source File

SOURCE=.\Keys.cpp
# End Source File
# Begin Source File

SOURCE=.\Keys.h
# End Source File
# Begin Source File

SOURCE=.\Message.h
# End Source File
# Begin Source File

SOURCE=.\Path.cpp
# End Source File
# Begin Source File

SOURCE=.\Path.h
# End Source File
# Begin Source File

SOURCE=.\Pointer.h
# End Source File
# Begin Source File

SOURCE=.\Referenced.cpp
# End Source File
# Begin Source File

SOURCE=.\Referenced.h
# End Source File
# Begin Source File

SOURCE=.\Traits.h
# End Source File
# Begin Source File

SOURCE=.\Viewer.cpp
# End Source File
# Begin Source File

SOURCE=.\Viewer.h
# End Source File
# Begin Source File

SOURCE=.\Viewport.h
# End Source File
# End Group
# Begin Group "Math"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Constants.h
# End Source File
# Begin Source File

SOURCE=.\Equality.h
# End Source File
# Begin Source File

SOURCE=.\Math.h
# End Source File
# Begin Source File

SOURCE=.\Matrix.h
# End Source File
# Begin Source File

SOURCE=.\Vector2.h
# End Source File
# Begin Source File

SOURCE=.\Vector3.h
# End Source File
# Begin Source File

SOURCE=.\Vector4.h
# End Source File
# End Group
# Begin Group "Config"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Config.h
# End Source File
# Begin Source File

SOURCE=.\Define.h
# End Source File
# Begin Source File

SOURCE=.\ErrorPolicy.h
# End Source File
# Begin Source File

SOURCE=.\Threads.h
# End Source File
# End Group
# Begin Group "Bin"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\RenderBin.cpp
# End Source File
# Begin Source File

SOURCE=.\RenderBin.h
# End Source File
# Begin Source File

SOURCE=.\RenderElement.cpp
# End Source File
# Begin Source File

SOURCE=.\RenderElement.h
# End Source File
# End Group
# Begin Group "Details"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Adaptors.h
# End Source File
# Begin Source File

SOURCE=.\Algorithms.h
# End Source File
# Begin Source File

SOURCE=.\Container.h
# End Source File
# Begin Source File

SOURCE=.\Functors.h
# End Source File
# Begin Source File

SOURCE=.\Predicates.h
# End Source File
# Begin Source File

SOURCE=.\PushPop.cpp
# End Source File
# Begin Source File

SOURCE=.\PushPop.h
# End Source File
# End Group
# End Group
# Begin Group "DLL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CoreDll.cpp
# End Source File
# Begin Source File

SOURCE=.\CoreDll.h
# End Source File
# End Group
# End Target
# End Project
