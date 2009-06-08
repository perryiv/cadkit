
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Linear interpolation.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_ALGORITHMS_LINERA_INTERPOLATION_H_
#define _GENERIC_NURBS_LIBRARY_ALGORITHMS_LINERA_INTERPOLATION_H_

#include <functional>


namespace GN {
namespace Math {


template < class V > struct LinearBlend : std::binary_function < typename V::value_type, typename V::value_type, typename V::value_type >
{
  typedef typename V::value_type ValueType;
  LinearBlend ( const ValueType &s ) : _scale ( s )
  {
  }
  ValueType operator () ( const ValueType &a, const ValueType &b )
  {
    return ( ( a + b ) * _scale );
  }
private:
  ValueType _scale;
};


}; // namespace Math
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_ALGORITHMS_LINERA_INTERPOLATION_H_
