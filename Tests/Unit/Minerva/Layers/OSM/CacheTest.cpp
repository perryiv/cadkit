
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/OSM/Cache.h"

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
    extents = Extents ( -180.0, -90.0, 180.0, 90.0 );

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

TEST_F(OSMCacheTest,Test01)
{
  EXPECT_FALSE ( cache->hasNodeData ( key, extents ) );
  EXPECT_FALSE ( cache->hasWayData ( key, extents ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make sure node data can be retrieved after saving.
//
///////////////////////////////////////////////////////////////////////////////

TEST_F(OSMCacheTest,Test02)
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
//  Make sure way data can be retrieved after saving.
//
///////////////////////////////////////////////////////////////////////////////

TEST_F(OSMCacheTest,Test03)
{
  cache->addWayData ( key, extents, ways );

  EXPECT_TRUE ( cache->hasNodeData ( key, extents ) );
  EXPECT_TRUE ( cache->hasWayData ( key, extents ) );

  Ways cachedWays;
  cache->getWayData ( key, extents, cachedWays );

  EXPECT_TRUE ( ways.size() == cachedWays.size() );
  EXPECT_TRUE ( cachedWays[0]->tags().size() == tags.size() );
}
