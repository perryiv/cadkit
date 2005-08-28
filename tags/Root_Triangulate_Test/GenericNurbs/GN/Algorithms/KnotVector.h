
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Algorithms to build knot vectors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_ALGORITHMS_KNOT_VECTOR_H_
#define _GENERIC_NURBS_LIBRARY_ALGORITHMS_KNOT_VECTOR_H_

#include "GN/Macros/ErrorCheck.h"
#include "GN/Limits/Nurbs.h"

#include <algorithm>


namespace GN {
namespace Algorithms {


///////////////////////////////////////////////////////////////////////////////
//
//  Build the knot vector.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class IndependentContainer,
  class ErrorCheckerType
>
struct KnotVector
{
  typedef typename IndependentContainer::size_type SizeType;
  typedef typename IndependentContainer::iterator IteratorType;
  typedef typename IndependentContainer::value_type IndependentType;
  typedef GN::Limits::Nurbs<SizeType> Limits;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  This makes the knot vector using either of Tiller's algorithms 
  //  (Eq 9.8 or Eq 9.69 in The NURBS Book).
  //
  //  It will always make knots.size(), knots.
  //
  //  If knots.size() = (u.size() + order) then it will 
  //  use the algorithm for interpolation (Eq 9.8). 
  //
  //  If knots.size() < (u.size() + order) it will use the algorithm for 
  //  approximation (Eq 9.69).
  //
  //  If knots.size() = (u.size() + order) 
  //  "plus or minus" a few, then it gets numerically unstable.
  //
  /////////////////////////////////////////////////////////////////////////////

  static void build ( const IndependentContainer &u,
                      const SizeType &order,
                      IndependentContainer &knots )
  {
    GN_ERROR_CHECK ( order >= Limits::MIN_ORDER );
    GN_ERROR_CHECK ( knots.size() >= order + order ); // Smallest knot vector allowed.
    GN_ERROR_CHECK ( knots.size() <= ( u.size() + order ) );
    GN_ERROR_CHECK ( u.size() >= Limits::MIN_NUM_CTR_PTS );

    // The number of control points.
    SizeType numCtrPts ( knots.size() - order );

    // Fill the left end of the knot vector.
    {
      IteratorType first ( knots.begin() );
      IteratorType last  ( first + order );
      std::fill ( first, last, u.front() );
    }

    // Fill the right end of the knot vector.
    {
      IteratorType first ( knots.begin() + numCtrPts );
      IteratorType last  ( first + order );
      std::fill ( first, last, u.back() );
    }

    // Interpolation
    if ( u.size() == numCtrPts )
    {
      // This is equation 9.8 from "The NURBS Book", page 365.
      IndependentType inverseOfDegree ( 1.0f / IndependentType ( order - 1 ) );
      SizeType numInteriorKnots ( numCtrPts - order );
      for ( SizeType i = 0; i < numInteriorKnots; ++i )
      {
        knots[order+i] = 0;
        for ( SizeType k = 0; k < order - 1; ++k )
          knots[order+i] += u[i+1+k];
        knots[order+i] *= inverseOfDegree;
      }
    }

    // Approximation
    else
    {
      // Algorithm in "The Nurbs Book" uses these variable names.
      SizeType n ( numCtrPts - 1 );
      SizeType p ( order - 1 );
      SizeType m ( u.size() );
      IndependentType d ( IndependentType ( m + 1 ) / IndependentType ( n - p + 1 ) );

      // Fill the interior knots.
      for ( SizeType j = 1; j <= (n-p); ++j )
      {
        // This is equation 9.69 from "The NURBS Book", page 412.
        SizeType i ( static_cast<SizeType> ( static_cast<IndependentType> ( j ) * d ) );
        IndependentType alpha ( j * d - IndependentType ( i ) );
        knots[order+j-1] = ( 1 - alpha ) * u[i-1] + alpha * u[i];
      }
    }
  }
};


}; // namespace Algorithms
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_ALGORITHMS_KNOT_VECTOR_H_
