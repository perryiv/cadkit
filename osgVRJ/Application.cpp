#include "osgVRJPrecompiled.h"
#include "Application.h"

#include <osgUtil/RenderStage>
#include <osgUtil/RenderBin>
#include <osg/FrameStamp>
#include <osg/LightModel>
#include <osg/MatrixTransform>
#include <osgUtil/UpdateVisitor>
#include "osg/AlphaFunc"
#include "osg/TexEnv"

#ifdef _OSG096_
#include <osgUtil/DisplayListVisitor>
#else
#include <osgUtil/GLObjectsVisitor>
#endif

#include <vrj/Kernel/Kernel.h>
#include "vrj/Draw/OGL/GlWindow.h"

#include <stdexcept>

using namespace osgVRJ;

#define CONSTRUCTOR_INITIALIZER_LIST\
  _scene_data(0x0),\
  _frame_stamp(new osg::FrameStamp),\
  _init_visitor(0x0),\
  _update_visitor(new osgUtil::UpdateVisitor),\
  _global_stateset(new osg::StateSet),\
  _scene_decorator(0x0),\
  _viewport(0x0),\
  _background_color(0,0,0,1),\
  _context_in_sync(false),\
  _frameNumber(0),\
  _initial_time(0.0),\
  _head_tracker(),\
  _sceneview()

Application::Application() : vrj::GlApp(),
  CONSTRUCTOR_INITIALIZER_LIST
{
  this->_construct();
}

Application::Application(vrj::Kernel* kern) : vrj::GlApp(kern),
  CONSTRUCTOR_INITIALIZER_LIST
{
  this->_construct();
}

namespace osgVRJ
{
  namespace Detail
  {
    struct LoadConfigFile
    {
      void operator() (const std::string& file)
      {
        vrj::Kernel* kern = vrj::Kernel::instance();
        kern->loadConfigFile( file.c_str() );
      }
    };
  };
};

Application::Application(const std::list<std::string>& configs) : vrj::GlApp(),
  CONSTRUCTOR_INITIALIZER_LIST
{
  this->_construct();
  std::for_each(configs.begin(),configs.end(), Detail::LoadConfigFile());
}

void Application::_construct()
{
  // Crank up the max number of contexts. When you figure out a good way to 
  // get the real number, then use it here instead of this hard-coded one. 
  // Note: this has to be done early. Waiting for init() or contextInit() is 
  // too late.
  osg::DisplaySettings::instance()->setMaxNumberOfGraphicsContexts ( 20 );
}

Application::~Application()
{
}

void Application::run()
{
  vrj::Kernel* kernel = vrj::Kernel::instance();
  kernel->setApplication( this );
  kernel->start();
  kernel->waitForKernelStop();
}

// a method to autocenter, places the node in front of viewer
void Application::viewAll ( osg::MatrixTransform* node, osg::Matrix::value_type zScale )
{
  // Get the bounding sphere of the group.
  osg::BoundingSphere bs = node->getBound();
  osg::Vec3 c = bs.center();

  // Push it back so we can see it.
  osg::Matrix matrix;
  osg::Matrix::value_type z ( zScale * bs.radius() + c[2] );
  matrix.makeTranslate( -c[0], -c[1], -z );
  node->postMult( matrix );
}

void Application::contextInit()
{
  // Make a new state.
  osg::ref_ptr<osg::State> state = new osg::State;

  // init visitor setup
#ifdef _OSG096_
  typedef osgUtil::DisplayListVisitor VisitorType;
#else
  typedef osgUtil::GLObjectsVisitor VisitorType;
#endif

  VisitorType::Mode dlvMode = VisitorType::COMPILE_STATE_ATTRIBUTES;
#ifndef __sgi
  dlvMode |= VisitorType::COMPILE_DISPLAY_LISTS;
#endif

  osg::ref_ptr<VisitorType> dlv = new VisitorType(dlvMode);
  dlv->setState( state.get() );
  dlv->setNodeMaskOverride( 0xffffffff );

  // render stuff setup
  osg::ref_ptr<osgUtil::RenderStage> rs = new osgUtil::RenderStage;
  osg::ref_ptr<osgUtil::StateGraph> sg = new osgUtil::StateGraph;

  // cull visitor setup
  osg::ref_ptr<osgUtil::CullVisitor> cv = new osgUtil::CullVisitor;
  cv->setRenderStage( rs.get() );
  cv->setStateGraph( sg.get() );

  osgUtil::SceneView* sv = new osgUtil::SceneView;
  sv->setState( state.get() );
  sv->setInitVisitor( dlv.get() );
  sv->setCullVisitor( cv.get() );
  sv->setRenderStage( rs.get() );
  sv->setStateGraph( sg.get() );

  unsigned int unique_context_id = vrj::GlDrawManager::instance()->getCurrentContext();
  sv->getState()->setContextID(unique_context_id);

  // allocates things if not already valid,
  // then assigns the sceneview instance the
  // same values as this class maintains
  setUpSceneViewWithData(sv);

  sv->init();
  (*_sceneview) = sv;  
}

