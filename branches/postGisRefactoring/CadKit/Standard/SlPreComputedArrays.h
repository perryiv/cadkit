
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlPreComputedArrays: Pre-computed arrays.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_MATH_H_
#define _CADKIT_STANDARD_LIBRARY_MATH_H_

#include "SlTypedefs.h"
#include "SlAssert.h"


namespace CadKit 
{
class SlPreComputedArrays
{
public:

  // Array sizes.
  enum
  {
    NUM_BINOMIAL_COEFFICIENT_INT_32    = 34,
    NUM_BINOMIAL_COEFFICIENT_UINT_32   = 34,
    NUM_BINOMIAL_COEFFICIENT_INT_64    = 67,
    NUM_BINOMIAL_COEFFICIENT_UINT_64   = 67,
    NUM_FACTORIAL_INT_32               = 13,
    NUM_FACTORIAL_UINT_32              = 13,
    NUM_FACTORIAL_INT_64               = 21,
    NUM_FACTORIAL_UINT_64              = 21,
    NUM_SPHERE_VERTS_0                 = 180,
    NUM_SPHERE_VERTS_1                 = 720,
    NUM_SPHERE_VERTS_2                 = 2880,
    NUM_SPHERE_VERTS_3                 = 11520,
    NUM_SPHERE_VERTS_4                 = 46080,
  };

  // Return the binomial coefficient, n is on top, k is on bottom.
  // The binomial coefficient of n over k = n! / ( ( n - k )! * k! ), 
  // but it's also Pascal's Triangle.
  static SlFloat64          getBinomialCoefficient ( SlFloat64 p, SlInt32 k );
  static SlInt32            getBinomialCoefficient ( SlInt32 n,   SlInt32 k );
  static SlUint32           getBinomialCoefficient ( SlUint32 n,  SlUint32 k );
  static SlInt64            getBinomialCoefficient ( SlInt64 n,   SlInt64 k );
  static SlUint64           getBinomialCoefficient ( SlUint64 n,  SlUint64 k );

  // Return the factorial.
  static SlInt32            getFactorial ( SlInt32 n );
  static SlUint32           getFactorial ( SlUint32 n );
  static SlInt64            getFactorial ( SlInt64 n );
  static SlUint64           getFactorial ( SlUint64 n );

  // Return the size of the sphere vertex arrays.
  static SlUint32           getNumSphereVerts0() { return NUM_SPHERE_VERTS_0; }
  static SlUint32           getNumSphereVerts1() { return NUM_SPHERE_VERTS_1; }
  static SlUint32           getNumSphereVerts2() { return NUM_SPHERE_VERTS_2; }
  static SlUint32           getNumSphereVerts3() { return NUM_SPHERE_VERTS_3; }
  static SlUint32           getNumSphereVerts4() { return NUM_SPHERE_VERTS_4; }

  // Return the sphere vertex arrays.
  static const SlFloat32 *  getSphereVerts0() { return _sphereVerts0; }
  static const SlFloat32 *  getSphereVerts1() { return _sphereVerts1; }
  static const SlFloat32 *  getSphereVerts2() { return _sphereVerts2; }
  static const SlFloat32 *  getSphereVerts3() { return _sphereVerts3; }
  static const SlFloat32 *  getSphereVerts4() { return _sphereVerts4; }

protected:

  // See note: 075b03f0-8d78-11d3-9848-0040054c86c7.
  static const SlInt32  _factorialInt32 [NUM_FACTORIAL_INT_32];
  static const SlUint32 _factorialUint32[NUM_FACTORIAL_UINT_32];
  static const SlInt64  _factorialInt64 [NUM_FACTORIAL_INT_64];
  static const SlUint64 _factorialUint64[NUM_FACTORIAL_UINT_64];

  static const SlInt32  _pascalsTriangleInt32 [NUM_BINOMIAL_COEFFICIENT_INT_32] [NUM_BINOMIAL_COEFFICIENT_INT_32];
  static const SlUint32 _pascalsTriangleUint32[NUM_BINOMIAL_COEFFICIENT_UINT_32][NUM_BINOMIAL_COEFFICIENT_UINT_32];
  static const SlInt64  _pascalsTriangleInt64 [NUM_BINOMIAL_COEFFICIENT_INT_64] [NUM_BINOMIAL_COEFFICIENT_INT_64];
  static const SlUint64 _pascalsTriangleUint64[NUM_BINOMIAL_COEFFICIENT_UINT_64][NUM_BINOMIAL_COEFFICIENT_UINT_64];

