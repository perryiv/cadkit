
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _osgVRJ_Application_
#define _osgVRJ_Application_

#include "Export.h"
#include "SharedDouble.h"

#include "OsgTools/Render/Renderer.h"
#include "OsgTools/Render/SceneManager.h"

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

namespace osgVRJ
{

  class OSG_VRJ_EXPORT Application : public vrj::GlApp, public osg::Referenced
  {
  public:
    // Typedefs.
    typedef OsgTools::Render::Renderer Renderer;
    typedef Renderer::RefPtr RendererPtr;

    // Constructors.
    Application();
    Application( vrj::Kernel* );
    Application( const std::list<std::string>& );  // valid vrj config files

    virtual void            init();
    virtual void            viewAll ( osg::MatrixTransform *mt, osg::Matrix::value_type zScale=2 );

    /// Add a light.
    void                    addLight ( osg::Light* light );

    /// Get/Set the background color.
    void                    setBackgroundColor( const osg::Vec4& bg ) { _background_color = bg; _context_in_sync=false; }
    const osg::Vec4&        getBackgroundColor() const { return _background_color; }

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

    /// Set the viewport.
    virtual void            _setViewport ( osg::Viewport*, vrj::GlDrawManager* );

    /// Set the renderer with proper data.
    virtual void            _initRenderer ( Renderer* );

    void                    _construct();
    Renderer*               _getContextSpecificRenderer();

    // Get the duration of the last frame in seconds.
    double                  _getFrameTime() const;

    /// No copying.
    Application ( const Application& );
    Application& operator = (const Application&);
    
    /// Use reference counting.
    virtual ~Application();

  private:
    // data members
    osg::Timer                         _timer;
    osg::ref_ptr< osg::FrameStamp >    _framestamp;
    osg::ref_ptr< osg::Viewport >      _viewport;

    osg::Vec4                          _background_color;

    bool                               _context_in_sync;
    
    osg::Timer_t                       _initial_time;
    osg::Timer_t                       _frameStart;
    double                             _frameTime;
    cluster::UserData < SharedDouble > _sharedFrameTime;

    vrj::GlContextData< RendererPtr >  _renderer;
    OsgTools::Render::SceneManager::RefPtr _sceneManager;
  };

}

#endif
