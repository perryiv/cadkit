
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlMinMax: Inline functions for finding the minimum and maximum.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_INLINE_MIN_MAX_H_
#define _CADKIT_STANDARD_LIBRARY_INLINE_MIN_MAX_H_

#include <algorithm>

#ifdef _WIN32
# ifdef  max
#  undef max
# endif
# ifdef  min
#  undef min
# endif
#endif


namespace CadKit {


///////////////////////////////////////////////////////////////////////////////
//
//  Several overloaded max functions.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline const T &max ( const T &a, const T &b )
{
  return std::max ( a, b );
}
template<class T> inline const T &max ( const T &a, const T &b, const T &c ) 
{
  return CadKit::max ( CadKit::max ( a, b ), c );
}
template<class T> inline const T &max ( const T &a, const T &b, const T &c, const T &d )
{
  return CadKit::max ( CadKit::max ( a, b, c ), d );
}
template<class T> inline const T &max ( const T &a, const T &b, const T &c, const T &d, const T &e )
{
  return CadKit::max ( CadKit::max ( a, b, c, d ), e );
}
template<class T> inline const T &max ( const T &a, const T &b, const T &c, const T &d, const T &e, const T &f )
{
  return CadKit::max ( CadKit::max ( a, b, c, d, e ), f );
}
template<class T> inline const T &max ( const T &a, const T &b, const T &c, const T &d, const T &e, const T &f, const T &g )
{
  return CadKit::max ( CadKit::max ( a, b, c, d, e, f ), g );
}
template<class T> inline const T &max ( const T &a, const T &b, const T &c, const T &d, const T &e, const T &f, const T &g, const T &h )
{
  return CadKit::max ( CadKit::max ( a, b, c, d, e, f, g ), h );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Several overloaded min functions.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline const T &min ( const T &a, const T &b )
{
  return std::min ( a, b );
}
template<class T> inline const T &min ( const T &a, const T &b, const T &c ) 
{
  return CadKit::min ( CadKit::min ( a, b ), c );
}
template<class T> inline const T &min ( const T &a, const T &b, const T &c, const T &d )
{
  return CadKit::min ( CadKit::min ( a, b, c ), d );
}
template<class T> inline const T &min ( const T &a, const T &b, const T &c, const T &d, const T &e )
{
  return CadKit::min ( CadKit::min ( a, b, c, d ), e );
}
template<class T> inline const T &min ( const T &a, const T &b, const T &c, const T &d, const T &e, const T &f )
{
  return CadKit::min ( CadKit::min ( a, b, c, d, e ), f );
}
template<class T> inline const T &min ( const T &a, const T &b, const T &c, const T &d, const T &e, const T &f, const T &g )
{
  return CadKit::min ( CadKit::min ( a, b, c, d, e, f ), g );
}
template<class T> inline const T &min ( const T &a, const T &b, const T &c, const T &d, const T &e, const T &f, const T &g, const T &h )
{
  return CadKit::min ( CadKit::min ( a, b, c, d, e, f, g ), h );
}


}; // namespace CadKit.


#endif // _CADKIT_STANDARD_LIBRARY_INLINE_MIN_MAX_H_
