
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/TileEngine/LandModelEllipsoid.h"

#include "Usul/File/Temp.h"
#include "Usul/Threads/Mutex.h"

#include "Serialize/XML/Serialize.h"
#include "Serialize/XML/Deserialize.h"

#include "osg/CoordinateSystemNode"

#include "gtest/gtest.h"

#include <iomanip>

inline std::ostream& operator<< ( std::ostream& os, const osg::Vec3d& point )
{
  const unsigned int w ( 10 ), p ( 10 );
  os << std::setw ( w ) << std::setprecision ( p ) << std::fixed << point[0] << " ";
  os << std::setw ( w ) << std::setprecision ( p ) << std::fixed << point[1] << " ";
  os << std::setw ( w ) << std::setprecision ( p ) << std::fixed << point[2];
  return os;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test ellipsoid serialization.
//
///////////////////////////////////////////////////////////////////////////////

TEST(EllipsoidTest,Serialize)
{
  // Make the land model.
  typedef Minerva::Core::TileEngine::LandModelEllipsoid Land;
  Land::Vec2d radii ( osg::WGS_84_RADIUS_EQUATOR, osg::WGS_84_RADIUS_POLAR );
  Land::RefPtr land ( new Land ( radii ) );
  
  // Make a temp file.
  Usul::File::Temp temp;
  
  // Serialize.
  Serialize::XML::serialize ( *land, temp.name() );
  
  XmlTree::XercesLife life;
  XmlTree::Document::ValidRefPtr document ( new XmlTree::Document );
  document->load ( temp.name() );

  Land::RefPtr land1 ( new Land );
  land1->deserialize ( *document );
  
  // Should be equal.
  ASSERT_EQ ( land->radiusEquator(), land1->radiusEquator() );
  ASSERT_EQ ( land->radiusPolar(),  land1->radiusPolar() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test conversion to xyz from lat,lon, elevation.
//
///////////////////////////////////////////////////////////////////////////////

TEST(EllipsoidTest,ToXYZ)
{
  // Make the land model.
  typedef Minerva::Core::TileEngine::LandModelEllipsoid Land;
  Land::Vec2d radii ( osg::WGS_84_RADIUS_EQUATOR, osg::WGS_84_RADIUS_POLAR );
  Land::RefPtr land ( new Land ( radii ) );
  
  osg::Vec3d point;
  land->latLonHeightToXYZ ( -33.0, 112.0, 350.0, point );
  
  const osg::Vec3d result ( -2005931.5194843430, 4964854.7327290708, -3454149.2647983050 );
  
  ASSERT_DOUBLE_EQ ( result[0], point[0] );
  ASSERT_DOUBLE_EQ ( result[1], point[1] );
  ASSERT_DOUBLE_EQ ( result[2], point[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test conversion to lat,lon, elevation from xyz.
//
///////////////////////////////////////////////////////////////////////////////

TEST(EllipsoidTest,ToLatLonElevation)
{
  // Make the land model.
  typedef Minerva::Core::TileEngine::LandModelEllipsoid Land;
  Land::Vec2d radii ( osg::WGS_84_RADIUS_EQUATOR, osg::WGS_84_RADIUS_POLAR );
  Land::RefPtr land ( new Land ( radii ) );
  
  const osg::Vec3d point ( -2005931.5194843430, 4964854.7327290708, -3454149.2647983050 );
  double lat ( 0.0 ), lon ( 0.0 ), elevation ( 0.0 );
  land->xyzToLatLonHeight ( point, lat, lon, elevation );
  
  const osg::Vec3d answer ( lat, lon, elevation );
  const osg::Vec3d result ( -33.0, 112.0, 350.0 );
  
  ASSERT_DOUBLE_EQ ( result[0], answer[0] );
  ASSERT_DOUBLE_EQ ( result[1], answer[1] );
  ASSERT_DOUBLE_EQ ( result[2], answer[2] );
}
