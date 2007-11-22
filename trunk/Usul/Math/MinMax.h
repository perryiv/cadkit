
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


/////////////////////////////////////////////////////////////////////////////
//
//  Return the minimum value.
//
/////////////////////////////////////////////////////////////////////////////

template < class T, class Pred > inline const T & minimum ( const T &t1, const T &t2, const T &t3, const T &t4, Pred pred )
{
  return ( Usul::Math::minimum ( Usul::Math::minimum ( t1, t2, pred ), Usul::Math::minimum ( t3, t4, pred ), pred ) );
}
template < class T > inline const T & minimum ( const T &t1, const T &t2, const T &t3, const T &t4 )
{
  return ( Usul::Math::minimum ( Usul::Math::minimum ( t1, t2 ), Usul::Math::minimum ( t3, t4 ) ) );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Return the minimum value.
//
/////////////////////////////////////////////////////////////////////////////

template < class T, class Pred > inline const T & minimum ( const T &t1, const T &t2, const T &t3, const T &t4, const T &t5, Pred pred )
{
  return ( Usul::Math::minimum ( Usul::Math::minimum ( t1, t2, pred ), Usul::Math::minimum ( t3, t4, t5, pred ), pred ) );
}
template < class T > inline const T & minimum ( const T &t1, const T &t2, const T &t3, const T &t4, const T &t5 )
{
  return ( Usul::Math::minimum ( Usul::Math::minimum ( t1, t2 ), Usul::Math::minimum ( t3, t4, t5 ) ) );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Clamp the value between min and max.
//
/////////////////////////////////////////////////////////////////////////////

template < typename T > inline T clamp ( const T& value, const T &min, const T& max )
{
  return minimum ( maximum ( value, min ), max );
}

} // namespace Math
} // namespace Usul


#endif // _USUL_MATH_MIN_MAX_H_
