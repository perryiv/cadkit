
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
        Real cx, Real cy, Real cz, 
        Real r, unsigned int depth, 
        Sequence &v, Sequence &n )
    {
      // If we are at the requested depth.
      if ( 0 == depth )
      {
        // Current size.
        unsigned int sv ( v.size() );
        unsigned int sn ( n.size() );

        // Make room in the sequences.
        v.resize ( sv + 3 );
        n.resize ( sn + 3 );

        // Add the normals. Note: the vertices are all a unit length from 
        // the center, so they are also the normal.
        n.at(sn)[0] = x1;
        n.at(sn)[1] = y1;
        n.at(sn)[2] = z1;
        ++sn;
        n.at(sn)[0] = x2;
        n.at(sn)[1] = y2;
        n.at(sn)[2] = z2;
        ++sn;
        n.at(sn)[0] = x3;
        n.at(sn)[1] = y3;
        n.at(sn)[2] = z3;

        // Scale by the radius. Do this first.
        x1 *= r;
        x2 *= r;
        x3 *= r;
        y1 *= r;
        y2 *= r;
        y3 *= r;
        z1 *= r;
        z2 *= r;
        z3 *= r;

        // Offset by the center. Do this second.
        x1 += cx;
        x2 += cx;
        x3 += cx;
        y1 += cx;
        y2 += cx;
        y3 += cx;
        z1 += cx;
        z2 += cx;
        z3 += cx;

        // Add the vertices.
        v.at(sv)[0] = x1;
        v.at(sv)[1] = y1;
        v.at(sv)[2] = z1;
        ++sv;
        v.at(sv)[0] = x2;
        v.at(sv)[1] = y2;
        v.at(sv)[2] = z2;
        ++sv;
        v.at(sv)[0] = x3;
        v.at(sv)[1] = y3;
        v.at(sv)[2] = z3;
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
        (*this) (  x1,  y1,  z1, x12, y12, z12, x31, y31, z31, cx, cy, cz, r, depth, v, n );
        (*this) (  x2,  y2,  z2, x23, y23, z23, x12, y12, z12, cx, cy, cz, r, depth, v, n );
        (*this) (  x3,  y3,  z3, x31, y31, z31, x23, y23, z23, cx, cy, cz, r, depth, v, n );
        (*this) ( x12, y12, z12, x23, y23, z23, x31, y31, z31, cx, cy, cz, r, depth, v, n );
      }
    }
  };
}; // namespace Detail


///////////////////////////////////////////////////////////////////////////////
//
//  Make a unit sphere at the origin.
//
///////////////////////////////////////////////////////////////////////////////

template < class Sequence > void sphere ( unsigned int numSubDivisions, Sequence &vertices, Sequence &normals )
{
  // Help the compiler.
  float cx ( 0 ), cy ( 0 ), cz ( 0 ), r ( 1 );
  sphere ( numSubDivisions, cx, cy, cz, r, vertices, normals );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a sequence of triangles that define a sphere. Subdivide n times.
//  Note: this will not make tri-strips.
//
///////////////////////////////////////////////////////////////////////////////

template < class Sequence, class Real > void sphere ( 
  unsigned int n, Real cx, Real cy, Real cz, Real r, Sequence &vertices, Sequence &normals )
{
  // Declare these constants used in the subdivision algorithm.
  const Real X ( static_cast < Real > ( 0.525731112119133606 ) );
  const Real Z ( static_cast < Real > ( 0.8506508083528655993 ) );

  // Reserve space in the sequences. It works out to 60 * 4 ^ n.
  const unsigned int need ( 60 * ::pow ( 4, n ) );
  vertices.reserve ( need );
  normals.reserve  ( need );

  // Make the subdivider.
  typedef Detail::Subdivide<Sequence,Real> Subdivide;
  Subdivide subdivider;

  // Call the sub-divider.
  subdivider ( -X,  0,  Z,  X,  0,  Z,  0,  Z,  X, cx, cy, cz, r, n, vertices, normals );
  subdivider ( -X,  0,  Z,  0,  Z,  X, -Z,  X,  0, cx, cy, cz, r, n, vertices, normals );
  subdivider ( -Z,  X,  0,  0,  Z,  X,  0,  Z, -X, cx, cy, cz, r, n, vertices, normals );
  subdivider (  0,  Z,  X,  Z,  X,  0,  0,  Z, -X, cx, cy, cz, r, n, vertices, normals );
  subdivider (  0,  Z,  X,  X,  0,  Z,  Z,  X,  0, cx, cy, cz, r, n, vertices, normals );
  subdivider (  Z,  X,  0,  X,  0,  Z,  Z, -X,  0, cx, cy, cz, r, n, vertices, normals );
  subdivider (  Z,  X,  0,  Z, -X,  0,  X,  0, -Z, cx, cy, cz, r, n, vertices, normals );
  subdivider (  0,  Z, -X,  Z,  X,  0,  X,  0, -Z, cx, cy, cz, r, n, vertices, normals );
  subdivider (  0,  Z, -X,  X,  0, -Z, -X,  0, -Z, cx, cy, cz, r, n, vertices, normals );
  subdivider ( -X,  0, -Z,  X,  0, -Z,  0, -Z, -X, cx, cy, cz, r, n, vertices, normals );
  subdivider (  0, -Z, -X,  X,  0, -Z,  Z, -X,  0, cx, cy, cz, r, n, vertices, normals );
  subdivider (  0, -Z, -X,  Z, -X,  0,  0, -Z,  X, cx, cy, cz, r, n, vertices, normals );
  subdivider (  0, -Z, -X,  0, -Z,  X, -Z, -X,  0, cx, cy, cz, r, n, vertices, normals );
  subdivider ( -Z, -X,  0,  0, -Z,  X, -X,  0,  Z, cx, cy, cz, r, n, vertices, normals );
  subdivider ( -X,  0,  Z,  0, -Z,  X,  X,  0,  Z, cx, cy, cz, r, n, vertices, normals );
  subdivider (  0, -Z,  X,  Z, -X,  0,  X,  0,  Z, cx, cy, cz, r, n, vertices, normals );
  subdivider ( -Z,  X,  0, -Z, -X,  0, -X,  0,  Z, cx, cy, cz, r, n, vertices, normals );
  subdivider ( -Z,  X,  0, -X,  0, -Z, -Z, -X,  0, cx, cy, cz, r, n, vertices, normals );
  subdivider ( -Z,  X,  0,  0,  Z, -X, -X,  0, -Z, cx, cy, cz, r, n, vertices, normals );
  subdivider (  0, -Z, -X, -Z, -X,  0, -X,  0, -Z, cx, cy, cz, r, n, vertices, normals );
}


}; // namespace Algorithms
}; // namespace Usul


#endif // _USUL_ALGORITHMS_SPHERE_SUB_DIVISION_H_
