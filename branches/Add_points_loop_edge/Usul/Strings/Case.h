
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Return lower or upper case string.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_STRINGS_CASE_H_
#define _USUL_STRINGS_CASE_H_

#include <algorithm>
#include <string>


namespace Usul {
namespace Strings {


///////////////////////////////////////////////////////////////////////////////
//
//  Get the string as lower-case.
//
///////////////////////////////////////////////////////////////////////////////

inline void lowerCase ( const std::string &in, std::string &out )
{
  out = in;
  std::transform ( out.begin(), out.end(), out.begin(), ::tolower );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the string as lower-case.
//
///////////////////////////////////////////////////////////////////////////////

inline std::string lowerCase ( const std::string &in )
{
  std::string out;
  Usul::Strings::lowerCase ( in, out );
  return out;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the string as upper-case.
//
///////////////////////////////////////////////////////////////////////////////

inline void upperCase ( const std::string &in, std::string &out )
{
  out = in;
  std::transform ( out.begin(), out.end(), out.begin(), ::toupper );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the string as upper-case.
//
///////////////////////////////////////////////////////////////////////////////

inline std::string upperCase ( const std::string &in )
{
  std::string out;
  Usul::Strings::upperCase ( in, out );
  return out;
}


}; // namespace Strings
}; // namespace Usul


#endif // _USUL_STRINGS_CASE_H_
