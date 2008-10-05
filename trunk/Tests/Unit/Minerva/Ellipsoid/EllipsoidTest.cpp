
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
#include "Usul/Print/Matrix.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Scope/RemoveFile.h"

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
  const std::string name ( Usul::File::Temp::file() );
  Usul::Scope::RemoveFile remove ( name );
  
  // Serialize.
  Serialize::XML::serialize ( *land, name );
  
  XmlTree::XercesLife life;
  XmlTree::Document::ValidRefPtr document ( new XmlTree::Document );
  document->load ( name );

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


///////////////////////////////////////////////////////////////////////////////
//
//  Test conversion to xyz from lat,lon, elevation.
//
///////////////////////////////////////////////////////////////////////////////

TEST(EllipsoidTest,ToMatrix)
{
  // Make the land model.
  typedef Minerva::Core::TileEngine::LandModelEllipsoid Land;
  Land::Vec2d radii ( osg::WGS_84_RADIUS_EQUATOR, osg::WGS_84_RADIUS_POLAR );
  Land::RefPtr land ( new Land ( radii ) );
  
  osg::Matrixd m ( land->planetRotationMatrix ( -33.0, 112.0, 350.0, 45.0 ) );

  //Usul::Print::matrix ( "", m.ptr(), std::cout, 30, 20 );
  
  ASSERT_DOUBLE_EQ ( -0.51135026580072939772, m.ptr()[0] );   
  ASSERT_DOUBLE_EQ ( -0.79988571614098480378, m.ptr()[4] );
  ASSERT_DOUBLE_EQ ( -0.31417152445622348322, m.ptr()[8] );
  ASSERT_DOUBLE_EQ ( -2005931.51948434300720691681, m.ptr()[12] );
  ASSERT_DOUBLE_EQ ( -0.62196201242244164575, m.ptr()[1] );
  ASSERT_DOUBLE_EQ ( 0.09218828745927509782, m.ptr()[5] );
  ASSERT_DOUBLE_EQ ( 0.77760180989935512308, m.ptr()[9] );
  ASSERT_DOUBLE_EQ ( 4964854.73272907081991434097, m.ptr()[13] );
  ASSERT_DOUBLE_EQ ( -0.59302964577578243244, m.ptr()[2] );
  ASSERT_DOUBLE_EQ ( 0.59302964577578254346, m.ptr()[6] );
  ASSERT_DOUBLE_EQ ( -0.54463903501502719529, m.ptr()[10] );
  ASSERT_DOUBLE_EQ ( -3454149.26479830499738454819, m.ptr()[14] );
  ASSERT_DOUBLE_EQ ( 0.0000000000, m.ptr()[3] );
  ASSERT_DOUBLE_EQ ( 0.0000000000, m.ptr()[7] );
  ASSERT_DOUBLE_EQ ( 0.0000000000, m.ptr()[11] );
  ASSERT_DOUBLE_EQ ( 1.0000000000, m.ptr()[15] );
}
