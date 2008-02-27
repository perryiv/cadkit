
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A 2D vector.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_TEMPLATE_2D_VECTOR_CLASS_H_
#define _USUL_TEMPLATE_2D_VECTOR_CLASS_H_

#include "Usul/Math/Math.h"
#include "Usul/Errors/AssertPolicy.h"


namespace Usul {
namespace Math {


template
<
  typename T,
  typename Index_ = unsigned int,
  typename ErrorChecker_ = Usul::Errors::AssertPolicy
>
class Vector2
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
  typedef Vector2 < T, Index_, ErrorChecker_ > ThisType;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Enums.
  //
  /////////////////////////////////////////////////////////////////////////////

  enum
  {
    SIZE = 2,
    LAST = SIZE - 1
  };


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Default constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  explicit Vector2()
  {
    _v[0] = T();
    _v[1] = T();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector2 ( const T v[SIZE] )
  {
    this->set ( v );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector2 ( T v0, T v1 )
  {
    this->set ( v0, v1 );
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

  void set ( const Vector2 &v )
  {
    _v[0] = v[0];
    _v[1] = v[1];
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
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the value.
  //
  /////////////////////////////////////////////////////////////////////////////

  void set ( T v0, T v1 )
  {
    _v[0] = v0;
    _v[1] = v1;
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
    ErrorChecker::bounds ( 1074498984, s, i );
    return _v[i];
  }
  const T &operator [] ( size_type i ) const
  {
    const size_type s ( ThisType::SIZE );
    ErrorChecker::bounds ( 1074498985, s, i );
    return _v[i];
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Multiplication operator.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector2 &operator *= ( T s )
  {
    _v[0] *= s;
    _v[1] *= s;
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Division operator.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector2 &operator /= ( T value )
  {
    T reciprocal = static_cast < T > ( 1 ) / value;
    _v[0] *= reciprocal;
    _v[1] *= reciprocal;
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Addition operator.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector2 &operator += ( T s )
  {
    _v[0] += s;
    _v[1] += s;
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Subtraction operator.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector2 &operator -= ( T s )
  {
    _v[0] -= s;
    _v[1] -= s;
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the negative of this vector.
  //
  /////////////////////////////////////////////////////////////////////////////

  const Vector2 operator - () const
  {
    return ThisType ( -_v[0], -_v[1] );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  See if they are equal.
  //
  /////////////////////////////////////////////////////////////////////////////

  bool equal ( const Vector2 &v ) const
  {
    return
      _v[0] == v[0] &&
      _v[1] == v[1];
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the dot-product.
  //
  /////////////////////////////////////////////////////////////////////////////

  T dot ( const Vector2 &v )
  {
    return 
      _v[0] * v[0] +
      _v[1] * v[1];
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

  value_type distanceSquared ( const Vector2 &v ) const
  {
    return 
      ( _v[0] - v[0] ) * ( _v[0] - v[0] ) +
      ( _v[1] - v[1] ) * ( _v[1] - v[1] );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Calculate the distance between the two vectors.
  //
  /////////////////////////////////////////////////////////////////////////////

  value_type distance ( const Vector2 &v ) const
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

typedef Vector2 < char           > Vec2c;
typedef Vector2 < short          > Vec2s;
typedef Vector2 < int            > Vec2i;
typedef Vector2 < long           > Vec2l;

typedef Vector2 < unsigned char  > Vec2uc;
typedef Vector2 < unsigned short > Vec2us;
typedef Vector2 < unsigned int   > Vec2ui;
typedef Vector2 < unsigned long  > Vec2ul;

typedef Vector2 < float          > Vec2f;
typedef Vector2 < double         > Vec2d;
typedef Vector2 < long double    > Vec2ld;


}; // namespace Math
}; // namespace Usul


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
inline Usul::Math::Vector2<T,I,E> operator + 
  ( const Usul::Math::Vector2<T,I,E> &a, const Usul::Math::Vector2<T,I,E> &b )
{
  return Usul::Math::Vector2<T,I,E> ( 
    a[0] + b[0], 
    a[1] + b[1] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Subtraction.
//
///////////////////////////////////////////////////////////////////////////////

template < class T, class I, class E > 
inline Usul::Math::Vector2<T,I,E> operator - 
  ( const Usul::Math::Vector2<T,I,E> &a, const Usul::Math::Vector2<T,I,E> &b )
{
  return Usul::Math::Vector2<T,I,E> ( 
    a[0] - b[0], 
    a[1] - b[1] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Scale.
//
///////////////////////////////////////////////////////////////////////////////

template < class T, class I, class E > 
inline Usul::Math::Vector2<T,I,E> operator * 
  ( const Usul::Math::Vector2<T,I,E> &v, T s )
{
  return Usul::Math::Vector2<T,I,E> ( 
    v[0] * s, 
    v[1] * s );
}
template < class T, class I, class E > 
inline Usul::Math::Vector2<T,I,E> operator * 
  ( T s, const Usul::Math::Vector2<T,I,E> &v )
{
  return Usul::Math::Vector2<T,I,E> ( 
    v[0] * s, 
    v[1] * s );
}


#endif // _USUL_TEMPLATE_2D_VECTOR_CLASS_H_