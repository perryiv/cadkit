
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
#include "Usul/Predicates/CloseFloat.h"
#include "Usul/Predicates/Tolerance.h"
#include "Usul/Print/Matrix.h"
#include "Usul/Print/Vector.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Scope/RemoveFile.h"

#include "Serialize/XML/Serialize.h"
#include "Serialize/XML/Deserialize.h"

#include "osg/CoordinateSystemNode"

#include "gtest/gtest.h"

#include <iomanip>


struct TestVec3d
{
  bool operator () ( const osg::Vec3d& lhs, const osg::Vec3d& rhs ) const
  {
    Usul::Predicates::Tolerance<double> pred ( 0.0000001 );
    return pred ( lhs[0], rhs[0] ) &&
           pred ( lhs[1], rhs[1] ) &&
           pred ( lhs[2], rhs[2] );
    /*return Usul::Predicates::CloseFloat<double>::compare ( lhs[0], rhs[0], 20 ) &&
           Usul::Predicates::CloseFloat<double>::compare ( lhs[1], rhs[1], 20 ) &&
           Usul::Predicates::CloseFloat<double>::compare ( lhs[2], rhs[2], 20 );*/
  }
};

inline std::ostream& operator<< ( std::ostream& os, const osg::Vec3d& point )
{
  const unsigned int w ( 25 ), p ( 15 );
  Usul::Print::vector ( "", point[0], point[1], point[2], os, w, p );
  return os;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a test fixture to hold the land model.
//
///////////////////////////////////////////////////////////////////////////////

class EllipsoidTest : public testing::Test
{
protected:
  virtual void SetUp()
  {
    Land::Vec2d radii ( osg::WGS_84_RADIUS_EQUATOR, osg::WGS_84_RADIUS_POLAR );
    _land = new Land ( radii );
  }

  typedef Minerva::Core::TileEngine::LandModelEllipsoid Land;
  Land::RefPtr _land;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Test ellipsoid serialization.
//
///////////////////////////////////////////////////////////////////////////////

TEST_F(EllipsoidTest,Serialize)
{ 
  // Make a temp file.
  const std::string name ( Usul::File::Temp::file() );
  Usul::Scope::RemoveFile remove ( name );
  
  // Serialize.
  Serialize::XML::serialize ( *_land, name );
  
  XmlTree::XercesLife life;
  XmlTree::Document::ValidRefPtr document ( new XmlTree::Document );
  document->load ( name );

  Land::RefPtr land1 ( new Land );
  land1->deserialize ( *document );
  
  // Should be equal.
  ASSERT_EQ ( _land->radiusEquator(), land1->radiusEquator() );
  ASSERT_EQ ( _land->radiusPolar(),   land1->radiusPolar() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convienence macro for ToXYZ test.
//
///////////////////////////////////////////////////////////////////////////////

#define TEST_TO_XYZ(lat,lon,elevation,x,y,z) {\
  osg::Vec3d point;\
  _land->latLonHeightToXYZ ( lat, lon, elevation, point[0], point[1], point[2] );\
  const osg::Vec3d result ( x, y, z );\
  ASSERT_PRED2 ( TestVec3d(), result, point );\
  }\


///////////////////////////////////////////////////////////////////////////////
//
//  Test conversion to xyz from lat,lon, elevation.
//
///////////////////////////////////////////////////////////////////////////////

TEST_F(EllipsoidTest,ToXYZ)
{
  // Test one value from each half hemisphere of the ellipsoid.
  TEST_TO_XYZ (  10.0,  45.0,   0.0,  4441954.876365073000000,  4441954.876365072100000,  1100248.547719956100000 );
  TEST_TO_XYZ ( -33.0, 112.0, 350.0, -2005931.5194843430,       4964854.7327290708,      -3454149.2647983050      );
  TEST_TO_XYZ ( -10.0, -45.0,   0.0,  4441954.876365073000000, -4441954.876365072100000, -1100248.547719956100000 );
  TEST_TO_XYZ (  10.0, -45.0,   0.0,  4441954.876365073000000, -4441954.876365072100000,  1100248.547719956100000 );

  // Test boundary conditions.
  TEST_TO_XYZ (   0.0,    0.0, 0.0,  6378137.000000000000000,  0.000000000000000,        0.000000000000000 );
  TEST_TO_XYZ (  90.0,    0.0, 0.0,        0.000000000391862,  0.000000000000000,  6356752.314199999900000 );
  TEST_TO_XYZ (  90.0,  180.0, 0.0,       -0.000000000391862,  0.000000000000000,  6356752.314199999900000 );
  TEST_TO_XYZ (   0.0,  180.0, 0.0, -6378137.000000000000000,  0.000000000781097,        0.000000000000000 );
  TEST_TO_XYZ ( -90.0,    0.0, 0.0,        0.000000000391862,  0.000000000000000, -6356752.314199999900000 );
  TEST_TO_XYZ (   0.0, -180.0, 0.0, -6378137.000000000000000, -0.000000000781097,        0.000000000000000 );
  TEST_TO_XYZ ( -90.0, -180.0, 0.0,       -0.000000000391862, -0.000000000000000, -6356752.314199999900000 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test conversion to lat,lon, elevation from xyz.
//
///////////////////////////////////////////////////////////////////////////////

TEST_F(EllipsoidTest,ToLatLonElevation)
{
  // Convert
  const osg::Vec3d point ( -2005931.5194843430, 4964854.7327290708, -3454149.2647983050 );
  double lat ( 0.0 ), lon ( 0.0 ), elevation ( 0.0 );
  _land->xyzToLatLonHeight ( point[0], point[1], point[2], lat, lon, elevation );

  const osg::Vec3d answer ( lat, lon, elevation );
  const osg::Vec3d result ( -33.0, 112.0, 350.0 );

  ASSERT_PRED2 ( TestVec3d(), result, answer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test conversion to xyz from lat,lon, elevation.
//
///////////////////////////////////////////////////////////////////////////////

TEST_F(EllipsoidTest,ToMatrix)
{
  typedef Minerva::Core::TileEngine::LandModel::Matrix Matrix;
  Matrix m ( _land->planetRotationMatrix ( -33.0, 112.0, 350.0, 45.0 ) );

  //Usul::Print::matrix ( "", m.ptr(), std::cout, 30, 20 );
  
  ASSERT_DOUBLE_EQ ( -0.51135026580072939772, m[0] );   
  ASSERT_DOUBLE_EQ ( -0.79988571614098480378, m[4] );
  ASSERT_DOUBLE_EQ ( -0.31417152445622348322, m[8] );
  ASSERT_DOUBLE_EQ ( -2005931.51948434300720691681, m[12] );
  ASSERT_DOUBLE_EQ ( -0.62196201242244164575, m[1] );
  ASSERT_DOUBLE_EQ ( 0.09218828745927509782, m[5] );
  ASSERT_DOUBLE_EQ ( 0.77760180989935512308, m[9] );
  ASSERT_DOUBLE_EQ ( 4964854.73272907081991434097, m[13] );
  ASSERT_DOUBLE_EQ ( -0.59302964577578243244, m[2] );
  ASSERT_DOUBLE_EQ ( 0.59302964577578254346, m[6] );
  ASSERT_DOUBLE_EQ ( -0.54463903501502719529, m[10] );
  ASSERT_DOUBLE_EQ ( -3454149.26479830499738454819, m[14] );
  ASSERT_DOUBLE_EQ ( 0.0000000000, m[3] );
  ASSERT_DOUBLE_EQ ( 0.0000000000, m[7] );
  ASSERT_DOUBLE_EQ ( 0.0000000000, m[11] );
  ASSERT_DOUBLE_EQ ( 1.0000000000, m[15] );
}
