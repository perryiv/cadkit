
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

#include <iomanip>
#include <limits>
#include <string>
#include <sstream>

namespace Usul
{
  namespace Strings
  {
    // Helper classes to only set floating point output.
    namespace Helper
    {
      template < class T > struct SetStream
      {
        SetStream ( std::ostream & )
        {
        }
      };
      template < class T > inline void setFloatStream ( std::ostream &out )
      {
        out.setf ( std::ios_base::fixed, std::ios_base::floatfield );
        out.precision ( std::numeric_limits<T>::digits10 );
      }
      template <> struct SetStream<float>
      {
        SetStream ( std::ostream &out )
        {
          setFloatStream<float> ( out );
        }
      };
      template <> struct SetStream<double>
      {
        SetStream ( std::ostream &out )
        {
          setFloatStream<double> ( out );
        }
      };
      template <> struct SetStream<long double>
      {
        SetStream ( std::ostream &out )
        {
          setFloatStream<long double> ( out );
        }
      };
    }

    /// Convert t to a string.
    inline std::string toString ( bool t )
    {
      return ( ( t ) ? "true" : "false" );
    }
    template < typename T >
    inline std::string toString ( const T& t )
    {
      std::ostringstream os;
      Helper::SetStream<T> setStream ( os );
      os << t;
      return os.str();
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
