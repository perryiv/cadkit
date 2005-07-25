
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Algorithm to sort the points. Assumes they define a convex-hull.
//  Useful references:
//  http://compgeom.cs.uiuc.edu/~jeffe/teaching/373/notes/x05-convexhull.pdf
//  http://www.cs.clemson.edu/~goddard/handouts/texts/840/part1.pdf
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_ALGORITHMS_CONVEX_HULL_SORT_H_
#define _USUL_ALGORITHMS_CONVEX_HULL_SORT_H_

#include "Usul/Errors/Assert.h"
#include "Usul/Predicates/CompareCoordinate.h"
#include "Usul/Math/Angle.h"

#include <algorithm>
#include <limits>


namespace Usul {
namespace Algorithms {


template < class PointsContainer > void convexHullSort ( PointsContainer &points )
{
  // Useful typedefs.
  typedef PointsContainer::iterator Itr;
  typedef PointsContainer::value_type Value;
  typedef Usul::Math::Angle<double,3> Angle;

  // Ignore containers without enough points.
  if ( points.size() <= 2 )
    return;

  // Find left-most point.
  Itr left ( std::min_element ( points.begin(), points.end(), Usul::Predicates::CompareCoordinate ( 0 ) ) );
  if ( points.end() == left )
    throw std::runtime_error ( "Error 1163900903: failed to find left-most element in container of points" );

  // Put this point in a new container and remove it from the first.
  PointsContainer ordered;
  ordered.insert ( ordered.end(), *left );
  points.erase ( left );

  // Initialize the first vector between vertices. Make it vertical.
  Value v0 ( 0.0f, 1.0f, 0.0f );

  // While there are still points in the original container...
  while ( false == points.empty() )
  {
    // Initialize the iterator and angle.
    Itr saved ( points.end() );
    double maxAngle ( 0 );

    // Get the current ordered point.
    const Value &o ( ordered.back() );

    // Loop through the remaining points.
    for ( Itr i = points.begin(); i != points.end(); ++i )
    {
      // Shortcut to the current point.
      const Value &p ( *i );

      // Get the vector between the two current points.
      const Value v1 ( p - o );

      // Get the angle between them.
      const double angle ( Angle::get ( v0, v1 ) );

      // If this angle is greater then save it.
      if ( angle > maxAngle )
      {
        maxAngle = angle;
        saved = i;
      }
    }

    // Should be true.
    USUL_ASSERT ( points.end() != saved );

    // Assign new "first" vector. Do this before inserting below.
    v0 = Value ( o - *saved );

    // Append the saved point to the list and remove it from the original.
    ordered.insert ( ordered.end(), *saved );
    points.erase ( saved );
  }

  // Copy the ordered points back into the original container.
  points = ordered;
}


} // namespace Algorithms
} // namespace Usul


#endif // _USUL_ALGORITHMS_CONVEX_HULL_SORT_H_
