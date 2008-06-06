
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_MATH_CURVATURE_H_
#define _GENERIC_NURBS_LIBRARY_MATH_CURVATURE_H_

#include "GN/Evaluate/Derivative.h"

#include "Usul/Math/Vector3.h"

namespace GN {
namespace Math {


///////////////////////////////////////////////////////////////////////////////
//
//  Calculate the curvature.
//  http://en.wikipedia.org/wiki/Curvature
//
///////////////////////////////////////////////////////////////////////////////

template < class CurveType >
inline double curvature ( const CurveType& curve, typename CurveType::IndependentArgument u )
{
  typedef typename CurveType::Vector Vector;
  typedef std::vector < Vector > Derivatives;

  // We are going to want the 1st and 2nd derivative.
  Derivatives ders ( 3 );
  ders[0] = Vector ( 3 );
  ders[1] = Vector ( 3 );
  ders[2] = Vector ( 3 );

  GN::Evaluate::derivative ( curve, u, 2, ders );

  //Usul::Math::Vec3d first  ( ders[0][0], ders[0][1], ders[0][2] );
  Usul::Math::Vec3d first ( ders[1][0], ders[1][1], ders[1][2] );
  Usul::Math::Vec3d second  ( ders[2][0], ders[2][1], ders[2][2] );

  double length ( first.length() );

  if ( 0.0 != length )
    return (first.cross ( second )).length() / ( length * length * length );

  return 0.0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Calculate the Torsion.
//  http://en.wikipedia.org/wiki/Torsion_of_curves
//
///////////////////////////////////////////////////////////////////////////////

template < class CurveType >
inline double torsion ( const CurveType& curve, typename CurveType::IndependentArgument u )
{
  typedef typename CurveType::Vector Vector;
  typedef std::vector < Vector > Derivatives;

  // We are going to want the 1st and 2nd derivative.
  Derivatives ders ( 4 );
  ders[0] = Vector ( 3 );
  ders[1] = Vector ( 3 );
  ders[2] = Vector ( 3 );
  ders[3] = Vector ( 3 );

  GN::Evaluate::derivative  ( curve, u, 3, ders );

  Usul::Math::Vec3d first  ( ders[1][0], ders[1][1], ders[1][2] );
  Usul::Math::Vec3d second ( ders[2][0], ders[2][1], ders[2][2] );
  Usul::Math::Vec3d third  ( ders[3][0], ders[3][1], ders[3][2] );

  Usul::Math::Vec3d t0 ( first.cross ( second ) );

  double length ( t0.length() );

  if ( 0.0 != length )
    return t0.dot ( third ) / ( length * length );

  return 0.0;
}

}
}


#endif
