
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

#include "Usul/Math/Constants.h"
#include "Usul/Math/Functions.h"

#include <stdexcept>
#include <cassert>


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
        Real d ( Usul::Math::sqrt ( x12 * x12 + y12 * y12 + z12 * z12 ) );
        if ( 0 == d )
          throw std::runtime_error ( "Error 1622864301, divide by zero" );
        Real invd = one / d;
        x12 *= invd;
        y12 *= invd;
        z12 *= invd;

        // Adjust the second point.
        d = Usul::Math::sqrt ( x23 * x23 + y23 * y23 + z23 * z23 );
        if ( 0 == d )
          throw std::runtime_error ( "Error 3368459612, divide by zero" );
        invd = one / d;
        x23 *= invd;
        y23 *= invd;
        z23 *= invd;

        // Adjust the third point.
        d = Usul::Math::sqrt ( x31 * x31 + y31 * y31 + z31 * z31 );
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
} // namespace Detail


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
  const unsigned int need ( 60 * Usul::Math::pow (static_cast< Real> (4), static_cast<Real> (n) ) );
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


///////////////////////////////////////////////////////////////////////////////
//
//  Make a sphere as a sequence of tri-strips. The mesh divides the sphere 
//  into "lines" of latitude and longitude.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class Real
>
struct LatLongSphere
{
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  LatLongSphere() :
    _centerX        ( 0 ),
    _centerY        ( 0 ),
    _centerZ        ( 0 ),
    _radius         ( 1 ),
    _normalLength   ( 1 ),
    _latitudeStart  ( Usul::Math::DEG_TO_RAD *  90 ),
    _latitudeEnd    ( Usul::Math::DEG_TO_RAD * -90 ),
    _longitudeStart ( Usul::Math::DEG_TO_RAD *   0 ),
    _longitudeEnd   ( Usul::Math::DEG_TO_RAD * 360 ),
    _numLatitude    ( 10 ),
    _numLongitude   ( 10 )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the members.
  //
  /////////////////////////////////////////////////////////////////////////////

  const Real &    centerX()        const { return _centerX;        }
  const Real &    centerY()        const { return _centerY;        }
  const Real &    centerZ()        const { return _centerZ;        }
  const Real &    radius()         const { return _radius;         }
  const Real &    normalLength()   const { return _normalLength;   }
  const Real &    latitudeStart()  const { return _latitudeStart;  }
  const Real &    latitudeEnd()    const { return _latitudeEnd;    }
  const Real &    longitudeStart() const { return _longitudeStart; }
  const Real &    longitudeEnd()   const { return _longitudeEnd;   }
  unsigned int    numLatitude()    const { return _numLatitude;    }
  unsigned int    numLongitude()   const { return _numLongitude;   }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the members.
  //
  /////////////////////////////////////////////////////////////////////////////

