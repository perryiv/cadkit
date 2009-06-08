
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

#include "Usul/Scope/StreamState.h"

#include <string>
#include <sstream>

namespace Usul
{
  namespace Strings
  {
    /// Convert t to a string.
    inline std::string toString ( bool t )
    {
      return ( ( t ) ? "true" : "false" );
    }
    template < typename T >
    inline std::string toString ( const T& t )
    {
      std::ostringstream out;
      Usul::Scope::Stream::MaxFloatPrecision<T> precision ( out );
      out << t;
      return out.str();
    }

    /// Convert a string to type T.
    inline void fromString ( const std::string& s, bool& t )
    {
      t = ( ( s == "0" || s == "false" || s == "FALSE" ) ? false : true );
    }

    /// Convert a string to type T.
    template < typename T >
    inline void fromString ( const std::string& s, T& t )
    {
      std::istringstream in ( s );
      in >> t;
    }

    /// Convert a string to type T.
    template < typename T >
    inline T fromString ( const std::string& s )
    {
      T t = T();
      Usul::Strings::fromString ( s, t );
      return t;
    }
  }
}


#endif // __USUL_STRINGS_CONVERT_H__
