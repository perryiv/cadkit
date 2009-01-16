
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
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include <limits>

using namespace Display::Render::OSG;

USUL_IMPLEMENT_TYPE_ID ( Renderer );


/////////////////////////////////////////////////////////////////////////////
//
//  Initialize OSG
//
/////////////////////////////////////////////////////////////////////////////

namespace
{
  struct Init
  {
    Init()
    {
      osg::Referenced::setThreadSafeReferenceCounting ( true );
    }
    ~Init()
    {
    }
  } _initOSG;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

Renderer::Renderer ( IUnknown::RefPtr context, IUnknown::RefPtr caller ) : BaseClass(),
  _viewer ( new osgUtil::SceneView ),
  _context ( context ),
  _caller ( caller )
{
  USUL_TRACE_SCOPE;

  // We need this interface.
  if ( false == _context.valid() )
    throw Usul::Exceptions::Exception ( "Error 1335654694: context interface not available" );
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
    viewer->cull();
    viewer->draw();

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
  if ( ( false == wIsGood ) || ( false == hIsGood ) )
    return;

  ViewerPtr viewer ( this->_getViewer() );
  if ( false == viewer.valid() )
    return;

  // Set the projection matrix.
  const double fovy  ( OsgTools::Render::Defaults::CAMERA_FOV_Y );
  double zNear ( OsgTools::Render::Defaults::CAMERA_Z_NEAR );
  double zFar  ( OsgTools::Render::Defaults::CAMERA_Z_FAR );
  double width ( w ), height ( h );
  double aspect ( width / height );
  viewer->setProjectionMatrixAsPerspective ( fovy, aspect, zNear, zFar );

Why not have a projection matrix member in the base renderer class?
And/or the above members for creating it?
How about the navigation matrix?
Should the projection matrix be in the viewer/canvas?
Should you use Usul::Math::Matrix44 or osg::Matrixd ?

  // Set the viewport.
  osg::ref_ptr<osg::Viewport> vp ( new osg::Viewport ( 0, 0, static_cast < int > ( w ), static_cast < int > ( h ) ) );
  viewer->setViewport ( vp );
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
