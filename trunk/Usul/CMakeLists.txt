
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
./Threads/Named.cpp
./Threads/Callback.cpp
./Threads/RecursiveMutex.cpp
./Threads/Task.cpp
./Threads/Pool.cpp
./Threads/Manager.cpp
./Threads/Thread.cpp
./Threads/Mutex.cpp
./Console/Feedback.cpp
./Shared/Preferences.cpp
./Factory/BaseCreator.cpp
./Factory/ObjectFactory.cpp
./Scope/Timer.cpp
./Jobs/Manager.cpp
./Jobs/Job.cpp
./Predicates/CanWrite.cpp
./System/Socket.cpp
./System/LastError.cpp
./System/DateTime.cpp
./System/Host.cpp
./System/Directory.cpp
./System/Screen.cpp
./System/Sleep.cpp
./System/Clock.cpp
./System/Memory.cpp
./System/Environment.cpp
./Types/Types.cpp
./File/Remove.cpp
./File/Rename.cpp
./File/Make.cpp
./File/Temp.cpp
./Commands/RenderLoop.cpp
./Commands/Command.cpp
./Commands/RenderingPasses.cpp
./Commands/PolygonMode.cpp
./Commands/ShadeModel.cpp
./Commands/History.cpp
./Resources/TextWindow.cpp
./Resources/CancelButton.cpp
./Resources/ProgressBar.cpp
./Resources/EventQueue.cpp
./Resources/StatusBar.cpp
./Resources/ReportErrors.cpp
./Resources/Manager.cpp
./Registry/Database.cpp
./Registry/Node.cpp
./Registry/Visitor.cpp
./User/Directory.cpp
./State/Busy.cpp
./ID/GUID.cpp
./Functions/Color.cpp
./Functions/GUID.cpp
./DLL/Listener.cpp
./DLL/Library.cpp
./DLL/LibraryPool.cpp
./DLL/Loader.cpp
./Trace/Scope.cpp
./Trace/Print.cpp
./Errors/Stack.cpp
./Errors/Error.cpp
./File/Log.cpp
./Functors/Interaction/Wand/WandPosition.cpp
./Functors/Interaction/Wand/WandRotation.cpp
./Functors/Interaction/Wand/WandMatrix.cpp
./Functors/Interaction/Navigate/Translate.cpp
./Functors/Interaction/Navigate/Direction.cpp
./Functors/Interaction/Navigate/Rotate.cpp
./Functors/Interaction/Navigate/Transform.cpp
./Functors/Interaction/Common/Sequence.cpp
./Functors/Interaction/Common/BaseFunctor.cpp
./Functors/Interaction/Matrix/InverseMatrix.cpp
./Functors/Interaction/Matrix/MatrixPair.cpp
./Functors/Interaction/Matrix/IdentityMatrix.cpp
./Functors/Interaction/Matrix/MatrixFunctor.cpp
./Functors/Interaction/Input/WandYaw.cpp
./Functors/Interaction/Input/WandRoll.cpp
./Functors/Interaction/Input/JoystickVertical.cpp
./Functors/Interaction/Input/JoystickHorizontal.cpp
./Functors/Interaction/Input/AnalogInput.cpp
./Functors/Interaction/Input/WandPitch.cpp
./Functors/Interaction/Input/WandAngle.cpp
./Base/InstanceManager.cpp
./Base/Referenced.cpp
./Base/Observed.cpp
./Base/Object.cpp
./Base/Typed.cpp
./CommandLine/Arguments.cpp
./CommandLine/Options.cpp
./CommandLine/Flags.cpp
./App/Application.cpp
./Documents/Document.cpp
./Documents/Manager.cpp
./Components/Manager.cpp
./Devices/Factory.cpp
./Devices/State.cpp
./Devices/BaseDevice.cpp
)


SET(LIBNAME Usul)


# Create a Shared Library
ADD_LIBRARY(${LIBNAME} SHARED ${SOURCES} )


# Link the Library	
IF(WIN32)
	TARGET_LINK_LIBRARIES( Usul User32 rpcrt4 Ws2_32 )
ELSE(WIN32)
	TARGET_LINK_LIBRARIES( Usul dl uuid )
ENDIF(WIN32)

INSTALL(TARGETS Usul
  LIBRARY DESTINATION bin
)
