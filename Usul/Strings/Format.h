
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  String formatting functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_STRINGS_FORMAT_H_
#define _USUL_STRINGS_FORMAT_H_

#include <sstream>


namespace Usul {
namespace Strings {


///////////////////////////////////////////////////////////////////////////////
//
//  Format the string.
//
///////////////////////////////////////////////////////////////////////////////

template < class T1, class T2 > 
inline std::string format ( const T1 &t1, const T2 &t2 )
{
  std::ostringstream out;
  out << t1 << t2;
  return out.str();
}
template < class T1, class T2, class T3 > 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3 )
{
  std::ostringstream out;
  out << t1 << t2 << t3;
  return out.str();
}
template < class T1, class T2, class T3, class T4 > 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4;
  return out.str();
}
template < class T1, class T2, class T3, class T4, class T5 > 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5;
  return out.str();
}
template < class T1, class T2, class T3, class T4, class T5, class T6 > 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5 << t6;
  return out.str();
}
template < class T1, class T2, class T3, class T4, class T5, class T6, class T7 > 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5 << t6 << t7;
  return out.str();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Format date string. Pass __DATE__ macro.
//
///////////////////////////////////////////////////////////////////////////////

inline std::string formatDate ( const std::string &d )
{
  // If the string is not this size then the logic below does not work.
  if ( 11 != d.size() )
    return d;

  const std::string year  ( d.begin() + 7, d.end() );
  const std::string month ( d.begin() + 0, d.begin() + 3 );
  const std::string day   ( d.begin() + 4, d.begin() + 6 );

  std::ostringstream out;
  out << day << '-' << month << '-' << year;

  return out.str();
}


}; // namespace Strings
}; // namespace Usul


#endif // _USUL_STRINGS_FORMAT_H_
