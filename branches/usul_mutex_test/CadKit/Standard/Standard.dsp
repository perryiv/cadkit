# Microsoft Developer Studio Project File - Name="Standard" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Standard - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Standard.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Standard.mak" CFG="Standard - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Standard - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Standard - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Standard - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /WX /GX /O2 /I "../" /D "_LIB" /D "_CADKIT_USE_PRECOMPILED_HEADERS" /D "_MBCS" /D "WIN32" /D "NDEBUG" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../bin/CkSl.lib"

!ELSEIF  "$(CFG)" == "Standard - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GX /Zi /Od /I "../" /D "_MBCS" /D "_LIB" /D "_CADKIT_USE_PRECOMPILED_HEADERS" /D "WIN32" /D "_DEBUG" /FD /GZ /c
# SUBTRACT CPP /Fr /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../bin/CkSld.lib"

!ENDIF 

# Begin Target

# Name "Standard - Win32 Release"
# Name "Standard - Win32 Debug"
# Begin Group "Precompiled"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SlClientHeaders.h
# End Source File
# Begin Source File

SOURCE=.\SlPrecompiled.cpp
# End Source File
# Begin Source File

SOURCE=.\SlPrecompiled.h
# End Source File
# End Group
# Begin Group "Vector"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SlVec2.h
# End Source File
# Begin Source File

SOURCE=.\SlVec2IO.h
# End Source File
# Begin Source File

SOURCE=.\SlVec3.h
# End Source File
# Begin Source File

SOURCE=.\SlVec3IO.h
# End Source File
# Begin Source File

SOURCE=.\SlVec4.h
# End Source File
# Begin Source File

SOURCE=.\SlVec4IO.h
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SlAbsolute.h
# End Source File
# Begin Source File

SOURCE=.\SlAssert.h
# End Source File
# Begin Source File

SOURCE=.\SlBinaryRecordParser.h
# End Source File
# Begin Source File

SOURCE=.\SlBitmask.h
# End Source File
# Begin Source File

SOURCE=.\SlCallback.h
# End Source File
# Begin Source File

SOURCE=.\SlCallbackList.h
# End Source File
# Begin Source File

SOURCE=.\SlCharacters.h
# End Source File
# Begin Source File

SOURCE=.\SlClamp.h
# End Source File
# Begin Source File

SOURCE=.\SlConstants.h
# End Source File
# Begin Source File

SOURCE=.\SlDeclareConst.h
# End Source File
# Begin Source File

SOURCE=.\SlEndian.h
# End Source File
# Begin Source File

SOURCE=.\SlFunctionDefines.h
# End Source File
# Begin Source File

SOURCE=.\SlInline.h
# End Source File
# Begin Source File

SOURCE=.\SlInlineMath.h
# End Source File
# Begin Source File

SOURCE=.\SlMinMax.h
# End Source File
# Begin Source File

SOURCE=.\SlPreComputedArrays.cpp
# End Source File
# Begin Source File

SOURCE=.\SlPreComputedArrays.h
# End Source File
# Begin Source File

SOURCE=.\SlScopedSet.h
# End Source File
# Begin Source File

SOURCE=.\SlSwap.h
# End Source File
# Begin Source File

SOURCE=.\SlTemplateSupport.h
# End Source File
# Begin Source File

SOURCE=.\SlTestEquality.h
# End Source File
# Begin Source File

SOURCE=.\SlTrackball.h
# End Source File
# Begin Source File

SOURCE=.\SlTruncate.h
# End Source File
# Begin Source File

SOURCE=.\SlTuple.h
# End Source File
# Begin Source File

SOURCE=.\SlTypedefs.h
# End Source File
# Begin Source File

SOURCE=.\SlViewport.h
# End Source File
# End Group
# Begin Group "Base Class"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SlBase.cpp
# End Source File
# Begin Source File

SOURCE=.\SlBase.h
# End Source File
# Begin Source File

