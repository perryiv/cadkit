
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Open scene graph renderer.
//
///////////////////////////////////////////////////////////////////////////////

#include "SceneGraph/OpenSceneGraph/Renderer.h"
#include "SceneGraph/OpenSceneGraph/Node.h"

#include "OsgTools/Convert/Matrix.h"
#include "OsgTools/Render/Defaults.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Trace/Trace.h"

using namespace SceneGraph::OSG;

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

Renderer::Renderer() : BaseClass(),
  _viewer ( new osgUtil::SceneView ),
  _context ( 0x0 )
{
  USUL_TRACE_SCOPE;
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
  _context = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre-render the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::preRender ( IUnknown::RefPtr projection, IUnknown::RefPtr scene )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Render the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::render ( IUnknown::RefPtr projection, IUnknown::RefPtr scene )
{
  USUL_TRACE_SCOPE;

  // Handle no context.
  IContext::QueryPtr context ( this->getOpenGLContext() );
  if ( false == context.valid() )
    return;

  // Return if it's not the correct thread.
  if ( false == context->isContextThread() )
    return;

  // Handle no viewer.
  ViewerPtr viewer ( this->_getViewer() );
  if ( false == viewer.valid() )
    return;

  // Set the projection.
  typedef Usul::Interfaces::SceneGraph::IProjectionMatrix IProjectionMatrix;
  IProjectionMatrix::QueryPtr pm ( projection );
  if ( true == pm.valid() )
  {
    const IProjectionMatrix::Matrix m1 ( pm->getProjectionMatrix() );
    const osg::Matrixd m2 ( Usul::Convert::Type<IProjectionMatrix::Matrix,osg::Matrixd>::convert ( m1 ) );
    viewer->setProjectionMatrix ( m2 );
  }

  // Set the viewport.
  typedef Usul::Interfaces::SceneGraph::IViewportGet IViewportGet;
  IViewportGet::QueryPtr gvp ( projection );
  if ( true == gvp.valid() )
  {
    double width ( 1 ), height ( 1 );
    gvp->getViewport ( width, height );
    if ( ( width > 0 ) && ( height > 0 ) )
    {
      viewer->setViewport ( 0, 0, static_cast < int > ( width ), static_cast < int > ( height ) );
    }
  }

  // Set the scene.
  typedef Usul::Interfaces::SceneGraph::IChild IChild;
  IChild::QueryPtr child ( scene );
  Node::RefPtr node ( dynamic_cast < Node * > ( child.get() ) );
  Node::NodePtr n ( ( true == node.valid() ) ? node->node() : Node::NodePtr ( 0x0 ) );
  viewer->setSceneData ( ( true == n.valid() ) ? n.get() : new osg::Group );

  // Make this context current.
  context->makeCurrent();

  // Render a single frame.
  viewer->cull();
  viewer->draw();

  // Swap the buffers.
  context->swapBuffers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Post-render the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::postRender ( IUnknown::RefPtr projection, IUnknown::RefPtr scene )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the viewer.
//
///////////////////////////////////////////////////////////////////////////////

const Renderer::ViewerPtr Renderer::_getViewer() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
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
  Guard guard ( this );
  return _viewer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the context.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown::RefPtr Renderer::getOpenGLContext() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _context;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the context.
//
///////////////////////////////////////////////////////////////////////////////

void Renderer::setOpenGLContext ( IUnknown::RefPtr context )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _context = context;
}
