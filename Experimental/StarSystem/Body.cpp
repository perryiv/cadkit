
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Body class.
//
///////////////////////////////////////////////////////////////////////////////

#include "StarSystem/Body.h"
#include "StarSystem/Tile.h"
#include "StarSystem/Visitor.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Trace/Trace.h"

#include "OsgTools/ShapeFactory.h"
#include "OsgTools/Group.h"

#include "ossim/base/ossimEllipsoid.h"

#include "osg/MatrixTransform"

#include <limits>

using namespace StarSystem;

STAR_SYSTEM_IMPLEMENT_NODE_CLASS ( Body );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Body::Body ( const Vec2d &r ) : BaseClass(),
  _transform ( new osg::MatrixTransform ),
  _ellipsoid ( new ossimEllipsoid ( r[Body::RADIUS_EQUATOR], r[Body::RADIUS_POLAR] ) )
{
  USUL_TRACE_SCOPE;

  // Not using smart pointers.
  _transform->ref();

  // Set ellipsoid radii.
  _ellipsoid->setAB ( r[Body::RADIUS_EQUATOR], r[Body::RADIUS_POLAR] );

  // Add a tile.
  Tile::ValidRefPtr tile 
    ( new Tile ( osg::Vec2d ( -180, -90 ), osg::Vec2d ( 180, 90 ), 10, 10, 
      this->radii()[Body::RADIUS_EQUATOR], this->radii()[Body::RADIUS_EQUATOR] * 7, _ellipsoid ) );
  _transform->addChild ( tile.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Body::~Body()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Body::_destroy ), "3973302267" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy
//
///////////////////////////////////////////////////////////////////////////////

void Body::_destroy()
{
  USUL_TRACE_SCOPE;
  Usul::Pointers::unreference ( _transform ); _transform = 0x0;
  delete _ellipsoid; _ellipsoid = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Node *Body::scene() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _transform;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *Body::scene()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _transform;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the center.
//
///////////////////////////////////////////////////////////////////////////////

void Body::center ( const Vec3d &c )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _transform->setMatrix ( osg::Matrix::translate ( c[0], c[1], c[2] ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the center.
//
///////////////////////////////////////////////////////////////////////////////

Body::Vec3d Body::center() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  const osg::Vec3d c ( _transform->getMatrix().getTrans() );
  return Vec3d ( c[0], c[1], c[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the maximum radius.
//
///////////////////////////////////////////////////////////////////////////////

double Body::maxRadius() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return ( ( 0x0 == _ellipsoid ) ? 1 : Usul::Math::maximum ( _ellipsoid->a(), _ellipsoid->b() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the radii.
//
///////////////////////////////////////////////////////////////////////////////

Body::Vec2d Body::radii() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return ( ( 0x0 == _ellipsoid ) ? Vec2d ( 1, 1 ) : Vec2d ( _ellipsoid->a(), _ellipsoid->b() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Body::_buildScene ( osgUtil::CullVisitor *cv )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  // Handle null visitor.
  if ( 0x0 == cv )
    return;

  // Remove all children in the group.
  //OsgTools::Group::removeAllChildren ( _transform );
}