SOURCE=.\SlBaseMacros.h
# End Source File
# Begin Source File

SOURCE=.\SlType.cpp
# End Source File
# Begin Source File

SOURCE=.\SlType.h
# End Source File
# End Group
# Begin Group "Strings"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SlConvertStrings.h
# End Source File
# Begin Source File

SOURCE=.\SlString.h
# End Source File
# Begin Source File

SOURCE=.\SlStringCase.h
# End Source File
# Begin Source File

SOURCE=.\SlStringCompare.h
# End Source File
# Begin Source File

SOURCE=.\SlStringFunctions.h
# End Source File
# Begin Source File

SOURCE=.\SlStringIO.h
# End Source File
# Begin Source File

SOURCE=.\SlStringReplace.h
# End Source File
# Begin Source File

SOURCE=.\SlStringTrim.h
# End Source File
# Begin Source File

SOURCE=.\SlUnicode.h
# End Source File
# End Group
# Begin Group "Threads"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SlSingleThreaded.h
# End Source File
# Begin Source File

SOURCE=.\SlThread.h
# End Source File
# End Group
# Begin Group "Color and Material"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SlColor.h
# End Source File
# Begin Source File

SOURCE=.\SlMaterial.h
# End Source File
# End Group
# Begin Group "Matrix"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SlMatrix44.h
# End Source File
# Begin Source File

SOURCE=.\SlMatrix44IO.h
# End Source File
# Begin Source File

SOURCE=.\SlMatrixNN.h
# End Source File
# Begin Source File

SOURCE=.\SlMatrixNNIO.h
# End Source File
# End Group
# Begin Group "Geometry"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SlBoundingBox.h
# End Source File
# Begin Source File

SOURCE=.\SlGeometry.h
# End Source File
# Begin Source File

SOURCE=.\SlLine2.h
# End Source File
# Begin Source File

SOURCE=.\SlLine3.h
# End Source File
# End Group
# Begin Group "Debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SlPrint.h
# End Source File
# Begin Source File

SOURCE=.\SlTrace.cpp
# End Source File
# Begin Source File

SOURCE=.\SlTrace.h
# End Source File
# End Group
# Begin Group "Reference Counting"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SlRefBase.cpp
# End Source File
# Begin Source File

SOURCE=.\SlRefBase.h
# End Source File
# End Group
# Begin Group "Containers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SlList.h
# End Source File
# Begin Source File

SOURCE=.\SlMatrixStack.h
# End Source File
# Begin Source File

SOURCE=.\SlPartitionedVector.h
# End Source File
# Begin Source File

SOURCE=.\SlStack.h
# End Source File
# End Group
# Begin Group "System"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SlSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\SlSystem.h
# End Source File
# End Group
# Begin Group "Messages"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SlMessage.h
# End Source File
# Begin Source File

SOURCE=.\SlMessageIds.h
# End Source File
# End Group
# Begin Group "Pointers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SlArrayPtr.h
# End Source File
# Begin Source File

SOURCE=.\SlAutoPtr.h
# End Source File
# Begin Source File

SOURCE=.\SlQueryPtr.h
# End Source File
# Begin Source File

SOURCE=.\SlRefPtr.h
# End Source File
# End Group
# Begin Group "File System"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SlFileStats.h
# End Source File
# Begin Source File

SOURCE=.\SlFileVerify.h
# End Source File
# Begin Source File

SOURCE=.\SlPathname.h
# End Source File
# End Group
# Begin Group "Functors"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SlErrorPolicy.h
# End Source File
# Begin Source File

SOURCE=.\SlFindExtreme.h
# End Source File
# Begin Source File

SOURCE=.\SlTruncateFunctor.h
# End Source File
# End Group
# Begin Group "IO"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SlManip.h
# End Source File
# Begin Source File

SOURCE=.\SlStreamSetReset.h
# End Source File
# End Group
# End Target
# End Project
