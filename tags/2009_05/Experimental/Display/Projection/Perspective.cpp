
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base projection class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Display/Projection/Perspective.h"

#include "Usul/Trace/Trace.h"

using namespace Display::Projection;

USUL_IMPLEMENT_TYPE_ID ( Perspective );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Perspective, Perspective::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Perspective::Perspective() : BaseClass(),
  _fov ( 45 ),
  _width ( 1 ),
  _height ( 1 ),
  _zNear ( 1 ),
  _zFar ( 10000 ),
  _matrix ( Matrix::getIdentity() ),
  _dirty ( true )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Perspective::~Perspective()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown * Perspective::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;

  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::SceneGraph::IPerspectiveProjection::IID:
    return static_cast < Usul::Interfaces::SceneGraph::IPerspectiveProjection* > ( this );
  case Usul::Interfaces::SceneGraph::IProjectionMatrix::IID:
    return static_cast < Usul::Interfaces::SceneGraph::IProjectionMatrix* > ( this );
  case Usul::Interfaces::SceneGraph::IViewportGet::IID:
    return static_cast < Usul::Interfaces::SceneGraph::IViewportGet* > ( this );
  case Usul::Interfaces::SceneGraph::IViewportSet::IID:
    return static_cast < Usul::Interfaces::SceneGraph::IViewportSet* > ( this );
  default:
    return 0x0;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get the projection matrix.
//
/////////////////////////////////////////////////////////////////////////////

Perspective::Matrix Perspective::getProjectionMatrix() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( true == _dirty )
  {
    Perspective *me ( const_cast < Perspective * > ( this ) );
    if ( 0x0 != me )
    {
      me->_updateMatrix();
    }
  }

  return _matrix;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get the perspective parameters.
//
/////////////////////////////////////////////////////////////////////////////

void Perspective::getPerspectiveProjection ( double &fov, double &aspect, double &zNear, double &zFar ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  fov = _fov;
  aspect = _width / _height;
  zNear = _zNear;
  zFar = _zFar;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the perspective parameters.
//
/////////////////////////////////////////////////////////////////////////////

void Perspective::setPerspectiveProjection ( double fov, double aspect, double zNear, double zFar )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  _fov = fov;
  _height = _width / aspect;
  _zNear = zNear;
  _zFar = zFar;
  _dirty = true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Update the matrix if it is dirty.
//
/////////////////////////////////////////////////////////////////////////////

void Perspective::_updateMatrix()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( true == _dirty )
  {
    const double aspect ( _width / _height );
    _matrix.perspective ( _fov, aspect, _zNear, _zFar );
    _dirty = false;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the viewport.
//
/////////////////////////////////////////////////////////////////////////////

void Perspective::setViewport ( double width, double height )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( width > 0 )
  {
    _width = width;
    _dirty = true;
  }

  if ( height > 0 )
  {
    _height = height;
    _dirty = true;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get the viewport.
//
/////////////////////////////////////////////////////////////////////////////

void Perspective::getViewport ( double &width, double &height ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  width = _width;
  height = _height;
}
