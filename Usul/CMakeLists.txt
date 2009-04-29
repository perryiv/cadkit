
#########################################################
#
#  CMakeLists for Usul
#
#########################################################

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

#########################################################
#
#  Add Source groups.
#
#########################################################

SOURCE_GROUP ( Adaptors    REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Adaptors/* )
SOURCE_GROUP ( Algorithms  REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Algorithms/* )
SOURCE_GROUP ( App         REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/App/* )
SOURCE_GROUP ( Base        REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Base/* )
SOURCE_GROUP ( Bits        REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Bits/* )
SOURCE_GROUP ( Cast        REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Cast/* )
SOURCE_GROUP ( CommandLine REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/CommandLine/* )
SOURCE_GROUP ( Commands    REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Commands/* )
SOURCE_GROUP ( Components  REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Components/* )
SOURCE_GROUP ( Config      REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Config/* )
SOURCE_GROUP ( Console     REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Console/* )
SOURCE_GROUP ( Containers  REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Containers/* )
SOURCE_GROUP ( Convert     REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Convert/* )
SOURCE_GROUP ( DLL         REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/DLL/* )
SOURCE_GROUP ( Diagnostics REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Diagnostics/* )
SOURCE_GROUP ( Documents   REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Documents/* )
SOURCE_GROUP ( Endian      REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Endian/* )
SOURCE_GROUP ( Errors      REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Errors/* )
SOURCE_GROUP ( Exceptions  REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Exceptions/* )
SOURCE_GROUP ( Export      REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Export/* )
SOURCE_GROUP ( Factory     REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Factory/* )
SOURCE_GROUP ( File        REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/File/* )
SOURCE_GROUP ( Functions   REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Functions/* )
SOURCE_GROUP ( Functors    REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Functors/* )
SOURCE_GROUP ( Headers     REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Headers/* )
SOURCE_GROUP ( ID          REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/ID/* )
SOURCE_GROUP ( IO          REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/IO/* )
SOURCE_GROUP ( Interfaces  REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Interfaces/* )
SOURCE_GROUP ( Jobs        REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Jobs/* )
SOURCE_GROUP ( Loops       REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Loops/* )
SOURCE_GROUP ( MPL         REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/MPL/* )
SOURCE_GROUP ( Math        REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Math/* )
SOURCE_GROUP ( Memory      REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Memory/* )
SOURCE_GROUP ( Network     REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Network/* )
SOURCE_GROUP ( Pointers    REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Pointers/* )
SOURCE_GROUP ( Policies    REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Policies/* )
SOURCE_GROUP ( Polygons    REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Polygons/* )
SOURCE_GROUP ( Predicates  REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Predicates/* )
SOURCE_GROUP ( Preprocess  REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Preprocess/* )
SOURCE_GROUP ( Print       REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}Print/* )
SOURCE_GROUP ( Properties  REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Properties/* )
SOURCE_GROUP ( Registry    REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Registry/* )
SOURCE_GROUP ( Resources   REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Resources/* )
SOURCE_GROUP ( Scope       REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Scope/* )
SOURCE_GROUP ( Shared      REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Shared/* )
SOURCE_GROUP ( Signals     REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Signals/* )
SOURCE_GROUP ( Strings     REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Strings/* )
SOURCE_GROUP ( System      REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/System/* )
SOURCE_GROUP ( Threads     REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Threads/* )
SOURCE_GROUP ( Trace       REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Trace/* )
SOURCE_GROUP ( Types       REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Types/* )
SOURCE_GROUP ( Units       REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/Units/* )
SOURCE_GROUP ( User        REGULAR_EXPRESSION ${PROJECT_SOURCE_DIR}/User/* )


#########################################################
#
#  List all the headers
#
#########################################################

SET ( HEADERS
./Adaptors/Bind.h
./Adaptors/Boost.h
./Adaptors/MemberFunction.h
./Adaptors/Random.h
./Algorithms/ConvexHullSort.h
./Algorithms/CopyIf.h
./Algorithms/Cylinder.h
./Algorithms/Extract.h
./Algorithms/Sphere.h
./Algorithms/TriStrip.h
./App/Application.h
./Base/BaseObject.h
./Base/InstanceManager.h
./Base/Object.h
./Base/Observed.h
./Base/Referenced.h
./Base/Typed.h
./Bits/Bits.h
./Cast/Cast.h
./CommandLine/Arguments.h
./CommandLine/Flags.h
./CommandLine/Options.h
./CommandLine/Parser.h
./Commands/Command.h
./Commands/GenericCheckCommand.h
./Commands/GenericCommand.h
./Commands/History.h
./Commands/PolygonMode.h
./Commands/RenderingPasses.h
./Commands/RenderLoop.h
./Commands/ShadeModel.h
./Components/Call.h
./Components/Create.h
./Components/Exceptions.h
./Components/Factory.h
./Components/Loader.h
./Components/Manager.h
./Config/Config.h
./Console/Feedback.h
./Containers/Array.h
./Containers/Array2D.h
./Containers/DynamicArray.h
./Containers/Hash.h
./Containers/OctTree.h
./Containers/Unknowns.h
./Containers/Vector.h
./Convert/Convert.h
./Convert/Generic.h
./Convert/Matrix44.h
./Convert/Vector2.h
./Convert/Vector3.h
./Convert/Vector4.h
./Diagnostics/StackTrace.h
./Diagnostics/StackTraceLinux.h
./Diagnostics/StackTraceMac.h
./Diagnostics/StackTraceWindows.h
./Diagnostics/Write.h
./Diagnostics/WriteLinux.h
./Diagnostics/WriteMac.h
./Diagnostics/WriteWindows.h
./DLL/Exceptions.h
./DLL/Library.h
./DLL/LibraryPool.h
./DLL/Listener.h
./DLL/Loader.h
./Documents/Document.h
./Documents/FileInfo.h
./Documents/Manager.h
./Endian/Endian.h
./Errors/Assert.h
./Errors/AssertPolicy.h
./Errors/Checker.h
./Errors/CompositePolicy.h
./Errors/DoNothingPolicy.h
./Errors/Error.h
./Errors/InvalidParameter.h
./Errors/PrintingPolicy.h
./Errors/PureVirtualCall.h
./Errors/Signals.h
./Errors/Stack.h
./Errors/ThrowingPolicy.h
./Exceptions/Canceled.h
./Exceptions/Catch.h
./Exceptions/Exception.h
./Exceptions/IO.h
./Exceptions/Thrower.h
./Exceptions/TimedOut.h
./Export/Export.h
./Factory/BaseCreator.h
./Factory/ObjectFactory.h
./Factory/RegisterCreator.h
./Factory/ScopedRegistration.h
./Factory/TypeCreator.h
./File/Boost.h
./File/Contents.h
./File/Find.h
./File/LineEnding.h
./File/Log.h
./File/Make.h
./File/Path.h
./File/Remove.h
./File/Rename.h
./File/Slash.h
./File/Stats.h
./File/StreamBuffer.h
./File/Temp.h
./Functions/Color.h
./Functions/Execute.h
./Functions/GLExtensions.h
./Functions/GUID.h
./Functions/SafeCall.h
./Functors/General/Delete.h
./Functors/General/If.h
./Functors/General/Increment.h
./Functors/General/Pair.h
./Functors/Interaction/Common/BaseFunctor.h
./Functors/Interaction/Common/Sequence.h
./Functors/Interaction/Input/AnalogInput.h
./Functors/Interaction/Input/JoystickHorizontal.h
./Functors/Interaction/Input/JoystickVertical.h
./Functors/Interaction/Input/WandAngle.h
./Functors/Interaction/Input/WandPitch.h
./Functors/Interaction/Input/WandRoll.h
./Functors/Interaction/Input/WandYaw.h
./Functors/Interaction/Matrix/IdentityMatrix.h
./Functors/Interaction/Matrix/InverseMatrix.h
./Functors/Interaction/Matrix/MatrixFunctor.h
./Functors/Interaction/Matrix/MatrixPair.h
./Functors/Interaction/Navigate/Direction.h
./Functors/Interaction/Navigate/Rotate.h
./Functors/Interaction/Navigate/Transform.h
./Functors/Interaction/Navigate/Translate.h
./Functors/Interaction/Wand/WandMatrix.h
./Functors/Interaction/Wand/WandPosition.h
./Functors/Interaction/Wand/WandRotation.h
./Headers/OpenGL.h
./Headers/Qt.h
./Headers/SStream.h
./Helper/AnimationNotify.h
./ID/GUID.h
./Interfaces/GUI/IAboutString.h
./Interfaces/GUI/ICancelButton.h
./Interfaces/GUI/IFlushEvents.h
./Interfaces/GUI/IGUIDelegate.h
./Interfaces/GUI/IGUIDelegateNotify.h
./Interfaces/GUI/IMaterialEditor.h
./Interfaces/GUI/IProgressBar.h
./Interfaces/GUI/IProgressBarFactory.h
./Interfaces/GUI/IQuestion.h
./Interfaces/GUI/IReportErrors.h
./Interfaces/GUI/ISetCursorType.h
./Interfaces/GUI/IStatusBar.h
./Interfaces/GUI/IUpdateGUI.h
./Interfaces/GUI/IUpdateTextWindow.h
./Interfaces/GUI/IUpdateTreeControls.h
./Interfaces/GUI/IWindow.h
./Interfaces/IActiveDocumentListener.h
./Interfaces/IActiveViewListener.h
./Interfaces/IAddRowLegend.h
./Interfaces/IAddSharedVertex.h
./Interfaces/IAddTriangle.h
./Interfaces/IAnimateNurbsCurve.h
./Interfaces/IAnimationNotify.h
./Interfaces/IAnimatePath.h
./Interfaces/IAnimateMatrices.h
./Interfaces/IArcGenReaderWriter.h
./Interfaces/IAxes.h
./Interfaces/IBackground.h
./Interfaces/IBackgroundColor.h
./Interfaces/IBooleanState.h
./Interfaces/IBuildScene.h
./Interfaces/IBusyState.h
./Interfaces/IButtonID.h
./Interfaces/IButtonPressListener.h
./Interfaces/IButtonPressSubject.h
./Interfaces/IButtonReleaseListener.h
./Interfaces/IButtonReleaseSubject.h
./Interfaces/ICalculatePerVertexNormals.h
./Interfaces/ICamera.h
./Interfaces/ICancel.h
./Interfaces/ICanceledStateGet.h
./Interfaces/ICanClose.h
./Interfaces/ICanInsert.h
./Interfaces/ICanRedo.h
./Interfaces/ICanUndo.h
./Interfaces/ICFDCommands.h
./Interfaces/IClassFactory.h
./Interfaces/ICleanUp.h
./Interfaces/IClippingDistance.h
./Interfaces/IClippingPlanes.h
./Interfaces/IClonable.h
./Interfaces/IColorEditor.h
./Interfaces/IColorsPerVertex.h
./Interfaces/ICommand.h
./Interfaces/ICommandExecuteListener.h
./Interfaces/ICommandQueueAdd.h
./Interfaces/IComputeTriangleNormals.h
./Interfaces/IContextMenuAdd.h
./Interfaces/ICoordinateTransform.h
./Interfaces/ICreateTriangleDocument.h
./Interfaces/ICullSceneVisitor.h
./Interfaces/IDatabasePager.h
./Interfaces/IDataChangedListener.h
./Interfaces/IDataChangedNotify.h
./Interfaces/IDecimateTriangles.h
./Interfaces/IDefaultGUIDelegate.h
./Interfaces/IDeleteAllConnected.h
./Interfaces/IDeletePrimitive.h
./Interfaces/IDirtyState.h
./Interfaces/IDisplaylists.h
./Interfaces/IDldNavigator.h
./Interfaces/IDocument.h
./Interfaces/IDocumentCreate.h
./Interfaces/IDocumentSelect.h
./Interfaces/IDrtCommands.h
./Interfaces/IElevationDatabase.h
./Interfaces/IExportImage.h
./Interfaces/IExportModel.h
./Interfaces/IExportScene.h
./Interfaces/IFindLoops.h
./Interfaces/IFlipNormal.h
./Interfaces/IFlipNormals.h
./Interfaces/IFrameDump.h
./Interfaces/IFrameInfo.h
./Interfaces/IFrameStamp.h
./Interfaces/IGeneralShader.h
./Interfaces/IGetBoundingBox.h
./Interfaces/IGetDocument.h
./Interfaces/IGetImageData.h
./Interfaces/IGetImageProperties.h
./Interfaces/IGetLoops.h
./Interfaces/IGetOptions.h
./Interfaces/IGetTriangleNormal.h
./Interfaces/IGetVertex.h
./Interfaces/IGetVertexNormal.h
./Interfaces/IGroup.h
./Interfaces/IGroupPrimitives.h
./Interfaces/IImageToGrayScale.h
./Interfaces/IInitNewDocument.h
./Interfaces/IIntersectListener.h
./Interfaces/IIntersectNotify.h
./Interfaces/IJobFinishedListener.h
./Interfaces/IJoystick.h
./Interfaces/IKeepAllConnected.h
./Interfaces/IKeyListener.h
./Interfaces/ILayer.h
./Interfaces/ILayerAddGUIQt.h
./Interfaces/ILayerExtents.h
./Interfaces/ILayerModifyGUIQt.h
./Interfaces/ILoadColorFile.h
./Interfaces/ILoadFileDialog.h
./Interfaces/IMaterials.h
./Interfaces/IMatrixManipulator.h
./Interfaces/IMatrixMultiply.h
./Interfaces/IMemoryPool.h
./Interfaces/IMenuAdd.h
./Interfaces/IModelsScene.h
./Interfaces/IModifiedObserver.h
./Interfaces/IModifiedSubject.h
./Interfaces/IMouseEventListener.h
./Interfaces/IMouseEventSubject.h
./Interfaces/IMpdNavigator.h
./Interfaces/IMpdWriter.h
./Interfaces/INavigationFunctor.h
./Interfaces/INewDocumentCreate.h
./Interfaces/INotify.h
./Interfaces/INotifyClose.h
./Interfaces/IOpenGLContext.h
./Interfaces/IOpenGLState.h
./Interfaces/IOpenSceneGraph.h
./Interfaces/IPasswordPrompt.h
./Interfaces/IPlanetCoordinates.h
./Interfaces/IPlugin.h
./Interfaces/IPluginInitialize.h
./Interfaces/IPolygonMode.h
./Interfaces/IProjectCoordinates.h
./Interfaces/IProjectionMatrix.h
./Interfaces/IQtDockWidgetMenu.h
./Interfaces/IRasterAlphas.h
./Interfaces/IRasterLayer.h
./Interfaces/IRead.h
./Interfaces/IReadImage.h
./Interfaces/IReadImageFile.h
./Interfaces/IReadProperties.h
./Interfaces/IReadTIFF.h
./Interfaces/IRedo.h
./Interfaces/IRedraw.h
./Interfaces/IRenderInfoOSG.h
./Interfaces/IRenderingPasses.h
./Interfaces/IRenderListener.h
./Interfaces/IRenderLoop.h
./Interfaces/IRenderNotify.h
./Interfaces/IRotationCenter.h
./Interfaces/ISaveFileDialog.h
./Interfaces/ISceneIntersect.h
./Interfaces/IScreenCapture.h
./Interfaces/ISerialize.h
./Interfaces/IShadeModel.h
./Interfaces/ISmoothTriangles.h
./Interfaces/ISnapShot.h
./Interfaces/ISpin.h
./Interfaces/IStreamListener.h
./Interfaces/IStringGridGet.h
./Interfaces/IStringGridSet.h
./Interfaces/ISubdivideTriangles.h
./Interfaces/ITextMatrix.h
./Interfaces/ITimeoutAnimate.h
./Interfaces/ITimeoutSpin.h
./Interfaces/ITimerNotify.h
./Interfaces/ITimerService.h
./Interfaces/ITimestepAnimation.h
./Interfaces/ITimeVaryingData.h
./Interfaces/IToolBarAdd.h
./Interfaces/ITrackball.h
./Interfaces/ITranslationSpeed.h
./Interfaces/ITreeNode.h
./Interfaces/ITriangle.h
./Interfaces/ITriangleSV.h
./Interfaces/ITriangulate.h
./Interfaces/ITriangulateGrid.h
./Interfaces/ITriangulateLoop.h
./Interfaces/IUndo.h
./Interfaces/IUnknown.h
./Interfaces/IUpdateCheck.h
./Interfaces/IUpdateEnable.h
./Interfaces/IUpdateListener.h
./Interfaces/IUpdateSceneVisitor.h
./Interfaces/IUpdateSubject.h
./Interfaces/IVaporIntrusionGUI.h
./Interfaces/IVertices.h
./Interfaces/IView.h
./Interfaces/IViewMatrix.h
./Interfaces/IViewMode.h
./Interfaces/IViewport.h
./Interfaces/IWandState.h
./Interfaces/IWorldInfo.h
./Interfaces/IWriteImageFile.h
./Interfaces/IWriteMovieFile.h
./Interfaces/Qt/IMainWindow.h
./Interfaces/Qt/IWorkspace.h
./IO/BinaryReader.h
./IO/BinaryWriter.h
./IO/Matrix44.h
./IO/Redirect.h
./IO/StreamSink.h
./IO/TextReader.h
./IO/TextWriter.h
./IO/Vector2.h
./IO/Vector3.h
./IO/Vector4.h
./Jobs/Job.h
./Jobs/Manager.h
./Math/Absolute.h
./Math/Angle.h
./Math/Barycentric.h
./Math/Constants.h
./Math/Equality.h
./Math/Finite.h
./Math/Functions.h
./Math/Interpolate.h
./Math/Matrix44.h
./Math/MinMax.h
./Math/NaN.h
./Math/Power.h
./Math/Quaternion.h
./Math/Transpose.h
./Math/Vector2.h
./Math/Vector3.h
./Math/Vector4.h
./Memory/Block.h
./Memory/Pool.h
./MPL/SameType.h
./MPL/StaticAssert.h
./MPL/TypeWrap.h
./Network/Curl.h
./Network/Names.h
./Network/WMS.h
./Pointers/ArrayDelete.h
./Pointers/ArrayDeletePolicy.h
./Pointers/BoostSerialize.h
./Pointers/DoNothingPolicy.h
./Pointers/Exceptions.h
./Pointers/Functions.h
./Pointers/Intrusive.h
./Pointers/NoReferencePolicy.h
./Pointers/NullPolicy.h
./Pointers/ObjectDelete.h
./Pointers/ObjectDeletePolicy.h
./Pointers/Pointers.h
./Pointers/QueryPointer.h
./Pointers/RefCountPolicy.h
./Pointers/SmartPointer.h
./Pointers/WeakPointer.h
./Policies/Update.h
./Predicates/CanWrite.h
./Predicates/CloseFloat.h
./Predicates/CompareCoordinate.h
./Predicates/DirExists.h
./Predicates/EndsWith.h
./Predicates/EqualVector.h
./Predicates/FileExists.h
./Predicates/IsOfType.h
./Predicates/LessVector.h
./Predicates/Tolerance.h
./Predicates/UnaryPair.h
./Preprocess/UniqueName.h
./Print/Matrix.h
./Print/Node.h
./Print/Vector.h
./Properties/Attribute.h
./Registry/Constants.h
./Registry/Convert.h
./Registry/Database.h
./Registry/Node.h
./Registry/Qt.h
./Registry/Visitor.h
./Resources/CancelButton.h
./Resources/EventQueue.h
./Resources/ProgressBar.h
./Resources/ReportErrors.h
./Resources/StatusBar.h
./Resources/TextWindow.h
./Scope/Caller.h
./Scope/CurrentDirectory.h
./Scope/GLBeginEnd.h
./Scope/GLPushPopAttrib.h
./Scope/GLPushPopMatrix.h
./Scope/PushPop.h
./Scope/RemoveFile.h
./Scope/Reset.h
./Scope/StreamState.h
./Scope/StxxlConfig.h
./Scope/Timer.h
./Shared/Preferences.h
./Signals/Actions.h
./Signals/Handler.h
./Strings/Case.h
./Strings/Convert.h
./Strings/Format.h
./Strings/Managed.h
./Strings/Qt.h
./Strings/Split.h
./Strings/Trim.h
./Strings/Unicode.h
./System/ClipBoard.h
./System/Clock.h
./System/DateTime.h
./System/Directory.h
./System/Environment.h
./System/Host.h
./System/LastError.h
./System/Memory.h
./System/Screen.h
./System/Sleep.h
./System/Socket.h
./Threads/Callback.h
./Threads/Guard.h
./Threads/Manager.h
./Threads/Map.h
./Threads/Mutex.h
./Threads/MutexTraits.h
./Threads/Named.h
./Threads/Pool.h
./Threads/Queue.h
./Threads/ReadWriteLocks.h
./Threads/ReadWriteMutex.h
./Threads/RecursiveMutex.h
./Threads/Safe.h
./Threads/Set.h
./Threads/SetThreadFactory.h
./Threads/Task.h
./Threads/Thread.h
./Threads/ThreadId.h
./Threads/Variable.h
./Trace/Print.h
./Trace/Scope.h
./Trace/Trace.h
./Types/BaseProxy.h
./Types/ObjectProxy.h
./Types/Types.h
./Units/Length.h
./User/Directory.h
)


#########################################################
#
#  List all the Sources.
#
#########################################################

SET (SOURCES
./Threads/Named.cpp
./Threads/Callback.cpp
./Threads/ReadWriteMutex.cpp
./Threads/RecursiveMutex.cpp
./Threads/Task.cpp
./Threads/Pool.cpp
./Threads/Manager.cpp
./Threads/Thread.cpp
./Threads/Mutex.cpp
./Console/Feedback.cpp
./Shared/Preferences.cpp
./Factory/ObjectFactory.cpp
./Scope/Timer.cpp
./Scope/CurrentDirectory.cpp
./Jobs/Manager.cpp
./Jobs/Job.cpp
./Predicates/CanWrite.cpp
./System/ClipBoard.cpp
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
./Registry/Database.cpp
./Registry/Node.cpp
./Registry/Visitor.cpp
./User/Directory.cpp
./ID/GUID.cpp
./Functions/GUID.cpp
./DLL/Listener.cpp
./DLL/Library.cpp
./DLL/LibraryPool.cpp
./DLL/Loader.cpp
./Trace/Scope.cpp
./Trace/Print.cpp
./Errors/Stack.cpp
./Errors/Error.cpp
./Errors/Assert.cpp
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
./Diagnostics/StackTrace.cpp
./Base/BaseObject.cpp
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
)

SET ( HEADERS ${HEADERS} ./System/Process.h )
SET ( SOURCES ${SOURCES} ./System/Process.cpp )
	
IF(NOT WIN32)
	SET ( HEADERS ${HEADERS} ./System/ProcessUnix.h )
	SET ( SOURCES ${SOURCES} ./System/ProcessUnix.cpp )
ENDIF(NOT WIN32)

SET(TARGET_NAME Usul)

# Create a Shared Library
ADD_LIBRARY( ${TARGET_NAME} SHARED ${HEADERS} ${SOURCES} )

# Add a library
CADKIT_ADD_LIBRARY ( ${TARGET_NAME} )

# Find needed system libraries.	
IF(WIN32)
     SET ( SYSTEM_LIBRARIES User32 rpcrt4 Ws2_32 Dbghelp )
ELSEIF(APPLE)
     FIND_LIBRARY(CARBON_LIBRARY Carbon)
     SET ( SYSTEM_LIBRARIES System ${CARBON_LIBRARY} )
ELSE(WIN32)
     SET ( SYSTEM_LIBRARIES dl uuid )
ENDIF(WIN32)

# Link the Library
TARGET_LINK_LIBRARIES( ${TARGET_NAME} ${SYSTEM_LIBRARIES} )

# Create the config file.
CONFIGURE_FILE ( ${CMAKE_CURRENT_SOURCE_DIR}/Config/Config.h.in.cmake ${CMAKE_CURRENT_SOURCE_DIR}/Config/Config.h )
