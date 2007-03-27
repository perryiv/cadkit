
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Example program demonstrating how to use the vector classes.
//
///////////////////////////////////////////////////////////////////////////////

#include "Standard/SlVec4.h"
#include "Standard/SlVec3.h"
#include "Standard/SlVec2.h"
#include "Standard/SlVec4IO.h"
#include "Standard/SlVec3IO.h"
#include "Standard/SlVec2IO.h"
#include <iostream>

using namespace CadKit;


template<class Vec> void testVector ( Vec &v1, Vec &v2 )
{
  typedef typename Vec::Type Type;

  // Print them to stdout using vector's "<<" operator.
  std::cout << "A01: v1 = " << v1 << std::endl;
  std::cout << "A02: v2 = " << v2 << std::endl;

  // Use compiler-generated copy constructor.
  Vec v3 ( v1 );
  std::cout << "A03: v3 = " << v3 << std::endl;

  // Test addition operator.
  Vec v4 = v1 + v2;
  std::cout << "A04: v4 = " << v4 << std::endl;

  // Test addition operator with copy constructor.
  Vec v5 ( v1 + v2 );
  std::cout << "A05: v5 = " << v5 << std::endl;

  // Use compiler-generated assignment operator.
  v5 = v1;
  std::cout << "A06: v5 = " << v5 << std::endl;

  // Absolute value.
  Vec v6 ( v1 ), v7 ( v2 );
  std::cout << "A07: v6 = " << v6 << std::endl;
  std::cout << "A08: v7 = " << v7 << std::endl;
  v6.absolute();
  v7.absolute();
  std::cout << "A09: after v6.absolute(), v6 = " << v6 << std::endl;
  std::cout << "A10: after v7.absolute(), v7 = " << v7 << std::endl;

  // Bisect.
  Vec v8;
  v8.bisect ( v1, v2 );
  std::cout << "A11: v1 = " << v1 << std::endl;
  std::cout << "A12: v2 = " << v2 << std::endl;
  std::cout << "A13: v8.bisect ( v1, v2 ), v8 = " << v8 << std::endl;

  // Dot product.
  std::cout << "A14: v1.dot ( v2 ) = " << v1.dot ( v2 ) << std::endl;

  // The distance between two vectors.
  std::cout << "A15: v1.getDistance ( v4 ) = " << v1.getDistance ( v4 ) << std::endl;

  // The squared distance between two vectors.
  std::cout << "A16: v1.getDistanceSquared ( v4 ) = " << v1.getDistanceSquared ( v4 ) << std::endl;

  // Length of a vectors.
  std::cout << "A17: v1.getLength() = " << v1.getLength() << std::endl;

  // The vector's internal state.
  std::cout << "A18: v1.getValue() = " << v1.getValue() << std::endl;

  // Interpolate.
  Vec v9;
  Type u = static_cast<Type>(0.25);
  v9.interpolate ( v1, v2, u );
  std::cout << "A19: v9.interpolate ( v1, v2, " << u << " ), v9 = " << v9 << std::endl;

  // Equality test.
  std::cout << "A20: v1.isEqual ( v2 ) = " << v1.isEqual ( v2 ) << std::endl;
  Type tol = static_cast<Type>(0.0001);
  std::cout << "A21: v1.isEqual ( v2, " << tol << " ) = " << v1.isEqual ( v2, tol ) << std::endl;

  // Inequality test.
  std::cout << "A22: v1.isNotEqual ( v2 ) = " << v1.isNotEqual ( v2 ) << std::endl;
  std::cout << "A23: v1.isNotEqual ( v2, " << tol << " ) = " << v1.isNotEqual ( v2, tol ) << std::endl;

  // Normalize, catch the old length.
  Vec v10 = v1;
  Type oldLength1 = v10.getLength();
  std::cout << "A24: before normalizing, v10 = " << v10 << ", length = " << oldLength1 << std::endl;
  Type oldLength2 = v10.normalize();
  SL_ASSERT ( oldLength1 == oldLength2 );
  Type newLength = v10.getLength();
  std::cout << "A25: after normalizing, v10 = " << v10 << ", length = " << newLength << std::endl;
  std::cout << "A26: 1 - length = " << static_cast<Type>(1) - newLength << std::endl;

  // Print the dimension.
  std::cout << "A27: Vec::getDimension() = " << Vec::getDimension() << std::endl;
}


