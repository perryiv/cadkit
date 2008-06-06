
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//  SlConstants: Typesafe constant definitions.
//
//////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_CONSTANTS_H_
#define _CADKIT_STANDARD_LIBRARY_CONSTANTS_H_

#include "SlDeclareConst.h"


namespace CadKit 
{
// Misc.
const double SL_POWELL_TAU_D      =  0.6180339887499;  // Powell tau d in 64 bit precision
const float  SL_POWELL_TAU_F      =  0.6180339887499f; // Powell tau d in 32 bit precision
const double SL_LOG_POWELL_TAU_D  = -0.20898764025;    // log of Powell tau d in 64 bit precision
const float  SL_LOG_POWELL_TAU_F  = -0.20898764025f;   // log of Powell tau d in 32 bit precision

// Unit conversions.
const double SL_RAD_TO_DEG_D      = 57.2957795131;    // Converts radians to degrees.
const float  SL_RAD_TO_DEG_F      = 57.2957795131f;   // Converts radians to degrees.
const double SL_DEG_TO_RAD_D      = 0.01745329252;    // Converts degrees to radians.
const float  SL_DEG_TO_RAD_F      = 0.01745329252f;   // Converts degrees to radians.
const double SL_METERS_TO_FEET_D  = 3.28083989501;
const double SL_METERS_TO_FEET_F  = 3.28083989501f;

// Log.
const double SL_E_D      = 2.7182818284590452354;
const float  SL_E_F      = 2.7182818284590452354f;
const double SL_LOG2E_D  = 1.4426950408889634074;   // Log (base 10) 2E
const float  SL_LOG2E_F  = 1.4426950408889634074f;  // Log (base 10) 2E
const double SL_LOG10E_D = 0.43429448190325182765;  // Log (base 10) 10E
const float  SL_LOG10E_F = 0.43429448190325182765f; // Log (base 10) 10E
const double SL_LN2_D    = 0.69314718055994530942;  // Natural Log of 2
const float  SL_LN2_F    = 0.69314718055994530942f; // Natural Log of 2
const double SL_LN10_D   = 2.30258509299404568402;  // Natural Log of 10
const float  SL_LN10_F   = 2.30258509299404568402f; // Natural Log of 10

// Trig.
const double SL_PI_D             = 3.14159265358979323846264338327950288419716939937510;
const float  SL_PI_F             = 3.14159265358979323846f; // PI, 32 bit precision
const float  SL_2_PI_F           = 6.28318530718f;          // 2 * PI, 32 bit precision
const double SL_2_PI_D           = 6.28318530718;           // 2 * PI, 64 bit precision
const double SL_INV_PI_D         = 0.31830988618379067154;  // 1 / PI, 64 bit precision
const float  SL_INV_PI_F         = 0.31830988618379067154f; // 1 / PI, 32 bit precision
const double SL_INV_2_PI_D       = 0.159154943092;          // 1 / (2*PI), 64 bit precision
const float  SL_INV_2_PI_F       = 0.159154943092f;         // 1 / (2*PI), 32 bit precision
const double SL_PI_OVER_2_D      = 1.57079632679489661923;  // PI / 2, 64 bit precision
const float  SL_PI_OVER_2_F      = 1.57079632679489661923f; // PI / 2, 32 bit precision
const double SL_PI_OVER_4_D      = 0.78539816339744830962;  // PI / 4, 64 bit precision
const float  SL_PI_OVER_4_F      = 0.78539816339744830962f; // PI / 4, 32 bit precision
const double SL_2_OVER_PI_D      = 0.63661977236758134308;  // 2 / PI, 64 bit precision
const float  SL_2_OVER_PI_F      = 0.63661977236758134308f; // 2 / PI, 32 bit precision
const double SL_2_OVER_SQRT_PI_D = 1.12837916709551257390;  // 2 / sqrt(PI), 64 bit precision
const float  SL_2_OVER_SQRT_PI_F = 1.12837916709551257390f; // 2 / sqrtf(PI), 32 bit precision
const double SL_2_PI_OVER_3_D    = 2.0943951023933334;      // (2 * PI) / 3, 64 bit precision
const float  SL_2_PI_OVER_3_F    = 2.0943951023933334f;     // (2 * PI) / 3, 32 bit precision

// Square Root.
const double SL_SQRT_2_D     = 1.41421356237309504880;  // Same as sqrt(2.0)
const float  SL_SQRT_2_F     = 1.41421356237309504880f; // Same as sqrtf(2.0f)
const double SL_INV_SQRT_2_D = 0.70710678118654752440;  // Same as 1.0 / sqrt(2.0)
const float  SL_INV_SQRT_2_F = 0.70710678118654752440f; // Same as 1.0f / sqrtf(2.0f)
const double SL_SQRT_3_D     = 1.73205080757;           // Same as sqrt(3.0)
const float  SL_SQRT_3_F     = 1.73205080757f;          // Same as sqrtf(3.0f)
const double SL_INV_SQRT_3_D = 0.577350269189;          // Same as 1.0 / sqrt(3.0)
const float  SL_INV_SQRT_3_F = 0.577350269189f;         // Same as 1.0f / sqrtf(3.0f)


///////////////////////////////////////////////////////////////////////////////
//
//  This magic line is needed for the explicit declarations that follow, 
//  or else they do not compile.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> class SlConstants{};


///////////////////////////////////////////////////////////////////////////////
//
//  For platform independence of the template magic below.
//
///////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && _MSC_VER <= 1200
# define _CADKIT_TEMPLATE_CLASS
#else
# define _CADKIT_TEMPLATE_CLASS template<>
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for declaring a class that returns integer constants.
//
///////////////////////////////////////////////////////////////////////////////

#define CADKIT_DECLARE_CONSTANT_CLASS_INTEGER(_Type,_zero,_one,_two) \
_CADKIT_TEMPLATE_CLASS class SlConstants<_Type> \
{ \
public: \
  static _Type zero() { return _zero; } \
  static _Type one()  { return _one; } \
  static _Type two()  { return _two; } \
}


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for declaring a class that returns floating-point constants.
//
///////////////////////////////////////////////////////////////////////////////

#define CADKIT_DECLARE_CONSTANT_CLASS_FLOAT(_Type,_zero,_half,_one,_two) \
_CADKIT_TEMPLATE_CLASS class SlConstants<_Type> \
{ \
public: \
  static _Type zero() { return _zero; } \
  static _Type half() { return _half; } \
  static _Type one()  { return _one; } \
  static _Type two()  { return _two; } \
}


///////////////////////////////////////////////////////////////////////////////
//
//  Explicit declarations of floating-point types.
//
///////////////////////////////////////////////////////////////////////////////

CADKIT_DECLARE_CONSTANT_CLASS_FLOAT ( long double, 0.0,  0.5,  1.0,  2.0  );
CADKIT_DECLARE_CONSTANT_CLASS_FLOAT (      double, 0.0,  0.5,  1.0,  2.0  );
CADKIT_DECLARE_CONSTANT_CLASS_FLOAT (      float,  0.0f, 0.5f, 1.0f, 2.0f );


///////////////////////////////////////////////////////////////////////////////
//
//  Explicit declarations of integer types.
//
///////////////////////////////////////////////////////////////////////////////

CADKIT_DECLARE_CONSTANT_CLASS_INTEGER (          char,  0, 1, 2 );
CADKIT_DECLARE_CONSTANT_CLASS_INTEGER ( unsigned char,  0, 1, 2 );
CADKIT_DECLARE_CONSTANT_CLASS_INTEGER (          int,   0, 1, 2 );
CADKIT_DECLARE_CONSTANT_CLASS_INTEGER ( unsigned int,   0, 1, 2 );
CADKIT_DECLARE_CONSTANT_CLASS_INTEGER (          short, 0, 1, 2 );
CADKIT_DECLARE_CONSTANT_CLASS_INTEGER ( unsigned short, 0, 1, 2 );
CADKIT_DECLARE_CONSTANT_CLASS_INTEGER (          long,  0, 1, 2 );
CADKIT_DECLARE_CONSTANT_CLASS_INTEGER ( unsigned long,  0, 1, 2 );
};


#endif // _CADKIT_STANDARD_LIBRARY_CONSTANTS_H_
