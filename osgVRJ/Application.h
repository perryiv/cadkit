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
#include "osgVRJ.h"

// vrj includes
#include <vrj/Draw/OGL/GlApp.h>
#include <vrj/Draw/OGL/GlContextData.h>

// osg
#include <osg/Referenced>
#include <osg/Matrix>
#include <osgUtil/SceneView>

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

namespace osgUtil
{
};


namespace osgVRJ
{

  class OSG_VRJ_EXPORT Application : public vrj::GlApp, public osg::Referenced
  {
  public:
    // constructor(s)
    Application();
    Application(vrj::Kernel*);
    Application(const std::list<std::string>&);  // valid vrj config files

    // method(s)
    virtual void contextInit();
    virtual void contextPreDraw();
    virtual void init();
    virtual void preFrame();
    virtual void postFrame();
    virtual void viewAll (osg::MatrixTransform *mt,osg::Matrix::value_type zScale=2);

    void setBackgroundColor(const osg::Vec4& bg) { _background_color = bg; _context_in_sync=false; }
    const osg::Vec4& getBackgroundColor() const { return _background_color; }

    osgUtil::SceneView* getContextSpecificSceneView();  // DANGER!

    void setFrameStamp(osg::FrameStamp* fs) { _frame_stamp = fs; _context_in_sync=false; }
    osg::FrameStamp* getFrameStamp() { return _frame_stamp.get(); }
    const osg::FrameStamp* getFrameStamp() const { return _frame_stamp.get(); }

    void setGlobalStateSet(osg::StateSet* gss) { _global_stateset = gss; _context_in_sync=false; }
    const osg::StateSet* getGlobalStateSet() const { return _global_stateset.get(); }
    osg::StateSet* getGlobalStateSet() { return _global_stateset.get(); }

    osg::Node* getSceneData();
    void setSceneData(osg::Node*);

    void setSceneDecorator(osg::Group*);
    const osg::Group* getSceneDecorator() const { return _scene_decorator.get(); }

    double getTimeSinceStart();

    void setUpdateVisitor(osg::NodeVisitor* uv) { _update_visitor = uv; _context_in_sync=false; }
    const osg::NodeVisitor* getUpdateVisitor() const { return _update_visitor.get(); }

    void normalize ( bool state );
    bool normalize() const;

    void quit();
    void run();
    void update();

  protected:

    virtual void draw();
    virtual void setViewportByDrawManager(osg::Viewport*,vrj::GlDrawManager*);
    virtual void setUpSceneViewWithData(osgUtil::SceneView*);
    virtual void initGlobalStateSet();

    void _construct();

    Application(const Application&);               // not implemented by design
    Application& operator = (const Application&);  // not implemented by design
    virtual ~Application();  // destructor

  private:
    // data members
    osg::ref_ptr<osg::Node> _scene_data;
    osg::ref_ptr<osg::FrameStamp> _frame_stamp;
    osg::ref_ptr<osg::NodeVisitor> _init_visitor;
    osg::ref_ptr<osg::NodeVisitor> _update_visitor;
    osg::ref_ptr<osg::StateSet> _global_stateset;
    osg::ref_ptr<osg::Group> _scene_decorator;
    osg::ref_ptr<osg::Viewport> _viewport;

    osg::Vec4 _background_color;

    bool _context_in_sync;
    unsigned int _frameNumber;
    double _initial_time;

    gadget::PositionInterface _head_tracker;
    vrj::GlContextData<osg::ref_ptr<osgUtil::SceneView> > _sceneview;
  };

};

#endif
