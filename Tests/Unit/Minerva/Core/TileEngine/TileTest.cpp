
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#if defined ( _MSC_VER ) && _MSC_VER >= 1400
#pragma warning ( disable : 4996 )
#endif

#include "Minerva/Core/ElevationData.h"
#include "Minerva/Core/TileEngine/Body.h"
#include "Minerva/Core/TileEngine/Tile.h"
#include "Minerva/Core/Functions/MakeBody.h"

#include "Usul/Predicates/Tolerance.h"

#include "gtest/gtest.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Make a test fixture to hold the tile.
//
///////////////////////////////////////////////////////////////////////////////

class TileTest : public testing::Test
{
protected:
  virtual void SetUp()
  {
    // Make a body.
    _body = Minerva::Core::Functions::makeEarth ( 0x0 );

    // Mesh size.
    const unsigned int rows ( 17 );
    const unsigned int columns ( 17 );

    // Extents.
    const double minLon ( -45.0 );
    const double minLat ( -22.5 );
    const double maxLon ( 45.0 );
    const double maxLat ( 22.5 ) ;

    // Level.
    const unsigned int level ( 0 );

    // Make the extents.
    _extents = Extents ( minLon, minLat, maxLon, maxLat );

    // Mesh size.
    Tile::MeshSize meshSize ( rows, columns );

    // Image size.
    Tile::ImageSize imageSize ( 256, 256 );

    // Make the elevation with a constant value.
    std::vector<float> data ( meshSize[0] * meshSize[1], 100.0 );
    Minerva::Core::ElevationData::RefPtr elevation ( new Minerva::Core::ElevationData ( meshSize[0], meshSize[1], data ) );

    // Make a tile.
    _tile = new Tile ( 0x0, Tile::NONE, level, _extents, meshSize, imageSize, 1.0, _body.get(), 0x0, elevation.get() );

    // Make the mesh.
    _tile->updateMesh();
  }

  typedef Minerva::Core::TileEngine::Body Body;
  typedef Minerva::Core::TileEngine::Tile Tile;
  typedef Tile::Extents Extents;

  Body::RefPtr _body;
  Tile::RefPtr _tile;
  Extents _extents;
};


struct TestCloseDouble
{
  bool operator () ( double lhs, double rhs ) const
  {
    Usul::Predicates::Tolerance<double> pred ( 0.001 );
    return pred ( lhs, rhs );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Test elevation.
//
///////////////////////////////////////////////////////////////////////////////

TEST_F(TileTest,Elevation)
{
  Extents extents ( _extents );
  Extents::Vertex mn ( _extents.minimum() );
  Extents::Vertex mx ( _extents.maximum() );

  // Number of steps to test
  const unsigned int steps ( 5 );

  for ( unsigned int i = 0; i < steps; ++i )
  {
    const double u ( static_cast<double> ( i ) / ( steps - 1 ) );

    for ( unsigned int j = 0; j < steps; ++j )
    {
      const double v ( static_cast<double> ( j ) / ( steps - 1 ) );
      
      const double lon ( mn[0] + u * ( mx[0] - mn[0] ) );
      const double lat ( mn[1] + v * ( mx[1] - mn[1] ) );

      const double elevation ( _tile->elevation ( lat, lon ) );

      //std::cout << "Lat: " << lat << " Long: " << lon << std::endl;
      //ASSERT_DOUBLE_EQ ( elevation, 100.0 );
      ASSERT_PRED2 ( TestCloseDouble(), elevation, 100.0 );
    }
  }
}
