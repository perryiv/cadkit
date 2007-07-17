
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Build the normals and vertices of the body of a unit cylinder.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_ALGORITHMS_CYLINDER_H__
#define __USUL_ALGORITHMS_CYLINDER_H__

#include "Usul/Math/UMath.h"

namespace Usul {
namespace Algorithms {

class Cylinder
{
public:
  Cylinder () : 
    _sides ( 0 ),
    _radius ( 0 )
  {
  }

  /// Get/Set the number of sides.
  unsigned int sides () const { return _sides; }
  void         sides ( unsigned int s ) { _sides = s; }

  /// Get/Set the radius.
  double       radius () const { return _radius; }
  void         radius ( double d ) { _radius = d; }

  /// Make the points and normals for the body of a unit cylinder on the y-axis.
  template < class Points, class Normals >
  void operator () ( Points& points, Normals& normals )
  {
    typedef typename Points::value_type Point;
    typedef typename Normals::value_type Normal;

    const float c ( 3.14159 / 180.0 );

    Point p ( 0.0, 1.0, 0.0 );

    for( unsigned int i = 0; i < _sides; ++i )
    {
      const float u ( static_cast < float > ( i ) / static_cast < float > ( _sides - 1 ) );
      const float theta ( u * 360.0 );
      const float x ( _radius * Usul::Math::sin ( c * theta ) );
      const float z ( _radius * Usul::Math::cos ( c * theta ) );

      Point v0 ( x, 0.0, z );

      normals.push_back( v0 );
      normals.push_back( v0 );
      points.push_back( v0 );
      points.push_back( v0 + p );
    }
  }

private:
  unsigned int _sides;
  double _radius;
};

}
}


#endif // __USUL_ALGORITHMS_CYLINDER_H__
