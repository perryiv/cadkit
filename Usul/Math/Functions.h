
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Math functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_MATH_H_
#define _USUL_MATH_H_


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to define THROW for the various platforms.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef __sgi
  #define _USUL_THROW
#elif __CYGWIN__
  #define _USUL_THROW
#elif _MSC_VER
  #define _USUL_THROW
#elif __APPLE__
  #define _USUL_THROW
#else
  #define _USUL_THROW throw()
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to define _In_ for the various platforms.
//  This fixes a warning with Visual Studio 2008.
//  See: http://msdn.microsoft.com/en-us/library/cc664879.aspx
//
///////////////////////////////////////////////////////////////////////////////

#if defined ( _MSC_VER ) && _MSC_VER > 1400
#include <sal.h>
#define _USUL_IN _In_
#else
#define _USUL_IN
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Both cmath and math.h are a pain. 
//
//  Note: Needed to add 'throws()' so that Linux gcc would accept the 
//  prototypes in a source file that also included math.h. I still think this 
//  is better than including math.h or cmath here. I've been down that road.
//
///////////////////////////////////////////////////////////////////////////////

extern "C"
{
  double sin   ( _USUL_IN double ) _USUL_THROW;
  double cos   ( _USUL_IN double ) _USUL_THROW;
  double tan   ( _USUL_IN double ) _USUL_THROW;
  double asin  ( _USUL_IN double ) _USUL_THROW;
  double acos  ( _USUL_IN double ) _USUL_THROW;
  double atan  ( _USUL_IN double ) _USUL_THROW;
  double sqrt  ( _USUL_IN double ) _USUL_THROW;
  double atan2 ( _USUL_IN double, _USUL_IN double ) _USUL_THROW;
  double pow   ( _USUL_IN double, _USUL_IN double ) _USUL_THROW;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Clean up.
//
///////////////////////////////////////////////////////////////////////////////

#undef _USUL_THROW
#undef _USUL_IN

namespace Usul {
namespace Math {


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to help create unary functors and a Detail function.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_DECLARE_UNARY_SPECIALIZATIONS(class_name,internal_function_name,external_function_name) \
namespace Detail \
{ \
  template < class Type > struct class_name \
  { \
    static Type calculate ( Type value ) \
    { \
      return static_cast < Type > ( internal_function_name ( static_cast < double > ( value ) ) ); \
    } \
  }; \
  template <> struct class_name < double > \
  { \
    static double calculate ( double value ) \
    { \
      return internal_function_name ( value ); \
    } \
  }; \
}; \
template < typename Type > inline Type external_function_name ( Type value ) \
{ \
  return Detail::class_name<Type>::calculate ( value ); \
}



///////////////////////////////////////////////////////////////////////////////
//
//  Macro to help create binary functors and a Detail function.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_DECLARE_BINARY_SPECIALIZATIONS(class_name,internal_function_name,external_function_name) \
namespace Detail \
{ \
  template < class Type > struct class_name \
  { \
    static Type calculate ( Type a, Type b ) \
    { \
      return static_cast < Type > ( internal_function_name ( static_cast < double > ( a ), static_cast < double > ( b ) ) ); \
    } \
  }; \
  template <> struct class_name < double > \
  { \
    static double calculate ( double a, double b ) \
    { \
      return internal_function_name ( a, b ); \
    } \
  }; \
}; \
template < typename Type > inline Type external_function_name ( Type a, Type b ) \
{ \
  return Detail::class_name<Type>::calculate ( a, b ); \
}


///////////////////////////////////////////////////////////////////////////////
//
//  Declare math functors that take a single argument.
//
///////////////////////////////////////////////////////////////////////////////

USUL_DECLARE_UNARY_SPECIALIZATIONS ( Sine,       ::sin,  sin );
USUL_DECLARE_UNARY_SPECIALIZATIONS ( Cosine,     ::cos,  cos );
USUL_DECLARE_UNARY_SPECIALIZATIONS ( Tangent,    ::tan,  tan );
USUL_DECLARE_UNARY_SPECIALIZATIONS ( ArcSine,    ::asin, asin );
USUL_DECLARE_UNARY_SPECIALIZATIONS ( ArcCosine,  ::acos, acos );
USUL_DECLARE_UNARY_SPECIALIZATIONS ( ArcTangent, ::atan, atan );
USUL_DECLARE_UNARY_SPECIALIZATIONS ( SquareRoot, ::sqrt, sqrt );


///////////////////////////////////////////////////////////////////////////////
//
//  Declare math functors that take two arguments.
//
///////////////////////////////////////////////////////////////////////////////

USUL_DECLARE_BINARY_SPECIALIZATIONS ( ArcTangent2, ::atan2, atan2 );
USUL_DECLARE_BINARY_SPECIALIZATIONS ( Power,       ::pow, pow );


///////////////////////////////////////////////////////////////////////////////
//
//  Clean up.
//
///////////////////////////////////////////////////////////////////////////////

#undef USUL_DECLARE_UNARY_SPECIALIZATIONS
#undef USUL_DECLARE_BINARY_SPECIALIZATIONS


} // namespace Math
} // namespace Usul


#endif // _USUL_MATH_H_
