
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
#include "VRV/Core/SharedDouble.h"
#include "VRV/Core/SharedMatrix.h"
#include "VRV/Devices/JoystickDevice.h"
#include "VRV/Devices/ButtonGroup.h"
#include "VRV/Devices/TrackerDevice.h"
#include "VRV/Functors/BaseFunctor.h"
#include "VRV/Interfaces/IModelAdd.h"
#include "VRV/Interfaces/IClippingDistance.h"
#include "VRV/Interfaces/INavigationScene.h"
#include "VRV/Interfaces/IModelsScene.h"
#include "VRV/Animate/Path.h"

#include "Usul/Interfaces/GUI/IProgressBarFactory.h"
#include "Usul/Interfaces/IUpdateSubject.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Interfaces/ICommand.h"
#include "Usul/Interfaces/ICommandQueueAdd.h"
#include "Usul/Interfaces/IActiveDocumentListener.h"
#include "Usul/Interfaces/IButtonPressListener.h"
#include "Usul/Interfaces/IButtonReleaseListener.h"
#include "Usul/Interfaces/IFrameStamp.h"
#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Interfaces/IMatrixMultiply.h"
#include "Usul/Interfaces/IJoystick.h"
#include "Usul/Interfaces/IWandState.h"
#include "Usul/Interfaces/ITranslationSpeed.h"
#include "Usul/Interfaces/IFrameInfo.h"
#include "Usul/Interfaces/IWorldInfo.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/Queue.h"
#include "Usul/Functors/Interaction/Common/Sequence.h"
#include "Usul/Functors/Interaction/Navigate/Transform.h"
#include "Usul/Functors/Interaction/Input/AnalogInput.h"

#include "OsgTools/Render/Renderer.h"
#include "OsgTools/Render/SceneManager.h"
#include "OsgTools/Render/FrameDump.h"
#include "OsgTools/Widgets/ProgressBarGroup.h"

#include "MenuKit/OSG/Menu.h"

#include "vrj/Draw/OGL/GlApp.h"
#include "vrj/Draw/OGL/GlContextData.h"
#include "plugins/ApplicationDataManager/UserData.h"

#include "osg/Referenced"
#include "osg/Matrix"
#include "osg/Timer"
#include "osg/Camera"

#include "osgDB/DatabasePager"

#include <list>
#include <string>
#include <queue>

// Forward declarations.
namespace vrj
{
  class Kernel;
  class GlDrawManager;
};

namespace osg
{
  class FrameStamp;
  class MatrixTransform;
  class Node;
  class Light;
};

namespace VRV { namespace Functors { class Transform; } }

