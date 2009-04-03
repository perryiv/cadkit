
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Math/Barycentric.h"
#include "Usul/Predicates/CloseFloat.h"
#include "Usul/Print/Vector.h"

#include "gtest/gtest.h"


struct TestVec3d
{
  bool operator () ( const Usul::Math::Vec3d& lhs, const Usul::Math::Vec3d& rhs ) const
  {
    return Usul::Predicates::CloseFloat<double>::compare ( lhs[0], rhs[0], 3 ) &&
         Usul::Predicates::CloseFloat<double>::compare ( lhs[1], rhs[1], 3 ) &&
         Usul::Predicates::CloseFloat<double>::compare ( lhs[2], rhs[2], 3 );
  }
};

inline std::ostream& operator<< ( std::ostream& os, const Usul::Math::Vec3d& v )
{
  const unsigned int w ( 15 ), p ( 10 );
  Usul::Print::vector ( "", v[0], v[1], v[2], os, w, p );
  return os;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Parameterize an extents within another extents.
//
///////////////////////////////////////////////////////////////////////////////

TEST(Barycentric,Test)
{
  // Triangle points to use.
  const Usul::Math::Vec3d t0 ( 0, 0, 0 ), t1 ( 1, 0, 0 ), t2 ( 0, 1, 0 );

  // Points to test.
  const Usul::Math::Vec3d pc ( 1.0/3.0, 1.0/3.0, 1.0/3.0 ); // Should be barycenter.
  const Usul::Math::Vec3d p0 ( 0.0,     0.0,     0.0     );
  const Usul::Math::Vec3d p1 ( 1.0,     0.0,     0.0     );
  const Usul::Math::Vec3d p2 ( 0.0,     1.0,     0.0     );

  // Answers to expect.
  const Usul::Math::Vec3d bc ( 1.0/3.0, 1.0/3.0, 1.0/3.0 );
  const Usul::Math::Vec3d b0 ( 1.0,     0.0,     0.0     );
  const Usul::Math::Vec3d b1 ( 0.0,     1.0,     0.0     );
  const Usul::Math::Vec3d b2 ( 0.0,     0.0,     1.0     );

  // Conduct the tests.
  ASSERT_PRED2 ( TestVec3d(), bc, Usul::Math::barycentric ( t0, t1, t2, pc ) );
  ASSERT_PRED2 ( TestVec3d(), b0, Usul::Math::barycentric ( t0, t1, t2, p0 ) );
  ASSERT_PRED2 ( TestVec3d(), b1, Usul::Math::barycentric ( t0, t1, t2, p1 ) );
  ASSERT_PRED2 ( TestVec3d(), b2, Usul::Math::barycentric ( t0, t1, t2, p2 ) );
}
