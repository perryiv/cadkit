
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
#include "VRV/Core/SharedDouble.h"
#include "VRV/Interfaces/IModelAdd.h"
#include "VRV/Interfaces/IClippingDistance.h"

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
                               public VRV::Interfaces::IClippingDistanceFloat
{
public:
  // Typedefs.
  typedef vrj::GlApp                           BaseClass;
  typedef OsgTools::Render::Renderer           Renderer;
  typedef Renderer::RefPtr                     RendererPtr;
  typedef VRV::Core::SharedDouble              SharedDouble;
  typedef Usul::Threads::RecursiveMutex        Mutex;
  typedef Usul::Threads::Guard<Mutex>          Guard;
  typedef OsgTools::Widgets::ProgressBarGroup  ProgressBars;

  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Constructors.
  Application();
  Application( vrj::Kernel* );

  // Destructor.
  virtual ~Application();

  virtual void            init();
  virtual void            viewAll ( osg::Node *mt, osg::Matrix::value_type zScale=2 );

  /// Add a light.
  void                    addLight ( osg::Light* light );

  /// Get/Set the background color.
  void                    setBackgroundColor( const osg::Vec4& bg ) { _backgroundColor = bg; _dirty = false; }
  const osg::Vec4&        getBackgroundColor() const { return _backgroundColor; }

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

protected:

  /// VR Juggler methods.
  virtual void            contextInit();
  virtual void            contextPreDraw();
  virtual void            preFrame();
  virtual void            latePreFrame();
  virtual void            postFrame();
  virtual void            draw();
  virtual void            contextClose();

  /// Set the viewport.
  virtual void            _setViewport ( osg::Viewport*, vrj::GlDrawManager* );

  /// Set the renderer with proper data.
  virtual void            _initRenderer ( Renderer* );

  void                    _construct();
  Renderer*               _getContextSpecificRenderer();

  // Get the duration of the last frame in seconds.
  double                  _getFrameTime() const;

  // Load VR Juggler config files.
  void                    _loadConfigFiles ( const std::vector < std::string > &configs );
  void                    _loadSimConfigs  ( const std::string& dir );
  void                    _loadSimConfigs();

  // Load the file(s).
  void                    _loadModelFile   ( const std::string &filename );

  // Set the near and far clipping planes based on the scene.
  void                    _setNearAndFarClippingPlanes();

  /// VRV::Interfaces::IClippingDistanceFloat
  /// Get/set the clipping distances.
  virtual void            getClippingDistances ( float &nearDist, float &farDist ) const;
  virtual void            setClippingDistances ( float nearDist, float farDist );

  /// VRV::Interfaces::IModelAdd
  virtual void            addModel ( osg::Node *model, const std::string& filename );

  /// No copying.
  Application ( const Application& );
  Application& operator = (const Application&);

private:
  // Typedefs.
  typedef osg::ref_ptr<osg::MatrixTransform>            MatTransPtr;

  // Data members.
  mutable Mutex                      _mutex;

  MatTransPtr                        _models;

  osg::Timer                         _timer;
  osg::ref_ptr< osg::FrameStamp >    _framestamp;
  osg::ref_ptr< osg::Viewport >      _viewport;

  osg::Vec4                          _backgroundColor;

  bool                               _dirty;
  
  osg::Timer_t                       _initialTime;
  osg::Timer_t                       _frameStart;
  double                             _frameTime;
  cluster::UserData < SharedDouble > _sharedFrameTime;

  vrj::GlContextData< RendererPtr >  _renderer;
  OsgTools::Render::SceneManager::RefPtr _sceneManager;
  ProgressBars::RefPtr                   _progressBars;

  osg::Vec2                              _clipDist;

  unsigned int _refCount;
};

}
}


#endif // __VRV_CORE_APPLICATION_H__
