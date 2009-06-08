
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/OSM/Cache.h"
#include "Minerva/Layers/OSM/LineString.h"

#include "Minerva/Core/Algorithms/Split.h"

#include "Usul/File/Remove.h"

#include "gtest/gtest.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Constants.
//
///////////////////////////////////////////////////////////////////////////////

const std::string DATABASE_FILE_NAME ( "osm_cache_test.db" );


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef CadKit::Database::SQLite::Connection Connection;
typedef Minerva::Layers::OSM::Cache Cache;
typedef Minerva::Layers::OSM::Extents Extents;
typedef Minerva::Layers::OSM::Object::Tags Tags;
typedef Minerva::Layers::OSM::Nodes Nodes;
typedef Minerva::Layers::OSM::Node Node;
typedef Minerva::Layers::OSM::Ways Ways;
typedef Minerva::Layers::OSM::Way Way;
typedef Node::Location Location;
typedef Minerva::Layers::OSM::LineString LineString;


///////////////////////////////////////////////////////////////////////////////
//
//  Make a test fixture to hold the cache.
//
///////////////////////////////////////////////////////////////////////////////

class OSMCacheTest : public testing::Test
{
protected:
  virtual void SetUp()
  {
    Usul::File::remove ( DATABASE_FILE_NAME );

    connection = new Connection ( DATABASE_FILE_NAME );
    cache = new Cache ( connection );

    key = "test_key";
    extents = Extents ( -10.0, -10.0, 10.0, 10.0 );

    tags.insert ( std::make_pair ( "key0", "value0" ) );
    tags.insert ( std::make_pair ( "key1", "value1" ) );

    nodes.push_back ( Node::create ( 0, Location ( -1.0, -1.0 ), boost::posix_time::second_clock::universal_time(), tags ) );
    nodes.push_back ( Node::create ( 1, Location ( 1.0, 1.0 ), boost::posix_time::second_clock::universal_time(), tags ) );

    ways.push_back ( Way::create ( 2, boost::posix_time::second_clock::universal_time(), tags, nodes ) );
  }

  virtual void TearDown()
  {
    cache = 0x0;
    connection = 0x0;

    Usul::File::remove ( DATABASE_FILE_NAME );
  }

  Connection::RefPtr connection;
  Cache::RefPtr cache;

  std::string key;
  Extents extents;

  Tags tags;
  Nodes nodes;
  Ways ways;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Cache should be empty.
//
///////////////////////////////////////////////////////////////////////////////

TEST_F(OSMCacheTest,TestEmptyCache)
{
  EXPECT_FALSE ( cache->hasNodeData ( key, extents ) );
  EXPECT_FALSE ( cache->hasLineData ( key, extents ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make sure node data can be retrieved after saving.
//
///////////////////////////////////////////////////////////////////////////////

TEST_F(OSMCacheTest,TestNodes)
{
  cache->addNodeData ( key, extents, nodes );

  EXPECT_TRUE ( cache->hasNodeData ( key, extents ) );

  Nodes cachedNodes;
  cache->getNodeData ( key, extents, cachedNodes );

  EXPECT_TRUE ( nodes.size() == cachedNodes.size() );
  EXPECT_TRUE ( cachedNodes[0]->tags().size() == tags.size() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make sure line string data can be retrieved after saving.
//
///////////////////////////////////////////////////////////////////////////////

TEST_F(OSMCacheTest,LineString)
{
  std::vector<LineString::RefPtr> lines;
  lines.push_back ( LineString::create ( ways[0] ) );

  cache->addLineData ( key, extents, lines );

  EXPECT_TRUE ( cache->hasLineData ( key, extents ) );

  Extents ll, lr, ul, ur;
  Minerva::Core::Algorithms::split ( extents, ll, lr, ul, ur );

  EXPECT_TRUE ( cache->hasLineData ( key, ll ) );
  EXPECT_TRUE ( cache->hasLineData ( key, lr ) );
  EXPECT_TRUE ( cache->hasLineData ( key, ul ) );
  EXPECT_TRUE ( cache->hasLineData ( key, ur ) );

  Extents right ( 10.0, -10.0, 30.0, 10.0 );
  EXPECT_FALSE ( cache->hasLineData ( key, right ) );

  Extents left ( -30.0, -10.0, -10.0, 10.0 );
  EXPECT_FALSE ( cache->hasLineData ( key, left ) );

  Extents bottom ( -10.0, -30.0, 30.0, -10.0 );
  EXPECT_FALSE ( cache->hasLineData ( key, bottom ) );

  Extents top ( -10.0, 10.0, 30.0, 30.0 );
  EXPECT_FALSE ( cache->hasLineData ( key, top ) );

  std::vector<LineString::RefPtr> cachedLines;
  cache->getLineData ( key, extents, cachedLines );

  EXPECT_TRUE ( lines.size() == cachedLines.size() );
}