  void            centerX         ( const Real &v )  { _centerX        = v; }
  void            centerY         ( const Real &v )  { _centerY        = v; }
  void            centerZ         ( const Real &v )  { _centerZ        = v; }
  void            radius          ( const Real &v )  { _radius         = v; }
  void            normalLength    ( const Real &v )  { _normalLength   = v; }
  void            latitudeStart   ( const Real &v )  { _latitudeStart  = v; }
  void            latitudeEnd     ( const Real &v )  { _latitudeEnd    = v; }
  void            longitudeStart  ( const Real &v )  { _longitudeStart = v; }
  void            longitudeEnd    ( const Real &v )  { _longitudeEnd   = v; }
  void            numLatitude     ( unsigned int v ) { _numLatitude    = v; }
  void            numLongitude    ( unsigned int v ) { _numLongitude   = v; }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Construct the sphere.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Points, class Normals, class Strips >
  void operator () ( Points &points, Normals &normals, Strips &strips )
  {
    // Handle bad state.
    if ( _numLatitude < 2 )
      throw std::runtime_error ( "Error 1548908475, invalid number of latitudinal segments" );
    if ( _numLongitude < 3 )
      throw std::runtime_error ( "Error 4079279165, invalid number of longitudinal segments" );

    // Make room.
    const unsigned int numStrips ( _numLatitude );
    const unsigned int rows      ( 1 + _numLatitude );
    const unsigned int columns   ( 1 + _numLongitude );
    const unsigned int numVerts  ( ( 2 * rows - 2 ) * columns );
    points.reserve  ( numVerts  );
    normals.reserve ( numVerts  );
    strips.reserve  ( numStrips );

    // Initialize. Note: we figure out everything as if it had a unit radius.
    Real diff ( _latitudeEnd - _latitudeStart );
    Real topA ( _latitudeStart );
    Real topX ( Usul::Math::cos ( topA ) );
    Real topY ( Usul::Math::sin ( topA ) );
    Real u ( Real ( 1 ) / Real ( numStrips ) );
    Real botA ( _latitudeStart + diff * u );
    Real botX ( Usul::Math::cos ( botA ) );
    Real botY ( Usul::Math::sin ( botA ) );

    // Make the first strip.
    this->_makeStrip ( columns, topX, topY, botX, botY, points, normals, strips );

    // Each tri-strip runs parallel to the equator. Loop through all but the last.
    for ( unsigned int i = 1; i < numStrips - 1; ++i )
    {
      // The bottom becomes the top.
      topX = botX;
      topY = botY;

      // The bottom latitudinal angle.
      u = Real ( i + 1 ) / Real ( numStrips );
      assert ( u > 0 && u < 1 );
      botA = _latitudeStart + diff * u;

      // The bottom point in the x-y plane.
      botX = Usul::Math::cos ( botA );
      botY = Usul::Math::sin ( botA );

      // Make the strip.
      this->_makeStrip ( columns, topX, topY, botX, botY, points, normals, strips );
    }

    // Make the last strip.
    topX = botX;
    topY = botY;
    botA = _latitudeEnd;
    botX = Usul::Math::cos ( botA );
    botY = Usul::Math::sin ( botA );
    this->_makeStrip ( columns, topX, topY, botX, botY, points, normals, strips );

    // Should be true.
    assert ( points.size()  == numVerts  );
    assert ( normals.size() == numVerts  );
    assert ( strips.size()  == numStrips );
  }

protected:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Construct one tri-strip.
  //
  /////////////////////////////////////////////////////////////////////////////

  template
  <
    class Points,
    class Normals,
    class Strips
  >
  void _makeStrip ( unsigned int columns, 
                    const Real &topX, 
                    const Real &topY, 
                    const Real &botX, 
                    const Real &botY, 
                    Points &points, 
                    Normals &normals,
                    Strips &strips )
  {
    // For convenience.
    typedef typename Points::value_type  Point;
    typedef typename Normals::value_type Normal;
    typedef typename Strips::value_type  Strip;

    // Set the start and count.
    const unsigned int length ( 2 * columns );
    strips.push_back ( Strip ( points.size(), length ) );

    // Loop through the columns in this tri-strip.
    for ( unsigned int j = 0; j < columns; ++j )
    {
      // The longitudinal angle. On the last one we set it to "_latitudeEnd".
      Real diff ( _longitudeEnd - _longitudeStart );
      Real longA ( ( columns - 1 == j ) ? _longitudeEnd : _longitudeStart + diff * Real ( j ) / Real ( columns - 1 ) );
      Real cosLongA ( Usul::Math::cos ( longA ) );
      Real sinLongA ( Usul::Math::sin ( longA ) );

      // Calculate the unit normals.
      Real tnx ( topX * cosLongA );
      Real tny ( topY );
      Real tnz ( topX * sinLongA );
      Real bnx ( botX * cosLongA );
      Real bny ( botY );
      Real bnz ( botX * sinLongA );

      // Set the point.
      points.push_back ( Point ( _centerX + _radius * bnx, _centerX + _radius * bny, _centerX + _radius * bnz ) );
      points.push_back ( Point ( _centerX + _radius * tnx, _centerX + _radius * tny, _centerX + _radius * tnz ) );
      

      // Set the normal.
//#ifdef __linux // Is the negation still needed?
//      normals.push_back ( Normal ( _normalLength * tnx, _normalLength * tny, _normalLength * tnz ) * -1 );
//      normals.push_back ( Normal ( _normalLength * bnx, _normalLength * bny, _normalLength * bnz ) * -1 ); 
//#else
      normals.push_back ( Normal ( _normalLength * bnx, _normalLength * bny, _normalLength * bnz ) ); 
      normals.push_back ( Normal ( _normalLength * tnx, _normalLength * tny, _normalLength * tnz ) );
//#endif
    }
  }

private:

  Real _centerX;
  Real _centerY;
  Real _centerZ;
  Real _radius;
  Real _normalLength;
  Real _latitudeStart;
  Real _latitudeEnd;
  Real _longitudeStart;
  Real _longitudeEnd;
  unsigned int _numLatitude;
  unsigned int _numLongitude;
};


} // namespace Algorithms
} // namespace Usul


#endif // _USUL_ALGORITHMS_SPHERE_SUB_DIVISION_H_