void Application::contextPreDraw()
{
  osgUtil::SceneView* sv = getContextSpecificSceneView();

  // if necessary, make data members sync up with the sceneview's data
  if(!_context_in_sync)
    setUpSceneViewWithData(sv);
}


// Small class that pushes the state in constructor and pops it in destructor.
// This is for exception safety.
namespace osgVRJ
{
  namespace Detail
  {
    struct OpenGlStackPushPop
    {
      OpenGlStackPushPop()
      {
        #if 0
        // Clear the errors.
        while ( GL_ERROR_NO != ::glGetError() ){}
        #endif

        glPushAttrib(GL_ALL_ATTRIB_BITS);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();

        glMatrixMode(GL_TEXTURE);
        glPushMatrix();
      }
      ~OpenGlStackPushPop()
      {
        // Check for errors.
        #if 0
        assert ( GL_ERROR_NO == ::glGetError() );
        #endif

        glMatrixMode(GL_TEXTURE);
        glPopMatrix();

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        glPopAttrib();
      }
    };
  };
};


void Application::draw()
{
  // For exception safety. Pushes attributes in constructor, pops them in destructor.
  Detail::OpenGlStackPushPop pushPop;

  osgUtil::SceneView* sv = getContextSpecificSceneView();
  vrj::GlDrawManager* mgr = vrj::GlDrawManager::instance();
  vprASSERT(mgr != NULL);

  // ? constantly adjust the viewport ?
  osg::Viewport* vp = sv->getViewport();
  setViewportByDrawManager(sv->getViewport(),mgr);

  // constantly update the projection matrix
  vrj::GlUserData* userData = mgr->currentUserData();
  vrj::Projection* projection = userData->getProjection();
  vrj::Frustum frustum = projection->getFrustum();
  sv->setProjectionMatrixAsFrustum(frustum[vrj::Frustum::VJ_LEFT],
				   frustum[vrj::Frustum::VJ_RIGHT],
				   frustum[vrj::Frustum::VJ_BOTTOM],
				   frustum[vrj::Frustum::VJ_TOP],
				   frustum[vrj::Frustum::VJ_NEAR],
				   frustum[vrj::Frustum::VJ_FAR]);

  // constantly update the view matrix
  gmtl::Matrix44f proj_mat = projection->getViewMatrix();

  osg::ref_ptr<osg::RefMatrix> osg_proj_xform_mat= new osg::RefMatrix;
  osg_proj_xform_mat->set(proj_mat.mData);
  sv->setViewMatrix(*osg_proj_xform_mat);

  sv->cull();
  sv->draw();
}

// should only be called from a context-specific thread
osgUtil::SceneView* Application::getContextSpecificSceneView()
{
  osg::ref_ptr<osgUtil::SceneView> sv = *(_sceneview);
  return( sv.get() );
}

osg::Node* Application::getSceneData()
{
  if(_scene_decorator.valid())
    return(_scene_decorator.get());

  else
    return(_scene_data.get());
}

double Application::getTimeSinceStart()
{
  return(_head_tracker->getTimeStamp().secd() - _initial_time);
}

void Application::init()
{
  _head_tracker.init("VJHead");
  _initial_time = _head_tracker->getTimeStamp().secd();

  if(_scene_data.valid())
    setSceneData(_scene_data.get());

  // frame stamp setup
  _frame_stamp->setFrameNumber(0);
  _frame_stamp->setReferenceTime(_initial_time);

  // update visitor setup
  _update_visitor->setFrameStamp(_frame_stamp.get());
  _update_visitor->setTraversalNumber(_frame_stamp->getFrameNumber());

  // Also set up the global state-set.
  this->initGlobalStateSet();
}

void Application::preFrame()
{
  // advance the time value of the frame stamp
  _frame_stamp->setFrameNumber(_frameNumber++);
  _frame_stamp->setReferenceTime(getTimeSinceStart());

  update();  // traverse the scene
}

void Application::postFrame()
{
  if(!_context_in_sync)
    _context_in_sync = true;
}

