
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_MATH_FRENET_SERRET_H_
#define _GENERIC_NURBS_LIBRARY_MATH_FRENET_SERRET_H_

#include "GN/Evaluate/Derivative.h"
#include "GN/Evaluate/Point.h"


namespace GN {
namespace Math {

///////////////////////////////////////////////////////////////////////////////
//
//  Calcuate the Frenet-Serret frame ( Tangent, Normal, and Bi-Normal ) 
//  for the given u value.
//  http://en.wikipedia.org/wiki/Frenet-Serret_formulas 
//
///////////////////////////////////////////////////////////////////////////////

template < class CurveType, class VectorType >
inline void frenetSerret ( const CurveType& curve, 
                           typename CurveType::IndependentArgument u,
                           VectorType &tangent,
                           VectorType &normal,
                           VectorType &binormal )
{
  typedef typename CurveType::Vector Vector;
  typedef std::vector < Vector > Derivatives;
  
  // We are going to want the 1st and 2nd derivative.
  Derivatives ders ( 3 );
  ders[0] = Vector ( 3 );
  ders[1] = Vector ( 3 );
  ders[2] = Vector ( 3 );

  GN::Evaluate::derivative ( curve, u, 2, ders );

  VectorType t ( ders[1][0], ders[1][1], ders[1][2] );
  VectorType n ( ders[2][0], ders[2][1], ders[2][2] );

  //t.normalize();
  n.normalize();

  tangent = t;
  normal = n;

  VectorType b ( tangent.cross ( normal ) );
  b.normalize();
  binormal = b;
}

}
}

#endif
