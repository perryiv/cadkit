
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Equality comparison operators. Use this header sparingly, it tends to 
//  create conflicts.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_MATH_TEMPLATE_EQUALITY_OPERATOR_H_
#define _USUL_MATH_TEMPLATE_EQUALITY_OPERATOR_H_


namespace Usul {
namespace Math {


///////////////////////////////////////////////////////////////////////////////
//
//  Equality.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > inline bool operator == ( const T &a, typename T &b )
{
  return a.equal ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Equality.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > inline bool operator != ( const T &a, typename T &b )
{
  return !( a.equal ( b ) );
}


}; // namespace Math
}; // namespace Usul


#endif // _USUL_MATH_TEMPLATE_EQUALITY_OPERATOR_H_
