# Microsoft Developer Studio Project File - Name="Interfaces" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Interfaces - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Interfaces.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Interfaces.mak" CFG="Interfaces - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Interfaces - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Interfaces - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Interfaces - Win32 Release"

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
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

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
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "Interfaces - Win32 Release"
# Name "Interfaces - Win32 Debug"
# Begin Source File

SOURCE=.\Enum.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Handles.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IChildQuery.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IClientData.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ICommandLine.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IControlled.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IDataRead.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IDataSource.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IDataTarget.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IDataWrite.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IEntityNotify.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IEntityQuery.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IFileExtension.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IFormatAttribute.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IInstanceQuery.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IInterfaceGroup.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ILoadOptions.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ILodOption.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IMessageNotify.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IMessagePriority.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IOutputAttribute.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IOutputPrecision.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IOutputStream.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IQueryArray.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IQueryColors.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IQueryNormals.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IQueryTexCoords.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IQueryVertices.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IScale.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IShapeQuery.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ISubEntityNotify.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ITriangleAppend.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IUnknown.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IZeroRange.h

!IF  "$(CFG)" == "Interfaces - Win32 Release"

!ELSEIF  "$(CFG)" == "Interfaces - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Target
# End Project
