
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//  SlConstants: Typesafe constant definitions.
//
//////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_CONSTANTS_H_
#define _CADKIT_STANDARD_LIBRARY_CONSTANTS_H_

#include "SlTypedefs.h"
#include "SlDeclareConst.h"


namespace CadKit 
{
// Misc.
const SlFloat64 SL_POWELL_TAU_D      = 0.6180339887499;  // Powell tau d in 64 bit precision
const SlFloat32 SL_POWELL_TAU_F      = 0.6180339887499f; // Powell tau d in 32 bit precision
const SlFloat64 SL_LOG_POWELL_TAU_D  = -0.20898764025;   // log of Powell tau d in 64 bit precision
const SlFloat32 SL_LOG_POWELL_TAU_F  = -0.20898764025f;  // log of Powell tau d in 32 bit precision
const SlFloat64 SL_INFINITY_D        = 1e20;
const SlFloat32 SL_INFINITY_F        = 1e20f;
const SlFloat64 SL_ZERO_D            = 1e-6;
const SlFloat32 SL_ZERO_F            = 1e-6f;

// Time.
const SlUint32 SL_MIN_DAY_OF_MONTH   = 1;
const SlUint32 SL_MAX_DAY_OF_MONTH   = 31;
const SlUint32 SL_MIN_MONTH          = 1;
const SlUint32 SL_MAX_MONTH          = 12;

// Unit conversions.
const SlFloat64 SL_RAD_TO_DEG_D      = 57.2957795131;    // Converts a number in radians to a number in degrees.
const SlFloat32 SL_RAD_TO_DEG_F      = 57.2957795131f;   // Converts a number in radians to a number in degrees.
const SlFloat64 SL_DEG_TO_RAD_D      = 0.01745329252;    // Converts a number in degrees to a number in radians.
const SlFloat32 SL_DEG_TO_RAD_F      = 0.01745329252f;   // Converts a number in degrees to a number in radians.
const SlFloat64 SL_METERS_TO_FEET_D  = 3.28083989501;
const SlFloat64 SL_METERS_TO_FEET_F  = 3.28083989501f;

// Floating Limits.
const SlFloat64 SL_MIN_64_BIT_FLOAT = -1.7e308;
const SlFloat64 SL_MIN_32_BIT_FLOAT = -3.4e38f;
const SlFloat64 SL_MAX_32_BIT_FLOAT =  3.4e38f;
const SlFloat64 SL_MAX_64_BIT_FLOAT =  1.7e308;

// Integer limits.
const SlUint8  SL_MAX_UNSIGNED_8_BIT_INT   = 255;
const SlUint16 SL_MAX_UNSIGNED_16_BIT_INT  = 65535;                                    // 0xffff.
const SlUint32 SL_MAX_UNSIGNED_32_BIT_INT  = SL_CONST_UINT32 ( 4294967295 );           // 0xffffffff.
const SlUint64 SL_MAX_UNSIGNED_64_BIT_INT  = SL_CONST_UINT64 ( 18446744073709551615 ); // 0xffffffffffffffff.
const SlInt8   SL_MAX_SIGNED_8_BIT_INT     = 127;
const SlInt16  SL_MAX_SIGNED_16_BIT_INT    = 32767;
const SlInt32  SL_MAX_SIGNED_32_BIT_INT    = 2147483647;
const SlInt64  SL_MAX_SIGNED_64_BIT_INT    = SL_CONST_INT64 ( 9223372036854775807 );
const SlInt8   SL_MIN_SIGNED_8_BIT_INT     = -128;
const SlInt16  SL_MIN_SIGNED_16_BIT_INT    = -32768;
const SlInt32  SL_MIN_SIGNED_32_BIT_INT    = -SL_MAX_SIGNED_32_BIT_INT - 1;
const SlInt64  SL_MIN_SIGNED_64_BIT_INT    = -SL_MAX_SIGNED_64_BIT_INT - 1;

// Log.
const SlFloat64 SL_E_D      = 2.7182818284590452354;
const SlFloat32 SL_E_F      = 2.7182818284590452354f;
const SlFloat64 SL_LOG2E_D  = 1.4426950408889634074;   // Log (base 10) 2E
const SlFloat32 SL_LOG2E_F  = 1.4426950408889634074f;  // Log (base 10) 2E
const SlFloat64 SL_LOG10E_D = 0.43429448190325182765;  // Log (base 10) 10E
const SlFloat32 SL_LOG10E_F = 0.43429448190325182765f; // Log (base 10) 10E
const SlFloat64 SL_LN2_D    = 0.69314718055994530942;  // Natural Log of 2
const SlFloat32 SL_LN2_F    = 0.69314718055994530942f; // Natural Log of 2
const SlFloat64 SL_LN10_D   = 2.30258509299404568402;  // Natural Log of 10
const SlFloat32 SL_LN10_F   = 2.30258509299404568402f; // Natural Log of 10

// Trig.
const SlFloat64 SL_PI_D             = 3.14159265358979323846264338327950288419716939937510;
const SlFloat32 SL_PI_F             = 3.14159265358979323846f; // PI, 32 bit precision
const SlFloat32 SL_2_PI_F           = 6.28318530718f;          // 2 * PI, 32 bit precision
const SlFloat64 SL_2_PI_D           = 6.28318530718;           // 2 * PI, 64 bit precision
const SlFloat64 SL_INV_PI_D         = 0.31830988618379067154;  // 1 / PI, 64 bit precision
const SlFloat32 SL_INV_PI_F         = 0.31830988618379067154f; // 1 / PI, 32 bit precision
const SlFloat64 SL_INV_2_PI_D       = 0.159154943092;          // 1 / (2*PI), 64 bit precision
const SlFloat32 SL_INV_2_PI_F       = 0.159154943092f;         // 1 / (2*PI), 32 bit precision
const SlFloat64 SL_PI_OVER_2_D      = 1.57079632679489661923;  // PI / 2, 64 bit precision
const SlFloat32 SL_PI_OVER_2_F      = 1.57079632679489661923f; // PI / 2, 32 bit precision
const SlFloat64 SL_PI_OVER_4_D      = 0.78539816339744830962;  // PI / 4, 64 bit precision
const SlFloat32 SL_PI_OVER_4_F      = 0.78539816339744830962f; // PI / 4, 32 bit precision
const SlFloat64 SL_2_OVER_PI_D      = 0.63661977236758134308;  // 2 / PI, 64 bit precision
const SlFloat32 SL_2_OVER_PI_F      = 0.63661977236758134308f; // 2 / PI, 32 bit precision
const SlFloat64 SL_2_OVER_SQRT_PI_D = 1.12837916709551257390;  // 2 / sqrt(PI), 64 bit precision
const SlFloat32 SL_2_OVER_SQRT_PI_F = 1.12837916709551257390f; // 2 / sqrtf(PI), 32 bit precision
const SlFloat64 SL_2_PI_OVER_3_D    = 2.0943951023933334;      // (2 * PI) / 3, 64 bit precision
const SlFloat32 SL_2_PI_OVER_3_F    = 2.0943951023933334f;     // (2 * PI) / 3, 32 bit precision

// Square Root.
const SlFloat64 SL_SQRT_2_D     = 1.41421356237309504880;  // Same as sqrt(2.0)
const SlFloat32 SL_SQRT_2_F     = 1.41421356237309504880f; // Same as sqrtf(2.0f)
const SlFloat64 SL_INV_SQRT_2_D = 0.70710678118654752440;  // Same as 1.0 / sqrt(2.0)
const SlFloat32 SL_INV_SQRT_2_F = 0.70710678118654752440f; // Same as 1.0f / sqrtf(2.0f)
const SlFloat64 SL_SQRT_3_D     = 1.73205080757;           // Same as sqrt(3.0)
const SlFloat32 SL_SQRT_3_F     = 1.73205080757f;          // Same as sqrtf(3.0f)
const SlFloat64 SL_INV_SQRT_3_D = 0.577350269189;          // Same as 1.0 / sqrt(3.0)
const SlFloat32 SL_INV_SQRT_3_F = 0.577350269189f;         // Same as 1.0f / sqrtf(3.0f)

// Special characters. Note: these are #defined instead of true constants 
// because if we introduce wchar_t we need <wchar.h>, plus we would need 
// 4 definitions instead of just 2 (2 for char, 2 for wchar_t).
#define SL_CHAR_COPYRIGHT      = 0xA9;
#define SL_CHAR_BULLET_SOLID   = 0xF9;
};

#endif