// application thread call settings done here will
// be pushed out to all SceneView instances in
// a context specific thread
void Application::initGlobalStateSet()
{
  // update visitor setup
  if(_update_visitor.valid())
    _update_visitor->setFrameStamp(_frame_stamp.get());

  // global state set setup
  _global_stateset = new osg::StateSet;
  _global_stateset->setGlobalDefaults();
  _global_stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON);
  _global_stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
  _global_stateset->setMode(GL_CULL_FACE,osg::StateAttribute::ON);

  osg::AlphaFunc* alpha_func = new osg::AlphaFunc;
  alpha_func->setFunction(osg::AlphaFunc::GREATER,0.0f);
  _global_stateset->setAttributeAndModes(alpha_func,osg::StateAttribute::ON);

  osg::TexEnv* texEnv = new osg::TexEnv;
  texEnv->setMode(osg::TexEnv::MODULATE);
  _global_stateset->setTextureAttributeAndModes(0,texEnv,osg::StateAttribute::ON);

  osg::LightModel* light_model = new osg::LightModel;
  light_model->setAmbientIntensity(osg::Vec4(0.1f,0.1f,0.1f,1.0f));
  _global_stateset->setAttributeAndModes(light_model,osg::StateAttribute::ON);

  // background color setup
  setBackgroundColor( osg::Vec4(0.2,0.2,0.4,1.0) );

  // flag requesting sync of data members with each of the sceneviews.
  _context_in_sync = false;
}

void Application::setSceneData(osg::Node* data)
{
  if(_scene_data == data)
    return;

  if(_scene_decorator.valid() && _scene_data.valid())
    _scene_decorator->removeChild( _scene_data.get() );

  _scene_data = data;

  if(_scene_decorator.valid() && _scene_data.valid())
    _scene_decorator->addChild( _scene_data.get() );

  _context_in_sync = false;
}

void Application::setSceneDecorator(osg::Group* decor)
{
  if(_scene_decorator == decor) return;

  _scene_decorator = decor;

  if( decor && _scene_data.valid())
    {
      decor->addChild(_scene_data.get());
    }

  _context_in_sync = false;
}

// should only be called from a context-specific thread
void Application::setUpSceneViewWithData(osgUtil::SceneView* sv)
{
  // apply the decorator concept when setting the
  // scene data for all the sceneviews
  if(_scene_decorator.valid())
    sv->setSceneData(_scene_decorator.get());

  else if(_scene_data.valid())
    sv->setSceneData(_scene_data.get());

  else
    sv->setSceneData( 0 );

  // take care of frame stamps
  if(_frame_stamp.valid())
    sv->setFrameStamp(_frame_stamp.get());

  // take care of global stateset
  if(_global_stateset.valid())
    sv->setGlobalStateSet(_global_stateset.get());

  // take care of background color
#ifdef _OSG096_
  sv->setBackgroundColor(_background_color);
#else
  sv->setClearColor( _background_color );
#endif

  sv->setLightingMode(osgUtil::SceneView::NO_SCENEVIEW_LIGHT);
  sv->setComputeNearFarMode(osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR);
}

void Application::setViewportByDrawManager(osg::Viewport* vp, vrj::GlDrawManager* mgr)
{
  float vpX,vpY,vpW,vpH;
  int winX,winY,winW,winH;

  mgr->currentUserData()->getViewport()->getOriginAndSize(vpX,vpY,vpW,vpH);
  mgr->currentUserData()->getGlWindow()->getOriginSize(winX,winY,winW,winH);

  unsigned int x,y,w,h;
  x = unsigned(vpX*float(winW));
  y = unsigned(vpY*float(winH));
  w = unsigned(vpW*float(winW));
  h = unsigned(vpH*float(winH));

  vp->setViewport(x,y,w,h);
}

void Application::update()
{
  if(_scene_data.valid() && _update_visitor.valid())
    {
      _update_visitor->reset();
      _update_visitor->setFrameStamp(_frame_stamp.get());

      if(_frame_stamp.valid())
	{
	  _update_visitor->setTraversalNumber(_frame_stamp->getFrameNumber());
	}

      getSceneData()->accept(*_update_visitor.get());
      getSceneData()->getBound();
    }
}

void Application::quit()
{
  vrj::Kernel::instance()->stop();
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the normalization state.
//
///////////////////////////////////////////////////////////////////////////////

void Application::normalize ( bool on )
{
  const osg::StateSet *temp = this->getGlobalStateSet();
  if ( !temp )
    throw std::runtime_error ( "Error 4271807142, failed to get global state set." );

  osg::ref_ptr<osg::StateSet> ss ( new osg::StateSet ( *temp ) );
  if ( !ss.valid() )
    throw std::runtime_error ( "Error 2255713473, failed to copy global state set." );

  ss->setMode ( GL_NORMALIZE, ( ( on ) ? osg::StateAttribute::ON : osg::StateAttribute::OFF ) );
  this->setGlobalStateSet ( ss.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
// Return the normalization state.
//
///////////////////////////////////////////////////////////////////////////////

bool Application::normalize() const
{
  const osg::StateSet *ss = this->getGlobalStateSet();
  return ( ( ss && osg::StateAttribute::ON == ss->getMode ( GL_NORMALIZE ) ) ? true : false );
}
