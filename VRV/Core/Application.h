
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_CORE_APPLICATION_H__
#define __VRV_CORE_APPLICATION_H__

#include "VRV/Export.h"
#include "VRV/Prefs/Settings.h"
#include "VRV/Core/BaseApplication.h"
#include "VRV/Core/SharedData.h"
#include "VRV/Interfaces/IModelAdd.h"
#include "VRV/Interfaces/INavigationScene.h"
#include "VRV/Interfaces/IAuxiliaryScene.h"
#include "VRV/Functors/Intersect.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Commands/GenericCommand.h"
#include "Usul/Commands/GenericCheckCommand.h"
#include "Usul/Interfaces/GUI/IProgressBarFactory.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Interfaces/IActiveDocumentListener.h"
#include "Usul/Interfaces/IBackgroundColor.h"
#include "Usul/Interfaces/IButtonPressListener.h"
#include "Usul/Interfaces/IButtonReleaseListener.h"
#include "Usul/Interfaces/IButtonPressSubject.h"
#include "Usul/Interfaces/IButtonReleaseSubject.h"
#include "Usul/Interfaces/IClippingDistance.h"
#include "Usul/Interfaces/ICamera.h"
#include "Usul/Interfaces/ICommand.h"
#include "Usul/Interfaces/ICommandQueueAdd.h"
#include "Usul/Interfaces/IFrameInfo.h"
#include "Usul/Interfaces/IFrameStamp.h"
#include "Usul/Interfaces/IGroup.h"
#include "Usul/Interfaces/IIntersectNotify.h"
#include "Usul/Interfaces/IIntersectListener.h"
#include "Usul/Interfaces/IJoystick.h"
#include "Usul/Interfaces/IMatrixMultiply.h"
#include "Usul/Interfaces/IModelsScene.h"
#include "Usul/Interfaces/INavigationFunctor.h"
#include "Usul/Interfaces/ITranslationSpeed.h"
#include "Usul/Interfaces/IPolygonMode.h"
#include "Usul/Interfaces/IRenderingPasses.h"
#include "Usul/Interfaces/IRenderListener.h"
#include "Usul/Interfaces/IRenderNotify.h"
#include "Usul/Interfaces/IRotationCenter.h"
#include "Usul/Interfaces/ISaveFileDialog.h"
#include "Usul/Interfaces/IShadeModel.h"
#include "Usul/Interfaces/ITextMatrix.h"
#include "Usul/Interfaces/IUpdateSubject.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Interfaces/IViewport.h"
#include "Usul/Interfaces/IView.h"
#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Interfaces/IWandState.h"
#include "Usul/Interfaces/IWorldInfo.h"
#include "Usul/Threads/Queue.h"
#include "Usul/Functors/Interaction/Common/Sequence.h"
#include "Usul/Functors/Interaction/Navigate/Transform.h"
#include "Usul/Functors/Interaction/Input/AnalogInput.h"

#include "OsgTools/Render/Renderer.h"
#include "OsgTools/Render/SceneManager.h"
#include "OsgTools/Render/FrameDump.h"
#include "OsgTools/Widgets/ProgressBarGroup.h"

#include "MenuKit/OSG/Menu.h"
#include "MenuKit/CommandVisitor.h"

#include "vrj/Draw/OGL/GlApp.h"
#include "vrj/Draw/OGL/GlContextData.h"
#include "plugins/ApplicationDataManager/UserData.h"

#include "osg/Referenced"
#include "osg/LightSource"
#include "osg/Matrix"
#include "osg/Timer"

#include "osgDB/DatabasePager"

#include <list>
#include <string>
#include <queue>


namespace osg
{
  class FrameStamp;
  class MatrixTransform;
  class Node;
};

namespace MenuKit { class Button; }
namespace OsgTools { namespace Utilities { class DeleteHandler; } }

