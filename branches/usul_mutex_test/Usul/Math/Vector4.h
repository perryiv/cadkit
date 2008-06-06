
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A 4D vector.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_TEMPLATE_4D_VECTOR_CLASS_H_
#define _USUL_TEMPLATE_4D_VECTOR_CLASS_H_

#include "Usul/Math/Functions.h"
#include "Usul/Errors/AssertPolicy.h"


namespace Usul {
namespace Math {


template
<
  typename T,
  typename Index_ = unsigned int,
  typename ErrorChecker_ = Usul::Errors::AssertPolicy
>
class Vector4
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Useful typedefs.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef T value_type;
  typedef Index_ size_type;
  typedef ErrorChecker_ ErrorChecker;
  typedef Vector4 < T, Index_, ErrorChecker_ > ThisType;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Enums.
  //
  /////////////////////////////////////////////////////////////////////////////

  enum
  {
    SIZE = 4,
    LAST = SIZE - 1
  };


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Default constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  explicit Vector4()
  {
    _v[0] = T();
    _v[1] = T();
    _v[2] = T();
    _v[3] = T();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector4 ( const T v[SIZE] )
  {
    this->set ( v );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector4 ( T v0, T v1, T v2, T v3 )
  {
    this->set ( v0, v1, v2, v3 );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the internal array.
  //
  /////////////////////////////////////////////////////////////////////////////

  T *get()
  {
    return _v;
  }
  const T *get() const
  {
    return _v;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the value.
  //
  /////////////////////////////////////////////////////////////////////////////

  void set ( const Vector4 &v )
  {
    _v[0] = v[0];
    _v[1] = v[1];
    _v[2] = v[2];
    _v[3] = v[3];
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the value.
  //
  /////////////////////////////////////////////////////////////////////////////

  void set ( const T v[SIZE] )
  {
    _v[0] = v[0];
    _v[1] = v[1];
    _v[2] = v[2];
    _v[3] = v[3];
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the value.
  //
  /////////////////////////////////////////////////////////////////////////////

  void set ( T v0, T v1, T v2, T v3 )
  {
    _v[0] = v0;
    _v[1] = v1;
    _v[2] = v2;
    _v[3] = v3;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Typecast operators. Note, these confuse some compilers with the end 
  //  result being that the operator [] we define below is ambiguous with 
  //  the "::operator []". Leaving this here as a reminder.
  //
  /////////////////////////////////////////////////////////////////////////////

  //operator T *() { return this->get(); }
  //operator const T *() const { return this->get(); }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Bracket operators.
  //
  /////////////////////////////////////////////////////////////////////////////

  T &operator [] ( size_type i )
  {
    const size_type s ( ThisType::SIZE );
    ErrorChecker::bounds ( 1074499043, s, i );
    return _v[i];
  }
  const T &operator [] ( size_type i ) const
  {
    const size_type s ( ThisType::SIZE );
    ErrorChecker::bounds ( 1074499044, s, i );
    return _v[i];
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Multiplication operator.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector4 &operator *= ( T s )
  {
    _v[0] *= s;
    _v[1] *= s;
    _v[2] *= s;
    _v[3] *= s;
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Division operator.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector4 &operator /= ( T value )
  {
    T reciprocal = static_cast < T > ( 1 ) / value;
    _v[0] *= reciprocal;
    _v[1] *= reciprocal;
    _v[2] *= reciprocal;
    _v[3] *= reciprocal;
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Addition operator.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector4 &operator += ( T s )
  {
    _v[0] += s;
    _v[1] += s;
    _v[2] += s;
    _v[3] += s;
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Addition operator.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector4 &operator += ( const Vector4 &v )
  {
    _v[0] += v[0];
    _v[1] += v[1];
    _v[2] += v[2];
    _v[3] += v[3];
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Subtraction operator.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector4 &operator -= ( T s )
  {
    _v[0] -= s;
    _v[1] -= s;
    _v[2] -= s;
    _v[3] -= s;
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the negative of this vector.
  //
  /////////////////////////////////////////////////////////////////////////////

  const Vector4 operator - () const
  {
    return ThisType ( -_v[0], -_v[1], -_v[2], -_v[3] );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  See if they are equal.
  //
  /////////////////////////////////////////////////////////////////////////////

  bool equal ( const Vector4 &v ) const
  {
    return
      _v[0] == v[0] &&
      _v[1] == v[1] &&
      _v[2] == v[2] &&
      _v[3] == v[3];
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the dot-product.
  //
  /////////////////////////////////////////////////////////////////////////////

  T dot ( const Vector4 &v )
  {
    return 
      _v[0] * v[0] +
      _v[1] * v[1] +
      _v[2] * v[2] +
      _v[3] * v[3];
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the length.
  //
  /////////////////////////////////////////////////////////////////////////////

  value_type length() const
  {
    return Usul::Math::sqrt ( this->dot ( *this ) );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Calculate the squared distance between the two vectors.
  //
  /////////////////////////////////////////////////////////////////////////////

  value_type distanceSquared ( const Vector4 &v ) const
  {
    return 
      ( _v[0] - v[0] ) * ( _v[0] - v[0] ) +
      ( _v[1] - v[1] ) * ( _v[1] - v[1] ) +
      ( _v[2] - v[2] ) * ( _v[2] - v[2] ) +
      ( _v[3] - v[3] ) * ( _v[3] - v[3] );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Calculate the distance between the two vectors.
  //
  /////////////////////////////////////////////////////////////////////////////

  value_type distance ( const Vector4 &v ) const
  {
    return Usul::Math::sqrt ( this->distanceSquared ( *this ) );
  }


private:

  T _v[SIZE];
};


///////////////////////////////////////////////////////////////////////////////
//
//  Useful typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef Vector4 < char           > Vec4c;
typedef Vector4 < short          > Vec4s;
typedef Vector4 < int            > Vec4i;
typedef Vector4 < long           > Vec4l;

typedef Vector4 < unsigned char  > Vec4uc;
typedef Vector4 < unsigned short > Vec4us;
typedef Vector4 < unsigned int   > Vec4ui;
typedef Vector4 < unsigned long  > Vec4ul;

typedef Vector4 < float          > Vec4f;
typedef Vector4 < double         > Vec4d;
typedef Vector4 < long double    > Vec4ld;


} // namespace Math
} // namespace Usul


///////////////////////////////////////////////////////////////////////////////
//
//  Operators that cannot be members.
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//
//  Addition.
//
///////////////////////////////////////////////////////////////////////////////

template < class T, class I, class E > 
inline Usul::Math::Vector4<T,I,E> operator + 
  ( const Usul::Math::Vector4<T,I,E> &a, const Usul::Math::Vector4<T,I,E> &b )
{
  return Usul::Math::Vector4<T,I,E> ( 
    a[0] + b[0], 
    a[1] + b[1], 
    a[2] + b[2], 
    a[3] + b[3] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Subtraction.
//
///////////////////////////////////////////////////////////////////////////////

template < class T, class I, class E > 
inline Usul::Math::Vector4<T,I,E> operator - 
  ( const Usul::Math::Vector4<T,I,E> &a, const Usul::Math::Vector4<T,I,E> &b )
{
  return Usul::Math::Vector4<T,I,E> ( 
    a[0] - b[0], 
    a[1] - b[1], 
    a[2] - b[2], 
    a[3] - b[3] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Scale.
//
///////////////////////////////////////////////////////////////////////////////

template < class T, class I, class E > 
inline Usul::Math::Vector4<T,I,E> operator * 
  ( const Usul::Math::Vector4<T,I,E> &v, T s )
{
  return Usul::Math::Vector4<T,I,E> ( 
    v[0] * s, 
    v[1] * s, 
    v[2] * s, 
    v[3] * s );
}
template < class T, class I, class E > 
inline Usul::Math::Vector4<T,I,E> operator * 
  ( T s, const Usul::Math::Vector4<T,I,E> &v )
{
  return Usul::Math::Vector4<T,I,E> ( 
    v[0] * s, 
    v[1] * s, 
    v[2] * s, 
    v[3] * s );
}


#endif // _USUL_TEMPLATE_4D_VECTOR_CLASS_H_
