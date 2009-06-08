
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Predicate classes to test if one vector is "equal" than another.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_PREDICATES_EQUAL_VECTOR_H_
#define _USUL_PREDICATES_EQUAL_VECTOR_H_

#include "Usul/MPL/SameType.h"


namespace Usul {
namespace Predicates {


///////////////////////////////////////////////////////////////////////////////
//
//  Generic definition.
//
///////////////////////////////////////////////////////////////////////////////

template < class E, unsigned int Size > struct EqualVector;


///////////////////////////////////////////////////////////////////////////////
//
//  Specialization for 3D vector.
//
///////////////////////////////////////////////////////////////////////////////

template < class E > struct EqualVector < E, 3 >
{
  // Require client to set the equality functor. Rely on compiler
  // for copy constructor and assignment operator.
  EqualVector ( E e ) : _equal ( e ){}

  // See if vector a is equal to vector b.
  template < class V > bool operator () ( const V &a, const V &b ) const
  {
    // Require this to be true.
    USUL_ASSERT_SAME_TYPE ( typename E::first_argument_type, 
                            typename E::second_argument_type );

    // See if they are equal.
    return ( _equal ( a[0], b[0] ) && _equal ( a[1], b[1] ) && _equal ( a[2], b[2] ) );
  }

private:

  E _equal;
};


} // namespace Predicates
} // namespace Usul


#endif // _USUL_PREDICATES_EQUAL_VECTOR_H_
