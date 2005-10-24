
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A 3D vector.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_TEMPLATE_3D_VECTOR_CLASS_H_
#define _USUL_TEMPLATE_3D_VECTOR_CLASS_H_

#include "Usul/Math/Math.h"
#include "Usul/Errors/AssertPolicy.h"
#include "Usul/Types/Types.h"


namespace Usul {
namespace Math {


template
<
  typename T,
  typename Index_ = unsigned int,
  typename ErrorChecker_ = Usul::Errors::AssertPolicy
>
class Vector3
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
  typedef Vector3 < T, Index_, ErrorChecker_ > ThisType;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Enums.
  //
  /////////////////////////////////////////////////////////////////////////////

  enum
  {
    SIZE = 3,
    LAST = SIZE - 1
  };


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Default constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  explicit Vector3()
  {
    _v[0] = T();
    _v[1] = T();
    _v[2] = T();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector3 ( const T v[SIZE] )
  {
    this->set ( v );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector3 ( T v0, T v1, T v2 )
  {
    this->set ( v0, v1, v2 );
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

  void set ( const Vector3 &v )
  {
    _v[0] = v[0];
    _v[1] = v[1];
    _v[2] = v[2];
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
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the value.
  //
  /////////////////////////////////////////////////////////////////////////////

  void set ( T v0, T v1, T v2 )
  {
    _v[0] = v0;
    _v[1] = v1;
    _v[2] = v2;
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
    ErrorChecker::bounds ( 1074499041, s, i );
    return _v[i];
  }
  const T &operator [] ( size_type i ) const
  {
    const size_type s ( ThisType::SIZE );
    ErrorChecker::bounds ( 1074499042, s, i );
    return _v[i];
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Multiplication operator.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector3 &operator *= ( T s )
  {
    _v[0] *= s;
    _v[1] *= s;
    _v[2] *= s;
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Division operator.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector3 &operator /= ( T value )
  {
    T reciprocal = static_cast < T > ( 1 ) / value;
    _v[0] *= reciprocal;
    _v[1] *= reciprocal;
    _v[2] *= reciprocal;
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Addition operator.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector3 &operator += ( const Vector3 &v )
  {
    _v[0] += v[0];
    _v[1] += v[1];
    _v[2] += v[2];
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Addition operator.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector3 &operator += ( T s )
  {
    _v[0] += s;
    _v[1] += s;
    _v[2] += s;
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Subtraction operator.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector3 &operator -= ( const Vector3 &v )
  {
    _v[0] -= v[0];
    _v[1] -= v[1];
    _v[2] -= v[2];
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Subtraction operator.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector3 &operator -= ( T s )
  {
    _v[0] -= s;
    _v[1] -= s;
    _v[2] -= s;
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the negative of this vector.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector3 operator - () const
  {
    return ThisType ( -_v[0], -_v[1], -_v[2] );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  See if they are equal.
  //
  /////////////////////////////////////////////////////////////////////////////

  bool equal ( const Vector3 &v ) const
  {
    return
      _v[0] == v[0] &&
      _v[1] == v[1] &&
      _v[2] == v[2];
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the dot-product.
  //
  /////////////////////////////////////////////////////////////////////////////

  T dot ( const Vector3 &v ) const
  {
    return 
      _v[0] * v[0] +
      _v[1] * v[1] +
      _v[2] * v[2];
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return the cross-product.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector3 cross ( const Vector3 &v ) const
  {
    return Vector3 ( 
      _v[1] * v[2] - _v[2] * v[1],
      _v[2] * v[0] - _v[0] * v[2],
      _v[0] * v[1] - _v[1] * v[0] );
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Return the angle between the two vectors.
  //
  ///////////////////////////////////////////////////////////////////////////////

  T angle ( const Vector3 &v ) const
  {
    // This is: theta = acosf ( A dot B / |A||B| ).
    T AdotB ( this->dot ( v ) );
    T lengthA ( this->length() );
    T lengthB ( v.length() );
    return Usul::Math::acos ( AdotB / ( lengthA * lengthB ) );
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

  value_type distanceSquared ( const Vector3 &v ) const
  {
    return 
      ( _v[0] - v[0] ) * ( _v[0] - v[0] ) +
      ( _v[1] - v[1] ) * ( _v[1] - v[1] ) +
      ( _v[2] - v[2] ) * ( _v[2] - v[2] );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Calculate the distance between the two vectors.
  //
  /////////////////////////////////////////////////////////////////////////////

  value_type distance ( const Vector3 &v ) const
  {
    return Usul::Math::sqrt ( this->distanceSquared ( *this ) );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Normalize, return the length prior to normalization.
  //
  /////////////////////////////////////////////////////////////////////////////

  T normalize()
  {
    T l ( this->length() );
    T il ( static_cast < T > ( 1 ) / l );

    _v[0] *= il;
    _v[1] *= il;
    _v[2] *= il;

    return l;
  }


private:

  T _v[SIZE];
};


///////////////////////////////////////////////////////////////////////////////
//
//  Useful typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef Vector3 < char           > Vec3c;
typedef Vector3 < short          > Vec3s;
typedef Vector3 < int            > Vec3i;
typedef Vector3 < long           > Vec3l;

typedef Vector3 < unsigned char  > Vec3uc;
typedef Vector3 < unsigned short > Vec3us;
typedef Vector3 < unsigned int   > Vec3ui;
typedef Vector3 < unsigned long  > Vec3ul;

typedef Vector3 < float          > Vec3f;
typedef Vector3 < double         > Vec3d;
typedef Vector3 < long double    > Vec3ld;


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
inline Usul::Math::Vector3<T,I,E> operator + 
  ( const Usul::Math::Vector3<T,I,E> &a, const Usul::Math::Vector3<T,I,E> &b )
{
  return Usul::Math::Vector3<T,I,E> ( 
    a[0] + b[0], 
    a[1] + b[1], 
    a[2] + b[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Subtraction.
//
///////////////////////////////////////////////////////////////////////////////

template < class T, class I, class E > 
inline Usul::Math::Vector3<T,I,E> operator - 
  ( const Usul::Math::Vector3<T,I,E> &a, const Usul::Math::Vector3<T,I,E> &b )
{
  return Usul::Math::Vector3<T,I,E> ( 
    a[0] - b[0], 
    a[1] - b[1], 
    a[2] - b[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Scale.
//
///////////////////////////////////////////////////////////////////////////////

template < class T, class I, class E > 
inline Usul::Math::Vector3<T,I,E> operator * 
  ( const Usul::Math::Vector3<T,I,E> &v, T s )
{
  return Usul::Math::Vector3<T,I,E> ( 
    v[0] * s, 
    v[1] * s, 
    v[2] * s );
}
template < class T, class I, class E > 
inline Usul::Math::Vector3<T,I,E> operator * 
  ( T s, const Usul::Math::Vector3<T,I,E> &v )
{
  return Usul::Math::Vector3<T,I,E> ( 
    v[0] * s, 
    v[1] * s, 
    v[2] * s );
}


#endif // _USUL_TEMPLATE_3D_VECTOR_CLASS_H_
