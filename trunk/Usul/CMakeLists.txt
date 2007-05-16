
PROJECT(Usul)

SET(CMakeModules "${PROJECT_SOURCE_DIR}/../CMakeModules")

# include common defines and macros for all cadkit projects.
INCLUDE ( ${CMakeModules}/Cadkit.cmake)

#setup output Directories
SET (LIBRARY_OUTPUT_PATH
	${CADKIT_BIN_DIR}
	CACHE PATH
	"Single Directory for all Libraries"
)

#setup the Executable output Directory
SET (EXECUTABLE_OUTPUT_PATH
	${CADKIT_BIN_DIR}
	CACHE PATH
	"Single Directory for all Executables."
)

#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/../" )

ADD_DEFINITIONS("-D_COMPILING_USUL")


#List the Sources
SET (SOURCES
	App/Controller.cpp
	Base/InstanceManager.cpp
	Base/Object.cpp
	Base/Referenced.cpp
	Base/Typed.cpp
	CommandLine/Arguments.cpp
	CommandLine/Flags.cpp
	CommandLine/Options.cpp
	Commands/Command.cpp
	Components/Manager.cpp
	Components/Object.cpp
	Components/Registry.cpp
	Devices/BaseDevice.cpp
	Devices/Factory.cpp
	Devices/State.cpp
	DLL/Library.cpp
	DLL/Listener.cpp
	DLL/Loader.cpp
	Documents/Document.cpp
	Documents/Manager.cpp
	Errors/Stack.cpp
	Errors/Error.cpp
	File/Make.cpp
	File/Remove.cpp
	File/Temp.cpp
	Functions/Color.cpp
	Functions/Guid.cpp
	ID/GUID.cpp
	Predicates/CanWrite.cpp
	Resources/CancelButton.cpp
	Resources/EventQueue.cpp
	Resources/MenuBar.cpp
	Resources/ProgressBar.cpp
	Resources/ReportErrors.cpp
	Resources/StatusBar.cpp
	Resources/TextWindow.cpp
	Scope/Timer.cpp
	Shared/Preferences.cpp
	State/Busy.cpp
	System/Clock.cpp
	System/DateTime.cpp
	System/Environment.cpp
	System/Host.cpp
	System/LastError.cpp
	System/Memory.cpp
	System/Screen.cpp
	System/Sleep.cpp
	System/Socket.cpp
	Threads/Mutex.cpp
	Threads/Named.cpp
	Threads/RecursiveMutex.cpp
	Threads/Task.cpp
	Threads/Thread.cpp
	Threads/Pool.cpp
	Threads/Callback.cpp
	Threads/Manager.cpp
	Trace/Scope.cpp
	Trace/Print.cpp
	Types/Types.cpp
	User/Directory.cpp
)


SET(LIBNAME Usul)


# Create a Shared Library
ADD_LIBRARY(${LIBNAME} SHARED ${SOURCES} )


# Link the Library	
TARGET_LINK_LIBRARIES(Usul)
