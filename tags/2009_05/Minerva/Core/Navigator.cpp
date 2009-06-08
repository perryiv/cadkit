
//////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Navigator.h"

#include "OsgTools/Convert.h"

#include "Usul/Errors/Assert.h"
#include "Usul/Math/Constants.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Math/Functions.h"
#include "Usul/Math/Angle.h"
#include "Usul/Math/Quaternion.h"

#include "osg/CoordinateSystemNode"
#include "osgUtil/IntersectVisitor"

using namespace Minerva::Core;

//////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Navigator::Navigator ( Body::RefPtr body, Usul::Interfaces::IProjectionMatrix::RefPtr projection ) : BaseClass(),
  _camera ( new Minerva::Core::Data::Camera ),
  _elevationSpeed ( 1e7 ),
  _pitchSpeed ( 0.5 ),
  _yawSpeed ( 20 ),
  _body ( body ),
  _projectionMatrix ( projection )
{
  this->home();
}


//////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Navigator::~Navigator()
{
  _camera = 0x0;
  _body = 0x0;
  _projectionMatrix = 0x0;
}


//////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation given the current position.
//
///////////////////////////////////////////////////////////////////////////////

double Navigator::_elevation() const
{
  return _camera->altitude();
}


//////////////////////////////////////////////////////////////////////////////
//
//  Set internal values by using the view matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Navigator::viewMatrix ( const osg::Matrixd& m )
{
  if ( _body.valid() )
  {
    // Get the eye position.
    Usul::Math::Vec3d eye ( m ( 3, 0 ), m ( 3, 1 ), m ( 3, 2 ) );

    Usul::Math::Vec3d point;
    _body->convertFromPlanet ( eye, point );

    _camera->longitude ( point[0] );
    _camera->latitude ( point[1] );
    _camera->altitude ( point[2] );

    Usul::Math::Matrix44d matrix;
    OsgTools::Convert::matrix ( m, matrix );
    Usul::Math::Quaterniond quat ( matrix );

    Usul::Math::Vec3d hpr ( quat.toEuler() );
    _camera->heading ( hpr[0] );
    _camera->tilt ( hpr[1] );
  }
}


//////////////////////////////////////////////////////////////////////////////
//
//  Return the view matrix.
//
///////////////////////////////////////////////////////////////////////////////

osg::Matrixd Navigator::viewMatrix() const
{
  osg::Matrixd result;
  OsgTools::Convert::matrix ( _camera->viewMatrix ( _body.valid() ? _body->landModel() : 0x0 ), result );
  return result;
}


//////////////////////////////////////////////////////////////////////////////
//
//  Go to the home position.
//
///////////////////////////////////////////////////////////////////////////////

void Navigator::home()
{
  _camera->latitude ( 0.0 );
  _camera->longitude ( -90.0 );
  _camera->altitude ( 2.0 * osg::WGS_84_RADIUS_EQUATOR );

  _camera->heading ( 0.0 );
  _camera->tilt ( 0.0 );
  _camera->roll ( 0.0 );
}


//////////////////////////////////////////////////////////////////////////////
//
//  Wrap the value if it's outside the range.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < typename T > T wrap ( T x, T a, T b )
  {
    if( a == b )
      return a;

    T z = x < a ? b : a;
    return ::fmod ( x - z, b - a ) + z;
  }
}


//////////////////////////////////////////////////////////////////////////////
//
//  Rotate.
//
///////////////////////////////////////////////////////////////////////////////

void Navigator::rotate ( double dx, double dy )
{
  double lat ( _camera->latitude() );
  double lon ( _camera->longitude() );

  lon = Detail::wrap ( lon +  dx, -180.0, 180.0 );
  lat += dy;

  // Handle conditions at the poles.
  if( lat > 90.0 )
  {
    lat = Usul::Math::clamp ( 90.0 - ( lat - 90.0 ), -90.0, 90.0 );
    lon = Detail::wrap ( lon + 180.0, -180.0, 180.0 );

    _camera->heading ( Detail::wrap ( _camera->heading() + 180.0, -180.0, 180.0 ) );
  }
  if ( lat < -90.0 )
  {
    lat = Usul::Math::clamp ( -90.0 + ( lat + 90.0 ), -90.0, 90.0 );
    lon = Detail::wrap ( lon + 180.0, -180.0, 180.0 );

    _camera->heading ( Detail::wrap ( _camera->heading() + 180.0, -180.0, 180.0 ) );
  }

  _camera->latitude ( lat );
  _camera->longitude ( lon );
}


