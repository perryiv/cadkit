
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

#ifndef _GENERIC_SCENE_GRAPH_CORE_TEMPLATE_2D_VECTOR_CLASS_H_
#define _GENERIC_SCENE_GRAPH_CORE_TEMPLATE_2D_VECTOR_CLASS_H_

#include "GSG/Core/Math.h"


namespace GSG {
namespace Detail {


template
<
  typename T,
  typename Index_,
  typename BoundsChecker_
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
  typedef BoundsChecker_ BoundsChecker;


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
    BoundsChecker ( Vector2::SIZE, i );
    return _v[i];
  }
  const T &operator [] ( size_type i ) const
  {
    BoundsChecker ( Vector2::SIZE, i );
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
    T reciprocal = static_cast < T > ( 1 ) / s;
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
    v[0] += s;
    v[1] += s;
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Subtraction operator.
  //
  /////////////////////////////////////////////////////////////////////////////

  Vector2 &operator -= ( T s )
  {
    v[0] -= s;
    v[1] -= s;
    return *this;
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
    return GSG::Math::sqrt ( this->dot ( *this ) );
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
    return GSG::Math::sqrt ( this->distanceSquared ( *this ) );
  }


private:

  T _v[SIZE];
};


}; // namespace Detail
}; // namespace GSG


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

template < class T, class I, class B > 
inline GSG::Detail::Vector2<T,I,B> operator + 
  ( const GSG::Detail::Vector2<T,I,B> &a, const GSG::Detail::Vector2<T,I,B> &b )
{
  return GSG::Detail::Vector2<T,I,B> ( 
    a[0] + b[0], 
    a[1] + b[1] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Subtraction.
//
///////////////////////////////////////////////////////////////////////////////

template < class T, class I, class B > 
inline GSG::Detail::Vector2<T,I,B> operator - 
  ( const GSG::Detail::Vector2<T,I,B> &a, const GSG::Detail::Vector2<T,I,B> &b )
{
  return GSG::Detail::Vector2<T,I,B> ( 
    a[0] - b[0], 
    a[1] - b[1] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Scale.
//
///////////////////////////////////////////////////////////////////////////////

template < class T, class I, class B > 
inline GSG::Detail::Vector2<T,I,B> operator * 
  ( const GSG::Detail::Vector2<T,I,B> &v, T s )
{
  return GSG::Detail::Vector2<T,I,B> ( 
    v[0] * s, 
    v[1] * s );
}
template < class T, class I, class B > 
inline GSG::Detail::Vector2<T,I,B> operator * 
  ( T s, const GSG::Detail::Vector2<T,I,B> &v )
{
  return GSG::Detail::Vector2<T,I,B> ( 
    v[0] * s, 
    v[1] * s );
}


#endif // _GENERIC_SCENE_GRAPH_CORE_TEMPLATE_2D_VECTOR_CLASS_H_
