
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Sphere sub-division algorithm.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ALGORITHMS_SPHERE_SUB_DIVISION_H_
#define _USUL_ALGORITHMS_SPHERE_SUB_DIVISION_H_

#include <stdexcept>


namespace Usul {
namespace Algorithms {


///////////////////////////////////////////////////////////////////////////////
//
//  Sub-divide a sphere.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class Sequence, class Real > struct Subdivide
  {
    void operator () 
      ( Real x1, Real y1, Real z1, 
        Real x2, Real y2, Real z2, 
        Real x3, Real y3, Real z3, 
        unsigned int depth, 
        Sequence &v )
    {
      // If we are at the requested depth.
      if ( 0 == depth )
      {
        // Current size.
        unsigned int sv ( v.size() );

        // Make room in the sequences.
        v.resize ( sv + 3 );

        // Add the vertices. Note: since they are unit length from 
        // the center they are also normals.
        v[sv][0] = x1;
        v[sv][1] = y1;
        v[sv][2] = z1;
        ++sv;
        v[sv][0] = x2;
        v[sv][1] = y2;
        v[sv][2] = z2;
        ++sv;
        v[sv][0] = x3;
        v[sv][1] = y3;
        v[sv][2] = z3;
      }

      // Otherwise...
      else
      {
        // Make three new points.
        Real x12 ( x1 + x2 );
        Real y12 ( y1 + y2 );
        Real z12 ( z1 + z2 );
        Real x23 ( x2 + x3 );
        Real y23 ( y2 + y3 );
        Real z23 ( z2 + z3 );
        Real x31 ( x3 + x1 );
        Real y31 ( y3 + y1 );
        Real z31 ( z3 + z1 );

        // Adjust the first point.
        const Real one ( static_cast < Real > ( 1 ) );
        Real d ( ::sqrt ( (double) ( x12 * x12 + y12 * y12 + z12 * z12 ) ) );
        if ( 0 == d )
          throw std::runtime_error ( "Error 1622864301, divide by zero" );
        Real invd = one / d;
        x12 *= invd;
        y12 *= invd;
        z12 *= invd;

        // Adjust the second point.
        d = ::sqrt ( (double) ( x23 * x23 + y23 * y23 + z23 * z23 ) );
        if ( 0 == d )
          throw std::runtime_error ( "Error 3368459612, divide by zero" );
        invd = one / d;
        x23 *= invd;
        y23 *= invd;
        z23 *= invd;

        // Adjust the third point.
        d = ::sqrt ( (double) ( x31 * x31 + y31 * y31 + z31 * z31 ) );
        if ( 0 == d )
          throw std::runtime_error ( "Error 2610396704, divide by zero" );
        invd = one / d;
        x31 *= invd;
        y31 *= invd;
        z31 *= invd;

        // Divide again.
        --depth;
        (*this) (  x1,  y1,  z1, x12, y12, z12, x31, y31, z31, depth, v );
        (*this) (  x2,  y2,  z2, x23, y23, z23, x12, y12, z12, depth, v );
        (*this) (  x3,  y3,  z3, x31, y31, z31, x23, y23, z23, depth, v );
        (*this) ( x12, y12, z12, x23, y23, z23, x31, y31, z31, depth, v );
      }
    }
  };
}; // namespace Detail



///////////////////////////////////////////////////////////////////////////////
//
//  Make a sequence of triangles that define a unit sphere. Subdivide n times.
//  Note: this will not make tri-strips. Also, the vertices are also normals.
//
///////////////////////////////////////////////////////////////////////////////

template < class Real, class Sequence > void sphere ( unsigned int n, Sequence &vertices )
{
  // Declare these constants used in the subdivision algorithm.
  const Real X ( static_cast < Real > ( 0.525731112119133606 ) );
  const Real Z ( static_cast < Real > ( 0.8506508083528655993 ) );

  // Reserve space in the sequences. It works out to 60 * 4 ^ n.
  const unsigned int need ( 60 * ::pow ( 4, n ) );
  vertices.reserve ( need );

  // Make the subdivider.
  typedef Detail::Subdivide<Sequence,Real> Subdivide;
  Subdivide subdivider;

  // Call the sub-divider.
  subdivider ( -X,  0,  Z,  X,  0,  Z,  0,  Z,  X, n, vertices );
  subdivider ( -X,  0,  Z,  0,  Z,  X, -Z,  X,  0, n, vertices );
  subdivider ( -Z,  X,  0,  0,  Z,  X,  0,  Z, -X, n, vertices );
  subdivider (  0,  Z,  X,  Z,  X,  0,  0,  Z, -X, n, vertices );
  subdivider (  0,  Z,  X,  X,  0,  Z,  Z,  X,  0, n, vertices );
  subdivider (  Z,  X,  0,  X,  0,  Z,  Z, -X,  0, n, vertices );
  subdivider (  Z,  X,  0,  Z, -X,  0,  X,  0, -Z, n, vertices );
  subdivider (  0,  Z, -X,  Z,  X,  0,  X,  0, -Z, n, vertices );
  subdivider (  0,  Z, -X,  X,  0, -Z, -X,  0, -Z, n, vertices );
  subdivider ( -X,  0, -Z,  X,  0, -Z,  0, -Z, -X, n, vertices );
  subdivider (  0, -Z, -X,  X,  0, -Z,  Z, -X,  0, n, vertices );
  subdivider (  0, -Z, -X,  Z, -X,  0,  0, -Z,  X, n, vertices );
  subdivider (  0, -Z, -X,  0, -Z,  X, -Z, -X,  0, n, vertices );
  subdivider ( -Z, -X,  0,  0, -Z,  X, -X,  0,  Z, n, vertices );
  subdivider ( -X,  0,  Z,  0, -Z,  X,  X,  0,  Z, n, vertices );
  subdivider (  0, -Z,  X,  Z, -X,  0,  X,  0,  Z, n, vertices );
  subdivider ( -Z,  X,  0, -Z, -X,  0, -X,  0,  Z, n, vertices );
  subdivider ( -Z,  X,  0, -X,  0, -Z, -Z, -X,  0, n, vertices );
  subdivider ( -Z,  X,  0,  0,  Z, -X, -X,  0, -Z, n, vertices );
  subdivider (  0, -Z, -X, -Z, -X,  0, -X,  0, -Z, n, vertices );
}


}; // namespace Algorithms
}; // namespace Usul


#endif // _USUL_ALGORITHMS_SPHERE_SUB_DIVISION_H_