//////////////////////////////////////////////////////////////////////////////
//
//  Rotate.
//
///////////////////////////////////////////////////////////////////////////////

void Navigator::rotate ( const MousePosition& point0, const MousePosition& point1 )
{
  Usul::Math::Vec3d pt0, pt1;
  if ( this->_intersectionPoint ( Usul::Math::Vec2d ( point0[0], point0[1] ), pt0 ) && 
       this->_intersectionPoint ( Usul::Math::Vec2d ( point1[0], point1[1] ), pt1 ) )
  {
    if ( _body.valid() )
    {
      Usul::Math::Vec3d llh0, llh1;
      _body->convertFromPlanet ( pt0, llh0 );
      _body->convertFromPlanet ( pt1, llh1 );

      const double dx ( llh1[0] - llh0[0] );
      const double dy ( llh1[1] - llh0[1] );
      this->rotate ( dx, dy );
    }
  }
}


//////////////////////////////////////////////////////////////////////////////
//
//  Change elevation.
//
///////////////////////////////////////////////////////////////////////////////

void Navigator::elevation ( double delta )
{
  const double dZoom ( this->_calculateLinearSpeed ( _camera->altitude() ) );
  _camera->altitude ( _camera->altitude() + ( delta * dZoom ) );
}


//////////////////////////////////////////////////////////////////////////////
//
//  Change elevation.
//
///////////////////////////////////////////////////////////////////////////////

void Navigator::elevation ( const MousePosition& point0, const MousePosition& point1 )
{
  const double dy ( point1[1] - point0[1] );
  this->elevation ( dy );
}


//////////////////////////////////////////////////////////////////////////////
//
//  Zoom along the line of sight.
//
///////////////////////////////////////////////////////////////////////////////

void Navigator::zoomLOS ( const MousePosition& point0, const MousePosition& point1 )
{
  const double dy ( point1[1] - point0[1] );

  Usul::Math::Vec3d pt0 ( this->_convertMouseToWorldSpace ( Usul::Math::Vec3d ( 0.0, 0.0, -1.0 ) ) );
  Usul::Math::Vec3d pt1 ( this->_convertMouseToWorldSpace ( Usul::Math::Vec3d ( 0.0, 0.0, 1.0 ) ) );

  Usul::Math::Vec3d direction ( pt0 - pt1 );
  direction.normalize();

  this->zoomAlongDirection ( direction, dy );
}


//////////////////////////////////////////////////////////////////////////////
//
//  Zoom along given direction.
//
///////////////////////////////////////////////////////////////////////////////

void Navigator::zoomAlongDirection ( const Usul::Math::Vec3d& direction, double delta )
{
  osg::Matrixd matrix ( this->viewMatrix() );
  Usul::Math::Vec3d eye ( matrix ( 3, 0 ), matrix ( 3, 1 ), matrix ( 3, 2 ) );

  const double dZoom ( this->_calculateLinearSpeed ( _camera->altitude() ) );
  const double amount ( dZoom * delta );

  // Move along the line-of-sight ray.
  Usul::Math::Vec3d newPositionXYZ ( eye + ( direction * amount ) );

  Usul::Math::Vec3d newPositionLlh;
  if ( _body.valid() )
    _body->convertFromPlanet ( newPositionXYZ, newPositionLlh );

  _camera->longitude ( newPositionLlh[0] );
  _camera->latitude ( newPositionLlh[1] );
  _camera->altitude ( newPositionLlh[2] );
}


//////////////////////////////////////////////////////////////////////////////
//
//  Change the look direction.
//
///////////////////////////////////////////////////////////////////////////////

void Navigator::look ( double dx, double dy )
{
  const double pitchSpeed ( this->pitchSpeed() );
  const double yawSpeed ( this->yawSpeed() );

  const double heading ( Detail::wrap < float > ( _camera->heading() - dx * yawSpeed, -180.0f, 180.0f ) );
  _camera->heading ( heading );
 
  const double pitch ( Usul::Math::clamp ( _camera->tilt() - dy * pitchSpeed, 0.0, osg::DegreesToRadians ( 90.0 ) ) );
  _camera->tilt ( pitch );
}


//////////////////////////////////////////////////////////////////////////////
//
//  Change the look direction.
//
///////////////////////////////////////////////////////////////////////////////

void Navigator::look ( const MousePosition& point0, const MousePosition& point1 )
{
  const double dx ( point1[0] - point0[0] );
  const double dy ( point1[1] - point0[1] );

  this->look ( dx, dy );
}


