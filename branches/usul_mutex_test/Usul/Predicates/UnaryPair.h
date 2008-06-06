
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Pair of unary predicates.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_PREDICATES_UNARY_PAIR_H_
#define _USUL_PREDICATES_UNARY_PAIR_H_

#include "Usul/MPL/SameType.h"

#include <functional>


namespace Usul {
namespace Predicates {


///////////////////////////////////////////////////////////////////////////////
//
//  Pair of unary predicates.
//
///////////////////////////////////////////////////////////////////////////////

template < class Pred1, class Operation, class Pred2 >
struct UnaryPair : public std::unary_function < typename Pred1::argument_type, bool >
{
  typedef typename Pred1::argument_type A1;
  typedef typename Pred2::argument_type A2;
  typedef typename Pred2::result_type R1;
  typedef typename Pred2::result_type R2;
  UnaryPair ( Pred1 p1, Operation op, Pred2 p2 ) : _p1 ( p1 ), _p2 ( p2 ), _op ( op )
  {
    USUL_ASSERT_SAME_TYPE ( A1, A2 );
    USUL_ASSERT_SAME_TYPE ( R1, R2 );
  }
  R1 operator() ( A1 a ) const
  {
    return _op ( _p1 ( a ), _p2 ( a ) );
  }
private:
  Pred1 _p1;
  Pred2 _p2;
  Operation _op;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to make a pair of unary predicates.
//
///////////////////////////////////////////////////////////////////////////////

template < class Pred1, class Operation, class Pred2 > 
inline UnaryPair < Pred1, Operation, Pred2 > unaryPair ( Pred1 p1, Operation op, Pred2 p2 )
{
  return UnaryPair < Pred1, Operation, Pred2 > ( p1, op, p2 );
}


} // namespace Predicates
} // namespace Usul


#endif // _USUL_PREDICATES_UNARY_PAIR_H_
