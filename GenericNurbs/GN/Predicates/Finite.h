
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Predicate class for checking if the floating point number is finite.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_PREDICATES_FINITE_H_
#define _GENERIC_NURBS_LIBRARY_PREDICATES_FINITE_H_

#include <functional>
#include <algorithm>


namespace GN {
namespace Predicates {


template < class SplineType > 
struct Finite : public std::unary_function < typename SplineType::ControlPointType, bool >
{
  typedef Finite<SplineType> ThisType;
  typedef typename SplineType::ControlPointType     ControlPointType;
  typedef typename SplineType::ControlPointArgument ControlPointArgument;
  typedef typename SplineType::ControlPointTester   ControlPointTester;
  typedef typename SplineType::Vector               Vector;

  bool operator () ( ControlPointArgument t ) const
  {
    return ControlPointTester::finite ( t );
  }

  static bool check ( const Vector &v )
  {
    return ( v.end() == std::find_if ( v.begin(), v.end(), std::not1 ( ThisType() ) ) );
  }
};


}; // namespace Predicates
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_PREDICATES_FINITE_H_
