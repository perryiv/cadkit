
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlVec3.h: A vector of 3 Ts.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_TEMPLATE_VECTOR_OF_3_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_TEMPLATE_VECTOR_OF_3_CLASS_H_

#include "SlAssert.h"
#include "SlAbsolute.h"
#include "SlTemplateSupport.h"
#include "SlConstants.h"
#include "SlClamp.h"

namespace CadKit
{
template<class T> class SlVec3
{
public:

  typedef T Real; // For readability.
  typedef T Type; // For the client.

  SlVec3();
  SlVec3 ( const T &v0, const T &v1, const T &v2 );

  void                    absolute();

  void                    bisect ( const SlVec3 &pt0, const SlVec3 &pt1 );

  void                    clamp ( const T &minValue, const T &maxValue );

  T                       dot ( const SlVec3 &vec ) const;

  SlVec3                  cross ( const SlVec3 &vec ) const;

  Real                    getAngle ( const SlVec3 &vec ) const;
  static unsigned short   getDimension() { return 3; }
  Real                    getDistance ( const SlVec3 &vec ) const;
  Real                    getDistanceSquared ( const SlVec3 &vec ) const;
  Real                    getLength() const;
  void                    getRealPart ( Real &v0, Real &v1 ) const;
  T *                     getValue() { return _v; }
  const T *               getValue() const { return _v; }
  void                    getValue ( T &v0, T &v1, T &v2 ) const;

  void                    interpolate ( const SlVec3 &pt0, const SlVec3 &pt1, const Real &u );
  bool                    isEqual ( const SlVec3 &vec ) const;
  bool                    isEqual ( const SlVec3 &vec, const T &tolerance ) const;
  bool                    isNotEqual ( const SlVec3 &vec ) const;
  bool                    isNotEqual ( const SlVec3 &vec, const T &tolerance ) const;

  const Real &            maximum() const;
  const Real &            minimum() const;

  Real                    normalize();

  // Typecast operators.
  operator       T *()       { return _v; }
  operator const T *() const { return _v; }

  // Bracket operators.
  // Note: Keep this a hard-coded type, otherwise VC++ yacks.
  T &                     operator [] ( int i );
  const T &               operator [] ( int i ) const;

  // Misc operators.
  SlVec3 &                operator *= ( const T &value );
  SlVec3 &                operator *= ( const SlVec3 &vec );
  SlVec3 &                operator /= ( const T &value );
  SlVec3 &                operator += ( const SlVec3 &vec );
  SlVec3 &                operator -= ( const SlVec3 &vec );
  SlVec3                  operator - () const;

  void                    orthogonal ( const SlVec3 &vec );

  void                    xyz2rtz(); // x-y-z to r-theta-z
  void                    rtz2xyz(); // r-theta-z to x-y-z

  void                    setLength ( const Real &length );
  void                    setValue ( const SlVec3 &vec );
  void                    setValue ( const T &v0, const T &v1, const T &v2 );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Execute the functor with all the elements. Make sure the argument is 
  //  a reference or else you will faul things up.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Functor > void operator () ( Functor &functor ) const
  {
    functor ( _v[0] );
    functor ( _v[1] );
    functor ( _v[2] );
  }

protected:

  T _v[3];
};


///////////////////////////////////////////////////////////////////////////////
//
//  Additional operators. These are not members of the class because compilers
//  vary too much in the proper syntax for friend functions in templates. 
//  See http://gcc.gnu.org/faq.html#friend and http://www.bero.org/gcc296.html
//
///////////////////////////////////////////////////////////////////////////////

template<class T> SlVec3<T> operator *  ( const SlVec3<T> &vec,  const T &value );
template<class T> SlVec3<T> operator *  ( const T &value,        const SlVec3<T> &vec );
template<class T> SlVec3<T> operator /  ( const SlVec3<T> &vec,  const T &value );
template<class T> SlVec3<T> operator +  ( const SlVec3<T> &vecA, const SlVec3<T> &vecB );
template<class T> SlVec3<T> operator -  ( const SlVec3<T> &vecA, const SlVec3<T> &vecB );
template<class T> bool      operator == ( const SlVec3<T> &vecA, const SlVec3<T> &vecB );
template<class T> bool      operator != ( const SlVec3<T> &vecA, const SlVec3<T> &vecB );

#ifdef _CADKIT_SL_VEC3_DECLARE_SCALAR_MEMBERS
template<class T> SlVec3<T> operator /  ( const SlVec3<T> &vecA, const SlVec3<T> &vecB );
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec3<T>::SlVec3()
{
  _v[0] = _v[1] = _v[2] = CadKit::SlConstants<T>::zero();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec3<T>::SlVec3 ( const T &v0, const T &v1, const T &v2 )
{
  _v[0] = v0;
  _v[1] = v1;
  _v[2] = v2;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make all the vector's components positive.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec3<T>::absolute()
{
  _v[0] = SL_ABS ( _v[0] );
  _v[1] = SL_ABS ( _v[1] );
  _v[2] = SL_ABS ( _v[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the dot product.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T SlVec3<T>::dot ( const SlVec3<T> &vec ) const
{
  return _v[0] * vec._v[0] +
         _v[1] * vec._v[1] +
         _v[2] * vec._v[2];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec3<T>::getValue ( T &v0, T &v1, T &v2 ) const
{
  v0 = _v[0];
  v1 = _v[1];
  v2 = _v[2];
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the vectors are equal within the given tolerance.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlVec3<T>::isEqual ( const SlVec3<T> &vec, const T &tolerance ) const
{
  return (
    ( SL_ABS ( _v[0] - vec._v[0] ) ) <= tolerance &&
    ( SL_ABS ( _v[1] - vec._v[1] ) ) <= tolerance &&
    ( SL_ABS ( _v[2] - vec._v[2] ) ) <= tolerance );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the vectors are not equal within the given tolerance.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlVec3<T>::isNotEqual ( const SlVec3<T> &vec, const T &tolerance ) const
{
  return ( false == this->isEqual ( vec, tolerance ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the vectors are equal.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlVec3<T>::isEqual ( const SlVec3<T> &vec ) const
{
  return (
    _v[0] == vec._v[0] && 
    _v[1] == vec._v[1] &&
    _v[2] == vec._v[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the vectors are not equal.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlVec3<T>::isNotEqual ( const SlVec3<T> &vec ) const
{
  return ( false == this->isEqual ( vec ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the vectors are equal within the given tolerance. Deliberately not 
//  a member function.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool isEqual ( const SlVec3<T> &v1, const SlVec3<T> &v2, const T &tolerance )
{
  return v1.isEqual ( v2, tolerance );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the array of vectors are equal within the given tolerance. 
//  Deliberately not a member function.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline bool isEqualArray ( const SlVec3<T> *v1, const SlVec3<T> *v2, const I &size )
{
  SL_ASSERT ( ( size > 0 && NULL != v1 && NULL != v2 ) || ( size == 0 && NULL == v1 && NULL == v2 ) );

  for ( I i = 0; i < size; ++i )
    if ( false == v1[i].isEqual ( v2[i] ) )
      return false;
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the array of vectors are equal within the given tolerance. 
//  Deliberately not a member function.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline bool isEqualArray ( const SlVec3<T> *v1, const SlVec3<T> *v2, const I &size, const T &tolerance )
{
  SL_ASSERT ( ( size > 0 && NULL != v1 && NULL != v2 ) || ( size == 0 && NULL == v1 && NULL == v2 ) );

  for ( I i = 0; i < size; ++i )
    if ( false == v1[i].isEqual ( v2[i], tolerance ) )
      return false;
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Multiply all the components by the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec3<T> &SlVec3<T>::operator *= ( const T &value )
{
  _v[0] *= value;
  _v[1] *= value;
  _v[2] *= value;

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Multiply all the components by the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec3<T> &SlVec3<T>::operator *= ( const SlVec3<T> &vec )
{
  _v[0] *= vec[0];
  _v[1] *= vec[1];
  _v[2] *= vec[2];

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Divide all the components by the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec3<T> &SlVec3<T>::operator /= ( const T &value )
{
  T invValue = CadKit::SlConstants<T>::one() / value;

  _v[0] *= invValue;
  _v[1] *= invValue;
  _v[2] *= invValue;

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the vector to this one.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec3<T> &SlVec3<T>::operator += ( const SlVec3<T> &vec )
{
  _v[0] += vec._v[0];
  _v[1] += vec._v[1];
  _v[2] += vec._v[2];

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Subtract the vector from this one.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec3<T> &SlVec3<T>::operator -= ( const SlVec3<T> &vec )
{
  _v[0] -= vec._v[0];
  _v[1] -= vec._v[1];
  _v[2] -= vec._v[2];

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the negative of this vector.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec3<T> SlVec3<T>::operator - () const
{
  return SlVec3<T> ( -_v[0], -_v[1], -_v[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the component-wise product with the given value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec3<T> operator * ( const SlVec3<T> &vec, const T &value )
{
  return SlVec3<T> ( 
    vec[0] * value, 
    vec[1] * value, 
    vec[2] * value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the component-wise product with the given value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec3<T> operator * ( const T &value, const SlVec3<T> &vec )
{
  return SlVec3<T> ( 
    vec[0] * value, 
    vec[1] * value, 
    vec[2] * value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the component-wise division with the given value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec3<T> operator / ( const SlVec3<T> &vec, const T &value )
{
  // Do not multiply by inverse because that fauls up integer vectors.
  return SlVec3<T> ( 
    vec[0] / value, 
    vec[1] / value, 
    vec[2] / value );
}


#ifdef _CADKIT_SL_VEC3_DECLARE_SCALAR_MEMBERS


///////////////////////////////////////////////////////////////////////////////
//
//  Return the component-wise division of the given vectors.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec3<T> operator / ( const SlVec3<T> &vecA, const SlVec3<T> &vecB )
{
  // Do not multiply by inverse because that fauls up integer vectors.
  return SlVec3<T> ( 
    vecA[0] / vecB[0], 
    vecA[1] / vecB[1], 
    vecA[2] / vecB[2] );
}


#endif // _CADKIT_SL_VEC3_DECLARE_SCALAR_MEMBERS


///////////////////////////////////////////////////////////////////////////////
//
//  Return the vector sum.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec3<T> operator + ( const SlVec3<T> &vecA, const SlVec3<T> &vecB )
{
  return SlVec3<T> ( 
    vecA[0] + vecB[0], 
    vecA[1] + vecB[1], 
    vecA[2] + vecB[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the vector difference.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec3<T> operator - ( const SlVec3<T> &vecA, const SlVec3<T> &vecB )
{
  return SlVec3<T> ( 
    vecA[0] - vecB[0], 
    vecA[1] - vecB[1], 
    vecA[2] - vecB[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if they're equal.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator == ( const SlVec3<T> &vecA, const SlVec3<T> &vecB )
{
  return vecA.isEqual ( vecB );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if they're not equal.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator != ( const SlVec3<T> &vecA, const SlVec3<T> &vecB )
{
  return vecA.isNotEqual ( vecB );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Bracket operator.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T &SlVec3<T>::operator [] ( int i )
{
  SL_ASSERT ( this && i >= 0 && i <= 2 );
  return _v[i];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Bracket operator.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline const T &SlVec3<T>::operator [] ( int i ) const
{
  SL_ASSERT ( this && i >= 0 && i <= 2 );
  return _v[i];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec3<T>::setValue ( const SlVec3<T> &vec )
{
  _v[0] = vec._v[0];
  _v[1] = vec._v[1];
  _v[2] = vec._v[2];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec3<T>::setValue ( const T &v0, const T &v1, const T &v2 )
{
  _v[0] = v0;
  _v[1] = v1;
  _v[2] = v2;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the cross product.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec3<T> SlVec3<T>::cross ( const SlVec3<T> &vec ) const
{
  return SlVec3<T> ( 
    _v[1] * vec[2] - _v[2] * vec[1],
    _v[2] * vec[0] - _v[0] * vec[2],
    _v[0] * vec[1] - _v[1] * vec[0] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the angle between the two vectors.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T SlVec3<T>::getAngle ( const SlVec3<T> &vec ) const
{
  // This is: theta = acosf ( A dot B / |A||B| ).

  T AdotB = this->dot ( vec );
  T lengthA = this->getLength();
  T lengthB = vec.getLength();
  return SL_ARC_COSINE ( AdotB / ( lengthA * lengthB ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the squared distance.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T SlVec3<T>::getDistanceSquared ( const SlVec3<T> &vec ) const
{
  return 
    ( _v[0] - vec._v[0] ) * ( _v[0] - vec._v[0] ) +
    ( _v[1] - vec._v[1] ) * ( _v[1] - vec._v[1] ) +
    ( _v[2] - vec._v[2] ) * ( _v[2] - vec._v[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the distance.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T SlVec3<T>::getDistance ( const SlVec3<T> &vec ) const
{
  return SL_SQRT ( this->getDistanceSquared ( vec ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the length.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T SlVec3<T>::getLength() const
{
  return SL_SQRT ( this->dot ( *this ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Bisect the two points.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec3<T>::bisect ( const SlVec3<T> &pt0, const SlVec3<T> &pt1 )
{
  // Do not call interpolate with "0.5" because that fauls up integer vectors.
  SlVec3<T> vec ( pt0 + pt1 );
  this->setValue ( vec / CadKit::SlConstants<T>::two() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clamp the vector's elements to the given range.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec3<T>::clamp ( const T &minValue, const T &maxValue )
{
  CadKit::clamp ( minValue, maxValue, _v[0] );
  CadKit::clamp ( minValue, maxValue, _v[1] );
  CadKit::clamp ( minValue, maxValue, _v[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the real part of this homogeneous vector.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec3<T>::getRealPart ( T &v0, T &v1 ) const
{
  T invW = CadKit::SlConstants<T>::one() / _v[2];
  v0 = _v[0] * invW;
  v1 = _v[1] * invW;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Linearly interpolate between two points.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec3<T>::interpolate ( const SlVec3<T> &pt0, const SlVec3<T> &pt1, const T &u )
{
  SlVec3<T> vec ( pt1 - pt0 );
  this->setValue ( pt0 + ( vec * u ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Normalize, return the length prior to normalization.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T SlVec3<T>::normalize()
{
  T length = this->getLength();
  T invLength = CadKit::SlConstants<T>::one() / length;

  _v[0] *= invLength;
  _v[1] *= invLength;
  _v[2] *= invLength;

  return length;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a vector that is orthogonal to the given vector.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec3<T>::orthogonal ( const SlVec3<T> &vec )
{
  const T CONST_0 ( CadKit::SlConstants<T>::zero() );
  const T CONST_1 ( CadKit::SlConstants<T>::one() );

  // Check impossible case.
  SL_ASSERT ( !( CONST_0 == vec._v[0] && CONST_0 == vec._v[1] && CONST_0 == vec._v[2] ) );

  // Check degenerate cases.
  if ( _v[0] == 0 )
  {
    this->setValue ( CONST_1, CONST_0, CONST_0 );
    return;
  }

  if ( _v[1] == 0 )
  {
    this->setValue ( CONST_0, CONST_1, CONST_0 );
    return;
  }

  if ( _v[2] == 0 )
  {
    this->setValue ( CONST_0, CONST_0, CONST_1 );
    return;
  }

  // Regular case.
  // Use the component with the biggest magnitude as the one we calculate for the normal. 
  // This is so the normal we find has the least chance of approaching a degenerate case.

  // Get the biggest magnitude.
  T max = SL_MAX ( _v[0], _v[1], _v[2] );

  // See which is biggest.
  if ( max == _v[0] )
  {
    this->setValue ( ( _v[1] * _v[1] + _v[2] * _v[2] ) / -_v[0], _v[1], _v[2] );
    return;
  }

  if ( max == _v[1] )
  {
    this->setValue ( _v[0], ( _v[0] * _v[0] + _v[2] * _v[2] ) / -_v[1], _v[2] );
    return;
  }

  this->setValue ( _v[0], _v[1], ( _v[0] * _v[0] + _v[1] * _v[1] ) / -_v[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the length.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec3<T>::setLength ( const T &newLength )
{
  T oldLength = this->getLength();
  T factor = newLength / oldLength;

  _v[0] *= factor;
  _v[1] *= factor;
  _v[2] *= factor;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the x-y-z vector into r-theta-z.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec3<T>::xyz2rtz()
{
  T r = SL_SQRT ( _v[0] * _v[0] + _v[1] * _v[1] ); // r^2 = x^2 + y^2
  T theta = SL_ARC_TANGENT_2 ( _v[1], _v[0] );     // theta = arctan ( y / x )
  this->setValue ( r, theta, _v[2] );              // z is unchanged
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the r-theta-z vector into x-y-z.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec3<T>::rtz2xyz()
{
  T x = _v[0] * SL_COSINE ( _v[1] ); // x = r cos ( theta )
  T y = _v[0] * SL_SINE   ( _v[1] ); // y = r sin ( theta )
  this->setValue ( x, y, _v[2] );    // z is unchanged
}


///////////////////////////////////////////////////////////////////////////////
//
//	Common types.
//
///////////////////////////////////////////////////////////////////////////////

typedef SlVec3<long double> SlVec3ld;
typedef SlVec3<double>      SlVec3d;
typedef SlVec3<float>       SlVec3f;

typedef SlVec3<long>        SlVec3l;
typedef SlVec3<int>         SlVec3i;
typedef SlVec3<short>       SlVec3s;


///////////////////////////////////////////////////////////////////////////////
//
//  Explicit declaration of constant types.
//
///////////////////////////////////////////////////////////////////////////////

CADKIT_DECLARE_CONSTANT_CLASS_FLOAT ( 
  SlVec3ld, 
  SlVec3ld ( 0.0, 0.0, 0.0 ), 
  SlVec3ld ( 0.5, 0.5, 0.5 ), 
  SlVec3ld ( 1.0, 1.0, 1.0 ),
  SlVec3ld ( 2.0, 2.0, 2.0 ) 
  );
CADKIT_DECLARE_CONSTANT_CLASS_FLOAT ( 
  SlVec3d,  
  SlVec3d ( 0.0, 0.0, 0.0 ), 
  SlVec3d ( 0.5, 0.5, 0.5 ), 
  SlVec3d ( 1.0, 1.0, 1.0 ),
  SlVec3d ( 2.0, 2.0, 2.0 ) 
  );
CADKIT_DECLARE_CONSTANT_CLASS_FLOAT ( 
  SlVec3f,  
  SlVec3f ( 0.0f, 0.0f, 0.0f ), 
  SlVec3f ( 0.5f, 0.5f, 0.5f ), 
  SlVec3f ( 1.0f, 1.0f, 1.0f ),
  SlVec3f ( 2.0f, 2.0f, 2.0f ) 
  );
CADKIT_DECLARE_CONSTANT_CLASS_INTEGER ( 
  SlVec3l,  
  SlVec3l ( 0, 0, 0 ), 
  SlVec3l ( 1, 1, 1 ),
  SlVec3l ( 2, 2, 2 ) 
  );
CADKIT_DECLARE_CONSTANT_CLASS_INTEGER ( 
  SlVec3i,  
  SlVec3i ( 0, 0, 0 ), 
  SlVec3i ( 1, 1, 1 ),
  SlVec3i ( 2, 2, 2 ) 
  );
CADKIT_DECLARE_CONSTANT_CLASS_INTEGER ( 
  SlVec3s,  
  SlVec3s ( 0, 0, 0 ), 
  SlVec3s ( 1, 1, 1 ),
  SlVec3s ( 2, 2, 2 ) 
  );


}; // namespace CadKit


#endif // _CADKIT_STANDARD_LIBRARY_TEMPLATE_VECTOR_OF_3_CLASS_H_
