#include "osgVRJPrecompiled.h"
#include "Application.h"

#include "osg/LightModel"
#include "osg/MatrixTransform"
#include "osg/AlphaFunc"
#include "osg/TexEnv"

#include "vrj/Kernel/Kernel.h"
#include "vrj/Draw/OGL/GlWindow.h"

#include <stdexcept>

using namespace osgVRJ;


///////////////////////////////////////////////////////////////////////////////
//
//  Initializer list.
//
///////////////////////////////////////////////////////////////////////////////

#define CONSTRUCTOR_INITIALIZER_LIST\
  _timer(),\
  _scene_data(0x0),\
  _global_stateset(new osg::StateSet),\
  _scene_decorator(0x0),\
  _framestamp( 0x0 ),\
  _viewport ( 0x0 ),\
  _background_color(0,0,0,1),\
  _context_in_sync(false),\
  _initial_time( static_cast < osg::Timer_t > ( 0.0 ) ),\
  _frameStart ( static_cast < osg::Timer_t > ( 0.0 ) ), \
  _sharedFrameTime(),\
  _frameTime  ( 1 ), \
  _renderer()


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::Application() : vrj::GlApp(),
  CONSTRUCTOR_INITIALIZER_LIST
{
  this->_construct();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

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


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::Application(const std::list<std::string>& configs) : vrj::GlApp(),
  CONSTRUCTOR_INITIALIZER_LIST
{
  this->_construct();
  std::for_each(configs.begin(),configs.end(), Detail::LoadConfigFile());
}


///////////////////////////////////////////////////////////////////////////////
//
//  Not sure if this is still needed.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_construct()
{
  // Crank up the max number of contexts. When you figure out a good way to 
  // get the real number, then use it here instead of this hard-coded one. 
  // Note: this has to be done early. Waiting for init() or contextInit() is 
  // too late.
  osg::DisplaySettings::instance()->setMaxNumberOfGraphicsContexts ( 20 );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Application::~Application()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the application.
//
///////////////////////////////////////////////////////////////////////////////

void Application::run()
{
  vrj::Kernel* kernel = vrj::Kernel::instance();
  kernel->setApplication( this );
  kernel->start();
  kernel->waitForKernelStop();
}


///////////////////////////////////////////////////////////////////////////////
//
//  A method to autocenter, places the node in front of viewer
//
///////////////////////////////////////////////////////////////////////////////

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


///////////////////////////////////////////////////////////////////////////////
//
//  Called after the context is initalized.
//
///////////////////////////////////////////////////////////////////////////////

void Application::contextInit()
{
  RendererPtr renderer ( new Renderer );

  renderer->init();

  unsigned int unique_context_id = vrj::GlDrawManager::instance()->getCurrentContext();

  renderer->uniqueID( unique_context_id );

  // allocates things if not already valid,
  // then assigns the sceneview instance the
  // same values as this class maintains
  setUpSceneViewWithData( renderer.get() );

  // Keep handle to framestamp.
  _framestamp = renderer->framestamp();

  // Keep handle to viewport.
  _viewport = renderer->viewport();

  (*_renderer) = renderer.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called after the context is made current, but before the frame draws.
//
///////////////////////////////////////////////////////////////////////////////

void Application::contextPreDraw()
{
  // if necessary, make data members sync up with the sceneview's data
  if(!_context_in_sync)
  {
    Renderer* renderer = _getContextSpecificSceneView();
    setUpSceneViewWithData( renderer );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Small class that pushes the state in constructor and pops it in destructor.
//  This is for exception safety.
//
///////////////////////////////////////////////////////////////////////////////

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

        //glMatrixMode(GL_TEXTURE);
        //glPushMatrix();
      }
      ~OpenGlStackPushPop()
      {
        // Check for errors.
        #if 0
        assert ( GL_ERROR_NO == ::glGetError() );
        #endif

        //glMatrixMode(GL_TEXTURE);
        //glPopMatrix();

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        glPopAttrib();
      }
    };
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw.
//
///////////////////////////////////////////////////////////////////////////////

void Application::draw()
{
  // For exception safety. Pushes attributes in constructor, pops them in destructor.
  Detail::OpenGlStackPushPop pushPop;

  Renderer* renderer = _getContextSpecificSceneView();

  vrj::GlDrawManager* mgr = vrj::GlDrawManager::instance();
  vprASSERT(mgr != NULL);

  // ? constantly adjust the viewport ?
  osg::Viewport* vp = renderer->viewport();
  setViewportByDrawManager( vp,mgr);

  // constantly update the projection matrix
  vrj::GlUserData* userData = mgr->currentUserData();
  vrj::Projection* projection = userData->getProjection();
  vrj::Frustum frustum = projection->getFrustum();
  
  // Do we need to do this each frame?
  renderer->setFrustum ( frustum[vrj::Frustum::VJ_LEFT],
				                 frustum[vrj::Frustum::VJ_RIGHT],
				                 frustum[vrj::Frustum::VJ_BOTTOM],
				                 frustum[vrj::Frustum::VJ_TOP],
				                 frustum[vrj::Frustum::VJ_NEAR],
				                 frustum[vrj::Frustum::VJ_FAR]);

  // constantly update the view matrix
  gmtl::Matrix44f proj_mat = projection->getViewMatrix();

  osg::ref_ptr<osg::RefMatrix> osg_proj_xform_mat= new osg::RefMatrix;
  osg_proj_xform_mat->set(proj_mat.mData);
  
  renderer->viewMatrix(*osg_proj_xform_mat);

  // Do the drawing.
  renderer->render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene view.
//  Should only be called from a context-specific thread.
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::Render::Renderer* Application::_getContextSpecificSceneView()
{
  RendererPtr renderer = *(_renderer);
  return renderer.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene data.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Application::getSceneData()
{
  if(_scene_decorator.valid())
    return(_scene_decorator.get());

  else
    return(_scene_data.get());
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the time elapsed since the start of the application.
//
///////////////////////////////////////////////////////////////////////////////

double Application::getTimeSinceStart()
{
  double time_since_start ( _timer.delta_s( _initial_time, _timer.tick() ) );
  return time_since_start;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the application.
//
///////////////////////////////////////////////////////////////////////////////

void Application::init()
{
  _initial_time = _timer.tick();

  if(_scene_data.valid())
    this->setSceneData(_scene_data.get());

  // Also set up the global state-set.
  this->initGlobalStateSet();

  // Initialize the shared frame time data.
  vpr::GUID guid ( "8297080d-c22c-41a6-91c1-188a331fabe5" );
  _sharedFrameTime.init ( guid, "viz0" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called before the frame.
//
///////////////////////////////////////////////////////////////////////////////

void Application::preFrame()
{
  // Write the frame time if we've suppose to.
  if( _sharedFrameTime.isLocal() )
  {
    if( _framestamp.valid() && _framestamp->getFrameNumber() % 300 == 0 )
      std::cerr << "Writing frame time: " << _frameTime << std::endl;

    // Capture the frame time.
    _sharedFrameTime->data = _frameTime;
  }

  // Mark the start of the frame.
  _frameStart = _timer.tick();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called after preFrame, but before the frame.
//
///////////////////////////////////////////////////////////////////////////////

void Application::latePreFrame()
{
  // Get the frame time.
  _frameTime = _sharedFrameTime->data;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the frame is done.
//
///////////////////////////////////////////////////////////////////////////////

void Application::postFrame()
{
  // Capture the frame time.
  _frameTime = _timer.delta_s( _frameStart, _timer.tick() );

#if 0
  // Write the frame time if we've suppose to.
  if( _sharedFrameTime.isLocal() )
  {
    double frameTime ( _timer.delta_s( _frameStart, _timer.tick() ) );
    if( _framestamp.valid() && _framestamp->getFrameNumber() % 300 == 0 )
      std::cerr << "Writing frame time: " << frameTime << std::endl;

    // Capture the frame time.
    _sharedFrameTime->data = frameTime;
  }
#endif

  if(!_context_in_sync)
    _context_in_sync = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Application thread call settings done here will
//  be pushed out to all SceneView instances in
//  a context specific thread
//
///////////////////////////////////////////////////////////////////////////////

void Application::initGlobalStateSet()
{
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


///////////////////////////////////////////////////////////////////////////////
//
//  Set the scene data.
//
///////////////////////////////////////////////////////////////////////////////

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


///////////////////////////////////////////////////////////////////////////////
//
//  Set the scene decorator.
//
///////////////////////////////////////////////////////////////////////////////

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


///////////////////////////////////////////////////////////////////////////////
//
//  Set up the scene view.
//  Should only be called from a context-specific thread.
//
///////////////////////////////////////////////////////////////////////////////

void Application::setUpSceneViewWithData( Renderer* renderer )
{
  // apply the decorator concept when setting the
  // scene data for all the sceneviews
  if(_scene_decorator.valid())
    renderer->scene (_scene_decorator.get());

  else if(_scene_data.valid())
    renderer->scene (_scene_data.get());

  else
    renderer->scene( 0x0 );

  // take care of global stateset
  if(_global_stateset.valid())
    renderer->viewer()->setGlobalStateSet(_global_stateset.get());

  // take care of background color
  renderer->backgroundColor( _background_color );

  renderer->viewer()->setLightingMode(osgUtil::SceneView::NO_SCENEVIEW_LIGHT);
  renderer->viewer()->setComputeNearFarMode(osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the viewport.
//
///////////////////////////////////////////////////////////////////////////////

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


///////////////////////////////////////////////////////////////////////////////
//
//  Quit the application.
//
///////////////////////////////////////////////////////////////////////////////

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


///////////////////////////////////////////////////////////////////////////////
//
//  Get the frame stamp.
//
///////////////////////////////////////////////////////////////////////////////

osg::FrameStamp* Application::getFrameStamp()
{
  return _framestamp.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the frame stamp.
//
///////////////////////////////////////////////////////////////////////////////

const osg::FrameStamp* Application::getFrameStamp() const
{
  return _framestamp.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the frame-time.
//
///////////////////////////////////////////////////////////////////////////////

double Application::_getFrameTime() const
{
  //return _sharedFrameTime->data;
  return _frameTime;
}

