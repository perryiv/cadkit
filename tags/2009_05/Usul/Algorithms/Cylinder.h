
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

#include "Usul/Math/Constants.h"
#include "Usul/Math/Functions.h"

namespace Usul {
namespace Algorithms {


/// Make the points and normals for the body of a unit cylinder on the y-axis.
template < class Points, class Normals >
inline void cylinder ( double radius, unsigned int sides, Points& points, Normals& normals, bool normalize )
{
  typedef typename Points::value_type Point;
  typedef typename Normals::value_type Normal;

  Point p ( 0.0, 1.0, 0.0 );

  for( unsigned int i = 0; i < sides; ++i )
  {
    const double u ( static_cast < double > ( i ) / static_cast < double > ( sides - 1 ) );
    const double theta ( u * Usul::Math::PIE * 2 );
    const double x ( radius * Usul::Math::sin ( theta ) );
    const double z ( radius * Usul::Math::cos ( theta ) );

    Point v0 ( static_cast < typename Point::value_type > ( x ), 
               static_cast < typename Point::value_type > ( 0 ), 
               static_cast < typename Point::value_type > ( z ) );

    // Store points now.
    points.push_back  ( v0 + p );
    points.push_back  ( v0 );

    // Normalize and save as normal vector.
    if ( normalize )
      v0.normalize();
    normals.push_back ( v0 );
    normals.push_back ( v0 );
  }
}

}
}


#endif // __USUL_ALGORITHMS_CYLINDER_H__
