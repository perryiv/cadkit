
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Compute the barycentric coordinates of a triangle.
//  Implementation adapted from "Real-time Collision Detection."
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_MATH_BARYCENTRIC_H__
#define __USUL_MATH_BARYCENTRIC_H__

#include "Usul/Math/Vector3.h"


namespace Usul {
namespace Math {

template < class T, class I, class E > 
inline Usul::Math::Vec3d barycentric ( const Usul::Math::Vector3<T,I,E>& v0, 
                                       const Usul::Math::Vector3<T,I,E>& v1, 
                                       const Usul::Math::Vector3<T,I,E>& v2, 
                                       const Usul::Math::Vector3<T,I,E>& p )
{
  typedef Usul::Math::Vector3<T,I,E> Vertex;

  // Compute vectors.
  const Vertex t0 ( v1 - v0 );
  const Vertex t1 ( v2 - v0 );
  const Vertex t2 ( p - v0 );

  // Compute dot products.
  const double dot00 ( t0.dot ( t0 ) );
  const double dot01 ( t0.dot ( t1 ) );
  const double dot11 ( t1.dot ( t1 ) );
  const double dot20 ( t2.dot ( t0 ) );
  const double dot21 ( t2.dot ( t1 ) );

  // Compute barycentric coordinates.
  const double invDenom ( 1.0 / ( dot00 * dot11 - dot01 * dot01 ) );
  const double v ( ( dot11 * dot20 - dot01 * dot21 ) * invDenom );
  const double w ( ( dot00 * dot21 - dot01 * dot20 ) * invDenom );
  const double u ( 1.0 - v - w );

  return Usul::Math::Vec3d ( u, v, w );
}

}
}


#endif // __USUL_MATH_BARYCENTRIC_H__
