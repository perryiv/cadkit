// Description:  This file contains
// the very basic necessities for any OSG code to work
// with VR Juggler.  It's sole purpose is to
// add necessary functionality to adaquately use
// the osgUtil::SceneView class, thus it provides features
// common to what the developer is used to with the
// osgProducer::CameraGroup class.

#ifndef _osgVRJ_Application_
#define _osgVRJ_Application_

// osgjuggler includes
#include "Export.h"
#include "SharedDouble.h"

#include "OsgTools/Render/Renderer.h"

// vrj includes
#include "vrj/Draw/OGL/GlApp.h"
#include "vrj/Draw/OGL/GlContextData.h"
#include "plugins/ApplicationDataManager/UserData.h"

// osg
#include "osg/Referenced"
#include "osg/Matrix"
#include "osg/Timer"

#include <list>
#include <string>


// forward declarations
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
    virtual void            viewAll (osg::MatrixTransform *mt,osg::Matrix::value_type zScale=2);

    /// Get/Set the background color.
    void                    setBackgroundColor(const osg::Vec4& bg) { _background_color = bg; _context_in_sync=false; }
    const osg::Vec4&        getBackgroundColor() const { return _background_color; }

    /// Get/Set the framestamp.
    osg::FrameStamp*        getFrameStamp();
    const osg::FrameStamp*  getFrameStamp() const;

    /// Get/Set the global state set.
    void                    setGlobalStateSet(osg::StateSet* gss) { _global_stateset = gss; _context_in_sync=false; }
    const osg::StateSet*    getGlobalStateSet() const { return _global_stateset.get(); }
    osg::StateSet*          getGlobalStateSet() { return _global_stateset.get(); }

    /// Get/Set the scene data.
    osg::Node*              getSceneData();
    void                    setSceneData(osg::Node*);

    /// Get/Set the scene decorator.
    void                    setSceneDecorator(osg::Group*);
    const osg::Group*       getSceneDecorator() const { return _scene_decorator.get(); }

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
    virtual void            setViewportByDrawManager(osg::Viewport*,vrj::GlDrawManager*);

    /// Set the renderer with proper data.
    virtual void            setUpSceneViewWithData( Renderer* );

    /// Initialize the global state set.
    virtual void            initGlobalStateSet();

    void                    _construct();
    Renderer*               _getContextSpecificSceneView();

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
    osg::ref_ptr<osg::Node>            _scene_data;
    osg::ref_ptr<osg::StateSet>        _global_stateset;
    osg::ref_ptr<osg::Group>           _scene_decorator;
    osg::ref_ptr<osg::FrameStamp>      _framestamp;
    osg::ref_ptr<osg::Viewport>        _viewport;

    osg::Vec4                          _background_color;

    bool                               _context_in_sync;
    
    osg::Timer_t                       _initial_time;
    osg::Timer_t                       _frameStart;
    double                                     _frameTime;
    mutable cluster::UserData < SharedDouble > _sharedFrameTime;

    vrj::GlContextData< RendererPtr >  _renderer;
  };

}

#endif
