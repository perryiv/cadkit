
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Derivative.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_MATH_DERIVATIVE_H_
#define _GENERIC_NURBS_LIBRARY_MATH_DERIVATIVE_H_

#include "GN/Math/Matrix.h"
#include "GN/Algorithms/BasisFunctions.h"

#include <algorithm>
#include <vector>

namespace GN {
namespace Evaluate {

  // d is the number of derivatives. Passing in 1 will return the first derivative and the point at u.
  template
  <
    class CurveType,
    class Derivatives
  >
  inline void derivative ( const CurveType& curve, 
                           typename CurveType::IndependentArgument u,
                           typename CurveType::SizeType d,
                           Derivatives &ders )
  {
    typedef typename CurveType::IndependentArgument IndependentArgument;
    typedef typename CurveType::SizeType SizeType;
    typedef typename CurveType::DependentType DependentType;
    typedef typename CurveType::Vector Vector;
    typedef typename CurveType::SizeContainer SizeContainer;
    typedef typename CurveType::DependentContainer MatrixContainer;
    typedef typename CurveType::DependentTester DependentTester;
    typedef typename CurveType::ErrorCheckerType ErrorCheckerType;
    typedef GN::Math::Matrix < SizeType, MatrixContainer, SizeContainer, DependentTester, ErrorCheckerType > Matrix;

    SizeType order ( curve.order() );
    SizeType degree ( order - 1 );
    SizeType du ( std::min<SizeType> ( degree, d ) );

    for ( SizeType k = order; k <= d; ++k )
    {
      Vector &der ( ders[k] );
      std::fill ( der.begin(), der.end(), static_cast<DependentType> ( 0 ) );
    }

    // Find the knot span.
    SizeType span ( GN::Algorithms::findKnotSpan ( curve, u ) );

    // Container to hold the derivatives.
    Matrix nders ( du + 1, order );

    // Find the basis function derivatives.
    GN::Algorithms::basisFunctionsDerivatives ( curve, span, u, du, nders );

    // Assign the derivative values.
    for ( SizeType k = 0; k <= du; ++k )
    {
      Vector &der ( ders[k] );

      // Initialize the derivative.
      std::fill ( der.begin(), der.end(), static_cast<DependentType> ( 0 ) );

      // We calculate the minimum of the point size and the dimenion.
      const SizeType dimension ( std::min<SizeType> ( der.size(), curve.dimension() ) );
      
      for ( SizeType j = 0; j < order; ++j )
      {
        // Do this here.
        SizeType index ( span - degree + j );

        // Calculate the coordinate for each dimension.
        for ( SizeType i = 0; i < dimension; ++i )
	        der[i] += nders(k,j) * curve.controlPoint ( i, index );
      }
    }
  }

}
}


#endif
