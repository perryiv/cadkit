
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

#include "OsgTools/Convert/Matrix.h"
#include "OsgTools/Group.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

using namespace Display::View;

USUL_IMPLEMENT_TYPE_ID ( Canvas );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Canvas, Canvas::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Canvas::Canvas ( IUnknown::RefPtr doc ) : BaseClass(),
  _flags ( 0 ),
  _renderers(),
  _scene ( new osg::Group ),
  _models ( new osg::MatrixTransform ),
  _clipped ( new osg::ClipNode ),
  _unclipped ( new osg::Group ),
  _decoration ( new osg::Group ),
  _document ( doc ),
  _viewer ( new osgViewer::Viewer )
{
  _scene->addChild ( _models.get() );
  _scene->addChild ( _decoration.get() );
  _models->addChild ( _clipped.get() );
  _models->addChild ( _unclipped.get() );
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
  // Do not lock up here because the mutex is not recursive and 
  // popRenderer() locks it for writing.

  // Call popRenderer() because it sets the renderer's scene to null.
  while ( true == this->renderer().valid() )
    this->popRenderer();

  // Local scope not necessary but good practice.
  {
    WriteLock lock ( this );
    _scene = 0x0;
    _models = 0x0;
    _clipped = 0x0;
    _unclipped = 0x0;
    _decoration = 0x0;
    _document = Usul::Interfaces::IUnknown::RefPtr ( 0x0 );
    _viewer = 0x0;
  }
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
  case Usul::Interfaces::IViewMatrix::IID:
    return static_cast < Usul::Interfaces::IViewMatrix* > ( this );
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

void Canvas::modelAdd ( NodePtr model, bool clipped )
{
  USUL_TRACE_SCOPE;

  if ( true == model.valid() )
  {
    WriteLock lock ( this );
    if ( true == clipped )
    {
      _clipped->addChild ( model.get() );
    }
    else
    {
      _unclipped->addChild ( model.get() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Push the new renderer.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::pushRenderer ( RendererPtr r )
{
  USUL_TRACE_SCOPE;

  if ( true == r.valid() )
  {
    WriteLock lock ( this );
    _renderers.push ( r );
    r->scene ( _scene );
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

  if ( true == _renderers.empty() )
    return;

  RendererPtr r ( _renderers.top() );
  _renderers.pop();

  if ( true == r.valid() )
    r->scene ( 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the renderer.
//
///////////////////////////////////////////////////////////////////////////////

const Canvas::RendererPtr Canvas::renderer() const
{
  USUL_TRACE_SCOPE;
  ReadLock lock ( this );
  return ( ( false == _renderers.empty() ) ? _renderers.top() : RendererPtr ( 0x0 ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the renderer.
//
///////////////////////////////////////////////////////////////////////////////

Canvas::RendererPtr Canvas::renderer()
{
  USUL_TRACE_SCOPE;
  ReadLock lock ( this );
  return ( ( false == _renderers.empty() ) ? _renderers.top() : RendererPtr ( 0x0 ) );
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

  // Always set and reset this state.
  Usul::Scope::Caller::RefPtr resetFlag ( Usul::Scope::makeCaller ( 
    Usul::Adaptors::bind2 ( Canvas::RENDERING, true,  Usul::Adaptors::memberFunction ( this, &Canvas::_setFlag ) ),
    Usul::Adaptors::bind2 ( Canvas::RENDERING, false, Usul::Adaptors::memberFunction ( this, &Canvas::_setFlag ) ) ) );

  // Handle no renderer.
  RendererPtr r ( this->renderer() );
  if ( false == r.valid() )
    return;

  // Render the scene.
  r->render();
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
//  Call this when you want the viewport to resize.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::resize ( unsigned int width, unsigned int height )
{
  USUL_TRACE_SCOPE;
  RendererPtr renderer ( this->renderer() );
  if ( true == renderer.valid() )
    renderer->resize ( width, height );
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

  // Get the bounding sphere of the group.
  osg::BoundingSphere bs ( _models->getBound() );
  osg::Vec3d c ( bs.center() );

  // Push it back so we can see it.
  osg::Matrixd matrix;
  osg::Matrixd::value_type z ( zScale * bs.radius() + c[2] );
  matrix.makeTranslate ( -c[0], -c[1], -z );

  // Post-multiply the current matrix.
  return ( Usul::Convert::Type<osg::Matrixd,Canvas::Matrix>::convert ( _models->getMatrix() * matrix ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the navigation matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::navigationMatrix ( const Matrix &m )
{
  USUL_TRACE_SCOPE;
  this->setViewMatrix ( Usul::Convert::Type<Canvas::Matrix,osg::Matrixd>::convert ( m ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the navigation matrix.
//
///////////////////////////////////////////////////////////////////////////////

Canvas::Matrix Canvas::navigationMatrix() const
{
  USUL_TRACE_SCOPE;
  return ( Usul::Convert::Type<osg::Matrixd,Canvas::Matrix>::convert ( this->getViewMatrix() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the view matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Canvas::setViewMatrix ( const osg::Matrixd &m )
{
  USUL_TRACE_SCOPE;
  WriteLock lock ( this );
  _models->setMatrix ( m );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the view matrix.
//
///////////////////////////////////////////////////////////////////////////////

osg::Matrixd Canvas::getViewMatrix() const
{
  USUL_TRACE_SCOPE;
  ReadLock lock ( this );
  return _models->getMatrix();
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