template<class Vec> void testVector4 ( Vec &v1, Vec &v2 )
{
  typedef typename Vec::Type Type;

  // Print the vector's elements.
  Type x, y, z, w;
  v1.getValue ( x, y, z, w );
  std::cout << "B01: v1's (x,y,z,w) = " << x << " " << y << " " << z << " " << w << std::endl;
  v2.getValue ( x, y, z, w );
  std::cout << "B02: v2's (x,y,z,w) = " << x << " " << y << " " << z << " " << w << std::endl;

  // Bracket operator.
  std::cout << "B03: v1 = " << v1[0] << " " << v1[1] << " " << v1[2] << " " << v1[3] << std::endl;
}


template<class Vec> void testVector3 ( Vec &v1, Vec &v2 )
{
  typedef typename Vec::Type Type;

  // Print the vector's elements.
  Type x, y, z;
  v1.getValue ( x, y, z );
  std::cout << "C01: v1's (x,y,z) = " << x << " " << y << " " << z << std::endl;
  v2.getValue ( x, y, z );
  std::cout << "C02: v2's (x,y,z) = " << x << " " << y << " " << z << std::endl;

  // Bracket operator.
  std::cout << "C03: v1 = " << v1[0] << " " << v1[1] << " " << v1[2] << std::endl;
}


template<class Vec> void testVector2 ( Vec &v1, Vec &v2 )
{
  typedef typename Vec::Type Type;

  // Print the vector's elements.
  Type x, y;
  v1.getValue ( x, y );
  std::cout << "D01: v1's (x,y) = " << x << " " << y << std::endl;
  v2.getValue ( x, y );
  std::cout << "D02: v2's (x,y) = " << x << " " << y << std::endl;

  // Bracket operator.
  std::cout << "D03: v1 = " << v1[0] << " " << v1[1] << std::endl;
}


