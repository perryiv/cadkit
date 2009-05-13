
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/Kml/Factory.h"

#include "XmlTree/Node.h"

#include "gtest/gtest.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Helper macro.
//
///////////////////////////////////////////////////////////////////////////////

#define TEST_COORDINATES(value)\
  {\
    XmlTree::Node::RefPtr node ( new XmlTree::Node ( name, value ) );\
    Vertices vertices;\
    Extents extents;\
    Factory::instance().parseCoordinates ( *node, vertices, extents );\
    EXPECT_TRUE ( 3 == vertices.size() );\
  }\

///////////////////////////////////////////////////////////////////////////////
//
//  Test the parsing of coordinats.
//
///////////////////////////////////////////////////////////////////////////////

TEST(KmlParseTest,Coordinates)
{
  typedef Minerva::Layers::Kml::Factory Factory;
  typedef Factory::Vertices Vertices;
  typedef Factory::Extents Extents;

  const std::string name ( "coordinates" );
  const std::string value0 ( "-111.891974719611,33.6110085690716,1.82926829268293 -111.893111790335,33.6110126582742,1.82926829268293 -111.893111790335,33.6110126582742,0" );
  const std::string value1 ( "-111.891974719611,33.6110085690716 -111.893111790335,33.6110126582742 -111.893111790335,33.6110126582742" );
  const std::string value2 ( "-111.891974719611,33.6110085690716,1.82926829268293\n -111.893111790335,33.6110126582742,1.82926829268293\n -111.893111790335,33.6110126582742,0" );
  const std::string value3 ( "-111.891974719611 , 33.6110085690716 , 1.82926829268293\n -111.893111790335 , 33.6110126582742 , 1.82926829268293\n -111.893111790335 , 33.6110126582742 , 0" );

  TEST_COORDINATES ( value0 );
  TEST_COORDINATES ( value1 );
  TEST_COORDINATES ( value2 );
  TEST_COORDINATES ( value3 );
}
