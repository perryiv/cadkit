
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Open scene graph renderer.
//
///////////////////////////////////////////////////////////////////////////////

#include "Display/Render/OSG/Renderer.h"

#include "OsgTools/Group.h"
#include "OsgTools/Render/Defaults.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Adaptors/Bind.h"
#include "Usul/Exceptions/Exception.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include "osgViewer/Renderer"

#include <limits>

using namespace Display::Render::OSG;

USUL_IMPLEMENT_TYPE_ID ( Renderer );


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

Renderer::Renderer ( IUnknown::RefPtr context, IUnknown::RefPtr caller ) : BaseClass(),
  _viewer ( new osgViewer::Viewer ),
  _window ( 0x0 ),
  _context ( context ),
  _caller ( caller )
{
  USUL_TRACE_SCOPE;

  // We need this interface.
  if ( false == _context.valid() )
    throw Usul::Exceptions::Exception ( "Error 1335654694: context interface not available" );

  // Default (dummy) sizes that will be reset in the resize event handler.
  const int width  ( 100 );
  const int height ( 100 );

  // This will also be reset.
  const double zNear ( 10 );
  const double zFar ( 10000 );

  // This was the default field-of-view in the Qt example.
  const double fieldOfView ( 30 );

  // We have an embedded window.
  _window = new osgViewer::GraphicsWindowEmbedded ( 0, 0, width, height );

  // Set the camera properties.
  CameraPtr camera ( _viewer->getCamera() );
  camera->setViewport ( new osg::Viewport ( 0, 0, width, height ) );
  camera->setProjectionMatrixAsPerspective ( fieldOfView, 1, zNear, zFar );
  camera->setGraphicsContext ( _window.get() );
  camera->setRenderOrder ( osg::Camera::POST_RENDER );
  camera->setReferenceFrame ( osg::Camera::ABSOLUTE_RF );
  camera->setViewMatrix ( osg::Matrix::identity() );
  camera->setComputeNearFarMode ( osg::CullSettings::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES );
  camera->setCullingMode ( osg::CullSettings::VIEW_FRUSTUM_CULLING );

  // Set viewer properties.
  _viewer->setDatabasePager ( 0x0 );
  _viewer->setThreadSafeReferenceCounting ( true );
  _viewer->setThreadSafeRefUnref ( true );
  _viewer->setThreadingModel ( osgViewer::ViewerBase::SingleThreaded );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

Renderer::~Renderer()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Renderer::_destroy ), "3249905849" );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destroy this instance.
//
/////////////////////////////////////////////////////////////////////////////

void Renderer::_destroy()
{
  USUL_TRACE_SCOPE;
  _window = 0x0;
  _viewer = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Render the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::_render()
{
  USUL_TRACE_SCOPE;

  // Handle no context.
  IContext::RefPtr context ( this->_getContext() );
  if ( false == context.valid() )
    return;

  // Return if it's not the correct thread.
  if ( false == context->isContextThread() )
    return;

  // Handle no viewer.
  ViewerPtr viewer ( this->_getViewer() );
  if ( false == viewer.valid() )
    return;

  // Need local scope.
  {
    WriteLock lock ( this->mutex() );

    // Make this context current.
    context->makeCurrent();

    // Render a single frame.
    viewer->frame();

    // Swap the buffers.
    context->swapBuffers();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Call this when you want the viewport to resize.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::resize ( unsigned int w, unsigned int h )
{
  USUL_TRACE_SCOPE;

  // Make sure it's in range.
  const bool wIsGood ( w < static_cast < unsigned int > ( std::numeric_limits<int>::max() ) );
  const bool hIsGood ( h < static_cast < unsigned int > ( std::numeric_limits<int>::max() ) );
  if ( wIsGood && hIsGood )
  {
    // Resize the window.
    GraphicsWindowPtr window ( this->_getWindow() );
    if ( true == window.valid() )
    {
      const int iw ( static_cast < int > ( w ) );
      const int ih ( static_cast < int > ( h ) );

      // Resize the event queue if we have one. Not sure what this is.
      if ( 0x0 != window->getEventQueue() )
      {
        window->getEventQueue()->windowResize ( 0, 0, iw, ih );
      }

      // Tell the "window" it has been resized. I think this updates 
      // the camera's viewport and the projection matrix.
      window->resized ( 0, 0, static_cast < int > ( w ), static_cast < int > ( h ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the window.
//
///////////////////////////////////////////////////////////////////////////////

const Renderer::GraphicsWindowPtr Renderer::_getWindow() const
{
  USUL_TRACE_SCOPE;
  ReadLock lock ( this->mutex() );
  return _window;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the window.
//
///////////////////////////////////////////////////////////////////////////////

Renderer::GraphicsWindowPtr Renderer::_getWindow()
{
  USUL_TRACE_SCOPE;
  ReadLock lock ( this->mutex() );
  return _window;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewer.
//
///////////////////////////////////////////////////////////////////////////////

const Renderer::ViewerPtr Renderer::_getViewer() const
{
  USUL_TRACE_SCOPE;
  ReadLock lock ( this->mutex() );
  return _viewer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewer.
//
///////////////////////////////////////////////////////////////////////////////

Renderer::ViewerPtr Renderer::_getViewer()
{
  USUL_TRACE_SCOPE;
  ReadLock lock ( this->mutex() );
  return _viewer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the camera.
//
///////////////////////////////////////////////////////////////////////////////

const Renderer::CameraPtr Renderer::_getCamera() const
{
  USUL_TRACE_SCOPE;
  ReadLock lock ( this->mutex() );
  return ( ( true == _viewer.valid() ) ? _viewer->getCamera() : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the camera.
//
///////////////////////////////////////////////////////////////////////////////

Renderer::CameraPtr Renderer::_getCamera()
{
  USUL_TRACE_SCOPE;
  ReadLock lock ( this->mutex() );
  return ( ( true == _viewer.valid() ) ? _viewer->getCamera() : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the context.
//
///////////////////////////////////////////////////////////////////////////////

const Renderer::IContext::RefPtr Renderer::_getContext() const
{
  USUL_TRACE_SCOPE;
  ReadLock lock ( this->mutex() );
  return _context;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the context.
//
///////////////////////////////////////////////////////////////////////////////

Renderer::IContext::RefPtr Renderer::_getContext()
{
  USUL_TRACE_SCOPE;
  ReadLock lock ( this->mutex() );
  return _context;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::scene ( NodePtr node )
{
  USUL_TRACE_SCOPE;
  ViewerPtr viewer ( this->_getViewer() );
  if ( true == viewer.valid() )
  {
    viewer->setSceneData ( node.get() );
  }
}
