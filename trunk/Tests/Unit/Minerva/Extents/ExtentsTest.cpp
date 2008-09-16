
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Extents.h"

#include "Usul/Math/Vector2.h"

#include "gtest/gtest.h"

typedef Minerva::Core::Extents<Usul::Math::Vec2d> Extents;


///////////////////////////////////////////////////////////////////////////////
//
//  Test a point inside the extents.
//
///////////////////////////////////////////////////////////////////////////////

TEST(ExtentsTest,PointInside)
{
  const Extents e ( 0, 0, 10, 10 );
  const Extents::Vertex p ( 5, 5 );

  // Should be true.
  EXPECT_TRUE ( e.contains ( p ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test a point outside the extents.
//
///////////////////////////////////////////////////////////////////////////////

TEST(ExtentsTest,PointOutside)
{
  const Extents e ( 0, 0, 10, 10 );
  const Extents::Vertex p ( 15, 15 );
  
  // Should be false.
  EXPECT_FALSE ( e.contains ( p ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test a point outside on the extents border.
//
///////////////////////////////////////////////////////////////////////////////

TEST(ExtentsTest,PointOn)
{
  const Extents e ( 0, 0, 10, 10 );
  const Extents::Vertex p ( 0, 5 );
  
  // Should be true.
  EXPECT_TRUE ( e.contains ( p ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test intersection of two extents.
//
///////////////////////////////////////////////////////////////////////////////

TEST(ExtentsTest,Intersects)
{
  const Extents e0 ( 0, 0, 10, 10 );
  const Extents e1 ( 5, 5, 15, 15 );
  const Extents e2 ( 15, 15, 20, 20 );
  
  // Should be true.
  EXPECT_TRUE ( e0.intersects ( e1 ) );
  EXPECT_TRUE ( e1.intersects ( e0 ) );
  
  // Should be false.
  EXPECT_FALSE ( e0.intersects ( e2 ) );
  EXPECT_FALSE ( e2.intersects ( e0 ) );
  
  // Should be true.
  EXPECT_TRUE ( e1.intersects ( e2 ) );
  EXPECT_TRUE ( e2.intersects ( e1 ) );
}