namespace VRV {
namespace Core {


class VRV_EXPORT Application : public VRV::Core::BaseApplication,
                               public VRV::Interfaces::IModelAdd,
                               public VRV::Interfaces::INavigationScene,
                               public Usul::Interfaces::IModelsScene,
                               public VRV::Interfaces::IAuxiliaryScene,
                               public Usul::Interfaces::IClippingDistance,
                               public Usul::Interfaces::IFrameInfo,
                               public Usul::Interfaces::IWorldInfo,
                               public Usul::Interfaces::IMatrixMultiplyDouble,
                               public Usul::Interfaces::IWandStateDouble,
                               public Usul::Interfaces::ITranslationSpeed,
                               public Usul::Interfaces::IProgressBarFactory,
                               public Usul::Interfaces::IUpdateSubject,
                               public Usul::Interfaces::ICommandQueueAdd,
                               public Usul::Interfaces::IActiveDocumentListener,
                               public Usul::Interfaces::IButtonPressListener,
                               public Usul::Interfaces::IButtonReleaseListener,
                               public Usul::Interfaces::IFrameStamp,
                               public Usul::Interfaces::IViewMatrix,
                               public Usul::Interfaces::IStatusBar,
                               public Usul::Interfaces::ICamera,
                               public Usul::Interfaces::IPolygonMode,
                               public Usul::Interfaces::IShadeModel,
                               public Usul::Interfaces::INavigationFunctor,
                               public Usul::Interfaces::IBackgroundColor,
                               public Usul::Interfaces::IRenderingPasses,
                               public Usul::Interfaces::IViewport,
                               public Usul::Interfaces::IView,
                               public Usul::Interfaces::ITextMatrix,
                               public Usul::Interfaces::ISaveFileDialog,
                               public Usul::Interfaces::IIntersectNotify,
                               public Usul::Interfaces::IButtonPressSubject,
                               public Usul::Interfaces::IButtonReleaseSubject,
                               public Usul::Interfaces::IGroup,
                               public Usul::Interfaces::IRotationCenterFloat,
                               public Usul::Interfaces::IRenderNotify
{
public:
  // Typedefs.
  typedef VRV::Core::BaseApplication           BaseClass;
  typedef OsgTools::Render::Renderer           Renderer;
  typedef Renderer::RefPtr                     RendererPtr;
  typedef std::vector < RendererPtr >          Renderers;
  typedef OsgTools::Widgets::ProgressBarGroup  ProgressBars;
  typedef Usul::Math::Matrix44d                Matrix;
  typedef std::vector < std::string >          Filenames;
  typedef VRV::Prefs::Settings                 Preferences;
  typedef VRV::Devices::ButtonGroup            Buttons;
  typedef VRV::Devices::TrackerDevice          Tracker;
  typedef VRV::Devices::JoystickDevice         Joystick;
  typedef MenuKit::OSG::Menu                   Menu;
  typedef USUL_REF_POINTER(Menu)               MenuPtr;
  typedef Usul::Interfaces::ICamera            ICamera;
  typedef ICamera::CameraOption                CameraOption;
  typedef Usul::Interfaces::IPolygonMode       IPolygonMode;
  typedef IPolygonMode::Mode                   PolygonMode;
  typedef Usul::Interfaces::IShadeModel        IShadeModel;
  typedef IShadeModel::Mode                    ShadeModel;
  typedef Usul::Interfaces::IRotationCenterFloat  IRotationCenter;
  typedef IRotationCenter::Vector                 Vector;
  typedef Usul::Interfaces::IUnknown              IUnknown;

  typedef Usul::Functors::Interaction::Common::BaseFunctor   Navigator;
  typedef Usul::Functors::Interaction::Input::AnalogInput    AnalogInput;
  typedef Usul::Functors::Interaction::Navigate::Transform   TransformFunctor;
  typedef Usul::Functors::Interaction::Common::Sequence      FavoriteFunctor;
  typedef std::map < std::string, AnalogInput::RefPtr >      AnalogInputs;
  typedef std::map < std::string, TransformFunctor::RefPtr > TransformFunctors;
  typedef std::map < std::string, FavoriteFunctor::RefPtr >  FavoriteFunctors;
  typedef FavoriteFunctors::iterator                         FavoriteIterator;

  typedef void (Application::*VoidFunction) ();
  typedef void (Application::*BoolFunction) ( bool );
  typedef bool (Application::*CheckFunction) () const;
  typedef Usul::Adaptors::MemberFunction < void, Application*, VoidFunction >  ExecuteFunctor;
  typedef Usul::Adaptors::MemberFunction < bool, Application*, CheckFunction > CheckFunctor;
  typedef Usul::Adaptors::MemberFunction < void, Application*, BoolFunction >  BoolFunctor;
  typedef Usul::Commands::GenericCommand < ExecuteFunctor >                    BasicCommand;
  typedef Usul::Commands::GenericCheckCommand < BoolFunctor, CheckFunctor >    CheckCommand;

  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Constructors.
  Application();

  // Destructor.
  virtual ~Application();

  /// Find an analog.
  JoystickPtr                   analogFind ( const std::string& key );

  /// Calibrate all the analogs.
  void                          analogsCalibrate();

  // Get the buttons.
  ButtonGroup*                  buttons();

  // Clean up.  Call before the Application is destroyed.
  void                          cleanup();

  virtual void                  viewAll ( osg::Node *mt, osg::Matrix::value_type zScale=2 );

  /// Get/Set the background color.
  virtual void                backgroundColor ( const Usul::Math::Vec4f& color );
  virtual void                backgroundColor ( const osg::Vec4f& color, unsigned int corner );
  virtual Usul::Math::Vec4f   backgroundColor () const;

  /// Get/Set the background corner.
  void                    setBackgroundCorners ( unsigned int corner );
  unsigned int            getBackgroundCorners() const;
  bool                    isBackgroundCorners( unsigned int corner ) const;

  /// Get/Set the framestamp ( Usul::Interfaces::IFrameStamp ).
  osg::FrameStamp*        frameStamp();
  const osg::FrameStamp*  frameStamp() const;

  /// Get/Set the scene data.
  osg::Node*              getSceneData();
  const osg::Node*        getSceneData() const;
  void                    setSceneData( osg::Node* );

  double                  getTimeSinceStart();

  void                    normalize ( bool state );
  bool                    normalize() const;

  /// Get the models node.
  osg::MatrixTransform*              models();
  const osg::MatrixTransform*        models() const;

  /// Get the viewport.
  osg::Viewport*          viewport()       { return _viewport.get(); }
  const osg::Viewport*    viewport() const { return _viewport.get(); }

  /// Export the next frame.
  void                    exportNextFrame ();

  /// Get/Set the number of rendering passes
  virtual void            renderingPasses ( unsigned int number );
  virtual unsigned int    renderingPasses () const;

  /// Get the Preferences.
  Preferences *           preferences ();
  const Preferences *     preferences () const;

  // Print the usage string.
  static void                   usage ( const std::string &exe, std::ostream &out );

  /// Add/Remove group from projection node
  osg::Group*             projectionGroupGet    ( const std::string& );
  void                    projectionGroupRemove ( const std::string& );
  bool                    projectionGroupHas    ( const std::string& ) const;

  /// Get/Set the frame dump flag.
  void                    frameDump ( bool b );
  bool                    frameDump () const;

  /// Get/Set the navigator.
  void                    navigator ( Navigator * );
  Navigator *             navigator ();
  const Navigator *       navigator () const;

  // Menu scene hiding functions
  bool                    menuSceneShowHide () const;
  void                    menuSceneShowHide ( bool show );

  /// Get/Set the menu.
  Menu *                  menu();
  const Menu *            menu() const;
  void                    menu ( Menu *);

  /// Get the status bar.
  Menu *                  statusBar();
  const Menu *            statusBar() const;

  // Get the begining of the favorites.
  FavoriteIterator        favoritesBegin();

  // Get the end of the favorites.
  FavoriteIterator        favoritesEnd();

  /// Toggle time based rendering.
  void                    timeBased ( bool b );
  bool                    timeBased () const;

  // Export functions.
  void                    exportWorld ();
  void                    exportWorldBinary ();
  void                    exportScene ();
  void                    exportSceneBinary ();
  void                    exportDocument ( const std::string& ext );

  // View functions.
  void                    viewWorld ();
  void                    viewScene ();

  /// Get/Set near far mode.
  void                    setComputeNearFar ( bool b );
  bool                    getComputeNearFar () const;
  bool                    isComputeNearFar ( osg::CullSettings::ComputeNearFarMode ) const;

  /// Reread the preference files and reinitialiaze.
  void                    reinitialize();

  /// Set/Get the allow intersections state
  void                    allowIntersections ( bool b );
  bool                    isAllowIntersections() const;

  /// Set/Get the seek mode.
  void                    seekMode  ( bool b );
  bool                    isSeekMode() const;
  
  /// VRV::Interfaces::ITranslationSpeed.
  virtual float                 translationSpeed () const;
  void                          translationSpeed ( float speed );

    /// Save/Restore state.
  void                          saveState() const;
  void                          restoreState();

  /// Get/Set show memory state.
  void                          setShowMemory( bool b );
  bool                          getShowMemory() const;

  /// Set/get the show back face culling state.
  void                          showBackFaces ( bool b );
  bool                          isBackFacesShowing() const;

	/// Set/get body centered rotation flag.
	void                          bodyCenteredRotation ( bool b );
	bool                          isBodyCenteredRotation() const;

protected:

  /// VR Juggler methods.
  virtual void                  _init();
  virtual void                  _preFrame();
  virtual void                  _latePreFrame();
  virtual void                  _postFrame();

  // Draw functions.
  virtual void                  _preDraw  ( OsgTools::Render::Renderer *renderer );
  void                          _draw     ( OsgTools::Render::Renderer *renderer );
  virtual void                  _postDraw ( OsgTools::Render::Renderer *renderer );

  /// Set the viewport.
  virtual void                  _setViewport ( osg::Viewport*, vrj::GlDrawManager* );

  void                          _construct();

  // Is this the head node?
  bool                          _isHeadNode() const;

  // Load the file(s).
  virtual void                  _loadModelFiles  ( const Filenames& filenames );

  // Set the near and far clipping planes based on the scene.
  void                          _setNearAndFarClippingPlanes();

  // Get the scene root.
  osg::Group*                   _sceneRoot();
  const osg::Group*             _sceneRoot() const;

  // Read the user's preferences.
  void                          _readUserPreferences();

  /// Update notify.
  void                          _updateNotify();
  virtual bool                  _allowNotify() const;

  /// Process commands.
  void                          _processCommands ();

  /// Navigate.
  virtual void                  _navigate ();

  /// Update the status bar text.
  void                          _updateStatusBar ( const std::string &text );

  // Parse the command-line arguments.
  void                          _parseCommandLine();

  // Read the user's functor config file.
  void                          _readFunctorFile();

  // Read the user's devices config file.
  void                          _readDevicesFile();

  // Set the current "camera" position as "home".
  void                          _setHome();

  // Initialize.
  void                          _initStatusBar();
  void                          _initLight();
  void                          _initMenu();

  // Initialize the menus.
  void                          _initFileMenu     ( MenuKit::Menu* menu );
  void                          _initEditMenu     ( MenuKit::Menu* menu );
  void                          _initViewMenu     ( MenuKit::Menu* menu );
  void                          _initNavigateMenu ( MenuKit::Menu* menu );
  void                          _initOptionsMenu  ( MenuKit::Menu* menu );

  /// Translation speed controls.
  void                          _increaseSpeed ();
  void                          _decreaseSpeed ();
  void                          _increaseSpeedTen ();
  void                          _decreaseSpeedTen ();

  // Handle the events, if any.
  bool                          _handleMenuEvent ( unsigned long id );
  bool                          _handleIntersectionEvent ( unsigned long id );
  bool                          _handleNavigationEvent ( unsigned long id );
  bool                          _handleSeekEvent ( unsigned long id );

  // Intersect.
  void                          _intersect();

  // Generate a string from the integer.
  std::string                   _counter ( unsigned int num ) const;

  // Write the scene to file.
  void                          _writeScene ( const std::string &filename, const osg::Node *node ) const;

  /// Get section for current document.
  std::string                   _documentSection () const;

  /// Caputure the pixels.
  void                          _capturePixels ( const std::string& filename );

  /// Get the next name for file to save.
  std::string                   _filename ( const std::string& base, const std::string& ext );

  /// Set the allow update state.
  void                          _setAllowUpdate ( bool );
  bool                          _isUpdateOn () const;

  // Set/Get the navigation matrix.
  void                          _navigationMatrix ( const osg::Matrixd& m );
  const osg::Matrixd&           _navigationMatrix() const;

  /// Get the screen shot directory.
  std::string                   _screenShotDirectory() const;

  // Notify of rendering.
  void                          _preRenderNotify ( Renderer* renderer );
  void                          _postRenderNotify ( Renderer* renderer );

  // Animate between two matrices.
  void                          _animate( const osg::Matrix& m1, const osg::Matrix& m2 );

  /// Get/Set assign button command.
  void                          _assignNextMenuSelection ( unsigned long button );
  bool                          _isAssignNextMenuSelection ( );

  /// Clear our assigned buttons.
  void                          _clearAssignedButtonCommands();

  /// Get/set the clipping distances (VRV::Interfaces::IClippingDistanceFloat).
  virtual void                  getClippingDistances ( double &nearDist, double &farDist ) const;
  virtual void                  setClippingDistances ( double nearDist, double farDist );

  /// VRV::Interfaces::IModelAdd
  virtual void                  addModel ( osg::Node *model, const std::string& filename );

  /// Get the duration of the last frame in seconds (VRV::Interfaces::IFrameInfo).
  virtual double                frameTime() const;

  /// Get the radius of the "world" (Usul::Interfaces::IWorldInfo).
  virtual double                worldRadius() const;

  ///  VRV::Interfaces::INavigationScene
  /// Get the navigation scene.
  virtual const osg::Group *    navigationScene() const;
  virtual osg::Group *          navigationScene();

  /// VRV::Interfaces::ModelsScene
  /// Get the models scene.
  virtual const osg::Group *    modelsScene() const;
  virtual osg::Group *          modelsScene();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  VRV::Interfaces::IMatrixMultiplyFloat
  //
  /////////////////////////////////////////////////////////////////////////////

  // Post-multiply the component's matrix by the given matrix.
  virtual void                  postMultiply ( const Matrix &M );

  // Pre-multiply the component's matrix by the given matrix.
  virtual void                  preMultiply ( const Matrix &M );

  /// Usul::Interfaces::IProgressBarFactory
  virtual IUnknown::RefPtr      createProgressBar ( bool waitIfNotGuiThread = false );

  // Get the wand's position (VRV::Interfaces::IWandStateFloat).
  virtual void                  wandPosition ( Usul::Math::Vec3d &p ) const;

  // Get the wand's rotation matrix (VRV::Interfaces::IWandStateFloat).
  virtual void                  wandRotation ( Matrix &R ) const;

  // Get the wand's matrix (VRV::Interfaces::IWandStateFloat).
  virtual void                  wandMatrix ( Matrix &M ) const;

  // Get/set the wand's offset (VRV::Interfaces::IWandStateFloat).
  virtual void                  wandOffset ( Usul::Math::Vec3d &v ) const;
  virtual void                  wandOffset ( const Usul::Math::Vec3d &v );

  /// Usul::Interfaces::ICommandQueueAdd
  virtual void                  addCommand ( Usul::Interfaces::ICommand* command );

  /// Usul::Interfaces::IActiveDocumentListener
  /// The active document has changed.
  virtual void                  activeDocumentChanged ( Usul::Interfaces::IUnknown *oldDoc, Usul::Interfaces::IUnknown *newDoc );

  /// Usul::Interfaces::IButtonPressListener
  /// Called when button is pressed.
  virtual bool                  buttonPressNotify ( Usul::Interfaces::IUnknown * );

  /// Usul::Interfaces::IButtonReleaseListener
  /// Called when button is released.
  virtual bool                  buttonReleaseNotify ( Usul::Interfaces::IUnknown * );

  /// Set the view matrix ( Usul::Interfaces::IViewMatrix ).
  /// Note: In this implementation, the navigation matrix is set.
  virtual void                  setViewMatrix ( const osg::Matrixf& );
  virtual void                  setViewMatrix ( const osg::Matrixd& );

  /// Get the view matrix ( Usul::Interfaces::IViewMatrix ).
  /// Note: In this implementation, the navigation matrix is set.
  virtual osg::Matrixd          getViewMatrix() const;

  // Set the status bar text (IStatusBar).
  virtual void                  setStatusBarText ( const std::string &text, bool force );

  // Set the camera (ICamera).
  virtual void                  camera ( CameraOption option );

  /// Set/get the polygon mode state (IPolygonMode).
  virtual void                  polygonMode ( PolygonMode mode );
  virtual PolygonMode           polygonMode() const;

  /// Set/get the shade model (IShadeModel).
  virtual void                  shadeModel ( ShadeModel mode );
  virtual ShadeModel            shadeModel() const;

  /// Get viewport parameters (IViewport).
  virtual double                x() const;
  virtual double                y() const;
  virtual double                height() const;
  virtual double                width() const;

  // Get the document (IView).
  virtual Usul::Interfaces::IDocument  * document();

  /// Get text at the (x,y) on the screen (ITextMatrix).
  virtual osgText::Text*        getText    ( unsigned int x, unsigned int y );

  /// Set text value (ITextMatrix).
  virtual void                  setText    ( unsigned int x, unsigned int y, const std::string& text, const osg::Vec4f& color, const osg::Vec4f& backDropColor = osg::Vec4f ( 0, 0, 0, 0 ) );

  /// Remove text (ITextMatrix)
  virtual void                  removeText ( unsigned int x, unsigned int y );

  // Get the name of the file to save to (ISaveFileDialog)
  virtual FileResult            getSaveFileName  ( const std::string &title = "Save", const Filters &filters = Filters() );

  // Get the auxiliary scene (CV::Interfaces::IAuxiliaryScene).
  virtual const osg::Group *    auxiliaryScene() const;
  virtual osg::Group *          auxiliaryScene();

  // Add the listener.
  virtual void                  addIntersectListener ( Usul::Interfaces::IUnknown *listener );
  virtual void                  addButtonPressListener ( Usul::Interfaces::IUnknown * );
  virtual void                  addButtonReleaseListener ( Usul::Interfaces::IUnknown * );
  virtual void                  addRenderListener ( IUnknown *listener );
  virtual void                  addUpdateListener ( Usul::Interfaces::IUnknown *caller = 0x0 );

  // Remove all listeners.
  virtual void                  clearIntersectListeners();
  virtual void                  clearButtonPressListeners();
  virtual void                  clearButtonReleaseListeners();
  virtual void                  clearRenderListeners();
    
  // Remove the listener.
  virtual void                  removeIntersectListener ( Usul::Interfaces::IUnknown *caller );
  virtual void                  removeButtonPressListener ( Usul::Interfaces::IUnknown * );
  virtual void                  removeButtonReleaseListener ( Usul::Interfaces::IUnknown * );
  virtual void                  removeRenderListener ( IUnknown *caller );
  virtual void                  removeUpdateListener ( Usul::Interfaces::IUnknown *caller = 0x0 );

  // IGroup.
  virtual osg::Group*           getGroup    ( const std::string& );
  virtual void                  removeGroup ( const std::string& );
  virtual bool                  hasGroup    ( const std::string& );

  // Get/set the center of rotation (IRotationCenter).
  Vector                        rotationCenter() const;
  void                          rotationCenter ( const Vector & );

  /// No copying.
  Application ( const Application& );
  Application& operator = (const Application&);

  // Joystick callbacks.
  struct JoystickCB : public VRV::Devices::Callback
  {
    JoystickCB ( Application *app ) : _app ( app ){}
    virtual void operator () ( VRV::Devices::Message m, Usul::Base::Referenced *caller );
  private:
    Application *_app;
  };

private:
  // Don't allow derived classes to implement these VR Juggler functions.
  // Implement the _function instead.  
  // This is to ensure that the functions are wrapped in a try/catch.
  virtual void            contextInit();
  virtual void            contextPreDraw();
  virtual void            contextPostDraw();
  virtual void            draw();
  virtual void            contextClose();

  // Typedefs.
  typedef osg::ref_ptr <osg::Group>                        GroupPtr;
  typedef osg::ref_ptr <osg::MatrixTransform>              MatTransPtr;
  typedef Usul::Interfaces::IUpdateListener                UpdateListener;
  typedef std::vector < UpdateListener::RefPtr >           UpdateListeners;
  typedef Usul::Interfaces::ICommand                       Command;
  typedef Command::QueryPtr                                CommandPtr;
  typedef std::queue< CommandPtr >                         CommandQueue_;
  USUL_DECLARE_QUEUE_CONFIG ( QueueConfig, Mutex, Guard, CommandQueue_ );
  typedef Usul::Threads::Queue<QueueConfig>                CommandQueue;
  typedef std::map<std::string, Usul::Math::Vec4f >        ColorMap;
  typedef VRV::Core::SharedData<double>                    SharedDouble;
  typedef VRV::Core::SharedData<std::string>               SharedString;
  typedef VRV::Core::SharedData<osg::Matrixd>              SharedMatrix;
  typedef Usul::Functors::Interaction::Common::BaseFunctor BaseFunctor;
  typedef BaseFunctor::RefPtr                              FunctorPtr;
  typedef Usul::Interfaces::IIntersectListener             IIntersectListener;
  typedef std::vector<IIntersectListener::RefPtr>          IntersectListeners;
  typedef Usul::Interfaces::IRenderListener                IRenderListener;
  typedef std::vector<IRenderListener::RefPtr>             RenderListeners;
  typedef std::map<unsigned long, CommandPtr>              ButtonMap;
  typedef std::map<unsigned int, std::string>              ButtonCommandsMap;
  typedef OsgTools::Utilities::DeleteHandler               DeleteHandler;

  enum Flags
  {
    SEEK_MODE           = 0x00000001,
    ASSIGN_NEXT_COMMAND = 0x00000002,
    SHOW_MEMORY         = 0x00000004,
  };

  // Data members.
  GroupPtr                               _root;
  MatTransPtr                            _navBranch;
  MatTransPtr                            _models;
  osg::Timer                             _timer;
  osg::ref_ptr< osg::FrameStamp >        _framestamp;
  osg::ref_ptr< osg::Viewport >          _viewport;
  Usul::Math::Vec4f                      _backgroundColor;
  bool                                   _dirty;
  osg::Timer_t                           _initialTime;
  osg::Timer_t                           _frameStart;
  double                                 _frameTime;
  cluster::UserData < SharedDouble >     _sharedFrameTime;
  cluster::UserData < SharedDouble >     _sharedReferenceTime;
  cluster::UserData < SharedMatrix >     _sharedMatrix;
  mutable cluster::UserData < SharedString >     _sharedScreenShotDirectory;
  vrj::GlContextData< RendererPtr >      _renderer;
  Renderers                              _renderers;
  OsgTools::Render::SceneManager::RefPtr _sceneManager;
  ProgressBars::RefPtr                   _progressBars;
  osg::Vec2d                             _clipDist;
  bool                                   _exportImage;
  Preferences::RefPtr                    _preferences;
  Usul::Math::Vec3d                      _wandOffset;
  osg::ref_ptr < osgDB::DatabasePager >  _databasePager;
  CommandQueue                           _commandQueue;
  OsgTools::Render::FrameDump            _frameDump;
  Navigator::RefPtr                      _navigator;
  unsigned int                           _refCount;
  bool                                   _menuSceneShowHide;
  MenuPtr                                _menu;
  MenuPtr                                _statusBar;
  std::string                            _preferencesFilename;
  std::string                            _functorFilename;
  std::string                            _deviceFilename;
  ButtonsPtr                             _buttons;
  TrackerPtr                             _tracker;
  Analogs                                _analogs;
  AnalogInputs                           _analogInputs;
  TransformFunctors                      _transformFunctors;
  FavoriteFunctors                       _favoriteFunctors;
  float                                  _translationSpeed;
  osg::Matrixd                           _home;
  bool                                   _timeBased;
  ColorMap                               _colorMap;
  unsigned int                           _count;
  bool                                   _allowUpdate;
  VRV::Functors::Intersect::RefPtr       _intersector;
  MatTransPtr                            _auxiliary;
  bool                                   _allowIntersections;
  DeleteHandler *                        _deleteHandler;
  Vector                                 _rotCenter;
  unsigned int                           _flags;
  UpdateListeners                        _updateListeners;
  RenderListeners                        _renderListeners;
  IntersectListeners                     _intersectListeners;
  ButtonMap                              _buttonMap;
  ButtonMap::key_type                    _buttonToAssign;
  unsigned int                           _selectButtonID;
  unsigned int                           _menuButtonID;
  std::string                            _menuNavigationAnalogID;
	bool                                   _bodyCenteredRotation;
  ButtonCommandsMap                      _buttonCommandsMap;
  osg::ref_ptr<osg::LightSource>         _lightSource;

};

}
}


#endif // __VRV_CORE_APPLICATION_H__
