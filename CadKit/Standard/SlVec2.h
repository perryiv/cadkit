
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
//  SlVec2.h: A vector of 2 Ts.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_TEMPLATE_VECTOR_OF_2_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_TEMPLATE_VECTOR_OF_2_CLASS_H_

#include "SlInlineMath.h"
#include "SlAssert.h"
#include "SlTemplateSupport.h"

#ifdef CADKIT_DEFINE_SL_VECTOR_2_OSTREAM_OPERATOR
#include <iostream.h>
#endif

#ifdef CADKIT_DEFINE_SL_VECTOR_2_STD_OSTREAM_OPERATOR
#include <iostream>
#endif

#ifdef CADKIT_DEFINE_SL_VECTOR_2_ISTREAM_OPERATOR
#include <iostream.h>
#endif

#ifdef CADKIT_DEFINE_SL_VECTOR_2_STD_ISTREAM_OPERATOR
#include <iostream>
#endif

// For convenience.
#define SL_VEC2_ZERO ( static_cast<T>(0) )
#define SL_VEC2_HALF ( static_cast<T>(0.5) )
#define SL_VEC2_ONE  ( static_cast<T>(1) )
#define SL_VEC2_TWO  ( static_cast<T>(2) )


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

  T                       dot ( const SlVec2 &vec ) const;

  Real                    getAngle ( const SlVec2 &vec ) const;
  static short            getDimension() { return 2; }
  Real                    getDistance ( const SlVec2 &vec ) const;
  Real                    getDistanceSquared ( const SlVec2 &vec ) const;
  Real                    getDistanceToLine ( const SlVec2 &linePoint1,const SlVec2 &linePoint2 ) const;
  Real                    getDistanceToLine ( const SlVec2 &lineVec ) const;
  Real                    getLength() const;
  T *                     getValue() { return _v; }
  const T *               getValue() const { return _v; }
  void                    getValue ( T &v0, T &v1 ) const;

  void                    interpolate ( const SlVec2 &pt0, const SlVec2 &pt1, const Real &u );
  bool                    isEqualTo ( const SlVec2 &vec ) const;
  bool                    isEqualTo ( const SlVec2 &vec, const T &tolerance ) const;
  bool                    isNotEqualTo ( const SlVec2 &vec ) const;
  bool                    isNotEqualTo ( const SlVec2 &vec, const T &tolerance ) const;

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

  // Friend function operators. See http://gcc.gnu.org/faq.html#friend 
  // and http://www.bero.org/gcc296.html
#if __GNUC__ >= 2
  template<class P> friend SlVec2<P> operator *  ( const SlVec2<P> &vec,  const T &value );
  template<class P> friend SlVec2<P> operator *  ( const T &value,        const SlVec2<P> &vec );
  template<class P> friend SlVec2<P> operator /  ( const SlVec2<P> &vec,  const T &value );
  template<class P> friend SlVec2<P> operator +  ( const SlVec2<P> &vecA, const SlVec2<P> &vecB );
  template<class P> friend SlVec2<P> operator -  ( const SlVec2<P> &vecA, const SlVec2<P> &vecB );
  template<class P> friend bool      operator == ( const SlVec2<P> &vecA, const SlVec2<P> &vecB );
  template<class P> friend bool      operator != ( const SlVec2<P> &vecA, const SlVec2<P> &vecB );
#else
  friend SlVec2           operator *  ( const SlVec2 &vec,  const T &value );
  friend SlVec2           operator *  ( const T &value,     const SlVec2 &vec );
  friend SlVec2           operator /  ( const SlVec2 &vec,  const T &value );
  friend SlVec2           operator +  ( const SlVec2 &vecA, const SlVec2 &vecB );
  friend SlVec2           operator -  ( const SlVec2 &vecA, const SlVec2 &vecB );
  friend bool             operator == ( const SlVec2 &vecA, const SlVec2 &vecB );
  friend bool             operator != ( const SlVec2 &vecA, const SlVec2 &vecB );
