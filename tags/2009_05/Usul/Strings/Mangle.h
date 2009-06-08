
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  String functions for mangeling. Examples include converting a url to a 
//  string that is a valid file name.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_STRINGS_MANGLE_H_
#define _USUL_STRINGS_MANGLE_H_

#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/replace.hpp"

#include <algorithm>
#include <string>


namespace Usul {
namespace Strings {
namespace Mangle {


///////////////////////////////////////////////////////////////////////////////
//
//  Mangle the input string.
//
///////////////////////////////////////////////////////////////////////////////

inline void url ( const std::string &in, std::string &out )
{
  // Handle empty strings.
  if ( true == in.empty() )
  {
    out.clear();
    return;
  }

  // Trim all characters left of the '@' character, if it's found.
  // This will remove username and password pairs.
  std::string::const_iterator i ( std::find ( in.begin(), in.end(), '@' ) );
  out.assign ( ( ( in.end() == i ) ? in.begin() : i + 1 ), in.end() );

  // Trim all characters right of the '?' character, if it's found.
  // This will remove special arguments needed in the "base" url. 
  // For example, a client ID needed to perform a WMS GetCapabilities request.
  out = std::string ( out.begin(), std::find ( out.begin(), out.end(), '?' ) );

  boost::replace_first ( out, "http://", " " );
  boost::replace_first ( out, "https://", " " );
  boost::trim_left ( out );

  std::replace ( out.begin(), out.end(), '@',  '_' );
  std::replace ( out.begin(), out.end(), ':',  '_' );
  std::replace ( out.begin(), out.end(), '/',  '_' );
  std::replace ( out.begin(), out.end(), '\\', '_' );
  std::replace ( out.begin(), out.end(), '&',  '_' );
  std::replace ( out.begin(), out.end(), '?',  '_' );
  std::replace ( out.begin(), out.end(), '.',  '_' );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Mangle the input string.
//
///////////////////////////////////////////////////////////////////////////////

inline std::string url ( const std::string &in )
{
  std::string out;
  Usul::Strings::Mangle::url ( in, out );
  return out;
}


} // namespace Mangle
} // namespace Strings
} // namespace Usul


#endif // _USUL_STRINGS_MANGLE_H_
