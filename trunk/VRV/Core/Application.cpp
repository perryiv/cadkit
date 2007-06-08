
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "VRV/Core/Application.h"

#include "Usul/Errors/Assert.h"

#include "OsgTools/State/StateSet.h"

#include "osg/MatrixTransform"

#include "vrj/Kernel/Kernel.h"
#include "vrj/Draw/OGL/GlWindow.h"

#include <stdexcept>

using namespace VRV::Core;


///////////////////////////////////////////////////////////////////////////////
//
//  Initializer list.
//
///////////////////////////////////////////////////////////////////////////////

#define CONSTRUCTOR_INITIALIZER_LIST\
  _timer(),\
  _framestamp( 0x0 ),\
  _viewport ( 0x0 ),\
  _backgroundColor( 0, 0, 0, 1 ),\
  _dirty ( false ),\
  _initialTime( static_cast < osg::Timer_t > ( 0.0 ) ),\
  _frameStart ( static_cast < osg::Timer_t > ( 0.0 ) ), \
  _sharedFrameTime(),\
  _frameTime  ( 1 ), \
  _renderer(), \
  _sceneManager ( new OsgTools::Render::SceneManager ), \
  _refCount ( 0 )


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
  // Make sure we don't have any references hanging around.
  USUL_ASSERT ( 0 == _refCount );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the application.
//
///////////////////////////////////////////////////////////////////////////////

void Application::run()
{
  vrj::Kernel* kernel ( vrj::Kernel::instance() );
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
  osg::BoundingSphere bs ( node->getBound() );
  osg::Vec3 c ( bs.center() );

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

  // Initialize the renderer.
  renderer->init();

  unsigned int uniqueContextId ( vrj::GlDrawManager::instance()->getCurrentContext() );

  renderer->uniqueID( uniqueContextId );

  // Turn off the default light.
  renderer->viewer()->setLightingMode ( osgUtil::SceneView::NO_SCENEVIEW_LIGHT );

  // Turn off computing of the near and far plane.
  renderer->viewer()->setComputeNearFarMode ( osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR );

  // Turn on back face culling.
  renderer->getGlobalStateSet()->setMode( GL_CULL_FACE, osg::StateAttribute::ON );

  this->_initRenderer( renderer.get() );

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
  if( !_dirty )
  {
    Renderer* renderer ( this->_getContextSpecificRenderer() );
    this->_initRenderer ( renderer );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Small class that pushes the state in constructor and pops it in destructor.
//  This is for exception safety.
//
///////////////////////////////////////////////////////////////////////////////

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


///////////////////////////////////////////////////////////////////////////////
//
//  Draw.
//
///////////////////////////////////////////////////////////////////////////////

void Application::draw()
{
  // For exception safety. Pushes attributes in constructor, pops them in destructor.
  Detail::OpenGlStackPushPop pushPop;

  Renderer* renderer ( this->_getContextSpecificRenderer() );

  vrj::GlDrawManager* mgr ( vrj::GlDrawManager::instance() );
  USUL_ASSERT ( 0x0 != mgr );

  // ? constantly adjust the viewport ?
  osg::Viewport* vp ( renderer->viewport() );
  this->_setViewport ( vp, mgr );

  // constantly update the projection matrix
  vrj::GlUserData* userData    ( mgr->currentUserData() );
  vrj::Projection* projection  ( userData->getProjection() );
  vrj::Frustum frustum         ( projection->getFrustum() );
  
  // We need to set this every frame to account for stereo.
  renderer->setFrustum ( frustum[vrj::Frustum::VJ_LEFT],
				                 frustum[vrj::Frustum::VJ_RIGHT],
				                 frustum[vrj::Frustum::VJ_BOTTOM],
				                 frustum[vrj::Frustum::VJ_TOP],
				                 frustum[vrj::Frustum::VJ_NEAR],
				                 frustum[vrj::Frustum::VJ_FAR] );

  // constantly update the view matrix
  gmtl::Matrix44f proj ( projection->getViewMatrix() );

  osg::ref_ptr< osg::RefMatrix > osgProj ( new osg::RefMatrix );
  osgProj->set(proj.mData);
  
  renderer->viewMatrix( *osgProj );

  // Do the drawing.
  renderer->update();
  renderer->render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the Renderer.
//  Should only be called from a context-specific thread.
//
///////////////////////////////////////////////////////////////////////////////

OsgTools::Render::Renderer* Application::_getContextSpecificRenderer()
{
  RendererPtr renderer = *(_renderer);
  return renderer.get();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the scene data.
//
///////////////////////////////////////////////////////////////////////////////

void Application::setSceneData( osg::Node* node )
{
  _sceneManager->model( node );

  // Is this needed?
  _dirty = false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene data.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Node* Application::getSceneData() const
{
  return _sceneManager->model();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene data.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Application::getSceneData()
{
  return _sceneManager->model();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the time elapsed since the start of the application.
//
///////////////////////////////////////////////////////////////////////////////

double Application::getTimeSinceStart()
{
  double time_since_start ( _timer.delta_s( _initialTime, _timer.tick() ) );
  return time_since_start;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the application.
//
///////////////////////////////////////////////////////////////////////////////

void Application::init()
{
  _initialTime = _timer.tick();

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

  // TODO: Write FrameStamp data.

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

  if( !_dirty )
    _dirty = true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set up the scene view.
//  Should only be called from a context-specific thread.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_initRenderer( Renderer* renderer )
{
  renderer->scene ( _sceneManager->scene() );

  // Set the background color.
  renderer->backgroundColor ( _backgroundColor );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the viewport.
//
///////////////////////////////////////////////////////////////////////////////

void Application::_setViewport( osg::Viewport* vp, vrj::GlDrawManager* mgr )
{
  float vpX,vpY,vpW,vpH;
  int winX,winY,winW,winH;

  mgr->currentUserData()->getViewport()->getOriginAndSize ( vpX, vpY, vpW, vpH );
  mgr->currentUserData()->getGlWindow()->getOriginSize ( winX, winY, winW, winH );

  unsigned int x ( static_cast < unsigned int > ( vpX * static_cast < float > ( winW ) ) );
  unsigned int y ( static_cast < unsigned int > ( vpY * static_cast < float > ( winH ) ) );
  unsigned int w ( static_cast < unsigned int > ( vpW * static_cast < float > ( winW ) ) );
  unsigned int h ( static_cast < unsigned int > ( vpH * static_cast < float > ( winH ) ) );

  vp->setViewport ( x, y, w, h );
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
  OsgTools::State::StateSet::setNormalize ( this->getSceneData(), on );
}


///////////////////////////////////////////////////////////////////////////////
//
// Return the normalization state.
//
///////////////////////////////////////////////////////////////////////////////

bool Application::normalize() const
{
  return OsgTools::State::StateSet::getNormalize ( this->getSceneData() );
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
  return _frameTime;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a light.
//
///////////////////////////////////////////////////////////////////////////////

void Application::addLight ( osg::Light* light )
{
  _sceneManager->globalLight ( light );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reference.  Keep track for debugging.
//
///////////////////////////////////////////////////////////////////////////////

void Application::ref()
{
  ++_refCount;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unreference.  Keep track for debugging..
//
///////////////////////////////////////////////////////////////////////////////

void Application::unref ( bool )
{
  --_refCount;
}
