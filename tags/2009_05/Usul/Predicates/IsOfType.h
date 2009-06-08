
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Predicate class for finding an instance of the same type.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_PREDICATES_IS_OF_TYPE_H_
#define _USUL_PREDICATES_IS_OF_TYPE_H_

#include <string>


namespace Usul {
namespace Predicates {


template < class FindType_ > struct IsOfType : std::unary_function < FindType_, bool >
{
  template < class T > bool operator () ( const T *t )
  {
    return ( 0x0 != dynamic_cast < const FindType_ * > ( t ) );
  }
};


} // namespace Predicates
} // namespace Usul


#endif // _USUL_PREDICATES_IS_OF_TYPE_H_
