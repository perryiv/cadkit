
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Min and max values.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_MATH_MIN_MAX_H_
#define _USUL_MATH_MIN_MAX_H_

#include <functional>

namespace Usul {
namespace Math {


/////////////////////////////////////////////////////////////////////////////
//
//  Return the minimum value.
//
/////////////////////////////////////////////////////////////////////////////

template < class T, class Pred > inline const T & minimum ( const T &t1, const T &t2, Pred pred )
{
  return ( pred ( t1, t2 ) ) ? t1 : t2;
}
template < class T > inline const T & minimum ( const T &t1, const T &t2 )
{
  return ( ( t1 < t2 ) ? t1 : t2 );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Return the maximum value.
//
/////////////////////////////////////////////////////////////////////////////

template < class T, class Pred > inline const T & maximum ( const T &t1, const T &t2, Pred pred )
{
  return ( pred ( t1, t2 ) ) ? t1 : t2;
}
template < class T > inline const T & maximum ( const T &t1, const T &t2 )
{
  return ( ( t1 > t2 ) ? t1 : t2 );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Return the minimum value.
//
/////////////////////////////////////////////////////////////////////////////

template < class T, class Pred > inline const T & minimum ( const T &t1, const T &t2, const T &t3, Pred pred )
{
  return ( pred ( t1, t2 ) ) ? pred ( t1, t3 ) : pred ( t2, t3 );
}
template < class T > inline const T & minimum ( const T &t1, const T &t2, const T &t3 )
{
  return ( ( t1 < t2 ) ? ( Usul::Math::minimum ( t1, t3 ) ) : Usul::Math::minimum ( t2, t3 ) );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Return the maximum value.
//
/////////////////////////////////////////////////////////////////////////////

template < class T, class Pred > inline const T & maximum ( const T &t1, const T &t2, const T &t3, Pred pred )
{
  return ( pred ( t1, t2 ) ) ? pred ( t1, t3 ) : pred ( t2, t3 );
}
template < class T > inline const T & maximum ( const T &t1, const T &t2, const T &t3 )
{
  return ( ( t1 > t2 ) ? ( Usul::Math::maximum ( t1, t3 ) ) : Usul::Math::maximum ( t2, t3 ) );
}


}; // namespace Math
}; // namespace Usul


#endif // _USUL_MATH_MIN_MAX_H_
