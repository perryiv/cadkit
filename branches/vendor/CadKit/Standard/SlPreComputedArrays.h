
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
//  SlPreComputedArrays: Pre-computed arrays.
//
//////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_MATH_H_
#define _CADKIT_STANDARD_LIBRARY_MATH_H_

#include "SlTypedefs.h"
#include "SlAssert.h"


namespace CadKit 
{
class SlPreComputedArrays
{
public:

  // Return the binomial coefficient, n is on top, k is on bottom.
  // The binomial coefficient of n over k = n! / ( ( n - k )! * k! ), 
  // but it's also Pascal's Triangle.
  static SlFloat64          getBinomialCoefficient ( SlFloat64 p, SlInt32 k );
  static SlInt32            getBinomialCoefficient ( SlInt32 n,   SlInt32 k )  { SL_ASSERT ( n >= 0 && n < 34 && k >= 0 && k <= n ); return _pascalsTriangleInt32 [n][k]; }
  static SlUint32           getBinomialCoefficient ( SlUint32 n,  SlUint32 k ) { SL_ASSERT ( n >= 0 && n < 34 && k >= 0 && k <= n ); return _pascalsTriangleUint32[n][k]; }
  static SlInt64            getBinomialCoefficient ( SlInt64 n,   SlInt64 k )  { SL_ASSERT ( n >= 0 && n < 67 && k >= 0 && k <= n ); return _pascalsTriangleInt64 [n][k]; }
  static SlUint64           getBinomialCoefficient ( SlUint64 n,  SlUint64 k ) { SL_ASSERT ( n >= 0 && n < 67 && k >= 0 && k <= n ); return _pascalsTriangleUint64[n][k]; }

  // Return the factorial.
  static SlInt32            getFactorial ( SlInt32 n )  { SL_ASSERT ( n >= 0 && n < 13 ); return _factorialInt32[n]; }
  static SlUint32           getFactorial ( SlUint32 n ) { SL_ASSERT ( n >= 0 && n < 13 ); return _factorialUint32[n]; }
  static SlInt64            getFactorial ( SlInt64 n )  { SL_ASSERT ( n >= 0 && n < 21 ); return _factorialInt64[n]; }
  static SlUint64           getFactorial ( SlUint64 n ) { SL_ASSERT ( n >= 0 && n < 21 ); return _factorialUint64[n]; }

  // Return the size of the sphere vertex arrays.
  static const SlInt32 &    getNumSphereVerts0() { return _numSphereVerts0; }
  static const SlInt32 &    getNumSphereVerts1() { return _numSphereVerts1; }
  static const SlInt32 &    getNumSphereVerts2() { return _numSphereVerts2; }
  static const SlInt32 &    getNumSphereVerts3() { return _numSphereVerts3; }
  static const SlInt32 &    getNumSphereVerts4() { return _numSphereVerts4; }

  // Return the sphere vertex arrays.
  static const SlFloat32 *  getSphereVerts0() { return _sphereVerts0; }
  static const SlFloat32 *  getSphereVerts1() { return _sphereVerts1; }
  static const SlFloat32 *  getSphereVerts2() { return _sphereVerts2; }
  static const SlFloat32 *  getSphereVerts3() { return _sphereVerts3; }
  static const SlFloat32 *  getSphereVerts4() { return _sphereVerts4; }

protected:

  // See note: 075b03f0-8d78-11d3-9848-0040054c86c7.
  static const SlInt32  _factorialInt32 [13];
  static const SlUint32 _factorialUint32[13];
  static const SlInt64  _factorialInt64 [21];
  static const SlUint64 _factorialUint64[21];

  static const SlInt32  _pascalsTriangleInt32 [34][34];
  static const SlUint32 _pascalsTriangleUint32[34][34];
  static const SlInt64  _pascalsTriangleInt64 [67][67];
  static const SlUint64 _pascalsTriangleUint64[67][67];

  static const SlInt32 _numSphereVerts0;
  static const SlInt32 _numSphereVerts1;
  static const SlInt32 _numSphereVerts2;
  static const SlInt32 _numSphereVerts3;
  static const SlInt32 _numSphereVerts4;

  static const SlFloat32 _sphereVerts0[180];
  static const SlFloat32 _sphereVerts1[720];
  static const SlFloat32 _sphereVerts2[2880];
  static const SlFloat32 _sphereVerts3[11520];
  static const SlFloat32 _sphereVerts4[46080];
};


//////////////////////////////////////////////////////////////////////////
//
//  Return the binomial coefficient, n is on top, k is on bottom.
//  The binomial coefficient of n over k = n! / ( ( n - k )! * k! ), 
//
//////////////////////////////////////////////////////////////////////////

inline SlFloat64 SlPreComputedArrays::getBinomialCoefficient ( SlFloat64 p, SlInt32 k )
{
  SL_ASSERT ( k >= 0 );
  
  SlFloat64 num = 1.0;
  for ( SlInt32 i=0; i<k; i++ ) num *= ( p - ((SlFloat64) i) );
  return ( num / SlPreComputedArrays::getFactorial ( k ) );
}

};


#endif