//////////////////////////////////////////////////////////////////////////////
//
//  Calculate where the line of sight intersects the planet.
//
///////////////////////////////////////////////////////////////////////////////

bool Navigator::_losIntersectionPoint ( Usul::Math::Vec3d& point )
{
  osg::Matrixd m ( this->viewMatrix() );
  Usul::Math::Vec3d eye ( m (3,0),m(3,1),m(3,2) );      
  Usul::Math::Vec3d center ( 0.0, 0.0, 0.0 );
  return this->_intersectionPoint ( eye, center, point );
}


//////////////////////////////////////////////////////////////////////////////
//
//  Calculate where the line segment intersects the planet.
//
///////////////////////////////////////////////////////////////////////////////

bool Navigator::_intersectionPoint ( const Usul::Math::Vec3d& pt0, const Usul::Math::Vec3d& pt1, Usul::Math::Vec3d& point )
{
  if( _body.valid() )
  {
    return _body->intersectWithTiles ( pt0, pt1, point );
  }

  return false;
}


//////////////////////////////////////////////////////////////////////////////
//
//  Calculate the speed given current parameters.
//
///////////////////////////////////////////////////////////////////////////////

double Navigator::_calculateLinearSpeed ( double altitude ) const
{
  const double defaultSpeed ( this->elevationSpeed() );

  const double param ( altitude / osg::WGS_84_RADIUS_EQUATOR );

  // Slow down as we get closer to the body.
  if ( param <= 1.0 )
  {
    // Make sure we always have a non-zero number.
    return Usul::Math::maximum ( defaultSpeed * param, 0.1 );
  }
  else
  {
    return defaultSpeed;
  }
}


//////////////////////////////////////////////////////////////////////////////
//
//  Calculate what the mouse coordinate is in world space.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec3d Navigator::_convertMouseToWorldSpace ( const Usul::Math::Vec3d& mouse ) const
{
  if ( _projectionMatrix.valid() )
  {
    // Get the matrix.
    osg::Matrixd M ( osg::Matrixd::inverse ( this->viewMatrix() ) );
    osg::Matrixd P ( _projectionMatrix->getProjectionMatrix() );
    osg::Matrixd MP ( M * P );
    osg::Matrixd IMP ( osg::Matrix::inverse ( MP ) );

    // Calculate the value.
    osg::Vec3d point ( osg::Vec3d ( mouse[0], mouse[1], mouse[2] ) * IMP );

    return Usul::Math::Vec3d ( point[0], point[1], point[2] );
  }

  return Usul::Math::Vec3d();
}


//////////////////////////////////////////////////////////////////////////////
//
//  Calculate the intersection point with the mouse.
//
///////////////////////////////////////////////////////////////////////////////

bool Navigator::_intersectionPoint ( const Usul::Math::Vec2d& mouse, Usul::Math::Vec3d& point )
{
  const double x ( mouse[0] );
  const double y ( mouse[1] );

  Usul::Math::Vec3d pt0 ( this->_convertMouseToWorldSpace ( Usul::Math::Vec3d ( x, y, -1.0 ) ) );
  Usul::Math::Vec3d pt1 ( this->_convertMouseToWorldSpace ( Usul::Math::Vec3d ( x, y, 1.0 ) ) );
  return this->_intersectionPoint ( pt0, pt1, point );
}


//////////////////////////////////////////////////////////////////////////////
//
//  Set the body.
//
///////////////////////////////////////////////////////////////////////////////

void Navigator::body ( Body::RefPtr body )
{
  _body = body;
}


//////////////////////////////////////////////////////////////////////////////
//
//  Set the projection matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Navigator::projectionMatrix ( Usul::Interfaces::IProjectionMatrix::RefPtr projection )
{
  _projectionMatrix = projection;
}


//////////////////////////////////////////////////////////////////////////////
//
//  Get a copy of the camera.
//
///////////////////////////////////////////////////////////////////////////////

Navigator::Camera* Navigator::copyCameraState() const
{
  Minerva::Core::Data::Camera::RefPtr camera ( new Minerva::Core::Data::Camera );
  camera->longitude ( _camera->longitude() );
  camera->latitude ( _camera->latitude() );
  camera->altitude ( _camera->altitude() );
  camera->heading ( _camera->heading() );
  camera->tilt ( _camera->tilt() );
  camera->roll ( _camera->roll() );

  return camera.release();
}
