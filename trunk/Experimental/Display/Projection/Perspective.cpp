
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
  _aspect ( 1 ),
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
  case Usul::Interfaces::SceneGraph::IViewportResize::IID:
    return static_cast < Usul::Interfaces::SceneGraph::IViewportResize* > ( this );
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
  aspect = _aspect;
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
  _aspect = aspect;
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
    _matrix.perspective ( _fov, _aspect, _zNear, _zFar );
    _dirty = false;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Update the viewport.
//
/////////////////////////////////////////////////////////////////////////////

void Perspective::resizeViewport ( unsigned int width, unsigned int height )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( ( width > 0 ) && ( height > 0 ) )
  {
    _aspect = ( static_cast < double > ( width ) / static_cast < double > ( height ) );
    _dirty = true;
  }
}