namespace VRV {
namespace Core {


class VRV_EXPORT Application : public vrj::GlApp,
                               public VRV::Interfaces::IModelAdd,
                               public VRV::Interfaces::IClippingDistanceFloat,
                               public Usul::Interfaces::IFrameInfo,
                               public Usul::Interfaces::IWorldInfo,
                               public VRV::Interfaces::INavigationScene,
                               public VRV::Interfaces::IModelsScene,
                               public Usul::Interfaces::IMatrixMultiplyFloat,
                               public Usul::Interfaces::IJoystickFloat,
                               public Usul::Interfaces::IWandStateFloat,
                               public Usul::Interfaces::ITranslationSpeed,
                               public Usul::Interfaces::IProgressBarFactory,
                               public Usul::Interfaces::IUpdateSubject,
                               public Usul::Interfaces::ICommandQueueAdd,
                               public Usul::Interfaces::IActiveDocumentListener,
                               public Usul::Interfaces::IButtonPressListener,
                               public Usul::Interfaces::IButtonReleaseListener,
                               public Usul::Interfaces::IFrameStamp,
                               public Usul::Interfaces::IViewMatrix
{
public:
  // Typedefs.
  typedef vrj::GlApp                           BaseClass;
  typedef OsgTools::Render::Renderer           Renderer;
  typedef Renderer::RefPtr                     RendererPtr;
  typedef std::vector < RendererPtr >          Renderers;
  typedef VRV::Core::SharedDouble              SharedDouble;
  typedef Usul::Threads::RecursiveMutex        Mutex;
  typedef Usul::Threads::Guard<Mutex>          Guard;
  typedef OsgTools::Widgets::ProgressBarGroup  ProgressBars;
  typedef Usul::Math::Matrix44f                Matrix44f;
  typedef std::vector < std::string >          Filenames;
  typedef VRV::Prefs::Settings                 Preferences;
  typedef VRV::Devices::ButtonGroup            Buttons;
  typedef VRV::Devices::TrackerDevice          Tracker;
  typedef VRV::Devices::JoystickDevice         Joystick;
  typedef VRV::Functors::BaseFunctor           Functor;
  typedef VRV::Functors::Transform             Transform;
  typedef MenuKit::OSG::Menu                   Menu;
  typedef USUL_VALID_REF_POINTER(Menu)         MenuPtr;

  typedef Usul::Functors::Interaction::Common::BaseFunctor   Navigator2;
  typedef Usul::Functors::Interaction::Input::AnalogInput    AnalogInput;
  typedef Usul::Functors::Interaction::Navigate::Transform   TransformFunctor;
  typedef Usul::Functors::Interaction::Common::Sequence      FavoriteFunctor;
  typedef std::map < std::string, AnalogInput::RefPtr >      AnalogInputs;
  typedef std::map < std::string, TransformFunctor::RefPtr > TransformFunctors;
  typedef std::map < std::string, FavoriteFunctor::RefPtr >  FavoriteFunctors;
  typedef FavoriteFunctors::const_iterator                   ConstFavoriteIterator;

  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Constructors.
  Application();

  // Destructor.
  virtual ~Application();

  virtual void            init();
  virtual void            viewAll ( osg::Node *mt, osg::Matrix::value_type zScale=2 );

  /// Add a light.
  void                    addLight ( osg::Light* light );

  /// Get/Set the background color.
  void                    setBackgroundColor( const osg::Vec4& bg );
  const osg::Vec4&        getBackgroundColor() const;

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

  void                    quit();
  void                    run();

  /// Get the models node.
  osg::MatrixTransform*              models();
  const osg::MatrixTransform*        models() const;

  /// Get the mutex.
  Mutex&                  mutex() const { return _mutex; }

  /// Get the viewport.
  osg::Viewport*          viewport()       { return _viewport.get(); }
  const osg::Viewport*    viewport() const { return _viewport.get(); }

  /// Export the next frame.
  void                    exportNextFrame ();

  /// Set the number of rendering passes
  void                    numRenderPasses ( unsigned int num );

  /// Get the Preferences.
  Preferences *           preferences ();
  const Preferences *     preferences () const;

  /// Get the buttons.
  Buttons *               buttons ();
  const Buttons *         buttons () const;

  /// Get the tracker.
  Tracker *               tracker ();
  const Tracker *         tracker () const;

  /// Get the joystick.
  Joystick *              joystick ();
  const Joystick *        joystick () const;

  /// Get/Set the analog trim.
  const Usul::Math::Vec2f&    analogTrim () const;
  void                        analogTrim ( float x, float y );

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
  void                    navigator ( Functor * );
  Functor *               navigator ();
  const Functor *         navigator () const;

  /// Get/Set the navigator.
  void                    navigator2 ( Navigator2 * );
  Navigator2 *            navigator2 ();
  const Navigator2 *      navigator2 () const;

  // Menu scene hiding functions
  bool                    menuSceneShowHide();
  void                    toggleMenuSceneShowHide( bool show );

  /// Append current camera.
  void                    appendCamera ();

  /// Start the animation.
  void                    startAnimation ();

  /// Clear animation.
  void                    clearAnimation ();

  /// Get/Set the number of animation steps.
  void                    animationSteps ( unsigned int steps );
  unsigned int            animationSteps ( ) const;

  /// Get/Set the menu.
  Menu *                  menu ();
  const Menu *            menu () const;
  void                    menu ( Menu *);

  /// Get the status bar.
  Menu *                  statusBar ();
  const Menu *            statusBar () const;

  /// Get/Set walk mode.
  void                          walkMode ();
  bool                          getWalkMode () const;

  /// Get/Set pole mode.
  void                          poleMode ();
  bool                          getPoleMode () const;

  /// Get/Set fly mode.
  void                          flyMode ();
  bool                          getFlyMode () const;

  // Get the begining of the favorites.
  ConstFavoriteIterator         favoritesBegin () const;

  // Get the end of the favorites.
  ConstFavoriteIterator         favoritesEnd () const;

  Navigator2*                   favorite ( const std::string& name );
protected:

  /// VR Juggler methods.
  virtual void            contextInit();
  virtual void            preFrame();
  virtual void            latePreFrame();
  virtual void            contextPreDraw();
  virtual void            contextPostDraw();
  virtual void            draw();
  virtual void            postFrame();
  virtual void            contextClose();

  // Draw functions.
  virtual void            _preDraw ( OsgTools::Render::Renderer *renderer );
  void                    _draw ( OsgTools::Render::Renderer *renderer );
  virtual void            _postDraw ( OsgTools::Render::Renderer *renderer );

  /// Set the viewport.
  virtual void            _setViewport ( osg::Viewport*, vrj::GlDrawManager* );

  void                    _construct();

  // Is this the head node?
  bool                    _isHeadNode() const;

  // Load VR Juggler config files.
  void                    _loadConfigFiles ( const std::vector < std::string > &configs );
  void                    _loadSimConfigs  ( const std::string& dir );
  void                    _loadSimConfigs();

  // Load the file(s).
  void                    _loadModelFiles  ( const Filenames& filename );

  // Set the near and far clipping planes based on the scene.
  void                    _setNearAndFarClippingPlanes();

  // Set/Get the navigation matrix.
  void                    _navigationMatrix ( const osg::Matrixd& m );
  const osg::Matrixd&     _navigationMatrix () const;

  // Get the scene root.
  osg::Group*             _sceneRoot();
  const osg::Group*       _sceneRoot() const;

  // Read the user's preferences.
  void                    _readUserPreferences();

  // Read the user's functor config file.
  void                    _readFunctorFile ();

  /// Increase/Decrease speed.
  void                    _increaseTranslateSpeed ( double amount );
  void                    _decreaseTranslateSpeed ( double amount );

  /// Update notify.
  virtual void            _updateNotify ();

  /// Process commands.
  void                    _processCommands ();

  /// Navigate.
  virtual void            _navigate ();

  /// Set the navigator as a pair.
  void                    _setNavigator ( Functor *first, Functor *second, unsigned int id );

  /// Get/Set the first navigator.
  void                    _firstNavigator ( Functor *first );
  Functor *               _firstNavigator ();
  const Functor *         _firstNavigator () const;

  /// Get/Set the second navigator.
  void                    _secondNavigator ( Functor *second );
  Functor *               _secondNavigator ();
  const Functor *         _secondNavigator () const;

  void                    _initStatusBar();

  void                    _updateStatusBar ( const std::string &text );

  // Parse the command-line arguments.
  void                    _parseCommandLine();

  /// Get/set the clipping distances (VRV::Interfaces::IClippingDistanceFloat).
  virtual void            getClippingDistances ( float &nearDist, float &farDist ) const;
  virtual void            setClippingDistances ( float nearDist, float farDist );

  /// VRV::Interfaces::IModelAdd
  virtual void            addModel ( osg::Node *model, const std::string& filename );

  /// Get the duration of the last frame in seconds (VRV::Interfaces::IFrameInfo).
  virtual double          frameTime() const;

  /// Get the radius of the "world" (VRV::Interfaces::IWorldInfo).
  virtual double          worldRadius() const;

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
  virtual void                  postMultiply ( const Matrix44f &M );

  // Pre-multiply the component's matrix by the given matrix.
  virtual void                  preMultiply ( const Matrix44f &M );

  /// Usul::Interfaces::IProgressBarFactory
  virtual Usul::Interfaces::IUnknown*   createProgressBar();

  //  VRV::Interfaces::IJoystickFloat
  // Get the joystick value in the range [-1,1].
  virtual float                 joystickHorizontal() const;
  virtual float                 joystickVertical()   const;

  // Get the wand's position (VRV::Interfaces::IWandStateFloat).
  virtual void                  wandPosition ( Usul::Math::Vec3f &p ) const;

  // Get the wand's rotation matrix (VRV::Interfaces::IWandStateFloat).
  virtual void                  wandRotation ( Matrix44f &R ) const;

  // Get the wand's matrix (VRV::Interfaces::IWandStateFloat).
  virtual void                  wandMatrix ( Matrix44f &M ) const;

  // Get/set the wand's offset (VRV::Interfaces::IWandStateFloat).
  virtual void                  wandOffset ( Usul::Math::Vec3f &v ) const;
  virtual void                  wandOffset ( const Usul::Math::Vec3f &v );

  /// VRV::Interfaces::ITranslationSpeed.
  virtual float                 translationSpeed () const;

  /// Add/Remove a update listener.
  virtual void                  addUpdateListener    ( Usul::Interfaces::IUnknown *caller = 0x0 );
  virtual void                  removeUpdateListener ( Usul::Interfaces::IUnknown *caller = 0x0 );

  /// Usul::Interfaces::ICommandQueueAdd
  virtual void                  addCommand ( Usul::Interfaces::ICommand* command );

  /// Usul::Interfaces::IActiveDocumentListener
  /// The active document has changed.
  virtual void                  activeDocumentChanged ( Usul::Interfaces::IUnknown *oldDoc, Usul::Interfaces::IUnknown *newDoc );

  /// Usul::Interfaces::IButtonPressListener
  /// Called when button is pressed.
  virtual void                  buttonPressNotify ( Usul::Interfaces::IUnknown * );

  /// Usul::Interfaces::IButtonReleaseListener
  /// Called when button is released.
  virtual void                  buttonReleaseNotify ( Usul::Interfaces::IUnknown * );

  /// Set the view matrix ( Usul::Interfaces::IViewMatrix ).
  /// Note: In this implementation, the navigation matrix is set.
  virtual void                  setViewMatrix ( const osg::Matrixf& );
  virtual void                  setViewMatrix ( const osg::Matrixd& );

  /// Get the view matrix ( Usul::Interfaces::IViewMatrix ).
  /// Note: In this implementation, the navigation matrix is set.
  virtual const osg::Matrixd&   getViewMatrix (  ) const;

  /// No copying.
  Application ( const Application& );
  Application& operator = (const Application&);

private:
  // Typedefs.
  typedef osg::ref_ptr < osg::MatrixTransform >            MatTransPtr;
  typedef osg::ref_ptr < osg::Group >                      GroupPtr;
  typedef VRV::Functors::BaseFunctor::RefPtr               FunctorPtr;
  typedef VRV::Devices::ButtonGroup                        ButtonGroup;
  typedef VRV::Devices::ButtonGroup::ValidRefPtr           ButtonsPtr;
  typedef VRV::Devices::TrackerDevice::ValidRefPtr         TrackerPtr;
  typedef VRV::Devices::JoystickDevice::ValidRefPtr        JoystickPtr;
  typedef Usul::Interfaces::IUpdateListener                UpdateListener;
  typedef std::vector < UpdateListener::RefPtr >           UpdateListeners;
  typedef Usul::Interfaces::ICommand                       Command;
  typedef Command::QueryPtr                                CommandPtr;
  typedef std::queue< CommandPtr >                         CommandQueue_;
  USUL_DECLARE_QUEUE_CONFIG ( QueueConfig, Mutex, Guard, CommandQueue_ );
  typedef Usul::Threads::Queue<QueueConfig>                CommandQueue;

  // Data members.
  mutable Mutex                          _mutex;
  GroupPtr                               _root;
  MatTransPtr                            _navBranch;
  MatTransPtr                            _models;
  osg::Timer                             _timer;
  osg::ref_ptr< osg::FrameStamp >        _framestamp;
  osg::ref_ptr< osg::Viewport >          _viewport;
  osg::Vec4                              _backgroundColor;
  bool                                   _dirty;
  osg::Timer_t                           _initialTime;
  osg::Timer_t                           _frameStart;
  double                                 _frameTime;
  cluster::UserData < SharedDouble >     _sharedFrameTime;
  cluster::UserData < SharedDouble >     _sharedReferenceTime;
  cluster::UserData < SharedMatrix >     _sharedMatrix;
  vrj::GlContextData< RendererPtr >      _renderer;
  Renderers                              _renderers;
  OsgTools::Render::SceneManager::RefPtr _sceneManager;
  ProgressBars::RefPtr                   _progressBars;
  osg::Vec2                              _clipDist;
  bool                                   _exportImage;
  Preferences::RefPtr                    _preferences;
  ButtonsPtr                             _buttons;
  TrackerPtr                             _tracker;
  JoystickPtr                            _joystick;
  Usul::Math::Vec2f                      _analogTrim;
  Usul::Math::Vec3f                      _wandOffset;
  osg::ref_ptr < osgDB::DatabasePager >  _databasePager;
  UpdateListeners                        _updateListeners;
  CommandQueue                           _commandQueue;
  OsgTools::Render::FrameDump            _frameDump;
  FunctorPtr                             _navigator;
  Navigator2::RefPtr                     _navigator2;
  unsigned int                           _refCount;
  bool                                   _menuSceneShowHide;
  VRV::Animate::Path::RefPtr             _path;
  MenuPtr                                _menu;
  MenuPtr                                _statusBar;
  std::string                            _functorFilename;
  AnalogInputs                           _analogInputs;
  TransformFunctors                      _transformFunctors;
  FavoriteFunctors                       _favoriteFunctors;
};

}
}


#endif // __VRV_CORE_APPLICATION_H__
