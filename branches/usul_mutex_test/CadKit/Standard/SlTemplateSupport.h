
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlTemplateSupport: Code to support the template classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_INLINE_SUPPORT_TEMPLATES_FUNCTIONS_H_
#define _CADKIT_STANDARD_LIBRARY_INLINE_SUPPORT_TEMPLATES_FUNCTIONS_H_

#include <math.h>


namespace CadKit {


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to help create unary functors and a helper function.
//
///////////////////////////////////////////////////////////////////////////////

#define CADKIT_DECLARE_UNARY_SPECIALIZATIONS(class_name,internal_function_name,external_function_name) \
template < class Type > struct class_name \
{ \
  Type operator () ( Type value ) \
  { \
    return static_cast < Type > ( internal_function_name ( static_cast < double > ( value ) ) ); \
  } \
}; \
template <> struct class_name < double > \
{ \
  double operator () ( double value ) \
  { \
    return internal_function_name ( value ); \
  } \
}; \
template < typename Type > Type external_function_name ( Type value ) \
{ \
  return class_name < Type > () ( value ); \
}



///////////////////////////////////////////////////////////////////////////////
//
//  Macro to help create binary functors and a helper function.
//
///////////////////////////////////////////////////////////////////////////////

#define CADKIT_DECLARE_BINARY_SPECIALIZATIONS(class_name,internal_function_name,external_function_name) \
template < class Type > struct class_name \
{ \
  Type operator () ( Type a, Type b ) \
  { \
    return static_cast < Type > ( internal_function_name ( static_cast < double > ( a ), static_cast < double > ( b ) ) ); \
  } \
}; \
template <> struct class_name < double > \
{ \
  double operator () ( double a, double b ) \
  { \
    return internal_function_name ( a, b ); \
  } \
}; \
template < typename Type > Type external_function_name ( Type a, Type b ) \
{ \
  return class_name < Type > () ( a, b ); \
}



///////////////////////////////////////////////////////////////////////////////
//
//  Declare math functors that take a single argument.
//
///////////////////////////////////////////////////////////////////////////////

CADKIT_DECLARE_UNARY_SPECIALIZATIONS ( Sine,       ::sin,  getSine );
CADKIT_DECLARE_UNARY_SPECIALIZATIONS ( Cosine,     ::cos,  getCosine );
CADKIT_DECLARE_UNARY_SPECIALIZATIONS ( Tangent,    ::tan,  getTangent );
CADKIT_DECLARE_UNARY_SPECIALIZATIONS ( ArcSine,    ::asin, getArcSine );
CADKIT_DECLARE_UNARY_SPECIALIZATIONS ( ArcCosine,  ::acos, getArcCosine );
CADKIT_DECLARE_UNARY_SPECIALIZATIONS ( ArcTangent, ::atan, getArcTangent );
CADKIT_DECLARE_UNARY_SPECIALIZATIONS ( SquareRoot, ::sqrt, getSquareRoot );


///////////////////////////////////////////////////////////////////////////////
//
//  Declare math functors that take a two arguments.
//
///////////////////////////////////////////////////////////////////////////////

CADKIT_DECLARE_BINARY_SPECIALIZATIONS ( ArcTangent2, ::atan2, getArcTangent2 );


///////////////////////////////////////////////////////////////////////////////
//
// For backward compatibility.
//
///////////////////////////////////////////////////////////////////////////////

#define SL_SQRT           CadKit::getSquareRoot
#define SL_SINE           CadKit::getSine
#define SL_COSINE         CadKit::getCosine
#define SL_TANGENT        CadKit::getTangent
#define SL_ARC_SINE       CadKit::getArcSine
#define SL_ARC_COSINE     CadKit::getArcCosine
#define SL_ARC_TANGENT    CadKit::getArcTangent
#define SL_ARC_TANGENT_2  CadKit::getArcTangent2


// Clean up.
#undef CADKIT_DECLARE_UNARY_SPECIALIZATIONS
#undef CADKIT_DECLARE_BINARY_SPECIALIZATIONS


}; // namespace CadKit


#endif // _CADKIT_STANDARD_LIBRARY_INLINE_SUPPORT_TEMPLATES_FUNCTIONS_H_
