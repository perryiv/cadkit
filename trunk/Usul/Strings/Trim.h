
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Trim functions. Could probably do this with boost/algorithm/string/ but 
//  I don't want Usul to depend on boost.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_STRINGS_TRIM_H_
#define _USUL_STRINGS_TRIM_H_

#include <algorithm>
#include <string>


namespace Usul {
namespace Strings {


///////////////////////////////////////////////////////////////////////////////
//
//  Remove trailing characters.
//
///////////////////////////////////////////////////////////////////////////////

inline bool trimRight ( std::string &s, char c = ' ' )
{
  if ( !s.empty() )
  {
    const std::string::size_type last ( s.size() - 1 );
    if ( c == s[last] )
    {
      s.resize ( last );
      return true;
    }
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove all trailing characters.
//
///////////////////////////////////////////////////////////////////////////////

inline void trimRightAll ( std::string &s, char c = ' ' )
{
  while ( true )
  {
    if ( false == Usul::Strings::trimRight ( s, c ) )
      break;
  }
}


} // namespace Strings
} // namespace Usul


#endif // _USUL_STRINGS_TRIM_H_
