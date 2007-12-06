
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "StarSystem/LandModelEllipsoid.h"

#include "Usul/Trace/Trace.h"

#include "ossim/base/ossimEllipsoid.h"

using namespace StarSystem;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LandModelEllipsoid::LandModelEllipsoid ( const Vec2d &r ) : 
  BaseClass(),
  _ellipsoid ( new ossimEllipsoid ( r[RADIUS_EQUATOR], r[RADIUS_POLAR] ) )
{
  // Set ellipsoid radii.
  _ellipsoid->setAB ( r[RADIUS_EQUATOR], r[RADIUS_POLAR] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

LandModelEllipsoid::~LandModelEllipsoid ()
{
  delete _ellipsoid; _ellipsoid = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the size.
//
///////////////////////////////////////////////////////////////////////////////

double LandModelEllipsoid::size () const
{
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

  return _ellipsoid->geodeticRadius ( lat );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert lat, lon, height to x,y,z.
//
///////////////////////////////////////////////////////////////////////////////

void LandModelEllipsoid::latLonHeightToXYZ ( double lat, double lon, double elevation, osg::Vec3f& point ) const
{
  USUL_TRACE_SCOPE;

  typedef osg::Vec3f::value_type ValueType;

  double x ( 0 ), y ( 0 ), z ( 0 );
  _ellipsoid->latLonHeightToXYZ ( lat, lon, elevation, x, y, z );
  point.set ( static_cast<ValueType> ( x ), static_cast<ValueType> ( y ), static_cast<ValueType> ( z ) );
}

