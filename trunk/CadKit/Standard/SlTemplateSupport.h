
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

///////////////////////////////////////////////////////////////////////////////
//
//  SlTemplateSupport: Overloaded functions so we can use them in templates.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_INLINE_SUPPORT_TEMPLATES_FUNCTIONS_H_
#define _CADKIT_STANDARD_LIBRARY_INLINE_SUPPORT_TEMPLATES_FUNCTIONS_H_

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <math.h>
#endif // _CADKIT_USE_PRECOMPILED_HEADERS


namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  Overload these functions so we can use them in templates.
//
///////////////////////////////////////////////////////////////////////////////

inline long double getSquareRoot ( const long double &num ) { return ::sqrtl ( num ); }
inline double      getSquareRoot ( const double &num )      { return ::sqrt  ( num ); }
inline float       getSquareRoot ( const float &num )       { return ::sqrtf ( num ); }
inline int         getSquareRoot ( const int  &num )        { return static_cast<int>   ( ::sqrt ( static_cast<double> ( num ) ) ); }
inline long        getSquareRoot ( const long &num )        { return static_cast<long>  ( ::sqrt ( static_cast<double> ( num ) ) ); }
inline short       getSquareRoot ( const short &num )       { return static_cast<short> ( ::sqrt ( static_cast<double> ( num ) ) ); }

inline long double getSine ( const long double &num ) { return ::sinl ( num ); }
inline double      getSine ( const double &num )      { return ::sin  ( num ); }
inline float       getSine ( const float &num )       { return ::sinf ( num ); }

inline long double getCosine ( const long double &num ) { return ::cosl ( num ); }
inline double      getCosine ( const double &num )      { return ::cos  ( num ); }
inline float       getCosine ( const float &num )       { return ::cosf ( num ); }

inline long double getTangent ( const long double &num ) { return ::tanl ( num ); }
inline double      getTangent ( const double &num )      { return ::tan  ( num ); }
inline float       getTangent ( const float &num )       { return ::tanf ( num ); }

inline long double getArcSine ( const long double &num ) { return ::asinl ( num ); }
inline double      getArcSine ( const double &num )      { return ::asin  ( num ); }
inline float       getArcSine ( const float &num )       { return ::asinf ( num ); }

inline long double getArcCosine ( const long double &num ) { return ::acosl ( num ); }
inline double      getArcCosine ( const double &num )      { return ::acos  ( num ); }
inline float       getArcCosine ( const float &num )       { return ::acosf ( num ); }

inline long double getArcTangent ( const long double &num ) { return ::atanl ( num ); }
inline double      getArcTangent ( const double &num )      { return ::atan  ( num ); }
inline float       getArcTangent ( const float &num )       { return ::atanf ( num ); }

inline long double getArcTangent2 ( const long double &y, const long double &x ) { return ::atan2l ( y, x ); }
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
