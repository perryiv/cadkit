
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


namespace GN {
namespace Algorithms {


///////////////////////////////////////////////////////////////////////////////
//
//  Parameterize the data.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class SplineType > struct Parameterize
  {
    typedef typename SplineType::UIntType UIntType;
    typedef typename SplineType::KnotType ParamType;
    typedef typename SplineType::KnotContainer::value_type ParamContainer;
    typedef typename SplineType::ControlPointContainer DataContainer;
    typedef typename SplineType::Power Power;
    typedef GN::Math::Distance2<SplineType> Distance;

    static void fit ( const DataContainer &points,
                      const UIntType &dimension,
                      const UIntType &order,
                      ParamType power,
                      ParamContainer &u )
    {
      // Handle trivial case.
      if ( points.empty() || points[0].empty() )
        return;

      // The last index.
      const UIntType numPts ( points[0].size() );
      UIntType n ( numPts - 1 );

      // Set first and last parameter.
      u.resize ( numPts );
      u[0] = 0;
      u[n] = 1;

      // Adjust the power to "hold" the square-root.
      power *= 0.5f;

      // Find the denominator.
      ParamType denom ( 0 );
      {
        for ( UIntType k = 1; k <= n; ++k )
        {
          // Find the distance between these 2 data points.
          ParamType dist ( Distance::squared ( points, k - 1, points, k ) );

          // Calculate the denominator. The "power" has the square-root in it.
          denom += Power::calculate ( dist, power );
        }
      }

      // Find the numerator.
      {
        for ( UIntType k = 1; k <= n; ++k )
        {
          // Find the distance between these 2 data points.
          ParamType dist ( Distance::squared ( points, k - 1, points, k ) );

          // Calculate the numerator. The "power" has the square-root in it.
          ParamType numer ( Power::calculate ( dist, power ) );

          // This is equation 9.5 (or 9.6, depending on the power) in "The NURBS Book", page 365.
          u[k] = u[k-1] + numer / denom;
        }
      }
    }
  };
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


///////////////////////////////////////////////////////////////////////////////
//
//  Parameterize the data.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > void parameterize
(
  const SplineType &s,
  const typename SplineType::ControlPointContainer &points,
  const typename SplineType::UIntType &dimension,
  const typename SplineType::UIntType &order,
  const typename SplineType::KnotType &power,
  typename SplineType::KnotContainer::value_type &u
)
{
  Detail::Parameterize<SplineType>::fit ( points, dimension, order, power, u );
}

}; // namespace Algorithms
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_ALGORITHMS_SORT_H_
