
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Parameterization algorithms.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_ALGORITHMS_PARAMETERIZATION_H_
#define _GENERIC_NURBS_LIBRARY_ALGORITHMS_PARAMETERIZATION_H_

#include "GN/Math/Distance.h"
#include "GN/MPL/SameType.h"


namespace GN {
namespace Algorithms {


///////////////////////////////////////////////////////////////////////////////
//
//  Parameterize the data.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class IndependentContainer,
  class DependentContainer,
  class PowerFunctor,
  class ErrorCheckerType
>
struct Parameterize
{
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Typedefs.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef typename DependentContainer::size_type SizeType;
  typedef typename IndependentContainer::value_type IndependentType;
  typedef GN::Math::DistanceSquared2d < DependentContainer, ErrorCheckerType > DistanceSquared;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Fit the data.
  //
  /////////////////////////////////////////////////////////////////////////////

  static void fit ( const DependentContainer &points,
                    IndependentType power,
                    IndependentContainer &u )
  {
    // Should be true.
    GN_ASSERT_SAME_TYPE ( SizeType, typename IndependentContainer::size_type );

    // Handle trivial case.
    if ( points.empty() || points[0].empty() )
      return;

    // The last index.
    const SizeType numPts ( points[0].size() );
    SizeType n ( numPts - 1 );

    // Set first and last parameter.
    u.resize ( numPts );
    u[0] = 0;
    u[n] = 1;

    // Adjust the power to "hold" the square-root.
    power *= 0.5f;

    // Find the denominator.
    IndependentType denom ( 0 );
    for ( SizeType k = 1; k <= n; ++k )
    {
      // Find the squared-distance between these 2 data points.
      IndependentType dist ( DistanceSquared::calculate ( points, k - 1, points, k ) );

      // Calculate the denominator. The "power" has the square-root in it.
      denom += PowerFunctor::calculate ( dist, power );
    }

    // Find the numerator.
    for ( SizeType k = 1; k <= n; ++k )
    {
      // Find the squared-distance between these 2 data points.
      IndependentType dist ( DistanceSquared::calculate ( points, k - 1, points, k ) );

      // Calculate the numerator. The "power" has the square-root in it.
      IndependentType numer ( PowerFunctor::calculate ( dist, power ) );

      // This is equation 9.5 (or 9.6, depending on the power) in "The NURBS Book", page 365.
      u[k] = u[k-1] + numer / denom;
    }
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constants.
//
///////////////////////////////////////////////////////////////////////////////

namespace Constants
{
  const float CHORDAL_FIT     ( 1.0f );
  const float CENTRIPETAL_FIT ( 0.5f );
};


}; // namespace Algorithms
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_ALGORITHMS_PARAMETERIZATION_H_
