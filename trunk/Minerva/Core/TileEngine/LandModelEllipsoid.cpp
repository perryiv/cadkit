
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/TileEngine/LandModelEllipsoid.h"

#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Math/Absolute.h"
#include "Usul/Math/Constants.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Trace/Trace.h"

#include "OsgTools/Convert.h"

#include "osg/CoordinateSystemNode"
#include "osg/Matrixd"

using namespace Minerva::Core::TileEngine;

USUL_FACTORY_REGISTER_CREATOR ( LandModelEllipsoid );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LandModelEllipsoid::LandModelEllipsoid ( const Vec2d &r ) : 
  BaseClass(),
  _ellipsoid ( new osg::EllipsoidModel ( r[RADIUS_EQUATOR], r[RADIUS_POLAR] ) )
{
  USUL_TRACE_SCOPE;

  // Set ellipsoid radii.
  this->_setRadii ( r[RADIUS_EQUATOR], r[RADIUS_POLAR] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

LandModelEllipsoid::~LandModelEllipsoid()
{
  USUL_TRACE_SCOPE;
  delete _ellipsoid; _ellipsoid = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert lat,lon,height to x,y,z.
//
///////////////////////////////////////////////////////////////////////////////

void LandModelEllipsoid::latLonHeightToXYZ ( double lat, double lon, double elevation, double& x, double& y, double& z ) const
{
  USUL_TRACE_SCOPE;

  lat *= Usul::Math::DEG_TO_RAD;
  lon *= Usul::Math::DEG_TO_RAD;

  Guard guard ( this );
  _ellipsoid->convertLatLongHeightToXYZ ( lat, lon, elevation, x, y, z );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert x,y,z to lat, lon, height.
//
///////////////////////////////////////////////////////////////////////////////

void LandModelEllipsoid::xyzToLatLonHeight ( double x, double y, double z, double& lat, double& lon, double& elevation ) const
{
  USUL_TRACE_SCOPE;

  {
    Guard guard ( this );
    _ellipsoid->convertXYZToLatLongHeight ( x, y, z, lat, lon, elevation );
  }

  lat *= Usul::Math::RAD_TO_DEG;
  lon *= Usul::Math::RAD_TO_DEG;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the mesh size for the extents.
//
///////////////////////////////////////////////////////////////////////////////

LandModel::MeshSize LandModelEllipsoid::meshSize ( const LandModel::Extents &extents, const LandModel::MeshSize &ms )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Get latitude as integer values.
  const long maxLat ( static_cast < long > ( extents.maxLat() ) );
  const long minLat ( static_cast < long > ( extents.minLat() ) );

  // If both ends are touching poles then return the default mesh.
  if ( ( 90 == maxLat ) && ( -90 == minLat ) )
    return ms;

  // Get the extreme latitude.
  const long extremeLat ( Usul::Math::maximum ( Usul::Math::absolute ( maxLat ), Usul::Math::absolute ( minLat ) ) );

  // If we're not touching the pole then return the default mesh.
  if ( 90 != extremeLat )
    return ms;

  // The cutoff latitude.
  const long cutoff ( 80 );

  // If we're not above "cutoff" degrees (or below -cutoff) then return default mesh.
  const bool north ( ( maxLat >  cutoff ) && ( minLat >  cutoff ) );
  const bool south ( ( maxLat < -cutoff ) && ( minLat < -cutoff ) );
  if ( ( false == north ) && ( false == south ) )
    return ms;

  // If we get to here then set the number of columns.
  return MeshSize ( 3, ms[0] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Serialize the members.
//
///////////////////////////////////////////////////////////////////////////////

void LandModelEllipsoid::serialize ( XmlTree::Node &parent ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Copy the map.
  Serialize::XML::DataMemberMap m ( _dataMemberMap );

  // Add additional entries.
  Vec2d r ( this->radiusEquator(), this->radiusPolar() );
  m.addMember ( "radii", r );

  // Write members to the node from local map.
  m.serialize ( parent );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize the members.
//
///////////////////////////////////////////////////////////////////////////////

void LandModelEllipsoid::deserialize ( const XmlTree::Node &node )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Copy the map.
  Serialize::XML::DataMemberMap m ( _dataMemberMap );

  // Add additional entries.
  Vec2d r ( this->radiusEquator(), this->radiusPolar() );
  m.addMember ( "radii", r );

  // Initialize locals and members from the the node.
  m.deserialize ( node );

  // Set members from locals.
  this->_setRadii ( r[RADIUS_EQUATOR], r[RADIUS_POLAR] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper trig functions.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template <class T>
  inline T sin ( T in )
  {
    return Usul::Math::sin ( in * Usul::Math::DEG_TO_RAD );
  }

  template <class T>
  inline T cos ( T in )
  {
    return Usul::Math::cos ( in * Usul::Math::DEG_TO_RAD );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Matrix to place items on the planet (i.e. local coordinates to world coordinates).
//  This is adapted from lsrSpace class in ossim.
//
///////////////////////////////////////////////////////////////////////////////

LandModelEllipsoid::Matrix LandModelEllipsoid::planetRotationMatrix ( double lat, double lon, double elevation, double heading ) const
{
  USUL_TRACE_SCOPE;

  const double sin_lat ( Helper::sin ( lat ) );
  const double cos_lat ( Helper::cos ( lat ) );
  const double sin_lon ( Helper::sin ( lon ) );
  const double cos_lon ( Helper::cos ( lon ) );
  
  // East-North-Up components.
  Usul::Math::Vec3d E ( -sin_lon,  cos_lon, 0.0 );
  Usul::Math::Vec3d N ( -sin_lat * cos_lon,  -sin_lat * sin_lon,  cos_lat );
  Usul::Math::Vec3d U ( E.cross ( N ) );

  const double cos_azim ( Helper::cos ( heading ) );
  const double sin_azim ( Helper::sin ( heading ) );
  Usul::Math::Vec3d X ( cos_azim * E - sin_azim * N );
  Usul::Math::Vec3d Y ( sin_azim * E + cos_azim * N );
  Usul::Math::Vec3d Z ( X.cross ( Y ) );

  // Get the position in earth-centered, earth-fixed (ECEF).
  osg::Vec3d p;
  this->latLonHeightToXYZ ( lat, lon, elevation, p.x(), p.y(), p.z() );

  // Create a rotation matrix from the local space at (lat,lon) to ECEF system.
  Usul::Math::Matrix44d m
      ( X[0], Y[0], Z[0], p[0],
        X[1], Y[1], Z[1], p[1],
        X[2], Y[2], Z[2], p[2],
        0.00, 0.00, 0.00, 1.00 );

  return m;
  /*osg::Matrixd result;
  OsgTools::Convert::matrix ( m, result );
  return result;*/
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the equator radius.
//
///////////////////////////////////////////////////////////////////////////////

double LandModelEllipsoid::radiusEquator() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _ellipsoid->getRadiusEquator();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the polar radius.
//
///////////////////////////////////////////////////////////////////////////////

double LandModelEllipsoid::radiusPolar() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _ellipsoid->getRadiusPolar();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the radii.
//
///////////////////////////////////////////////////////////////////////////////

void LandModelEllipsoid::_setRadii ( double equator, double polar )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _ellipsoid->setRadiusEquator ( equator );
  _ellipsoid->setRadiusPolar   ( polar   );
}
