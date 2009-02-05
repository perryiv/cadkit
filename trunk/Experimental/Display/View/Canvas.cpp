
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Canvas class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Display/View/Canvas.h"
#include "Display/Projection/Perspective.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Components/Manager.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/MPL/SameType.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include <stdexcept>

using namespace Display::View;

USUL_IMPLEMENT_TYPE_ID ( Canvas );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Canvas, Canvas::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs for this file.
//
///////////////////////////////////////////////////////////////////////////////

namespace Display
{
  namespace View
  {
    typedef Usul::Interfaces::SceneGraph::ITransformGroup ITransformGroup;
    typedef Usul::Interfaces::SceneGraph::IClippedGroup IClippedGroup;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Canvas::Canvas ( IUnknown::RefPtr doc ) : BaseClass(),
  _flags ( 0 ),
  _renderers(),
  _scene(),
  _models(),
  _clipped(),
  _unclipped(),
  _decoration(),
  _document ( doc ),
  _projection ( doc )
{
  USUL_TRACE_SCOPE;

  if ( false == _projection.valid() )
  {
    typedef Display::Projection::Perspective Perspective;
    Perspective::RefPtr perspective ( new Perspective );
    _projection = perspective;
  }

  typedef Usul::Interfaces::SceneGraph::IFactory IFactory;
  IFactory::QueryPtr factory ( Usul::Components::Manager::instance().getInterface ( IFactory::IID ) );
  if ( false == factory.valid() )
    throw std::runtime_error ( "Error 2034597026: No scene-graph factory found" );

  _scene  = factory->createObject ( IGroup::IID );
  _models = factory->createObject ( ITransformGroup::IID );
  _clipped = factory->createObject ( IClippedGroup::IID );
  _unclipped = factory->createObject ( IGroup::IID );
  _decoration = factory->createObject ( IGroup::IID );

  _scene->appendChild ( _models.get() );
  _scene->appendChild ( _decoration.get() );
  _models->appendChild ( _clipped.get() );
  _models->appendChild ( _unclipped.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Canvas::~Canvas()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Canvas::_destroy ), "4278674688" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::_destroy()
{
  USUL_TRACE_SCOPE;
  this->clear();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Clear the internal state. Call this when you're done with it.
//
/////////////////////////////////////////////////////////////////////////////

void Canvas::clear()
{
  USUL_TRACE_SCOPE;
  WriteLock lock ( this );

  // Clear the renderers.
  while ( false == _renderers.empty() )
    _renderers.pop();

  // Clear these members.
  _scene = IGroup::RefPtr ( 0x0 );
  _models = IGroup::RefPtr ( 0x0 );
  _clipped = IGroup::RefPtr ( 0x0 );
  _unclipped = IGroup::RefPtr ( 0x0 );
  _decoration = IGroup::RefPtr ( 0x0 );
  _document = IDocument::RefPtr ( 0x0 );
  _projection = IProjectionMatrix::RefPtr ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown * Canvas::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;

  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IRedraw::IID:
    return static_cast < Usul::Interfaces::IRedraw* > ( this );
  case Usul::Interfaces::IView::IID:
    return static_cast < Usul::Interfaces::IView* > ( this );
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the flags.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Canvas::flags() const
{
  USUL_TRACE_SCOPE;
  ReadLock lock ( this );
  return _flags;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we rendering?
//
///////////////////////////////////////////////////////////////////////////////

bool Canvas::isRendering() const
{
  USUL_TRACE_SCOPE;
  return ( true == Usul::Bits::has ( this->flags(), Canvas::RENDERING ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a model.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::modelAdd ( IUnknown::RefPtr model, bool clipped )
{
  USUL_TRACE_SCOPE;

  if ( true == model.valid() )
  {
    WriteLock lock ( this );
    if ( true == clipped )
    {
      _clipped->appendChild ( model );
    }
    else
    {
      _unclipped->appendChild ( model );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Push the new renderer.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::pushRenderer ( IUnknown::RefPtr renderer )
{
  USUL_TRACE_SCOPE;

  if ( true == renderer.valid() )
  {
    WriteLock lock ( this );
    _renderers.push ( renderer );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pop the top renderer.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::popRenderer()
{
  USUL_TRACE_SCOPE;
  WriteLock lock ( this );

  if ( false == _renderers.empty() )
  {
    _renderers.pop();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the renderer.
//
///////////////////////////////////////////////////////////////////////////////

const Usul::Interfaces::IUnknown::RefPtr Canvas::renderer() const
{
  USUL_TRACE_SCOPE;
  ReadLock lock ( this );
  return ( ( false == _renderers.empty() ) ? _renderers.top() : IUnknown::RefPtr ( 0x0 ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the renderer.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown::RefPtr Canvas::renderer()
{
  USUL_TRACE_SCOPE;
  ReadLock lock ( this );
  return ( ( false == _renderers.empty() ) ? _renderers.top() : IUnknown::RefPtr ( 0x0 ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Render the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::render()
{
  USUL_TRACE_SCOPE;

  // Don't re-enter.
  if ( true == this->isRendering() )
    return;

  // Handle no scene.
  Usul::Interfaces::IUnknown::RefPtr scene ( this->_getScene() );
  if ( false == scene.valid() )
    return;

  // Always set and reset this state.
  Usul::Scope::Caller::RefPtr resetFlag ( Usul::Scope::makeCaller ( 
    Usul::Adaptors::bind2 ( Canvas::RENDERING, true,  Usul::Adaptors::memberFunction ( this, &Canvas::_setFlag ) ),
    Usul::Adaptors::bind2 ( Canvas::RENDERING, false, Usul::Adaptors::memberFunction ( this, &Canvas::_setFlag ) ) ) );

  // Get the projection.
  Usul::Interfaces::IUnknown::RefPtr proj ( this->projection() );

  // Get the renderer.
  Usul::Interfaces::IUnknown::RefPtr top ( this->renderer() );

  // Pre-render.
  Usul::Interfaces::SceneGraph::IPreRender::QueryPtr pre ( top );
  if ( true == pre.valid() )
  {
    pre->preRender ( proj, scene );
  }

  // Render.
  Usul::Interfaces::SceneGraph::IRender::QueryPtr renderer ( top );
  if ( true == renderer.valid() )
  {
    renderer->render ( proj, scene );
  }

  // Post-render.
  Usul::Interfaces::SceneGraph::IPostRender::QueryPtr post ( top );
  if ( true == post.valid() )
  {
    post->postRender ( proj, scene );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flag.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::_setFlag ( unsigned int bit, bool state )
{
  USUL_TRACE_SCOPE;
  WriteLock lock ( this );
  _flags = Usul::Bits::set ( _flags, bit, state );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the document.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::document ( Usul::Interfaces::IUnknown::RefPtr unknown )
{
  USUL_TRACE_SCOPE;
  WriteLock lock ( this );
  _document = unknown;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the document.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IDocument *Canvas::document()
{
  USUL_TRACE_SCOPE;
  ReadLock lock ( this );
  return _document.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the document.
//
///////////////////////////////////////////////////////////////////////////////

const Usul::Interfaces::IDocument *Canvas::document() const
{
  USUL_TRACE_SCOPE;
  ReadLock lock ( this );
  return _document.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Call this when the window has resized.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::resize ( double width, double height )
{
  USUL_TRACE_SCOPE;
  typedef Usul::Interfaces::SceneGraph::IViewportSet IViewportSet;

  IViewportSet::QueryPtr v ( this->projection() );
  if ( true == v.valid() )
  {
    v->setViewport ( width, height );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the matrix that will view all the models.
//
///////////////////////////////////////////////////////////////////////////////

Canvas::Matrix Canvas::navigationViewAll ( double zScale ) const
{
  USUL_TRACE_SCOPE;
  ReadLock lock ( this );

  typedef Usul::Interfaces::SceneGraph::IViewAllMatrix IViewAllMatrix;
  IViewAllMatrix::QueryPtr viewAll ( _models.get() );
  return ( ( true == viewAll.valid() ) ? viewAll->getViewAllMatrix ( zScale ) : Matrix::getIdentity() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the navigation matrix.
//
///////////////////////////////////////////////////////////////////////////////

Canvas::Matrix Canvas::navigationMatrix() const
{
  USUL_TRACE_SCOPE;
  ReadLock lock ( this );

  typedef Usul::Interfaces::SceneGraph::ITransformMatrix ITransformMatrix;
  ITransformMatrix::QueryPtr transform ( _models.get() );
  return ( ( true == transform.valid() ) ? transform->getTransformMatrix() : Matrix::getIdentity() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the navigation matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::navigationMatrix ( const Canvas::Matrix &m )
{
  USUL_TRACE_SCOPE;
  WriteLock lock ( this );

  typedef Usul::Interfaces::SceneGraph::ITransformMatrix ITransformMatrix;
  ITransformMatrix::QueryPtr transform ( _models.get() );
  if ( true == transform.valid() )
  {
    transform->setTransformMatrix ( m );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Redraw the view now.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::redraw()
{
  USUL_TRACE_SCOPE;
  this->render();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Hide/show the statistics.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::setStatsDisplay ( bool )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the projection.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown::RefPtr Canvas::projection()
{
  USUL_TRACE_SCOPE;
  ReadLock lock ( this );
  return IUnknown::QueryPtr ( _projection );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the projection.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::projection ( Usul::Interfaces::IUnknown::RefPtr p )
{
  USUL_TRACE_SCOPE;
  WriteLock lock ( this );
  _projection = p;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown::RefPtr Canvas::_getScene() const
{
  USUL_TRACE_SCOPE;
  ReadLock lock ( this );
  return IUnknown::RefPtr ( IUnknown::QueryPtr ( _scene.get() ) );
}