#endif

  // I/O.
  #ifdef CADKIT_DEFINE_SL_VECTOR_2_OSTREAM_OPERATOR
  friend ::ostream &      operator << ( ::ostream &out, const SlVec2 &vec );
  #endif
  #ifdef CADKIT_DEFINE_SL_VECTOR_2_STD_OSTREAM_OPERATOR
  friend std::ostream &   operator << ( std::ostream &out, const SlVec2 &vec );
  #endif
  #ifdef CADKIT_DEFINE_SL_VECTOR_2_ISTREAM_OPERATOR
  friend ::istream &      operator >> ( ::istream &in, SlVec2 &vec );
  #endif
  #ifdef CADKIT_DEFINE_SL_VECTOR_2_STD_ISTREAM_OPERATOR
  friend std::istream &   operator >> ( std::istream &in, SlVec2 &vec );
  #endif

  void                    orthogonal ( const SlVec2 &n );

  void                    setLength ( const Real &length );
  void                    setValue ( const SlVec2 &vec );
  void                    setValue ( const T &v0, const T &v1 );

protected:

  T _v[2];
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec2<T>::SlVec2()
{
  _v[0] = _v[1] = SL_VEC2_ZERO;
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

template<class T> inline bool SlVec2<T>::isEqualTo ( const SlVec2<T> &vec, const T &tolerance ) const
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

template<class T> inline bool SlVec2<T>::isNotEqualTo ( const SlVec2<T> &vec, const T &tolerance ) const
{
  return ( false == this->isEqualTo ( vec, tolerance ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the vectors are equal.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlVec2<T>::isEqualTo ( const SlVec2<T> &vec ) const
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

template<class T> inline bool SlVec2<T>::isNotEqualTo ( const SlVec2<T> &vec ) const
{
  return ( false == this->isEqualTo ( vec ) );
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
  T invValue = SL_VEC2_ONE / value;

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
    vec._v[0] * value, 
    vec._v[1] * value );
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
    vec._v[0] / value, 
    vec._v[1] / value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the vector sum.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec2<T> operator + ( const SlVec2<T> &vecA, const SlVec2<T> &vecB )
{
  return SlVec2<T> ( 
    vecA._v[0] + vecB._v[0], 
    vecA._v[1] + vecB._v[1] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the vector difference.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec2<T> operator - ( const SlVec2<T> &vecA, const SlVec2<T> &vecB )
{
  return SlVec2<T> ( 
    vecA._v[0] - vecB._v[0], 
    vecA._v[1] - vecB._v[1] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if they're equal.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator == ( const SlVec2<T> &vecA, const SlVec2<T> &vecB )
{
  return vecA.isEqualTo ( vecB );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if they're not equal.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator != ( const SlVec2<T> &vecA, const SlVec2<T> &vecB )
{
  return vecA.isNotEqualTo ( vecB );
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


#ifdef CADKIT_DEFINE_SL_VECTOR_2_OSTREAM_OPERATOR


///////////////////////////////////////////////////////////////////////////////
//
//  Stream operator.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline ::ostream &operator << ( ::ostream &out, const SlVec2<T> &vec )
{
  out << vec[0] << " " << vec[1];
  return out;
}


#endif // CADKIT_DEFINE_SL_VECTOR_2_OSTREAM_OPERATOR


#ifdef CADKIT_DEFINE_SL_VECTOR_2_STD_OSTREAM_OPERATOR


///////////////////////////////////////////////////////////////////////////////
//
//  Stream operator.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline std::ostream &operator << ( std::ostream &out, const SlVec2<T> &vec )
{
  out << vec[0] << " " << vec[1];
  return out;
}


#endif // CADKIT_DEFINE_SL_VECTOR_2_STD_OSTREAM_OPERATOR


#ifdef CADKIT_DEFINE_SL_VECTOR_2_ISTREAM_OPERATOR


///////////////////////////////////////////////////////////////////////////////
//
//  Stream operator.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline ::istream &operator >> ( ::istream &in, SlVec2<T> &vec )
{
  in >> vec[0] >> vec[1];
  return in;
}


#endif // CADKIT_DEFINE_SL_VECTOR_2_ISTREAM_OPERATOR


#ifdef CADKIT_DEFINE_SL_VECTOR_2_STD_ISTREAM_OPERATOR


///////////////////////////////////////////////////////////////////////////////
//
//  Stream operator.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline std::istream &operator >> ( std::istream &in, SlVec2<T> &vec )
{
  in >> vec[0] >> vec[1];
  return in;
}


#endif // CADKIT_DEFINE_SL_VECTOR_2_STD_ISTREAM_OPERATOR


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
  this->setValue ( vec / SL_VEC2_TWO );
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
  T invLength = SL_VEC2_ONE / length;

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


}; // namespace CadKit

#endif // _CADKIT_STANDARD_LIBRARY_TEMPLATE_VECTOR_OF_2_CLASS_H_
