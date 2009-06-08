
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

template<class T> inline const T &max ( const T &a, const T &b, const T &c ) 
{
  return std::max ( std::max ( a, b ), c );
}
template<class T> inline const T &max ( const T &a, const T &b, const T &c, const T &d )
{
  return std::max ( CadKit::max ( a, b, c ), d );
}
template<class T> inline const T &max ( const T &a, const T &b, const T &c, const T &d, const T &e )
{
  return std::max ( CadKit::max ( a, b, c, d ), e );
}
template<class T> inline const T &max ( const T &a, const T &b, const T &c, const T &d, const T &e, const T &f )
{
  return std::max ( CadKit::max ( a, b, c, d, e ), f );
}
template<class T> inline const T &max ( const T &a, const T &b, const T &c, const T &d, const T &e, const T &f, const T &g )
{
  return std::max ( CadKit::max ( a, b, c, d, e, f ), g );
}
template<class T> inline const T &max ( const T &a, const T &b, const T &c, const T &d, const T &e, const T &f, const T &g, const T &h )
{
  return std::max ( CadKit::max ( a, b, c, d, e, f, g ), h );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Several overloaded min functions.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline const T &min ( const T &a, const T &b, const T &c ) 
{
  return std::min ( std::min ( a, b ), c );
}
template<class T> inline const T &min ( const T &a, const T &b, const T &c, const T &d )
{
  return std::min ( CadKit::min ( a, b, c ), d );
}
template<class T> inline const T &min ( const T &a, const T &b, const T &c, const T &d, const T &e )
{
  return std::min ( CadKit::min ( a, b, c, d ), e );
}
template<class T> inline const T &min ( const T &a, const T &b, const T &c, const T &d, const T &e, const T &f )
{
  return std::min ( CadKit::min ( a, b, c, d, e ), f );
}
template<class T> inline const T &min ( const T &a, const T &b, const T &c, const T &d, const T &e, const T &f, const T &g )
{
  return std::min ( CadKit::min ( a, b, c, d, e, f ), g );
}
template<class T> inline const T &min ( const T &a, const T &b, const T &c, const T &d, const T &e, const T &f, const T &g, const T &h )
{
  return std::min ( CadKit::min ( a, b, c, d, e, f, g ), h );
}


}; // namespace CadKit.


#endif // _CADKIT_STANDARD_LIBRARY_INLINE_MIN_MAX_H_
