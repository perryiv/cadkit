
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Matrix44: A 4x4 matrix class that uses a 1D array of 16.
//  The matrix is stored in the array as follows:
//  0  4   8  11
//  1  5   9  12
//  2  6  10  13
//  3  7  11  14
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_4_BY_4_TEMPLATE_MATRIX_CLASS_H_
#define _USUL_4_BY_4_TEMPLATE_MATRIX_CLASS_H_

#include "Usul/Math/Vector4.h"
#include "Usul/Math/Vector3.h"

#include <algorithm>


namespace Usul {
namespace Math {


template
<
  typename T,
  typename Index_ = unsigned int,
  typename ErrorChecker_ = Usul::Errors::AssertPolicy
>
class Matrix44
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
  typedef Matrix44 < T, Index_, ErrorChecker_ > ThisType;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Enumerations.
  //
  /////////////////////////////////////////////////////////////////////////////

  enum
  {
    LAST            = 15,
    DIMENSION       = 4,
    SIZE            = 16,
    TRANSLATION_X   = 12,
    TRANSLATION_Y   = 13,
    TRANSLATION_Z   = 14,
    SCALE_X         = 0,
    SCALE_Y         = 5,
    SCALE_Z         = 10,
  };


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Default constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  explicit Matrix44()
  {
    this->identity();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Matrix44 ( const T m[SIZE] )
  {
    this->set ( m );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Matrix44 ( 
    T a0, T a4, T a8,  T a12,
    T a1, T a5, T a9,  T a13,
    T a2, T a6, T a10, T a14,
    T a3, T a7, T a11, T a15 )
  {
    this->set ( 
      a0, a4, a8,  a12,
      a1, a5, a9,  a13,
      a2, a6, a10, a14,
      a3, a7, a11, a15 );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Template constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Matrix_ > Matrix44 ( const Matrix_ &M )
  {
    this->set ( M );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Transpose this matrix. (There's probably an optimized way to do this).
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Matrix_ > static void transpose ( Matrix_ &m )
  {
    std::swap ( m[1],  m[4]  );
    std::swap ( m[2],  m[8]  );
    std::swap ( m[3],  m[12] );
    std::swap ( m[6],  m[9]  );
    std::swap ( m[7],  m[13] );
    std::swap ( m[11], m[14] );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Transpose this matrix. (There's probably an optimized way to do this).
  //
  /////////////////////////////////////////////////////////////////////////////

  void transpose()
  {
    transpose ( *this );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Makes a matrix like a call to glIdentity() and glFrustum() would.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Matrix_, typename Real_ > static void frustum ( 
    Real_ left, 
    Real_ right, 
    Real_ bottom, 
    Real_ top, 
    Real_ zNear, 
    Real_ zFar, 
    Matrix_ &m )
  {
    m[0]  = ( 2 * zNear ) / ( right - left );
    m[1]  = 0;
    m[2]  = 0;
    m[3]  = 0;

    m[4]  = 0;
    m[5]  = ( 2 * zNear ) / ( top - bottom );
    m[6]  = 0;
    m[7]  = 0;

    m[8]  = ( right + left ) / ( right - left );
    m[9]  = ( top + bottom ) / ( top - bottom );
    m[10] = - ( zFar + zNear ) / ( zFar - zNear );
    m[11] = -1;

    m[12] = 0;
    m[13] = 0;
    m[14] = -( 2 * zFar * zNear ) / ( zFar - zNear );
    m[15] = 0;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Makes a matrix like a call to glIdentity() and glFrustum() would.
  //
  /////////////////////////////////////////////////////////////////////////////

  void frustum ( T left, T right, T bottom, T top, T zNear, T zFar )
  {
    frustum ( left, right, bottom, top, zNear, zFar, _m );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Makes a matrix like a call to glIdentity() and gluPerspective() would.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Matrix_, typename Real_ > static void perspective ( 
    Real_ fovyInRadians, 
    Real_ aspect, 
    Real_ zNear, 
    Real_ zFar, 
    Matrix_ &m )
  {
    Real_ ymax ( zNear * Usul::Math::tan ( fovyInRadians * static_cast<Real_>(0.5) ) );
    Real_ ymin ( -ymax );
    Real_ xmin ( ymin * aspect );
    Real_ xmax ( ymax * aspect );
    frustum ( xmin, xmax, ymin, ymax, zNear, zFar, m );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Makes a matrix like a call to glIdentity() and gluPerspective() would.
  //
  /////////////////////////////////////////////////////////////////////////////

  void perspective ( T fovyInRadians, T aspect, T zNear, T zFar )
  {
    perspective ( fovyInRadians, aspect, zNear, zFar, _m );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set this matrix to the identity.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Matrix_ > static void identity ( Matrix_ &m )
  {
    m[0] = 1; m[4] = 0; m[8]  = 0; m[12] = 0;
    m[1] = 0; m[5] = 1; m[9]  = 0; m[13] = 0;
    m[2] = 0; m[6] = 0; m[10] = 1; m[14] = 0;
    m[3] = 0; m[7] = 0; m[11] = 0; m[15] = 1;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set this matrix to the identity.
  //
  /////////////////////////////////////////////////////////////////////////////

  void identity()
  {
    identity ( *this );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Return identity matrix.
  //
  /////////////////////////////////////////////////////////////////////////////

  static ThisType getIdentity()
  {
    ThisType m;
    return m;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the determinant of the matrix.
  //
  /////////////////////////////////////////////////////////////////////////////

  T determinant() const
  {
    return
      _m[0]  * this->_determinant3x3 ( 1, 2, 3, 1, 2, 3 ) -
      _m[4]  * this->_determinant3x3 ( 1, 2, 3, 0, 2, 3 ) +
      _m[8]  * this->_determinant3x3 ( 1, 2, 3, 0, 1, 3 ) -
      _m[12] * this->_determinant3x3 ( 1, 2, 3, 0, 1, 2 );
  }

  
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the given matrix to be the inverse of this matrix.
  //
  /////////////////////////////////////////////////////////////////////////////

  bool inverse ( ThisType &M ) const
  {
    T d = this->determinant();

    if ( 0 == d ) 
      return false;

    M[0]  =   this->_determinant3x3 ( 1, 2, 3, 1, 2, 3 );
    M[4]  = - this->_determinant3x3 ( 1, 2, 3, 0, 2, 3 );
    M[8]  =   this->_determinant3x3 ( 1, 2, 3, 0, 1, 3 );
    M[12] = - this->_determinant3x3 ( 1, 2, 3, 0, 1, 2 );

    M[1]  = - this->_determinant3x3 ( 0, 2, 3, 1, 2, 3 );
    M[5]  =   this->_determinant3x3 ( 0, 2, 3, 0, 2, 3 );
    M[9]  = - this->_determinant3x3 ( 0, 2, 3, 0, 1, 3 );
    M[13] =   this->_determinant3x3 ( 0, 2, 3, 0, 1, 2 );

    M[2]  =   this->_determinant3x3 ( 0, 1, 3, 1, 2, 3 );
    M[6]  = - this->_determinant3x3 ( 0, 1, 3, 0, 2, 3 );
    M[10] =   this->_determinant3x3 ( 0, 1, 3, 0, 1, 3 );
    M[14] = - this->_determinant3x3 ( 0, 1, 3, 0, 1, 2 );

    M[3]  = - this->_determinant3x3 ( 0, 1, 2, 1, 2, 3 );
    M[7]  =   this->_determinant3x3 ( 0, 1, 2, 0, 2, 3 );
    M[11] = - this->_determinant3x3 ( 0, 1, 2, 0, 1, 3 );
    M[15] =   this->_determinant3x3 ( 0, 1, 2, 0, 1, 2 );

    M.transpose();
    M /= d;

    return true;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Invert this matrix.
  //
  /////////////////////////////////////////////////////////////////////////////

  bool invert()
  {
    ThisType M;
    if ( this->inverse ( M ) )
    {
      this->set ( M );
      return true;
    }
    return false;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the matrix like a call to "glIdentity(); glLookAt();"
  //  eye    = your vantage point.
  //  center = the place you want to look.
  //  up     = a vector that defines "up".
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Matrix_, class Vector3_ > static void lookAt ( 
    const Vector3_ &eye, const Vector3_ &center, const Vector3_ &up, Matrix_ &m )
  {
    Vector3_ upVec = up;
    normalize ( upVec );

    Vector3_ n ( eye - center );
    normalize ( n );

    Vector3_ v ( upVec - center );
    T d ( dot ( v, n ) );
    v -= n * d;
    normalize ( v );

    Vector3_ u ( cross ( v, n ) );

    m[0]  = u[0];
    m[4]  = u[1];
    m[8]  = u[2];

    m[1]  = v[0];
    m[5]  = v[1];
    m[9]  = v[2];
  
    m[2]  = n[0];
    m[6]  = n[1];
    m[10] = n[2];
  
    m[12] = - dot ( u, eye );
    m[13] = - dot ( v, eye );
    m[14] = - dot ( n, eye );
  
    m[3]  = 0;
    m[7]  = 0;
    m[11] = 0;
    m[15] = 1;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the matrix like a call to "glIdentity(); glLookAt();"
  //  eye    = your vantage point.
  //  center = the place you want to look.
  //  up     = a vector that defines "up".
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Vector3_ > void lookAt ( 
    const Vector3_ &eye, const Vector3_ &center, const Vector3_ &up )
  {
    lookAt ( eye, center, up, _m );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Multiply, c = a * b.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class M1_, class M2_, class M3_ > static void multiply ( 
    const M1_ &a, const M2_ &b, M3_ &c )
  {
    c[0]  = a[0] * b[0]  + a[4] * b[1]  + a[8]  * b[2]  + a[12] * b[3];
    c[1]  = a[1] * b[0]  + a[5] * b[1]  + a[9]  * b[2]  + a[13] * b[3];
    c[2]  = a[2] * b[0]  + a[6] * b[1]  + a[10] * b[2]  + a[14] * b[3];
    c[3]  = a[3] * b[0]  + a[7] * b[1]  + a[11] * b[2]  + a[15] * b[3];

    c[4]  = a[0] * b[4]  + a[4] * b[5]  + a[8]  * b[6]  + a[12] * b[7];
    c[5]  = a[1] * b[4]  + a[5] * b[5]  + a[9]  * b[6]  + a[13] * b[7];
    c[6]  = a[2] * b[4]  + a[6] * b[5]  + a[10] * b[6]  + a[14] * b[7];
    c[7]  = a[3] * b[4]  + a[7] * b[5]  + a[11] * b[6]  + a[15] * b[7];

    c[8]  = a[0] * b[8]  + a[4] * b[9]  + a[8]  * b[10] + a[12] * b[11];
    c[9]  = a[1] * b[8]  + a[5] * b[9]  + a[9]  * b[10] + a[13] * b[11];
    c[10] = a[2] * b[8]  + a[6] * b[9]  + a[10] * b[10] + a[14] * b[11];
    c[11] = a[3] * b[8]  + a[7] * b[9]  + a[11] * b[10] + a[15] * b[11];

    c[12] = a[0] * b[12] + a[4] * b[13] + a[8]  * b[14] + a[12] * b[15];
    c[13] = a[1] * b[12] + a[5] * b[13] + a[9]  * b[14] + a[13] * b[15];
    c[14] = a[2] * b[12] + a[6] * b[13] + a[10] * b[14] + a[14] * b[15];
    c[15] = a[3] * b[12] + a[7] * b[13] + a[11] * b[14] + a[15] * b[15];
  }

  
  /////////////////////////////////////////////////////////////////////////////
  //
  //  Multiply this matrix on the left by the given matrix. The answer 
  //  becomes this matrix.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Matrix_ > void preMultiply ( const Matrix_ &M )
  {
    T a[Matrix44::SIZE];
    multiply ( M, _m, a );
    this->set ( a );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Multiply this matrix on the right by the given matrix. The answer 
  //  becomes this matrix.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Matrix_ > void postMultiply ( const Matrix_ &M )
  {
    T a[Matrix44::SIZE];
    multiply ( _m, M, a );
    this->set ( a );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the internal array.
  //
  /////////////////////////////////////////////////////////////////////////////

  T *get()
  {
    return _m;
  }
  const T *get() const
  {
    return _m;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the value.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Matrix_ > void set ( const Matrix_ &M )
  {
    _m[0] = M[0]; _m[4] = M[4]; _m[8]  = M[8];  _m[12] = M[12];
    _m[1] = M[1]; _m[5] = M[5]; _m[9]  = M[9];  _m[13] = M[13];
    _m[2] = M[2]; _m[6] = M[6]; _m[10] = M[10]; _m[14] = M[14];
    _m[3] = M[3]; _m[7] = M[7]; _m[11] = M[11]; _m[15] = M[15];
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the value.
  //
  /////////////////////////////////////////////////////////////////////////////

  void set ( const T m[SIZE] )
  {
    _m[0] = m[0]; _m[4] = m[4]; _m[8]  = m[8];  _m[12] = m[12];
    _m[1] = m[1]; _m[5] = m[5]; _m[9]  = m[9];  _m[13] = m[13];
    _m[2] = m[2]; _m[6] = m[6]; _m[10] = m[10]; _m[14] = m[14];
    _m[3] = m[3]; _m[7] = m[7]; _m[11] = m[11]; _m[15] = m[15];
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set the value.
  //
  /////////////////////////////////////////////////////////////////////////////

  void set ( 
    T a0, T a4, T a8,  T a12,
    T a1, T a5, T a9,  T a13,
    T a2, T a6, T a10, T a14,
    T a3, T a7, T a11, T a15 )
  {
    _m[0] = a0; _m[4] = a4; _m[8]  = a8;  _m[12] = a12;
    _m[1] = a1; _m[5] = a5; _m[9]  = a9;  _m[13] = a13;
    _m[2] = a2; _m[6] = a6; _m[10] = a10; _m[14] = a14;
    _m[3] = a3; _m[7] = a7; _m[11] = a11; _m[15] = a15;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Set all the values.
  //
  /////////////////////////////////////////////////////////////////////////////

  void set ( T a )
  {
    _m[0] = a; _m[4] = a; _m[8]  = a; _m[12] = a;
    _m[1] = a; _m[5] = a; _m[9]  = a; _m[13] = a;
    _m[2] = a; _m[6] = a; _m[10] = a; _m[14] = a;
    _m[3] = a; _m[7] = a; _m[11] = a; _m[15] = a;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Bracket operators.
  //
  /////////////////////////////////////////////////////////////////////////////

  T &operator [] ( size_type i )
  {
    const size_type s ( ThisType::SIZE );
    ErrorChecker::bounds ( 1074498986, s, i );
    return _m[i];
  }
  const T &operator [] ( size_type i ) const
  {
    const size_type s ( ThisType::SIZE );
    ErrorChecker::bounds ( 1074498987, s, i );
    return _m[i];
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Multiplication and assignment operator.
  //
  /////////////////////////////////////////////////////////////////////////////

  Matrix44 &operator *= ( const Matrix44 &M )
  {
    this->postMultiply ( M );
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Multiply all the members by the scaler value.
  //
  /////////////////////////////////////////////////////////////////////////////

  Matrix44 &operator *= ( T s )
  {
    _m[0] *= s; _m[4] *= s; _m[8]  *= s; _m[12] *= s;
    _m[1] *= s; _m[5] *= s; _m[9]  *= s; _m[13] *= s;
    _m[2] *= s; _m[6] *= s; _m[10] *= s; _m[14] *= s;
    _m[3] *= s; _m[7] *= s; _m[11] *= s; _m[15] *= s;
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Divide all the members by the scaler value.
  //
  /////////////////////////////////////////////////////////////////////////////

  Matrix44 &operator /= ( T s )
  {
    s = 1 / s;
    _m[0] *= s; _m[4] *= s; _m[8]  *= s; _m[12] *= s;
    _m[1] *= s; _m[5] *= s; _m[9]  *= s; _m[13] *= s;
    _m[2] *= s; _m[6] *= s; _m[10] *= s; _m[14] *= s;
    _m[3] *= s; _m[7] *= s; _m[11] *= s; _m[15] *= s;
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Access the i'th row and j'th column of the matrix as if it were a 2D 
  //  array, like matrix(i,j).
  //
  /////////////////////////////////////////////////////////////////////////////

  T &operator () ( size_type i, size_type j )
  {
    const size_type d ( ThisType::DIMENSION );
    const size_type s ( ThisType::SIZE );
    ErrorChecker::bounds ( 1076363757, d, i );
    ErrorChecker::bounds ( 1076363758, d, j );
    ErrorChecker::bounds ( 1076363759, s, j * d + i );
    return _m[j * d + i];
  }
  const T &operator () ( size_type i, size_type j ) const
  {
    const size_type d ( ThisType::DIMENSION );
    const size_type s ( ThisType::SIZE );
    ErrorChecker::bounds ( 1076363760, d, i );
    ErrorChecker::bounds ( 1076363761, d, j );
    ErrorChecker::bounds ( 1076363762, s, j * d + i );
    return _m[j * d + i];
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  See if they are equal.
  //
  /////////////////////////////////////////////////////////////////////////////

  bool equal ( const Matrix44 &M ) const
  {
    return ( 
      _m[0] == M[0] && _m[4] == M[4] && _m[8]  == M[8]  && _m[12] == M[12] && 
      _m[1] == M[1] && _m[5] == M[5] && _m[9]  == M[9]  && _m[13] == M[13] && 
      _m[2] == M[2] && _m[6] == M[6] && _m[10] == M[10] && _m[14] == M[14] && 
      _m[3] == M[3] && _m[7] == M[7] && _m[11] == M[11] && _m[15] == M[15] );
  }


  //////////////////////////////////////////////////////////////////////////
  //
  //  Make a translation.
  //
  //////////////////////////////////////////////////////////////////////////

  template < class Vector3_ > void makeTranslation ( const Vector3_ &t )
  {
    _m[0] = 1; _m[4] = 0; _m[8]  = 0; _m[12] = t[0];
    _m[1] = 0; _m[5] = 1; _m[9]  = 0; _m[13] = t[1];
    _m[2] = 0; _m[6] = 0; _m[10] = 1; _m[14] = t[2];
    _m[3] = 0; _m[7] = 0; _m[11] = 0; _m[15] = 1;
  }


  //////////////////////////////////////////////////////////////////////////
  //
  //  Get the translation portion of the matrix.
  //
  //////////////////////////////////////////////////////////////////////////

  template < class Vector3_ > void getTranslation ( Vector3_ &t ) const
  {
    t[0] = _m[TRANSLATION_X];
    t[1] = _m[TRANSLATION_Y];
    t[2] = _m[TRANSLATION_Z];
  }


  //////////////////////////////////////////////////////////////////////////
  //
  //  Get the translation portion of the matrix.
  //
  //////////////////////////////////////////////////////////////////////////

  template < class Vector3_ > Vector3_ getTranslation() const
  {
    Vector3_ v;
    this->getTranslation ( v );
    return v;
  }


  //////////////////////////////////////////////////////////////////////////
  //
  //  Set the translation portion of the matrix.
  //
  //////////////////////////////////////////////////////////////////////////

  template < class Vector3_ > void setTranslation ( const Vector3_ &t )
  {
    _m[TRANSLATION_X] = t[0];
    _m[TRANSLATION_Y] = t[1];
    _m[TRANSLATION_Z] = t[2];
  }


  //////////////////////////////////////////////////////////////////////////
  //
  //  Return a translation matrix.
  //
  //////////////////////////////////////////////////////////////////////////

  template < class Vector3_ > static ThisType translation ( const Vector3_ &t )
  {
    ThisType m;
    m.makeTranslation ( t );
    return m;
  }


  //////////////////////////////////////////////////////////////////////////
  //
  //  Return a translation matrix.
  //
  //////////////////////////////////////////////////////////////////////////

  template < class X, class Y, class Z > static ThisType translation ( const X &x, const Y &y, const Z &z )
  {
    return ThisType::translation ( Usul::Math::Vector3<value_type> 
        ( static_cast<value_type> ( x ), static_cast<value_type> ( y ), static_cast<value_type> ( z ) ) );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Make a rotation about an arbitrary axis.
  //  cosine  = cos ( angle )
  //  sine    = sin ( angle )
  //  axis    = a normalized vector that specifies the axis to rotate about.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Vector3_, typename Real_ > void makeRotation
    ( Real_ cosine, Real_ sine, const Vector3_ &axis )
  {
    ErrorChecker ( 1076182690, cosine >= -1 && cosine <= 1 );
    ErrorChecker ( 1076182690, sine   >= -1 && sine   <= 1 );

    const Real_ x ( axis[0] );
    const Real_ y ( axis[1] );
    const Real_ z ( axis[2] );
    const Real_ one_minus_cosine ( 1 - cosine );

    _m[0]  = x * x + ( 1 - x * x ) * cosine;
    _m[1]  = x * y * one_minus_cosine + z * sine;
    _m[2]  = x * z * one_minus_cosine - y * sine;
    _m[3]  = 0;

    _m[4]  = y * x * one_minus_cosine - z * sine;
    _m[5]  = y * y + ( 1 - y * y ) * cosine;
    _m[6]  = y * z * one_minus_cosine + x * sine;
    _m[7]  = 0;

    _m[8]  = z * x * one_minus_cosine + y * sine;
    _m[9]  = z * y * one_minus_cosine - x * sine;
    _m[10] = z * z + ( 1 - z * z ) * cosine;
    _m[11] = 0;

    _m[12] = 0;
    _m[13] = 0;
    _m[14] = 0;
    _m[15] = 1;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Make a rotation about an arbitrary axis.
  //  radians = an angle in radians
  //  axis    = a normalized vector that specifies the axis to rotate about.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Vector3_, typename Real_ > void makeRotation ( Real_ radians, const Vector3_ &axis )
  {
    Real_ cosine ( Usul::Math::cos ( radians ) );
    Real_ sine   ( Usul::Math::sin ( radians ) );
    this->makeRotation ( cosine, sine, axis );
  }


  //////////////////////////////////////////////////////////////////////////
  //
  //  Make a scale.
  //
  //////////////////////////////////////////////////////////////////////////

  template < class Vector3_ > void makeScale ( const Vector3_ &s )
  {
    _m[0] = s[0]; _m[4] =    0; _m[8]  =    0; _m[12] = 0;
    _m[1] =    0; _m[5] = s[1]; _m[9]  =    0; _m[13] = 0;
    _m[2] =    0; _m[6] =    0; _m[10] = s[2]; _m[14] = 0;
    _m[3] =    0; _m[7] =    0; _m[11] =    0; _m[15] = 1;
  }


  //////////////////////////////////////////////////////////////////////////
  //
  //  Set the scale.
  //
  //////////////////////////////////////////////////////////////////////////

  template < class Vector3_ > void setScale ( const Vector3_ &s )
  {
    _m[SCALE_X] = s[0];
    _m[SCALE_Y] = s[1];
    _m[SCALE_Z] = s[2];
  }


  //////////////////////////////////////////////////////////////////////////
  //
  //  Return a scale matrix.
  //
  //////////////////////////////////////////////////////////////////////////

  template < class Vector3_ > static ThisType scale ( const Vector3_ &s )
  {
    ThisType m;
    m.makeScale ( s );
    return m;
  }

protected:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Get the determinant of the specified 3x3 submatrix.
  //
  /////////////////////////////////////////////////////////////////////////////

  T _determinant3x3 ( size_type r1, size_type r2, size_type r3, size_type c1, size_type c2, size_type c3 ) const
  {
    // Convert to the correct indices.
    c1 *= 4;
    c2 *= 4;
    c3 *= 4;

    // Use the bracket operator so that bounds-checking is performed.
    const ThisType &m = *this;

    // Return the answer.
    return 
      m[r1+c1] * ( m[r2+c2] * m[r3+c3] - m[r2+c3] * m[r3+c2] ) -
      m[r1+c2] * ( m[r2+c1] * m[r3+c3] - m[r2+c3] * m[r3+c1] ) +
      m[r1+c3] * ( m[r2+c1] * m[r3+c2] - m[r2+c2] * m[r3+c1] );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Internal data.
  //
  /////////////////////////////////////////////////////////////////////////////

  T _m[Matrix44::SIZE];
};


///////////////////////////////////////////////////////////////////////////////
//
//  Useful typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef Matrix44 < char           > Matrix44c;
typedef Matrix44 < short          > Matrix44s;
typedef Matrix44 < int            > Matrix44i;
typedef Matrix44 < long           > Matrix44l;

typedef Matrix44 < unsigned char  > Matrix44uc;
typedef Matrix44 < unsigned short > Matrix44us;
typedef Matrix44 < unsigned int   > Matrix44ui;
typedef Matrix44 < unsigned long  > Matrix44ul;

typedef Matrix44 < float          > Matrix44f;
typedef Matrix44 < double         > Matrix44d;
typedef Matrix44 < long double    > Matrix44ld;


} // namespace Math
} // namespace Usul


///////////////////////////////////////////////////////////////////////////////
//
//  Operators that cannot be members.
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//
//  Multiply two matrices.
//
///////////////////////////////////////////////////////////////////////////////

template < class T, class I, class E > 
inline Usul::Math::Matrix44<T,I,E> operator * 
  ( const Usul::Math::Matrix44<T,I,E> &left, const Usul::Math::Matrix44<T,I,E> &right )
{
  typedef Usul::Math::Matrix44<T,I,E> Matrix44;
  Matrix44 answer;
  Matrix44::multiply ( left, right, answer );
  return answer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Multiply by a 3D vector.
//
///////////////////////////////////////////////////////////////////////////////

template < class T, class I, class E > 
inline Usul::Math::Vector3<T,I,E> operator * 
  ( const Usul::Math::Matrix44<T,I,E> &M, const Usul::Math::Vector3<T,I,E> &b )
{
  typedef Usul::Math::Matrix44<T,I,E> Matrix44;
  typedef Usul::Math::Vector3<T,I,E> Vector3;
  typedef Usul::Math::Vector4<T,I,E> Vector4;
  Vector4 v;
  v[3] =    1 / ( M[3] * b[0] + M[7] * b[1] + M[11] * b[2] + M[15] );
  v[2] = v[3] * ( M[2] * b[0] + M[6] * b[1] + M[10] * b[2] + M[14] );
  v[1] = v[3] * ( M[1] * b[0] + M[5] * b[1] + M[9]  * b[2] + M[13] );
  v[0] = v[3] * ( M[0] * b[0] + M[4] * b[1] + M[8]  * b[2] + M[12] );
  return Vector3 ( v[0], v[1], v[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Multiply by a 4D vector.
//
///////////////////////////////////////////////////////////////////////////////

template < class T, class I, class E > 
inline Usul::Math::Vector4<T,I,E> operator * 
  ( const Usul::Math::Matrix44<T,I,E> &M, const Usul::Math::Vector4<T,I,E> &b )
{
  typedef Usul::Math::Matrix44<T,I,E> Matrix44;
  typedef Usul::Math::Vector4<T,I,E> Vector4;
  return Vector4
  (
    M[0] * b[0] + M[4] * b[1] + M[8]  * b[2] + M[12] * b[3],
    M[1] * b[0] + M[5] * b[1] + M[9]  * b[2] + M[13] * b[3],
    M[2] * b[0] + M[6] * b[1] + M[10] * b[2] + M[14] * b[3],
    M[3] * b[0] + M[7] * b[1] + M[11] * b[2] + M[15] * b[3] 
  );
}


#endif // _USUL_4_BY_4_TEMPLATE_MATRIX_CLASS_H_
