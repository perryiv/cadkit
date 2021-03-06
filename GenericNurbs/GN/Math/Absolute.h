
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Absolute value.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_MATH_ABSOLUTE_VALUE_H_
#define _GENERIC_NURBS_LIBRARY_MATH_ABSOLUTE_VALUE_H_


namespace GN {
namespace Math {


template < class T > inline T absolute ( T v )
{
  return ( v > 0 ) ? v : -v;
}


}; // namespace Math
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_MATH_ABSOLUTE_VALUE_H_
