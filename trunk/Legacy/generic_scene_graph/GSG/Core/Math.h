
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

#ifndef _GENERIC_SCENE_GRAPH_CORE_MATH_H_
#define _GENERIC_SCENE_GRAPH_CORE_MATH_H_

#include "GSG/Core/Constants.h"


// Both cmath and math.h can be troublesome.
extern "C"
{
  double sin   ( double );
  double cos   ( double );
  double tan   ( double );
  double asin  ( double );
  double acos  ( double );
  double atan  ( double );
  double sqrt  ( double );
  double atan2 ( double, double );
  double pow   ( double, double );
};


namespace GSG {
namespace Math {


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to help create unary functors and a Detail function.
//
///////////////////////////////////////////////////////////////////////////////

#define GSG_DECLARE_UNARY_SPECIALIZATIONS(class_name,internal_function_name,external_function_name) \
namespace Detail \
{ \
  template < class Type > struct class_name \
  { \
    Type operator () ( Type value ) const \
    { \
      return static_cast < Type > ( internal_function_name ( static_cast < double > ( value ) ) ); \
    } \
  }; \
  template <> struct class_name < double > \
  { \
    double operator () ( double value ) const \
    { \
      return internal_function_name ( value ); \
    } \
  }; \
}; \
template < typename Type > inline Type external_function_name ( Type value ) \
{ \
  return Detail::class_name < Type > () ( value ); \
}



///////////////////////////////////////////////////////////////////////////////
//
//  Macro to help create binary functors and a Detail function.
//
///////////////////////////////////////////////////////////////////////////////

#define GSG_DECLARE_BINARY_SPECIALIZATIONS(class_name,internal_function_name,external_function_name) \
namespace Detail \
{ \
  template < class Type > struct class_name \
  { \
    Type operator () ( Type a, Type b ) const \
    { \
      return static_cast < Type > ( internal_function_name ( static_cast < double > ( a ), static_cast < double > ( b ) ) ); \
    } \
  }; \
  template <> struct class_name < double > \
  { \
    double operator () ( double a, double b ) const \
    { \
      return internal_function_name ( a, b ); \
    } \
  }; \
}; \
template < typename Type > inline Type external_function_name ( Type a, Type b ) \
{ \
  return Detail::class_name < Type > () ( a, b ); \
}


///////////////////////////////////////////////////////////////////////////////
//
//  Declare math functors that take a single argument.
//
///////////////////////////////////////////////////////////////////////////////

GSG_DECLARE_UNARY_SPECIALIZATIONS ( Sine,       ::sin,  sin );
GSG_DECLARE_UNARY_SPECIALIZATIONS ( Cosine,     ::cos,  cos );
GSG_DECLARE_UNARY_SPECIALIZATIONS ( Tangent,    ::tan,  tan );
GSG_DECLARE_UNARY_SPECIALIZATIONS ( ArcSine,    ::asin, asin );
GSG_DECLARE_UNARY_SPECIALIZATIONS ( ArcCosine,  ::acos, acos );
GSG_DECLARE_UNARY_SPECIALIZATIONS ( ArcTangent, ::atan, atan );
GSG_DECLARE_UNARY_SPECIALIZATIONS ( SquareRoot, ::sqrt, sqrt );


///////////////////////////////////////////////////////////////////////////////
//
//  Declare math functors that take a two arguments.
//
///////////////////////////////////////////////////////////////////////////////

GSG_DECLARE_BINARY_SPECIALIZATIONS ( ArcTangent2, ::atan2, atan2 );
GSG_DECLARE_BINARY_SPECIALIZATIONS ( Power,       ::pow,   pow );


///////////////////////////////////////////////////////////////////////////////
//
//  For making "pow" for integer types.
//
///////////////////////////////////////////////////////////////////////////////

#define GSG_DECLARE_INTEGER_POWER_FUNCTIONS(Type_) \
template < typename Number_ > inline Number_ pow ( Number_ n, Type_ p ) \
{ \
  return ( !p ) ? 1 : n * pow ( n, p - 1 ); \
}

///////////////////////////////////////////////////////////////////////////////
//
//  Special case "pow" for integer types.
//
///////////////////////////////////////////////////////////////////////////////

GSG_DECLARE_INTEGER_POWER_FUNCTIONS ( unsigned char  );
GSG_DECLARE_INTEGER_POWER_FUNCTIONS ( unsigned short );
GSG_DECLARE_INTEGER_POWER_FUNCTIONS ( unsigned int   );
GSG_DECLARE_INTEGER_POWER_FUNCTIONS ( unsigned long  );


///////////////////////////////////////////////////////////////////////////////
//
//  Clean up.
//
///////////////////////////////////////////////////////////////////////////////

#undef GSG_DECLARE_UNARY_SPECIALIZATIONS
#undef GSG_DECLARE_BINARY_SPECIALIZATIONS


/////////////////////////////////////////////////////////////////////////////
//
//  Return the absolute value.
//
/////////////////////////////////////////////////////////////////////////////

template < class T > inline T abs ( T v )
{
  return ( v > 0 ) ? v : -v;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Convert radians to degrees.
//
/////////////////////////////////////////////////////////////////////////////

template < class T > inline T radToDeg ( T v )
{
  return v * RAD_TO_DEG;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Convert degrees to radians.
//
/////////////////////////////////////////////////////////////////////////////

template < class T > inline T degToRad ( T v )
{
  return v * DEG_TO_RAD;
}


}; // namespace Math
}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_MATH_H_
