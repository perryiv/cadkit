
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Perry Miller
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  String conversion functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_REGISTRY_STRING_CONVERSIONS_H_
#define _USUL_REGISTRY_STRING_CONVERSIONS_H_

#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector4.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Strings/Split.h"

#include <list>
#include <sstream>
#include <string>


namespace Usul { namespace Registry {


///////////////////////////////////////////////////////////////////////////////
//
//  Generic conversion not implemented by design.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > struct Convert;


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for defining scalar converters.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_REGISTRY_DEFINE_CONVERTER_SCALAR(the_type) \
template <> struct Convert < the_type > \
{ \
  static std::string to ( const the_type &v ) \
  { \
    std::ostringstream os; \
    os << v; \
    return os.str(); \
  } \
  static void from ( const std::string &s, the_type &v ) \
  { \
    std::istringstream in ( s ); \
    in >> v; \
  } \
}


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for defining vector converters.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_REGISTRY_DEFINE_CONVERTER_VECTOR_2(the_type) \
template <> struct Convert < the_type > \
{ \
  static std::string to ( const the_type &v ) \
  { \
    std::ostringstream os; \
    os << v[0] << ' ' << v[1]; \
    return os.str(); \
  } \
  static void from ( const std::string &s, the_type &v ) \
  { \
    std::istringstream in ( s ); \
    in >> v[0] >> v[1]; \
  } \
}


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for defining vector converters.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_REGISTRY_DEFINE_CONVERTER_VECTOR_3(the_type) \
template <> struct Convert < the_type > \
{ \
  static std::string to ( const the_type &v ) \
  { \
    std::ostringstream os; \
    os << v[0] << ' ' << v[1] << ' ' << v[2]; \
    return os.str(); \
  } \
  static void from ( const std::string &s, the_type &v ) \
  { \
    std::istringstream in ( s ); \
    in >> v[0] >> v[1] >> v[2]; \
  } \
}


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for defining vector converters.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_REGISTRY_DEFINE_CONVERTER_VECTOR_4(the_type) \
template <> struct Convert < the_type > \
{ \
  static std::string to ( const the_type &v ) \
  { \
    std::ostringstream os; \
    os << v[0] << ' ' << v[1] << ' ' << v[2] << ' ' << v[3]; \
    return os.str(); \
  } \
  static void from ( const std::string &s, the_type &v ) \
  { \
    std::istringstream in ( s ); \
    in >> v[0] >> v[1] >> v[2] >> v[3]; \
  } \
}


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for defining vector converters.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_REGISTRY_DEFINE_CONVERTER_MATRIX_4x4(the_type) \
template <> struct Convert < the_type > \
{ \
  static std::string to ( const the_type &m ) \
  { \
    std::ostringstream os; \
    os << m[0] << ' ' << m[4] << ' ' << m[ 8] << ' ' << m[12] << ' '; \
    os << m[1] << ' ' << m[5] << ' ' << m[ 9] << ' ' << m[13] << ' '; \
    os << m[2] << ' ' << m[6] << ' ' << m[10] << ' ' << m[14] << ' '; \
    os << m[3] << ' ' << m[7] << ' ' << m[11] << ' ' << m[15]; \
    return os.str(); \
  } \
  static void from ( const std::string &s, the_type &m ) \
  { \
    std::istringstream in ( s ); \
    in >> m[0] >> m[4] >> m[ 8] >> m[12]; \
    in >> m[1] >> m[5] >> m[ 9] >> m[13]; \
    in >> m[2] >> m[6] >> m[10] >> m[14]; \
    in >> m[3] >> m[7] >> m[11] >> m[15]; \
  } \
}


///////////////////////////////////////////////////////////////////////////////
//
//  Converter for a list of strings.
//
///////////////////////////////////////////////////////////////////////////////

template <> struct Convert < std::list < std::string > >
{
  typedef std::list < std::string > StringList;
  static std::string to ( const StringList &sl )
  {
    if ( true == sl.empty() )
      return "";

    std::ostringstream out;
    StringList::const_iterator i ( sl.begin() );
    out << ( *i ); ++i;
    for ( ; i != sl.end(); ++i )
      out << ';' << ( *i );

    return out.str();
  }
  static void from ( const std::string &s, StringList &sl )
  {
    sl.clear();

    if ( true == s.empty() )
      return;

    Usul::Strings::split ( s, ';', true, sl );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Converter for a bool.
//
///////////////////////////////////////////////////////////////////////////////

template <> struct Convert < bool >
{
  static std::string to ( const bool &b )
  {
    return b ? "true" : "false";
  }
  static void from ( const std::string &s, bool &b )
  {
    std::string copy ( s );
    std::transform ( copy.begin (), copy.end(), copy.begin (), ::tolower );
    b = ( "true" == copy ? true : false );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Converter for a string.
//
///////////////////////////////////////////////////////////////////////////////

template <> struct Convert < std::string >
{
  static std::string to ( const std::string &s )
  {
    return s;
  }
  static void from ( const std::string &s0, std::string & s1 )
  {
    s1 = s0;
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Helper macro to define scalar and derived converters.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_REGISTRY_DEFINE_CONVERTER(postfix,the_type) \
  USUL_REGISTRY_DEFINE_CONVERTER_SCALAR ( the_type ); \
  USUL_REGISTRY_DEFINE_CONVERTER_VECTOR_2 ( Usul::Math::Vec2##postfix ); \
  USUL_REGISTRY_DEFINE_CONVERTER_VECTOR_3 ( Usul::Math::Vec3##postfix ); \
  USUL_REGISTRY_DEFINE_CONVERTER_VECTOR_4 ( Usul::Math::Vec4##postfix ); \
  USUL_REGISTRY_DEFINE_CONVERTER_MATRIX_4x4 ( Usul::Math::Matrix44##postfix )

///////////////////////////////////////////////////////////////////////////////
//
//  Define some common converters.
//
///////////////////////////////////////////////////////////////////////////////

USUL_REGISTRY_DEFINE_CONVERTER ( c,  char           );
USUL_REGISTRY_DEFINE_CONVERTER ( s,  short          );
USUL_REGISTRY_DEFINE_CONVERTER ( i,  int            );
USUL_REGISTRY_DEFINE_CONVERTER ( l,  long           );
USUL_REGISTRY_DEFINE_CONVERTER ( uc, unsigned char  );
USUL_REGISTRY_DEFINE_CONVERTER ( us, unsigned short );
USUL_REGISTRY_DEFINE_CONVERTER ( ui, unsigned int   );
USUL_REGISTRY_DEFINE_CONVERTER ( ul, unsigned long  );
USUL_REGISTRY_DEFINE_CONVERTER ( f,  float          );
USUL_REGISTRY_DEFINE_CONVERTER ( d,  double         );
USUL_REGISTRY_DEFINE_CONVERTER ( ld, long double    );


} // namespace Registry
} // namespace Usul


#endif // _USUL_REGISTRY_STRING_CONVERSIONS_H_
