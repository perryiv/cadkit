
# Include all variables, flags, etc...
include Make/header.mak

# C++ source files.
CPP_FILES = \
  ./Threads/Named.cpp \
  ./Threads/Callback.cpp \
  ./Threads/RecursiveMutex.cpp \
  ./Threads/Task.cpp \
  ./Threads/Pool.cpp \
  ./Threads/Manager.cpp \
  ./Threads/Thread.cpp \
  ./Threads/Mutex.cpp \
  ./Console/Feedback.cpp \
  ./Shared/Preferences.cpp \
  ./Factory/BaseCreator.cpp \
  ./Factory/ObjectFactory.cpp \
  ./Scope/Timer.cpp \
  ./Scope/CurrentDirectory.cpp \
  ./Jobs/Manager.cpp \
  ./Jobs/Job.cpp \
  ./Predicates/CanWrite.cpp \
  ./System/Socket.cpp \
  ./System/LastError.cpp \
  ./System/DateTime.cpp \
  ./System/Host.cpp \
  ./System/Directory.cpp \
  ./System/Screen.cpp \
  ./System/Sleep.cpp \
  ./System/Clock.cpp \
  ./System/Memory.cpp \
  ./System/Environment.cpp \
  ./Types/Types.cpp \
  ./File/Remove.cpp \
  ./File/Rename.cpp \
  ./File/Make.cpp \
  ./File/Temp.cpp \
  ./Commands/RenderLoop.cpp \
  ./Commands/Command.cpp \
  ./Commands/RenderingPasses.cpp \
  ./Commands/PolygonMode.cpp \
  ./Commands/ShadeModel.cpp \
  ./Commands/History.cpp \
  ./Resources/TextWindow.cpp \
  ./Resources/CancelButton.cpp \
  ./Resources/ProgressBar.cpp \
  ./Resources/EventQueue.cpp \
  ./Resources/StatusBar.cpp \
  ./Resources/ReportErrors.cpp \
  ./Resources/Manager.cpp \
  ./Registry/Database.cpp \
  ./Registry/Node.cpp \
  ./Registry/Visitor.cpp \
  ./User/Directory.cpp \
  ./ID/GUID.cpp \
  ./Functions/Color.cpp \
  ./Functions/GUID.cpp \
  ./DLL/Listener.cpp \
  ./DLL/Library.cpp \
  ./DLL/LibraryPool.cpp \
  ./DLL/Loader.cpp \
  ./Trace/Scope.cpp \
  ./Trace/Print.cpp \
  ./Errors/Stack.cpp \
  ./Errors/Error.cpp \
  ./Errors/Assert.cpp \
  ./File/Log.cpp \
  ./Functors/Interaction/Wand/WandPosition.cpp \
  ./Functors/Interaction/Wand/WandRotation.cpp \
  ./Functors/Interaction/Wand/WandMatrix.cpp \
  ./Functors/Interaction/Navigate/Translate.cpp \
  ./Functors/Interaction/Navigate/Direction.cpp \
  ./Functors/Interaction/Navigate/Rotate.cpp \
  ./Functors/Interaction/Navigate/Transform.cpp \
  ./Functors/Interaction/Common/Sequence.cpp \
  ./Functors/Interaction/Common/BaseFunctor.cpp \
  ./Functors/Interaction/Matrix/InverseMatrix.cpp \
  ./Functors/Interaction/Matrix/MatrixPair.cpp \
  ./Functors/Interaction/Matrix/IdentityMatrix.cpp \
  ./Functors/Interaction/Matrix/MatrixFunctor.cpp \
  ./Functors/Interaction/Input/WandYaw.cpp \
  ./Functors/Interaction/Input/WandRoll.cpp \
  ./Functors/Interaction/Input/JoystickVertical.cpp \
  ./Functors/Interaction/Input/JoystickHorizontal.cpp \
  ./Functors/Interaction/Input/AnalogInput.cpp \
  ./Functors/Interaction/Input/WandPitch.cpp \
  ./Functors/Interaction/Input/WandAngle.cpp \
  ./Base/InstanceManager.cpp \
  ./Base/Referenced.cpp \
  ./Base/Observed.cpp \
  ./Base/Object.cpp \
  ./Base/Typed.cpp \
  ./CommandLine/Arguments.cpp \
  ./CommandLine/Options.cpp \
  ./CommandLine/Flags.cpp \
  ./App/Application.cpp \
  ./Documents/Document.cpp \
  ./Documents/Manager.cpp \
  ./Components/Manager.cpp

LOCAL_INCLUDE = \
	-I../

# The name of the thing we are making.
TARGET = Usul

# Include the rules for make...
include Make/library.mak
