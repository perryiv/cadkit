
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Predicate classes to test if one vector is "less" than another.
//
//  If vector a is "equal" to vector b, then a is not less than b.
//  We check this case first in order to accommodate special tests for 
//  "equal", such as Usul::Predicates::CloseFloat.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_PREDICATES_LESS_VECTOR_H_
#define _USUL_PREDICATES_LESS_VECTOR_H_

#include "Usul/MPL/SameType.h"


namespace Usul {
namespace Predicates {


///////////////////////////////////////////////////////////////////////////////
//
//  Generic definition.
//
///////////////////////////////////////////////////////////////////////////////

template < class E, unsigned int Size > struct LessVector;


///////////////////////////////////////////////////////////////////////////////
//
//  Specialization for 3D vector.
//
///////////////////////////////////////////////////////////////////////////////

template < class E > struct LessVector < E, 3 >
{
  // Useful typedefs.
  typedef E EqualPred;

  // Require client to set the equality functor. Rely on compiler
  // for copy constructor and assignment operator.
  LessVector ( EqualPred e ) : _equal ( e ){}

  // See if vector a is "less" than vector b.
  template < class V > bool operator () ( const V &a, const V &b ) const
  {
    // Require this to be true.
    USUL_ASSERT_SAME_TYPE ( typename EqualPred::first_argument_type, 
                            typename EqualPred::second_argument_type );

    // First, see if they are equal. See note at the top.
    if ( _equal ( a[0], b[0] ) && _equal ( a[1], b[1] ) && _equal ( a[2], b[2] ) )
      return false;

    // Now perform same algorithm as "osg::Vec3f::operator<()"
    if ( a[0] < b[0] )
      return true;
    else if ( a[0] > b[0] )
      return false;
    else if ( a[1] < b[1] )
      return true;
    else if ( a[1] > b[1] )
      return false;
    else
      return ( a[2] < b[2] );
  }

private:

  EqualPred _equal;
};


} // namespace Predicates
} // namespace Usul


#endif // _USUL_PREDICATES_LESS_VECTOR_H_