int main ( int argc, char **argv )
{
  // Declare some vectors.
  SlVec4ld ld4a (  1.0,  2.0,  3.0,  4.0 );
  SlVec4ld ld4b ( -5.0,  6.0,  7.0,  8.0 );
  SlVec4d   d4a (  1.0,  2.0,  3.0,  4.0 );
  SlVec4d   d4b ( -5.0,  6.0,  7.0,  8.0 );
  SlVec4f   f4a (  1.0f, 2.0f, 3.0f, 4.0f );
  SlVec4f   f4b ( -5.0f, 6.0f, 7.0f, 8.0f );
  SlVec4s   s4a (  1,    2,    3,    4 );
  SlVec4s   s4b ( -5,    6,    7,    8 );
  SlVec4i   i4a (  1,    2,    3,    4 );
  SlVec4i   i4b ( -5,    6,    7,    8 );
  SlVec4l   l4a (  1,    2,    3,    4 );
  SlVec4l   l4b ( -5,    6,    7,    8 );

  SlVec3ld ld3a (  1.0,  2.0,  3.0  );
  SlVec3ld ld3b ( -5.0,  6.0,  7.0  );
  SlVec3d   d3a (  1.0,  2.0,  3.0  );
  SlVec3d   d3b ( -5.0,  6.0,  7.0  );
  SlVec3f   f3a (  1.0f, 2.0f, 3.0f );
  SlVec3f   f3b ( -5.0f, 6.0f, 7.0f );
  SlVec3s   s3a (  1,    2,    3    );
  SlVec3s   s3b ( -5,    6,    7    );
  SlVec3i   i3a (  1,    2,    3    );
  SlVec3i   i3b ( -5,    6,    7    );
  SlVec3l   l3a (  1,    2,    3    );
  SlVec3l   l3b ( -5,    6,    7    );

  SlVec2ld ld2a (  1.0,  2.0  );
  SlVec2ld ld2b ( -5.0,  6.0  );
  SlVec2d   d2a (  1.0,  2.0  );
  SlVec2d   d2b ( -5.0,  6.0  );
  SlVec2f   f2a (  1.0f, 2.0f );
  SlVec2f   f2b ( -5.0f, 6.0f );
  SlVec2s   s2a (  1,    2    );
  SlVec2s   s2b ( -5,    6    );
  SlVec2i   i2a (  1,    2    );
  SlVec2i   i2b ( -5,    6    );
  SlVec2l   l2a (  1,    2    );
  SlVec2l   l2b ( -5,    6    );

  // Test them.
  std::cout << "---- Testing SlVec4ld" << std::endl; ::testVector ( ld4a, ld4b ); ::testVector4 ( ld4a, ld4b );
  std::cout << "---- Testing SlVec4d"  << std::endl; ::testVector (  d4a,  d4b ); ::testVector4 (  d4a,  d4b );
  std::cout << "---- Testing SlVec4f"  << std::endl; ::testVector (  f4a,  f4b ); ::testVector4 (  f4a,  f4b );
  std::cout << "---- Testing SlVec4l"  << std::endl; ::testVector (  l4a,  l4b ); ::testVector4 (  l4a,  l4b );
  std::cout << "---- Testing SlVec4i"  << std::endl; ::testVector (  i4a,  i4b ); ::testVector4 (  i4a,  i4b );
  std::cout << "---- Testing SlVec4s"  << std::endl; ::testVector (  s4a,  s4b ); ::testVector4 (  s4a,  s4b );

  std::cout << "---- Testing SlVec3ld" << std::endl; ::testVector ( ld3a, ld3b ); ::testVector3 ( ld3a, ld3b );
  std::cout << "---- Testing SlVec3d"  << std::endl; ::testVector (  d3a,  d3b ); ::testVector3 (  d3a,  d3b );
  std::cout << "---- Testing SlVec3f"  << std::endl; ::testVector (  f3a,  f3b ); ::testVector3 (  f3a,  f3b );
  std::cout << "---- Testing SlVec3l"  << std::endl; ::testVector (  l3a,  l3b ); ::testVector3 (  l3a,  l3b );
  std::cout << "---- Testing SlVec3i"  << std::endl; ::testVector (  i3a,  i3b ); ::testVector3 (  i3a,  i3b );
  std::cout << "---- Testing SlVec3s"  << std::endl; ::testVector (  s3a,  s3b ); ::testVector3 (  s3a,  s3b );

  std::cout << "---- Testing SlVec2ld" << std::endl; ::testVector ( ld2a, ld2b ); ::testVector2 ( ld2a, ld2b );
  std::cout << "---- Testing SlVec2d"  << std::endl; ::testVector (  d2a,  d2b ); ::testVector2 (  d2a,  d2b );
  std::cout << "---- Testing SlVec3f"  << std::endl; ::testVector (  f2a,  f2b ); ::testVector2 (  f2a,  f2b );
  std::cout << "---- Testing SlVec2l"  << std::endl; ::testVector (  l2a,  l2b ); ::testVector2 (  l2a,  l2b );
  std::cout << "---- Testing SlVec2i"  << std::endl; ::testVector (  i2a,  i2b ); ::testVector2 (  i2a,  i2b );
  std::cout << "---- Testing SlVec2s"  << std::endl; ::testVector (  s2a,  s2b ); ::testVector2 (  s2a,  s2b );

  // Space transformations.
  SlVec3d pt ( 1.1, 2.2, 3.3 );
  SlVec3d save ( pt );
  pt.xyz2rtz();
  std::cout << "Z01: rtz = " << pt << std::endl;
  pt.rtz2xyz();
  std::cout << "Z02: xyz = " << pt << std::endl;
  SL_ASSERT ( pt.isEqual ( save, 1e-10 ) );

  // Wait for user to press the 'any' key.
  std::cin.get();

  return 0;
}
