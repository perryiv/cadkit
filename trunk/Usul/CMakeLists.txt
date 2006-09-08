
PROJECT(Usul)

# If the user has defined USUL_BIN_DIR as an environment variable
#  then use that directory as the output directory. Otherwise set the output
#  directory to be inside the current build directory.
# First check to see if it was defined in another project
IF ( NOT USUL_BIN_DIR ) 
set(USUL_BIN_DIR "$ENV{USUL_BIN_DIR}")
ENDIF ( NOT USUL_BIN_DIR )
# This will be true if the user has NOT defined an environment variable
IF ( NOT USUL_BIN_DIR )
set(USUL_BIN_DIR ${PROJECT_BINARY_DIR}/Bin/${CMAKE_BUILD_TYPE} )
ENDIF ( NOT USUL_BIN_DIR )



#setup output Directories
SET (LIBRARY_OUTPUT_PATH
	${USUL_BIN_DIR}
	CACHE PATH
	"Single Directory for all Libraries"
	FORCE
)

#setup the Executable output Directory
SET (EXECUTABLE_OUTPUT_PATH
	${USUL_BIN_DIR}
	CACHE PATH
	"Single Directory for all Executables."
	FORCE
)

#Include the Top Level Directory
INCLUDE_DIRECTORIES( "${PROJECT_SOURCE_DIR}/../" )

#List the Sources
SET (SOURCES
	App/Controller.cpp
	Base/InstanceManager.cpp
	Base/Referenced.cpp
	Base/Typed.cpp
	CommandLine/Arguments.cpp
	CommandLine/Flags.cpp
	CommandLine/Options.cpp
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
	File/Make.cpp
	File/Remove.cpp
	File/Temp.cpp
	Functions/Color.cpp
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
	System/Socket.cpp
	Threads/Mutex.cpp
	Threads/RecursiveMutex.cpp
	Types/Types.cpp
	User/Directory.cpp
)


# Create a Shared Library
ADD_LIBRARY(Usul SHARED ${SOURCES} )

# Link the Library	
TARGET_LINK_LIBRARIES(Usul)
	
