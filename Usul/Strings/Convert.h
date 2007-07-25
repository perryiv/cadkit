
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_STRINGS_CONVERT_H__
#define __USUL_STRINGS_CONVERT_H__

#include <string>
#include <sstream>

namespace Usul
{
  namespace Strings
  {
    /// Convert t to a string.
    template < typename T >
    std::string toString( const T& t )
    {
      std::ostringstream os;
      os << t;
      return os.str();
    }

    /// Convert a string to type T.
    template < typename T >
    void fromString ( const std::string& s, T& t )
    {
      std::istringstream in ( s );
      in >> t;
    }
  }
}


#endif // __USUL_STRINGS_CONVERT_H__
