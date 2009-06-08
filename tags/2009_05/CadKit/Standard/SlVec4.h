
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlVec4.h: A vector of 4 Ts.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_TEMPLATE_VECTOR_OF_4_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_TEMPLATE_VECTOR_OF_4_CLASS_H_

#include "SlAssert.h"
#include "SlAbsolute.h"
#include "SlTemplateSupport.h"
#include "SlConstants.h"
#include "SlClamp.h"
#include "SlTruncate.h"

namespace CadKit
{
template<class T> class SlVec4
{
public:

  typedef T Real; // For readability.
  typedef T Type; // For the client.

  SlVec4();
  SlVec4 ( const T &v0, const T &v1, const T &v2, const T &v3 );

  void                    absolute();

  void                    bisect ( const SlVec4 &pt0, const SlVec4 &pt1 );

  void                    clamp ( const T &minValue, const T &maxValue );

  T                       dot ( const SlVec4 &vec ) const;

  static unsigned short   getDimension() { return 4; }
  Real                    getDistance ( const SlVec4 &vec ) const;
  Real                    getDistanceSquared ( const SlVec4 &vec ) const;
  Real                    getLength() const;
  void                    getRealPart ( Real &v0, Real &v1, Real &v2 ) const;
  T *                     getValue() { return _v; }
  const T *               getValue() const { return _v; }
  void                    getValue ( T &v0, T &v1, T &v2, T &v3 ) const;

  void                    interpolate ( const SlVec4 &pt0, const SlVec4 &pt1, const Real &u );
  bool                    isEqual ( const SlVec4 &vec ) const;
  bool                    isEqual ( const SlVec4 &vec, const T &tolerance ) const;
  bool                    isNotEqual ( const SlVec4 &vec ) const;
  bool                    isNotEqual ( const SlVec4 &vec, const T &tolerance ) const;

  const Real &            maximum() const;
  const Real &            minimum() const;

  Real                    normalize();

  // Typecast operators.
  operator T *()             { return _v; }
  operator const T *() const { return _v; }

  // Bracket operators.
  // Note: Keep this a hard-coded type, otherwise VC++ yacks.
  T &                     operator [] ( int i );
  const T &               operator [] ( int i ) const;

  // Misc operators.
  SlVec4 &                operator *= ( const T &value );
  SlVec4 &                operator *= ( const SlVec4 &vec );
  SlVec4 &                operator /= ( const T &value );
  SlVec4 &                operator += ( const SlVec4 &vec );
  SlVec4 &                operator -= ( const SlVec4 &vec );
  SlVec4                  operator - () const;

  void                    setLength ( const Real &length );

  void                    setValue ( const SlVec4 &vec );
  void                    setValue ( const T &v0, const T &v1, const T &v2, const T &v3 );

  void                    truncate ( const T &negativeZero, const T &positiveZero );

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
    functor ( _v[3] );
  }


protected:

  T _v[4];
};


///////////////////////////////////////////////////////////////////////////////
//
//  Additional operators. These are not members of the class because compilers
//  vary too much in the proper syntax for friend functions in templates. 
//  See http://gcc.gnu.org/faq.html#friend and http://www.bero.org/gcc296.html
//
///////////////////////////////////////////////////////////////////////////////

