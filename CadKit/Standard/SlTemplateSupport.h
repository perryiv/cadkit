
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

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <math.h>
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Macros to help define the "long double" math functions.
//
///////////////////////////////////////////////////////////////////////////////

#if _WIN32 || __sgi
# define _CADKIT_LONG_DOUBLE_STANDARD_MATH_FUNCTION_1(function,num) function##l ( num )
# define _CADKIT_LONG_DOUBLE_STANDARD_MATH_FUNCTION_2(function,num1,num2) function##l ( num1, num2 )
#elif __GNUC__
# define _CADKIT_LONG_DOUBLE_STANDARD_MATH_FUNCTION_1(function,num) \
  static_cast < long double > ( function ( static_cast < double > ( num ) ) )
# define _CADKIT_LONG_DOUBLE_STANDARD_MATH_FUNCTION_2(function,num1,num2) \
  static_cast < long double > ( function ( static_cast < double > ( num1 ), static_cast < double > ( num2 ) ) )
#else
TODO
#endif


namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  Overload these functions so we can use them in templates.
//
///////////////////////////////////////////////////////////////////////////////

inline long double getSquareRoot ( const long double &num ) { return _CADKIT_LONG_DOUBLE_STANDARD_MATH_FUNCTION_1 ( ::sqrt, num ); }
inline double      getSquareRoot ( const double &num )      { return ::sqrt  ( num ); }
inline float       getSquareRoot ( const float &num )       { return ::sqrtf ( num ); }
inline int         getSquareRoot ( const int  &num )        { return static_cast<int>   ( ::sqrt ( static_cast<double> ( num ) ) ); }
inline long        getSquareRoot ( const long &num )        { return static_cast<long>  ( ::sqrt ( static_cast<double> ( num ) ) ); }
inline short       getSquareRoot ( const short &num )       { return static_cast<short> ( ::sqrt ( static_cast<double> ( num ) ) ); }

inline long double getSine ( const long double &num ) { return _CADKIT_LONG_DOUBLE_STANDARD_MATH_FUNCTION_1 ( ::sin, num ); }
inline double      getSine ( const double &num )      { return ::sin  ( num ); }
inline float       getSine ( const float &num )       { return ::sinf ( num ); }

inline long double getCosine ( const long double &num ) { return _CADKIT_LONG_DOUBLE_STANDARD_MATH_FUNCTION_1 ( ::cos, num ); }
inline double      getCosine ( const double &num )      { return ::cos  ( num ); }
inline float       getCosine ( const float &num )       { return ::cosf ( num ); }

inline long double getTangent ( const long double &num ) { return _CADKIT_LONG_DOUBLE_STANDARD_MATH_FUNCTION_1 ( ::tan, num ); }
inline double      getTangent ( const double &num )      { return ::tan  ( num ); }
inline float       getTangent ( const float &num )       { return ::tanf ( num ); }

inline long double getArcSine ( const long double &num ) { return _CADKIT_LONG_DOUBLE_STANDARD_MATH_FUNCTION_1 ( ::asin, num ); }
inline double      getArcSine ( const double &num )      { return ::asin  ( num ); }
inline float       getArcSine ( const float &num )       { return ::asinf ( num ); }

inline long double getArcCosine ( const long double &num ) { return _CADKIT_LONG_DOUBLE_STANDARD_MATH_FUNCTION_1 ( ::acos, num ); }
inline double      getArcCosine ( const double &num )      { return ::acos  ( num ); }
inline float       getArcCosine ( const float &num )       { return ::acosf ( num ); }

inline long double getArcTangent ( const long double &num ) { return _CADKIT_LONG_DOUBLE_STANDARD_MATH_FUNCTION_1 ( ::atan, num ); }
inline double      getArcTangent ( const double &num )      { return ::atan  ( num ); }
inline float       getArcTangent ( const float &num )       { return ::atanf ( num ); }

inline long double getArcTangent2 ( const long double &y, const long double &x ) { return _CADKIT_LONG_DOUBLE_STANDARD_MATH_FUNCTION_2 ( ::atan2, y, x ); }
inline double      getArcTangent2 ( const double &y,      const double &x )      { return ::atan2  ( y, x ); }
inline float       getArcTangent2 ( const float &y,       const float &x )       { return ::atan2f ( y, x ); }


}; // namespace CadKit


///////////////////////////////////////////////////////////////////////////////
//
// For convenience.
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


#endif // _CADKIT_STANDARD_LIBRARY_INLINE_SUPPORT_TEMPLATES_FUNCTIONS_H_
