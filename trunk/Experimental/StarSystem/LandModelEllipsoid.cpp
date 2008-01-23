
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "StarSystem/LandModelEllipsoid.h"

#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Math/Absolute.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Trace/Trace.h"

#include "ossim/base/ossimEllipsoid.h"
#include "ossim/base/ossimGpt.h"
#include "ossim/base/ossimLsrSpace.h"
#include "ossim/base/ossimMatrix4x4.h"

#include "osg/Matrixd"

using namespace StarSystem;

USUL_FACTORY_REGISTER_CREATOR ( LandModelEllipsoid );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LandModelEllipsoid::LandModelEllipsoid ( const Vec2d &r ) : 
  BaseClass(),
  _ellipsoid ( new ossimEllipsoid ( r[RADIUS_EQUATOR], r[RADIUS_POLAR] ) )
{
  USUL_TRACE_SCOPE;

  // Set ellipsoid radii.
  _ellipsoid->setAB ( r[RADIUS_EQUATOR], r[RADIUS_POLAR] );
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
//  Get the size.
//
///////////////////////////////////////////////////////////////////////////////

double LandModelEllipsoid::size() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _ellipsoid->a();
}
  

///////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation at given lat, lon.
//
///////////////////////////////////////////////////////////////////////////////

double LandModelEllipsoid::elevation ( double lat, double lon ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _ellipsoid->geodeticRadius ( lat );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert lat,lon,height to x,y,z.
//
///////////////////////////////////////////////////////////////////////////////

void LandModelEllipsoid::latLonHeightToXYZ ( double lat, double lon, double elevation, osg::Vec3f& point ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  typedef osg::Vec3f::value_type ValueType;

  double x ( 0 ), y ( 0 ), z ( 0 );
  _ellipsoid->latLonHeightToXYZ ( lat, lon, elevation, x, y, z );
  point.set ( static_cast<ValueType> ( x ), static_cast<ValueType> ( y ), static_cast<ValueType> ( z ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert x,y,z to lat, lon, height.
//
///////////////////////////////////////////////////////////////////////////////

void LandModelEllipsoid::xyzToLatLonHeight ( const osg::Vec3f& point, double& lat, double& lon, double& elevation ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  _ellipsoid->XYZToLatLonHeight ( point.x(), point.y(), point.z(), lat, lon, elevation );
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
  Vec2d r ( _ellipsoid->a(), _ellipsoid->b() );
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
  Vec2d r ( _ellipsoid->a(), _ellipsoid->b() );
  m.addMember ( "radii", r );

  // Initialize locals and members from the the node.
  m.deserialize ( node );

  // Set members from locals.
  _ellipsoid->setAB ( r[RADIUS_EQUATOR], r[RADIUS_POLAR] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Matrix to place items on the planet (i.e. local coordinates to world coordinates).
//
///////////////////////////////////////////////////////////////////////////////

osg::Matrixd LandModelEllipsoid::planetRotationMatrix ( double lat, double lon, double elevation, double heading ) const
{
  osg::Vec3f p;
  this->latLonHeightToXYZ ( lat, lon, elevation, p );
  ossimLsrSpace lsrSpace (ossimGpt( lat, lon, elevation ), heading );
  
  ossimMatrix4x4 lsrMatrix( lsrSpace.lsrToEcefRotMatrix() );
  NEWMAT::Matrix m ( lsrMatrix.getData() );

  return osg::Matrixd( m[0][0], m[1][0], m[2][0], 0.0,
                       m[0][1], m[1][1], m[2][1], 0.0,
                       m[0][2], m[1][2], m[2][2], 0.0,
                       p[0], p[1], p[2], 1.0);
}
