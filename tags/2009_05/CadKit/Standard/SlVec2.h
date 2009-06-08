
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlVec2.h: A vector of 2 Ts.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_TEMPLATE_VECTOR_OF_2_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_TEMPLATE_VECTOR_OF_2_CLASS_H_

#include "SlAssert.h"
#include "SlAbsolute.h"
#include "SlTemplateSupport.h"
#include "SlConstants.h"
#include "SlClamp.h"

namespace CadKit
{
template<class T> class SlVec2
{
public:

  typedef T Real; // For readability.
  typedef T Type; // For the client.

  SlVec2();
  SlVec2 ( const T &v0, const T &v1 );

  void                    absolute();

  void                    bisect ( const SlVec2 &pt0, const SlVec2 &pt1 );

  void                    clamp ( const T &minValue, const T &maxValue );

  T                       dot ( const SlVec2 &vec ) const;

  Real                    getAngle ( const SlVec2 &vec ) const;
  static unsigned short   getDimension() { return 2; }
  Real                    getDistance ( const SlVec2 &vec ) const;
  Real                    getDistanceSquared ( const SlVec2 &vec ) const;
  Real                    getDistanceToLine ( const SlVec2 &linePoint1,const SlVec2 &linePoint2 ) const;
  Real                    getDistanceToLine ( const SlVec2 &lineVec ) const;
  Real                    getLength() const;
  T *                     getValue() { return _v; }
  const T *               getValue() const { return _v; }
  void                    getValue ( T &v0, T &v1 ) const;

  void                    interpolate ( const SlVec2 &pt0, const SlVec2 &pt1, const Real &u );
  bool                    isEqual ( const SlVec2 &vec ) const;
  bool                    isEqual ( const SlVec2 &vec, const T &tolerance ) const;
  bool                    isNotEqual ( const SlVec2 &vec ) const;
  bool                    isNotEqual ( const SlVec2 &vec, const T &tolerance ) const;

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
  SlVec2 &                operator *= ( const T &value );
  SlVec2 &                operator *= ( const SlVec2 &vec );
  SlVec2 &                operator /= ( const T &value );
  SlVec2 &                operator += ( const SlVec2 &vec );
  SlVec2 &                operator -= ( const SlVec2 &vec );
  SlVec2                  operator - () const;

  void                    orthogonal ( const SlVec2 &n );

  void                    setLength ( const Real &length );
  void                    setValue ( const SlVec2 &vec );
  void                    setValue ( const T &v0, const T &v1 );

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
  }

protected:

  T _v[2];
};


///////////////////////////////////////////////////////////////////////////////
//
//  Additional operators. These are not members of the class because compilers
//  vary too much in the proper syntax for friend functions in templates. 
//  See http://gcc.gnu.org/faq.html#friend and http://www.bero.org/gcc296.html
//
///////////////////////////////////////////////////////////////////////////////