template<class T> SlVec4<T> operator *  ( const SlVec4<T> &vec,  const T &value );
template<class T> SlVec4<T> operator *  ( const T &value,        const SlVec4<T> &vec );
template<class T> SlVec4<T> operator /  ( const SlVec4<T> &vec,  const T &value );
template<class T> SlVec4<T> operator +  ( const SlVec4<T> &vecA, const SlVec4<T> &vecB );
template<class T> SlVec4<T> operator -  ( const SlVec4<T> &vecA, const SlVec4<T> &vecB );
template<class T> bool      operator == ( const SlVec4<T> &vecA, const SlVec4<T> &vecB );
template<class T> bool      operator != ( const SlVec4<T> &vecA, const SlVec4<T> &vecB );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec4<T>::SlVec4()
{
  _v[0] = _v[1] = _v[2] = _v[3] = CadKit::SlConstants<T>::zero();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec4<T>::SlVec4 ( const T &v0, const T &v1, const T &v2, const T &v3 )
{
  _v[0] = v0;
  _v[1] = v1;
  _v[2] = v2;
  _v[3] = v3;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make all the vector's components positive.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec4<T>::absolute()
{
  _v[0] = SL_ABS ( _v[0] );
  _v[1] = SL_ABS ( _v[1] );
  _v[2] = SL_ABS ( _v[2] );
  _v[3] = SL_ABS ( _v[3] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the dot product.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T SlVec4<T>::dot ( const SlVec4<T> &vec ) const
{
  return _v[0] * vec._v[0] +
         _v[1] * vec._v[1] +
         _v[2] * vec._v[2] +
         _v[3] * vec._v[3];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec4<T>::getValue ( T &v0, T &v1, T &v2, T &v3 ) const
{
  v0 = _v[0];
  v1 = _v[1];
  v2 = _v[2];
  v3 = _v[3];
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the vectors are equal within the given tolerance.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlVec4<T>::isEqual ( const SlVec4<T> &vec, const T &tolerance ) const
{
  return 
    ( ( SL_ABS ( _v[0] - vec._v[0] ) ) <= tolerance &&
      ( SL_ABS ( _v[1] - vec._v[1] ) ) <= tolerance &&
      ( SL_ABS ( _v[2] - vec._v[2] ) ) <= tolerance &&
      ( SL_ABS ( _v[3] - vec._v[3] ) ) <= tolerance );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the vectors are not equal within the given tolerance.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlVec4<T>::isNotEqual ( const SlVec4<T> &vec, const T &tolerance ) const
{
  return ( false == this->isEqual ( vec, tolerance ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the vectors are equal.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlVec4<T>::isEqual ( const SlVec4<T> &vec ) const
{
  return 
    ( _v[0] == vec._v[0] && 
      _v[1] == vec._v[1] &&
      _v[2] == vec._v[2] &&
      _v[3] == vec._v[3] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the vectors are not equal.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlVec4<T>::isNotEqual ( const SlVec4<T> &vec ) const
{
  return ( false == this->isEqual ( vec ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the vectors are equal within the given tolerance. Deliberately not 
//  a member function.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool isEqual ( const SlVec4<T> &v1, const SlVec4<T> &v2, const T &tolerance )
{
  return v1.isEqual ( v2, tolerance );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the array of vectors are equal within the given tolerance. 
//  Deliberately not a member function.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline bool isEqualArray ( const SlVec4<T> *v1, const SlVec4<T> *v2, const I &size )
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

template<class T, class I> inline bool isEqualArray ( const SlVec4<T> *v1, const SlVec4<T> *v2, const I &size, const T &tolerance )
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

template<class T> inline SlVec4<T> &SlVec4<T>::operator *= ( const T &value )
{
  _v[0] *= value;
  _v[1] *= value;
  _v[2] *= value;
  _v[3] *= value;

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Multiply all the components by the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec4<T> &SlVec4<T>::operator *= ( const SlVec4<T> &vec )
{
  _v[0] *= vec[0];
  _v[1] *= vec[1];
  _v[2] *= vec[2];
  _v[3] *= vec[3];

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Divide all the components by the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec4<T> &SlVec4<T>::operator /= ( const T &value )
{
  T invValue = CadKit::SlConstants<T>::one() / value;

  _v[0] *= invValue;
  _v[1] *= invValue;
  _v[2] *= invValue;
  _v[3] *= invValue;

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the vector to this one.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec4<T> &SlVec4<T>::operator += ( const SlVec4<T> &vec )
{
  _v[0] += vec._v[0];
  _v[1] += vec._v[1];
  _v[2] += vec._v[2];
  _v[3] += vec._v[3];

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Subtract the vector from this one.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec4<T> &SlVec4<T>::operator -= ( const SlVec4<T> &vec )
{
  _v[0] -= vec._v[0];
  _v[1] -= vec._v[1];
  _v[2] -= vec._v[2];
  _v[3] -= vec._v[3];

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the negative of this vector.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec4<T> SlVec4<T>::operator - () const
{
  return SlVec4<T> ( -_v[0], -_v[1], -_v[2], -_v[3] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the component-wise product with the given value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec4<T> operator * ( const SlVec4<T> &vec, const T &value )
{
  return SlVec4<T> ( 
    vec[0] * value, 
    vec[1] * value, 
    vec[2] * value,
    vec[3] * value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the component-wise product with the given value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec4<T> operator * ( const T &value, const SlVec4<T> &vec )
{
  return SlVec4<T> ( 
    vec[0] * value, 
    vec[1] * value, 
    vec[2] * value,
    vec[3] * value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the component-wise division with the given value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec4<T> operator / ( const SlVec4<T> &vec, const T &value )
{
  // Do not multiply by inverse because that fauls up integer vectors.
  return SlVec4<T> ( 
    vec[0] / value, 
    vec[1] / value, 
    vec[2] / value,
    vec[3] / value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the vector sum.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec4<T> operator + ( const SlVec4<T> &vecA, const SlVec4<T> &vecB )
{
  return SlVec4<T> ( 
    vecA[0] + vecB[0], 
    vecA[1] + vecB[1], 
    vecA[2] + vecB[2],
    vecA[3] + vecB[3] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the vector difference.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec4<T> operator - ( const SlVec4<T> &vecA, const SlVec4<T> &vecB )
{
  return SlVec4<T> ( 
    vecA[0] - vecB[0], 
    vecA[1] - vecB[1], 
    vecA[2] - vecB[2],
    vecA[3] - vecB[3] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if they're equal.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator == ( const SlVec4<T> &vecA, const SlVec4<T> &vecB )
{
  return vecA.isEqual ( vecB );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if they're not equal.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator != ( const SlVec4<T> &vecA, const SlVec4<T> &vecB )
{
  return vecA.isNotEqual ( vecB );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Bracket operator.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T &SlVec4<T>::operator [] ( int i )
{
  SL_ASSERT ( this && i >= 0 && i <= 3 );
  return _v[i];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Bracket operator.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline const T &SlVec4<T>::operator [] ( int i ) const
{
  SL_ASSERT ( this && i >= 0 && i <= 3 );
  return _v[i];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec4<T>::setValue ( const SlVec4<T> &vec )
{
  _v[0] = vec._v[0];
  _v[1] = vec._v[1];
  _v[2] = vec._v[2];
  _v[3] = vec._v[3];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec4<T>::setValue ( const T &v0, const T &v1, const T &v2, const T &v3 )
{
  _v[0] = v0;
  _v[1] = v1;
  _v[2] = v2;
  _v[3] = v3;
}




///////////////////////////////////////////////////////////////////////////////
//
//  Get the squared distance.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T SlVec4<T>::getDistanceSquared ( const SlVec4<T> &vec ) const
{
  return ( _v[0] - vec._v[0] ) * ( _v[0] - vec._v[0] ) +
         ( _v[1] - vec._v[1] ) * ( _v[1] - vec._v[1] ) +
         ( _v[2] - vec._v[2] ) * ( _v[2] - vec._v[2] ) +
         ( _v[3] - vec._v[3] ) * ( _v[3] - vec._v[3] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the distance.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T SlVec4<T>::getDistance ( const SlVec4<T> &vec ) const
{
  return SL_SQRT ( this->getDistanceSquared ( vec ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the length.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T SlVec4<T>::getLength() const
{
  return SL_SQRT ( this->dot ( *this ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Bisect the two points.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec4<T>::bisect ( const SlVec4<T> &pt0, const SlVec4<T> &pt1 )
{
  // Do not call interpolate with "0.5" because that fauls up integer vectors.
  SlVec4<T> vec ( pt0 + pt1 );
  this->setValue ( vec / CadKit::SlConstants<T>::two() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clamp the vector's elements to the given range.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec4<T>::clamp ( const T &minValue, const T &maxValue )
{
  CadKit::clamp ( minValue, maxValue, _v[0] );
  CadKit::clamp ( minValue, maxValue, _v[1] );
  CadKit::clamp ( minValue, maxValue, _v[2] );
  CadKit::clamp ( minValue, maxValue, _v[3] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Truncate the vector's elements.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec4<T>::truncate ( const T &negativeZero, const T &positiveZero )
{
  CadKit::truncate ( negativeZero, CadKit::SlConstants<T>::zero(), positiveZero, _v[0] );
  CadKit::truncate ( negativeZero, CadKit::SlConstants<T>::zero(), positiveZero, _v[1] );
  CadKit::truncate ( negativeZero, CadKit::SlConstants<T>::zero(), positiveZero, _v[2] );
  CadKit::truncate ( negativeZero, CadKit::SlConstants<T>::zero(), positiveZero, _v[3] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the real part of this homogeneous vector.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec4<T>::getRealPart ( T &v0, T &v1, T &v2 ) const
{
  T invW = CadKit::SlConstants<T>::one() / _v[3];
  v0 = _v[0] * invW;
  v1 = _v[1] * invW;
  v2 = _v[2] * invW;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Linearly interpolate between two points.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec4<T>::interpolate ( const SlVec4<T> &pt0, const SlVec4<T> &pt1, const T &u )
{
  SlVec4<T> vec ( pt1 - pt0 );
  this->setValue ( pt0 + ( vec * u ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Normalize, return the length prior to normalization.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T SlVec4<T>::normalize()
{
  T length = this->getLength();
  T invLength = CadKit::SlConstants<T>::one() / length;

  _v[0] *= invLength;
  _v[1] *= invLength;
  _v[2] *= invLength;
  _v[3] *= invLength;

  return length;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the length.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec4<T>::setLength ( const T &newLength )
{
  T oldLength = this->getLength();
  T factor = newLength / oldLength;

  _v[0] *= factor;
  _v[1] *= factor;
  _v[2] *= factor;
  _v[3] *= factor;
}


///////////////////////////////////////////////////////////////////////////////
//
//	Common types.
//
///////////////////////////////////////////////////////////////////////////////

typedef SlVec4<long double> SlVec4ld;
typedef SlVec4<double>      SlVec4d;
typedef SlVec4<float>       SlVec4f;

typedef SlVec4<long>        SlVec4l;
typedef SlVec4<int>         SlVec4i;
typedef SlVec4<short>       SlVec4s;


///////////////////////////////////////////////////////////////////////////////
//
//  Explicit declaration of constant types.
//
///////////////////////////////////////////////////////////////////////////////

CADKIT_DECLARE_CONSTANT_CLASS_FLOAT ( 
  SlVec4ld, 
  SlVec4ld ( 0.0, 0.0, 0.0, 0.0 ), 
  SlVec4ld ( 0.5, 0.5, 0.5, 0.5 ), 
  SlVec4ld ( 1.0, 1.0, 1.0, 1.0 ),
  SlVec4ld ( 2.0, 2.0, 2.0, 2.0 ) 
  );
CADKIT_DECLARE_CONSTANT_CLASS_FLOAT ( 
  SlVec4d,  
  SlVec4d ( 0.0, 0.0, 0.0, 0.0 ), 
  SlVec4d ( 0.5, 0.5, 0.5, 0.5 ), 
  SlVec4d ( 1.0, 1.0, 1.0, 1.0 ),
  SlVec4d ( 2.0, 2.0, 2.0, 2.0 ) 
  );
CADKIT_DECLARE_CONSTANT_CLASS_FLOAT ( 
  SlVec4f,  
  SlVec4f ( 0.0f, 0.0f, 0.0f, 0.0f ), 
  SlVec4f ( 0.5f, 0.5f, 0.5f, 0.5f ), 
  SlVec4f ( 1.0f, 1.0f, 1.0f, 1.0f ),
  SlVec4f ( 2.0f, 2.0f, 2.0f, 2.0f ) 
  );
CADKIT_DECLARE_CONSTANT_CLASS_INTEGER ( 
  SlVec4l,  
  SlVec4l ( 0, 0, 0, 0 ), 
  SlVec4l ( 1, 1, 1, 1 ),
  SlVec4l ( 2, 2, 2, 2 ) 
  );
CADKIT_DECLARE_CONSTANT_CLASS_INTEGER ( 
  SlVec4i,  
  SlVec4i ( 0, 0, 0, 0 ), 
  SlVec4i ( 1, 1, 1, 1 ),
  SlVec4i ( 2, 2, 2, 2 ) 
  );
CADKIT_DECLARE_CONSTANT_CLASS_INTEGER ( 
  SlVec4s,  
  SlVec4s ( 0, 0, 0, 0 ), 
  SlVec4s ( 1, 1, 1, 1 ),
  SlVec4s ( 2, 2, 2, 2 ) 
  );


}; // namespace CadKit

#endif // _CADKIT_STANDARD_LIBRARY_TEMPLATE_VECTOR_OF_4_CLASS_H_
