
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Extents.h"
#include "Minerva/Core/Algorithms/Split.h"

#include "Usul/Math/Vector2.h"
#include "Usul/Predicates/Tolerance.h"
#include "Usul/Print/Vector.h"

#include "gtest/gtest.h"

typedef Minerva::Core::Extents<Usul::Math::Vec2d> Extents;

struct TestExtents
{
  bool operator () ( const Extents& lhs, const Extents& rhs ) const
  {
    Usul::Predicates::Tolerance<double> pred ( 0.0000001 );
    return pred ( lhs[0], rhs[0] ) &&
           pred ( lhs[1], rhs[1] ) &&
           pred ( lhs[2], rhs[2] ) &&
           pred ( lhs[3], rhs[3] );
  }
};

inline std::ostream& operator<< ( std::ostream& os, const Extents& extents )
{
  const unsigned int w ( 25 ), p ( 15 );
  Usul::Print::vector ( "", extents[0], extents[1], extents[2], extents[3], os, w, p );
  return os;
}

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


///////////////////////////////////////////////////////////////////////////////
//
//  Test split of extents.
//
///////////////////////////////////////////////////////////////////////////////

TEST(ExtentsTest,Split)
{
  const Extents extents0 ( 5, 5, 6, 6 );
  Extents ll, lr, ul, ur;
  extents0.split ( ll, lr, ul, ur );

  ASSERT_PRED2 ( TestExtents(), Extents ( 5, 5, 5.5, 5.5 ), ll );
  ASSERT_PRED2 ( TestExtents(), Extents ( 5.5, 5, 6, 5.5 ), lr );
  ASSERT_PRED2 ( TestExtents(), Extents ( 5, 5.5, 5.5, 6 ), ul );
  ASSERT_PRED2 ( TestExtents(), Extents ( 5.5, 5.5, 6, 6 ), ur );
}
