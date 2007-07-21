
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
#include "VRV/Devices/JoystickDevice.h"
#include "VRV/Devices/ButtonGroup.h"
#include "VRV/Devices/TrackerDevice.h"
#include "VRV/Functors/BaseFunctor.h"
#include "VRV/Interfaces/IModelAdd.h"
#include "VRV/Interfaces/IClippingDistance.h"
#include "VRV/Interfaces/IFrameInfo.h"
#include "VRV/Interfaces/IWorldInfo.h"
#include "VRV/Interfaces/INavigationScene.h"
#include "VRV/Interfaces/IModelsScene.h"
#include "VRV/Interfaces/IMatrixMultiply.h"

#include "Usul/Interfaces/GUI/IProgressBarFactory.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include "OsgTools/Render/Renderer.h"
#include "OsgTools/Render/SceneManager.h"
#include "OsgTools/Widgets/ProgressBarGroup.h"

#include "vrj/Draw/OGL/GlApp.h"
#include "vrj/Draw/OGL/GlContextData.h"
#include "plugins/ApplicationDataManager/UserData.h"

#include "osg/Referenced"
#include "osg/Matrix"
#include "osg/Timer"

#include <list>
#include <string>


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

namespace VRV {
namespace Core {


class VRV_EXPORT Application : public vrj::GlApp,
                               public VRV::Interfaces::IModelAdd,
                               public VRV::Interfaces::IClippingDistanceFloat,
                               public VRV::Interfaces::IFrameInfo,
                               public VRV::Interfaces::IWorldInfo,
                               public VRV::Interfaces::INavigationScene,
                               public VRV::Interfaces::IModelsScene,
                               public VRV::Interfaces::IMatrixMultiplyFloat,
                               public Usul::Interfaces::IProgressBarFactory
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

  /// Get/Set the framestamp.
  osg::FrameStamp*        getFrameStamp();
  const osg::FrameStamp*  getFrameStamp() const;

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

  /// Get/Set the viewport.
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

protected:

  /// VR Juggler methods.
  virtual void            contextInit();
  virtual void            preFrame();
  virtual void            latePreFrame();
  virtual void            contextPreDraw();
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

  /// Increase/Decrease speed.
  void                    _increaseTranslateSpeed ( double amount );
  void                    _decreaseTranslateSpeed ( double amount );

  /// VRV::Interfaces::IClippingDistanceFloat
  /// Get/set the clipping distances.
  virtual void            getClippingDistances ( float &nearDist, float &farDist ) const;
  virtual void            setClippingDistances ( float nearDist, float farDist );

  /// VRV::Interfaces::IModelAdd
  virtual void            addModel ( osg::Node *model, const std::string& filename );

  /// VRV::Interfaces::IFrameInfo
  /// Get the duration of the last frame in seconds.
  virtual double          frameTime() const;

  /// VRV::Interfaces::IWorldInfo
  /// Get the radius of the "world".
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

  /// No copying.
  Application ( const Application& );
  Application& operator = (const Application&);

private:
  // Typedefs.
  typedef osg::ref_ptr < osg::MatrixTransform >            MatTransPtr;
  typedef osg::ref_ptr < osg::Group >                      GroupPtr;
  typedef VRV::Functors::BaseFunctor::RefPtr               FunctorPtr;
  typedef VRV::Devices::ButtonGroup::ValidRefPtr           ButtonsPtr;
  typedef VRV::Devices::TrackerDevice::ValidRefPtr         TrackerPtr;
  typedef VRV::Devices::JoystickDevice::ValidRefPtr        JoystickPtr;

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

  unsigned int                           _refCount;
};

}
}


#endif // __VRV_CORE_APPLICATION_H__