  static const SlFloat32 _sphereVerts0[NUM_SPHERE_VERTS_0];
  static const SlFloat32 _sphereVerts1[NUM_SPHERE_VERTS_1];
  static const SlFloat32 _sphereVerts2[NUM_SPHERE_VERTS_2];
  static const SlFloat32 _sphereVerts3[NUM_SPHERE_VERTS_3];
  static const SlFloat32 _sphereVerts4[NUM_SPHERE_VERTS_4];
};


///////////////////////////////////////////////////////////////////////////////
//
//  Return the factorial.
//
///////////////////////////////////////////////////////////////////////////////

inline SlInt32 SlPreComputedArrays::getFactorial ( SlInt32 n )
{
  SL_ASSERT ( n >= 0 && n < NUM_FACTORIAL_INT_32 );
  return _factorialInt32[n];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the factorial.
//
///////////////////////////////////////////////////////////////////////////////

inline SlUint32 SlPreComputedArrays::getFactorial ( SlUint32 n )
{
  SL_ASSERT ( n < NUM_FACTORIAL_UINT_32 );
  return _factorialUint32[n];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the factorial.
//
///////////////////////////////////////////////////////////////////////////////

inline SlInt64 SlPreComputedArrays::getFactorial ( SlInt64 n )
{
  SL_ASSERT ( n >= 0 && n < NUM_FACTORIAL_INT_64 );
  return _factorialInt64[n];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the factorial.
//
///////////////////////////////////////////////////////////////////////////////

inline SlUint64 SlPreComputedArrays::getFactorial ( SlUint64 n )
{
  SL_ASSERT ( n < NUM_FACTORIAL_UINT_64 );
  return _factorialUint64[n];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the binomial coefficient, n is on top, k is on bottom.
//  The binomial coefficient of n over k = n! / ( ( n - k )! * k! ), 
//
///////////////////////////////////////////////////////////////////////////////

inline SlFloat64 SlPreComputedArrays::getBinomialCoefficient ( SlFloat64 p, SlInt32 k )
{
  SL_ASSERT ( k >= 0 );
  
  SlFloat64 num = 1.0;
  for ( SlInt32 i=0; i<k; i++ ) num *= ( p - ((SlFloat64) i) );
  return ( num / SlPreComputedArrays::getFactorial ( k ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the binomial coefficient, n is on top, k is on bottom.
//  The binomial coefficient of n over k = n! / ( ( n - k )! * k! ), 
//
///////////////////////////////////////////////////////////////////////////////

inline SlInt32 SlPreComputedArrays::getBinomialCoefficient ( SlInt32 n, SlInt32 k )
{
  SL_ASSERT ( n >= 0 && n < NUM_BINOMIAL_COEFFICIENT_INT_32 );
  SL_ASSERT ( k >= 0 && k <= n );
  return _pascalsTriangleInt32[n][k];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the binomial coefficient, n is on top, k is on bottom.
//  The binomial coefficient of n over k = n! / ( ( n - k )! * k! ), 
//
///////////////////////////////////////////////////////////////////////////////

inline SlUint32 SlPreComputedArrays::getBinomialCoefficient ( SlUint32 n, SlUint32 k )
{
  SL_ASSERT ( n < NUM_BINOMIAL_COEFFICIENT_UINT_32 );
  SL_ASSERT ( k <= n );
  return _pascalsTriangleUint32[n][k];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the binomial coefficient, n is on top, k is on bottom.
//  The binomial coefficient of n over k = n! / ( ( n - k )! * k! ), 
//
///////////////////////////////////////////////////////////////////////////////

inline SlInt64 SlPreComputedArrays::getBinomialCoefficient ( SlInt64 n, SlInt64 k )
{
  SL_ASSERT ( n >= 0 && n < NUM_BINOMIAL_COEFFICIENT_INT_64 );
  SL_ASSERT ( k >= 0 && k <= n );
  return _pascalsTriangleInt64[n][k];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the binomial coefficient, n is on top, k is on bottom.
//  The binomial coefficient of n over k = n! / ( ( n - k )! * k! ), 
//
///////////////////////////////////////////////////////////////////////////////

inline SlUint64 SlPreComputedArrays::getBinomialCoefficient ( SlUint64 n, SlUint64 k )
{
  SL_ASSERT ( n < NUM_BINOMIAL_COEFFICIENT_INT_64 );
  SL_ASSERT ( k <= n );
  return _pascalsTriangleUint64[n][k];
}


}; // namespace CadKit.


#endif

