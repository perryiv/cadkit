
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

#include <sstream>


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
  static void from ( const std::string& s, the_type &v ) \
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
  static void from ( const std::string& s, the_type &v ) \
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
  static void from ( const std::string& s, the_type &v ) \
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
  static void from ( const std::string& s, the_type &v ) \
  { \
    std::istringstream in ( s ); \
    in >> v[0] >> v[1] >> v[2] >> v[3]; \
  } \
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper macro to define scalar and derived converters.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_REGISTRY_DEFINE_CONVERTER(postfix,the_type) \
  USUL_REGISTRY_DEFINE_CONVERTER_SCALAR ( the_type ); \
  USUL_REGISTRY_DEFINE_CONVERTER_VECTOR_2 ( Usul::Math::Vec2##postfix ); \
  USUL_REGISTRY_DEFINE_CONVERTER_VECTOR_3 ( Usul::Math::Vec3##postfix ); \
  USUL_REGISTRY_DEFINE_CONVERTER_VECTOR_4 ( Usul::Math::Vec4##postfix )

///////////////////////////////////////////////////////////////////////////////
//
//  Define some common converters.
//
///////////////////////////////////////////////////////////////////////////////

USUL_REGISTRY_DEFINE_CONVERTER_SCALAR ( std::string );
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