template<class T> SlVec2<T> operator *  ( const SlVec2<T> &vec,  const T &value );
template<class T> SlVec2<T> operator *  ( const T &value,        const SlVec2<T> &vec );
template<class T> SlVec2<T> operator /  ( const SlVec2<T> &vec,  const T &value );
template<class T> SlVec2<T> operator +  ( const SlVec2<T> &vecA, const SlVec2<T> &vecB );
template<class T> SlVec2<T> operator -  ( const SlVec2<T> &vecA, const SlVec2<T> &vecB );
template<class T> bool      operator == ( const SlVec2<T> &vecA, const SlVec2<T> &vecB );
template<class T> bool      operator != ( const SlVec2<T> &vecA, const SlVec2<T> &vecB );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec2<T>::SlVec2()
{
  _v[0] = _v[1] = CadKit::SlConstants<T>::zero();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec2<T>::SlVec2 ( const T &v0, const T &v1 )
{
  _v[0] = v0;
  _v[1] = v1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make all the vector's components positive.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec2<T>::absolute()
{
  _v[0] = SL_ABS ( _v[0] );
  _v[1] = SL_ABS ( _v[1] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the dot product.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T SlVec2<T>::dot ( const SlVec2<T> &vec ) const
{
  return _v[0] * vec._v[0] +
         _v[1] * vec._v[1];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec2<T>::getValue ( T &v0, T &v1 ) const
{
  v0 = _v[0];
  v1 = _v[1];
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the vectors are equal within the given tolerance.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlVec2<T>::isEqual ( const SlVec2<T> &vec, const T &tolerance ) const
{
  return 
    ( ( SL_ABS ( _v[0] - vec._v[0] ) ) <= tolerance &&
      ( SL_ABS ( _v[1] - vec._v[1] ) ) <= tolerance );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the vectors are not equal within the given tolerance.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlVec2<T>::isNotEqual ( const SlVec2<T> &vec, const T &tolerance ) const
{
  return ( false == this->isEqual ( vec, tolerance ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the vectors are equal.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlVec2<T>::isEqual ( const SlVec2<T> &vec ) const
{
  return 
    ( _v[0] == vec._v[0] && 
      _v[1] == vec._v[1] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the vectors are not equal.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlVec2<T>::isNotEqual ( const SlVec2<T> &vec ) const
{
  return ( false == this->isEqual ( vec ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the vectors are equal within the given tolerance. Deliberately not 
//  a member function.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool isEqual ( const SlVec2<T> &v1, const SlVec2<T> &v2, const T &tolerance )
{
  return v1.isEqual ( v2, tolerance );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the array of vectors are equal within the given tolerance. 
//  Deliberately not a member function.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline bool isEqualArray ( const SlVec2<T> *v1, const SlVec2<T> *v2, const I &size )
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

template<class T, class I> inline bool isEqualArray ( const SlVec2<T> *v1, const SlVec2<T> *v2, const I &size, const T &tolerance )
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

template<class T> inline SlVec2<T> &SlVec2<T>::operator *= ( const T &value )
{
  _v[0] *= value;
  _v[1] *= value;

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Multiply all the components by the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec2<T> &SlVec2<T>::operator *= ( const SlVec2<T> &vec )
{
  _v[0] *= vec[0];
  _v[1] *= vec[1];

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Divide all the components by the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec2<T> &SlVec2<T>::operator /= ( const T &value )
{
  T invValue = CadKit::SlConstants<T>::one() / value;

  _v[0] *= invValue;
  _v[1] *= invValue;

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the vector to this one.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec2<T> &SlVec2<T>::operator += ( const SlVec2<T> &vec )
{
  _v[0] += vec._v[0];
  _v[1] += vec._v[1];

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Subtract the vector from this one.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec2<T> &SlVec2<T>::operator -= ( const SlVec2<T> &vec )
{
  _v[0] -= vec._v[0];
  _v[1] -= vec._v[1];

  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the negative of this vector.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec2<T> SlVec2<T>::operator - () const
{
  return SlVec2<T> ( -_v[0], -_v[1] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the component-wise product with the given value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec2<T> operator * ( const SlVec2<T> &vec, const T &value )
{
  return SlVec2<T> ( 
    vec[0] * value, 
    vec[1] * value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the component-wise product with the given value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec2<T> operator * ( const T &value, const SlVec2<T> &vec )
{
  return SlVec2<T> ( 
    vec[0] * value, 
    vec[1] * value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the component-wise division with the given value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec2<T> operator / ( const SlVec2<T> &vec, const T &value )
{
  // Do not multiply by inverse because that fauls up integer vectors.
  return SlVec2<T> ( 
    vec[0] / value, 
    vec[1] / value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the vector sum.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec2<T> operator + ( const SlVec2<T> &vecA, const SlVec2<T> &vecB )
{
  return SlVec2<T> ( 
    vecA[0] + vecB[0], 
    vecA[1] + vecB[1] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the vector difference.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec2<T> operator - ( const SlVec2<T> &vecA, const SlVec2<T> &vecB )
{
  return SlVec2<T> ( 
    vecA[0] - vecB[0], 
    vecA[1] - vecB[1] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if they're equal.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator == ( const SlVec2<T> &vecA, const SlVec2<T> &vecB )
{
  return vecA.isEqual ( vecB );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if they're not equal.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator != ( const SlVec2<T> &vecA, const SlVec2<T> &vecB )
{
  return vecA.isNotEqual ( vecB );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Bracket operator.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T &SlVec2<T>::operator [] ( int i )
{
  SL_ASSERT ( this && i >= 0 && i <= 1 );
  return _v[i];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Bracket operator.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline const T &SlVec2<T>::operator [] ( int i ) const
{
  SL_ASSERT ( this && i >= 0 && i <= 1 );
  return _v[i];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec2<T>::setValue ( const SlVec2<T> &vec )
{
  _v[0] = vec._v[0];
  _v[1] = vec._v[1];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec2<T>::setValue ( const T &v0, const T &v1 )
{
  _v[0] = v0;
  _v[1] = v1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the angle between the two vectors.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T SlVec2<T>::getAngle ( const SlVec2<T> &vec ) const
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

template<class T> inline T SlVec2<T>::getDistanceSquared ( const SlVec2<T> &vec ) const
{
  return ( _v[0] - vec._v[0] ) * ( _v[0] - vec._v[0] ) +
         ( _v[1] - vec._v[1] ) * ( _v[1] - vec._v[1] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the distance.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T SlVec2<T>::getDistance ( const SlVec2<T> &vec ) const
{
  return SL_SQRT ( this->getDistanceSquared ( vec ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the length.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T SlVec2<T>::getLength() const
{
  return SL_SQRT ( this->dot ( *this ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the distance from a point to a line.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T SlVec2<T>::getDistanceToLine ( const SlVec2<T> &linePoint1, const SlVec2<T> &linePoint2 ) const
{
  SlVec2<T> line ( linePoint2 - linePoint1 ); // Used to be linePoint1 - linePoint2.
  SlVec2<T> temp1 ( ( *this ) - linePoint2 );
  SlVec2<T> temp2 ( temp1.cross ( line ) );
  return temp2.getLength() / line.getLength();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the distance from a point to a line.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T SlVec2<T>::getDistanceToLine ( const SlVec2<T> &line ) const
{
  SlVec2<T> temp ( this->cross ( line ) );
  return ( ( temp ).getLength() ) / ( line.getLength() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Bisect the two points.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec2<T>::bisect ( const SlVec2<T> &pt0, const SlVec2<T> &pt1 )
{
  // Do not call interpolate with "0.5" because that fauls up integer vectors.
  SlVec2<T> vec ( pt0 + pt1 );
  this->setValue ( vec / CadKit::SlConstants<T>::two() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clamp the vector's elements to the given range.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec2<T>::clamp ( const T &minValue, const T &maxValue )
{
  CadKit::clamp ( minValue, maxValue, _v[0] );
  CadKit::clamp ( minValue, maxValue, _v[1] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Linearly interpolate between two points.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec2<T>::interpolate ( const SlVec2<T> &pt0, const SlVec2<T> &pt1, const T &u )
{
  SlVec2<T> vec ( pt1 - pt0 );
  this->setValue ( pt0 + ( vec * u ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Normalize, return the length prior to normalization.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T SlVec2<T>::normalize()
{
  T length = this->getLength();
  T invLength = CadKit::SlConstants<T>::one() / length;

  _v[0] *= invLength;
  _v[1] *= invLength;

  return length;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a vector that is orthogonal to the given vector.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec2<T>::orthogonal ( const SlVec2<T> &vec )
{
  SL_ASSERT ( 0 ); // TODO.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the length.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlVec2<T>::setLength ( const T &newLength )
{
  T oldLength = this->getLength();
  T factor = newLength / oldLength;

  _v[0] *= factor;
  _v[1] *= factor;
}


///////////////////////////////////////////////////////////////////////////////
//
//	Common types.
//
///////////////////////////////////////////////////////////////////////////////

typedef SlVec2<long double> SlVec2ld;
typedef SlVec2<double>      SlVec2d;
typedef SlVec2<float>       SlVec2f;

typedef SlVec2<long>        SlVec2l;
typedef SlVec2<int>         SlVec2i;
typedef SlVec2<short>       SlVec2s;


///////////////////////////////////////////////////////////////////////////////
//
//  Explicit declaration of constant types.
//
///////////////////////////////////////////////////////////////////////////////

CADKIT_DECLARE_CONSTANT_CLASS_FLOAT ( 
  SlVec2ld, 
  SlVec2ld ( 0.0, 0.0 ), 
  SlVec2ld ( 0.5, 0.5 ), 
  SlVec2ld ( 1.0, 1.0 ),
  SlVec2ld ( 2.0, 2.0 ) 
  );
CADKIT_DECLARE_CONSTANT_CLASS_FLOAT ( 
  SlVec2d,  
  SlVec2d ( 0.0, 0.0 ), 
  SlVec2d ( 0.5, 0.5 ), 
  SlVec2d ( 1.0, 1.0 ),
  SlVec2d ( 2.0, 2.0 ) 
  );
CADKIT_DECLARE_CONSTANT_CLASS_FLOAT ( 
  SlVec2f,  
  SlVec2f ( 0.0f, 0.0f ), 
  SlVec2f ( 0.5f, 0.5f ), 
  SlVec2f ( 1.0f, 1.0f ),
  SlVec2f ( 2.0f, 2.0f ) 
  );
CADKIT_DECLARE_CONSTANT_CLASS_INTEGER ( 
  SlVec2l,  
  SlVec2l ( 0, 0 ), 
  SlVec2l ( 1, 1 ),
  SlVec2l ( 2, 2 ) 
  );
CADKIT_DECLARE_CONSTANT_CLASS_INTEGER ( 
  SlVec2i,  
  SlVec2i ( 0, 0 ), 
  SlVec2i ( 1, 1 ),
  SlVec2i ( 2, 2 ) 
  );
CADKIT_DECLARE_CONSTANT_CLASS_INTEGER ( 
  SlVec2s,  
  SlVec2s ( 0, 0 ), 
  SlVec2s ( 1, 1 ),
  SlVec2s ( 2, 2 ) 
  );


}; // namespace CadKit

#endif // _CADKIT_STANDARD_LIBRARY_TEMPLATE_VECTOR_OF_2_CLASS_H_
