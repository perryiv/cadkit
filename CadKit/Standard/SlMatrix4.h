
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
//  SlMatrix4: A 4x4 matrix class that uses a 1D array of 16.
//  The matrix is stored in the array as follows:
//  0  4   8  11
//  1  5   9  12
//  2  6  10  13
//  3  7  11  14
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_4_BY_4_TEMPLATE_MATRIX_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_4_BY_4_TEMPLATE_MATRIX_CLASS_H_

#include "SlVec4.h"
#include "SlVec3.h"

// For convenience.
#define SL_MATRIX4_ZERO ( static_cast<T> ( 0 ) )
#define SL_MATRIX4_ONE  ( static_cast<T> ( 1 ) )
#define SL_MATRIX4_HALF ( static_cast<T> ( 0.5 ) )
#define SL_MATRIX4_TWO  ( static_cast<T> ( 2 ) )
#define SL_MATRIX4_IDENTITY_D 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0
#define SL_MATRIX4_IDENTITY_F 1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f
#define SL_MATRIX4_IDENTITY_I 1,   0,   0,   0,   0,   1,   0,   0,   0,   0,   1,   0,   0,   0,   0,   1


namespace CadKit
{
template<class T> class SlMatrix4
{
public:

  typedef T Array4x4[4][4];
  typedef T Real; // For readability.
  typedef T Type; // For the client.
  typedef SlMatrix4<T> Matrix4;
  typedef SlVec3<T> Vec3;
  typedef SlVec4<T> Vec4;

  SlMatrix4 ( const bool &setToIdentity = true );
  SlMatrix4 ( const T m[16] );
  SlMatrix4 ( const Array4x4 &m );
  SlMatrix4 ( 
    const T &a0, const T &a4, const T &a8,  const T &a12,
    const T &a1, const T &a5, const T &a9,  const T &a13,
    const T &a2, const T &a6, const T &a10, const T &a14,
    const T &a3, const T &a7, const T &a11, const T &a15 );

  void                    absolute();
  static SlMatrix4        absolute ( const SlMatrix4 &M );

  bool                    billboard();

  T                       det() const;
  T                       det3() const;

  bool                    getBillboard ( SlMatrix4 &R ) const;
  static void             getFrustum ( const T &left, const T &right, const T &bottom, const T &top, const T &zNear, const T &zFar, Matrix4 &M );
  bool                    getInverse ( SlMatrix4 &M ) const;
  static void             getLookAt ( const Vec3 &eye, const Vec3 &center, const Vec3 &up, Matrix4 &M );
  static void             getPerspective ( const T &fovyInRadians, const T &aspect, const T &zNear, const T &zFar, Matrix4 &M );
  void                    getRotationAndScale ( SlMatrix4 &R ) const;
  void                    getScale ( Vec3 &scale ) const;
  void                    getTranslation ( Vec3 &translation ) const;
  void                    getTranspose ( SlMatrix4 &M ) const;

  T *                     getValue() { return _m; }
  const T *               getValue() const { return _m; }
  void                    getValue ( T m[16] ) const;
  void                    getValue ( Array4x4 &m ) const;
  void                    getValue ( 
                            T &a0, T &a4, T &a8,  T &a12,
                            T &a1, T &a5, T &a9,  T &a13,
                            T &a2, T &a6, T &a10, T &a14,
                            T &a3, T &a7, T &a11, T &a15 ) const;

  bool                    isEqualTo ( const SlMatrix4 &M ) const;
  bool                    isEqualTo ( const SlMatrix4 &M, const T &tolerance ) const;
  bool                    isNotEqualTo ( const SlMatrix4 &M ) const;
  bool                    isNotEqualTo ( const SlMatrix4 &M, const T &tolerance ) const;

  void                    identity();
  bool                    invert();

  bool                    isIdentity() const;
  bool                    isIdentity ( const T &tolerance ) const;
  bool                    isInverse ( const SlMatrix4 &M ) const;
  bool                    isInverse ( const SlMatrix4 &M, const T &tolerance ) const;
  bool                    isOrthogonal() const;
  bool                    isOrthogonal ( const T &tolerance ) const;
  bool                    isTranspose ( const SlMatrix4 &M ) const;

  void                    lookAt ( const Vec3 &eye, const Vec3 &center, const Vec3 &up );

  static void             multiply ( const SlMatrix4<T> &A, const SlMatrix4<T> &B, SlMatrix4<T> &C );
  void                    multLeft ( const SlMatrix4 &M );
  void                    multRight ( const SlMatrix4 &M );
  void                    multByUpper3x3 ( const Vec3 &input, Vec3 &output ) const;

  // Typecast operators.
  operator T *()             { return _m; }
  operator const T *() const { return _m; }

  // Bracket operators.
  // Note: Keep this a hard-coded type, otherwise VC++ yacks.
  T &                     operator [] ( int i );
  const T &               operator [] ( int i ) const;

  // Misc operators.
  SlMatrix4 &             operator *= ( const SlMatrix4 &M );
  SlMatrix4 &             operator /= ( const T &value );

  // Access the i'th row and j'th column of the matrix (as if it were a 2D array), like matrix(i,j).
  const T &               operator () ( const int &i, const int &j ) const;
  T &                     operator () ( const int &i, const int &j );

  // Transform by "a twist about an arbitrary axis". Make sure "axis" is normalized.
  void                    rotate ( const T &radians, const Vec3 &axis );

  void                    rotateX ( const T &radians );
  void                    rotateY ( const T &radians );
  void                    rotateZ ( const T &radians );

  void                    scale ( const Vec3 &s );

  void                    setLookAt ( const Vec3 &eye, const Vec3 &center, const Vec3 &up );
  void                    setScale ( const Vec3 &s );

  // Set the matrix from the given quaternion rotation.
  void                    setFromQuaternion ( const Vec4 &quat );

  void                    setFrustum ( const T &left, const T &right, const T &bottom, const T &top, const T &zNear, const T &zFar );
  void                    setPerspective ( const T &fovyInRadians, const T &aspect, const T &zNear, const T &zFar );

  // Set the twist about an arbitrary axis. Call the second one if you already 
  // have the trig function values.
  void                    setRotation ( const T &radians, const Vec3 &axis );
  void                    setRotation ( const T &cosine, const T &sine, const Vec3 &axis );

  // Only copies the upper 3x3.
  void                    setRotation ( const SlMatrix4 &M );

  void                    setRotationX ( const T &radians );
  void                    setRotationY ( const T &radians );
  void                    setRotationZ ( const T &radians );

  // Set this matrix to rotate the given x-axis to the global x-axis,
  // and the given y-axis to the global y-axis.
  // Pass two normalized orthogonal vectors, otherwise results are undefined.
  void                    setRotationXY ( const Vec3 &x, const Vec3 &y );
  void                    setRotationYZ ( const Vec3 &y, const Vec3 &z );
  void                    setRotationZX ( const Vec3 &z, const Vec3 &x );

  void                    setTranslation ( const Vec3 &t );

  void                    setValue ( const SlMatrix4 &M );
  void                    setValue ( const Array4x4 &m );
  void                    setValue ( const T vec[16] );
  void                    setValue ( 
                            const T &a0, const T &a4, const T &a8,  const T &a12,
                            const T &a1, const T &a5, const T &a9,  const T &a13,
                            const T &a2, const T &a6, const T &a10, const T &a14,
                            const T &a3, const T &a7, const T &a11, const T &a15 );

  void                    translate ( const Vec3 &t );

  void                    transpose();

protected:

  T _m[16];

  T                       _det3 ( int r1, int r2, int r3, int c1, int c2, int c3 ) const;

  void                    _getOrthogonalCandidate ( SlMatrix4 &M ) const;

  static void             _multiply ( const T a[16], const T b[16], T c[16] );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Additional operators. These are not members of the class because compilers
//  vary too much in the proper syntax for friend functions in templates. 
//  See http://gcc.gnu.org/faq.html#friend and http://www.bero.org/gcc296.html
//
///////////////////////////////////////////////////////////////////////////////

template<class T> SlMatrix4<T> operator *  ( const SlMatrix4<T> &A, const SlMatrix4<T> &B );
template<class T> SlVec4<T>    operator *  ( const SlMatrix4<T> &M, const SlVec4<T> &v );
template<class T> SlVec3<T>    operator *  ( const SlMatrix4<T> &M, const SlVec3<T> &v );
template<class T> SlVec4<T>    operator /  ( const SlVec4<T> &v,    const SlMatrix4<T> &M );
template<class T> SlVec3<T>    operator /  ( const SlVec3<T> &v,    const SlMatrix4<T> &M );
template<class T> bool         operator == ( const SlMatrix4<T> &A, const SlMatrix4<T> &B );
template<class T> bool         operator != ( const SlMatrix4<T> &A, const SlMatrix4<T> &B );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlMatrix4<T>::SlMatrix4 ( const bool &setToIdentity )
{
  // Set to identity if we are supposed to.
  if ( setToIdentity )
    this->identity();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlMatrix4<T>::SlMatrix4 ( const T m[16] )
{
  _m[0] = m[0]; _m[4] = m[4]; _m[8]  = m[8];  _m[12] = m[12];
  _m[1] = m[1]; _m[5] = m[5]; _m[9]  = m[9];  _m[13] = m[13];
  _m[2] = m[2]; _m[6] = m[6]; _m[10] = m[10]; _m[14] = m[14];
  _m[3] = m[3]; _m[7] = m[7]; _m[11] = m[11]; _m[15] = m[15];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlMatrix4<T>::SlMatrix4 ( const Array4x4 &m )
{
  _m[0] = m[0][0]; _m[4] = m[0][1]; _m[8]  = m[0][2]; _m[12] = m[0][3];
  _m[1] = m[1][0]; _m[5] = m[1][1]; _m[9]  = m[1][2]; _m[13] = m[1][3];
  _m[2] = m[2][0]; _m[6] = m[2][1]; _m[10] = m[2][2]; _m[14] = m[2][3];
  _m[3] = m[3][0]; _m[7] = m[3][1]; _m[11] = m[3][2]; _m[15] = m[3][3];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlMatrix4<T>::SlMatrix4 (
  const T &a0, const T &a4, const T &a8,  const T &a12,
  const T &a1, const T &a5, const T &a9,  const T &a13,
  const T &a2, const T &a6, const T &a10, const T &a14,
  const T &a3, const T &a7, const T &a11, const T &a15 )
{
  _m[0] = a0; _m[4] = a4; _m[8]  = a8;  _m[12] = a12;
  _m[1] = a1; _m[5] = a5; _m[9]  = a9;  _m[13] = a13;
  _m[2] = a2; _m[6] = a6; _m[10] = a10; _m[14] = a14;
  _m[3] = a3; _m[7] = a7; _m[11] = a11; _m[15] = a15;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Bracket operator.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T &SlMatrix4<T>::operator [] ( int i )
{
  SL_ASSERT ( this && i >= 0 && i <= 15 );
  return _m[i];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Bracket operator.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline const T &SlMatrix4<T>::operator [] ( int i ) const
{
  SL_ASSERT ( this && i >= 0 && i <= 15 );
  return _m[i];
}


//////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::setValue ( const Matrix4 &M )
{
  _m[0] = M._m[0]; _m[4] = M._m[4]; _m[8]  = M._m[8];  _m[12] = M._m[12];
  _m[1] = M._m[1]; _m[5] = M._m[5]; _m[9]  = M._m[9];  _m[13] = M._m[13];
  _m[2] = M._m[2]; _m[6] = M._m[6]; _m[10] = M._m[10]; _m[14] = M._m[14];
  _m[3] = M._m[3]; _m[7] = M._m[7]; _m[11] = M._m[11]; _m[15] = M._m[15];
}


//////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::setValue ( const Array4x4 &m )
{
  _m[0] = m[0][0]; _m[4] = m[0][1]; _m[8]  = m[0][2]; _m[12] = m[0][3];
  _m[1] = m[1][0]; _m[5] = m[1][1]; _m[9]  = m[1][2]; _m[13] = m[1][3];
  _m[2] = m[2][0]; _m[6] = m[2][1]; _m[10] = m[2][2]; _m[14] = m[2][3];
  _m[3] = m[3][0]; _m[7] = m[3][1]; _m[11] = m[3][2]; _m[15] = m[3][3];
}


//////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::setValue ( const T m[16] )
{
  _m[0] = m[0]; _m[4] = m[4]; _m[8]  = m[8];  _m[12] = m[12];
  _m[1] = m[1]; _m[5] = m[5]; _m[9]  = m[9];  _m[13] = m[13];
  _m[2] = m[2]; _m[6] = m[6]; _m[10] = m[10]; _m[14] = m[14];
  _m[3] = m[3]; _m[7] = m[7]; _m[11] = m[11]; _m[15] = m[15];
}


//////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::setValue ( 
  const T &a0, const T &a4, const T &a8,  const T &a12,
  const T &a1, const T &a5, const T &a9,  const T &a13,
  const T &a2, const T &a6, const T &a10, const T &a14,
  const T &a3, const T &a7, const T &a11, const T &a15 )
{
  _m[0] = a0; _m[4] = a4; _m[8]  = a8;  _m[12] = a12;
  _m[1] = a1; _m[5] = a5; _m[9]  = a9;  _m[13] = a13;
  _m[2] = a2; _m[6] = a6; _m[10] = a10; _m[14] = a14;
  _m[3] = a3; _m[7] = a7; _m[11] = a11; _m[15] = a15;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make all the components positive.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::absolute()
{
  _m[0]  = SL_ABS ( _m[0] );
  _m[1]  = SL_ABS ( _m[1] );
  _m[2]  = SL_ABS ( _m[2] );
  _m[3]  = SL_ABS ( _m[3] );

  _m[4]  = SL_ABS ( _m[4] );
  _m[5]  = SL_ABS ( _m[5] );
  _m[6]  = SL_ABS ( _m[6] );
  _m[7]  = SL_ABS ( _m[7] );

  _m[8]  = SL_ABS ( _m[8] );
  _m[9]  = SL_ABS ( _m[9] );
  _m[10] = SL_ABS ( _m[10] );
  _m[11] = SL_ABS ( _m[11] );

  _m[12] = SL_ABS ( _m[12] );
  _m[13] = SL_ABS ( _m[13] );
  _m[14] = SL_ABS ( _m[14] );
  _m[15] = SL_ABS ( _m[15] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the absolute value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline SlMatrix4<T> SlMatrix4<T>::absolute ( const Matrix4 &M )
{
  return SlMatrix4 (
    SL_ABS ( M[0] ), SL_ABS ( M[4] ), SL_ABS ( M[8] ),  SL_ABS ( M[12] ),
    SL_ABS ( M[1] ), SL_ABS ( M[5] ), SL_ABS ( M[9] ),  SL_ABS ( M[13] ),
    SL_ABS ( M[2] ), SL_ABS ( M[6] ), SL_ABS ( M[10] ), SL_ABS ( M[14] ),
    SL_ABS ( M[3] ), SL_ABS ( M[7] ), SL_ABS ( M[11] ), SL_ABS ( M[15] ) );
}


//////////////////////////////////////////////////////////////////////////
//
//  Multiply, c = a * b.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::_multiply 
  ( const T a[16], const T b[16], T c[16] )
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


//////////////////////////////////////////////////////////////////////////
//
//  Multiply this matrix on the left by the given matrix. The answer 
//  becomes this matrix.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::multLeft ( const Matrix4 &M )
{
  T a[16];
  this->_multiply ( M._m, _m, a );
  this->setValue ( a );
}


//////////////////////////////////////////////////////////////////////////
//
//  Multiply this matrix on the right by the given matrix. The answer 
//  becomes this matrix.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::multRight ( const Matrix4 &M )
{
  T a[16];
  this->_multiply ( _m, M._m, a );
  this->setValue ( a );
}


//////////////////////////////////////////////////////////////////////////
//
//  Multiply the input vector by the upper 3x3 only.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::multByUpper3x3 ( const Vec3 &b, Vec3 &a ) const
{
  a[0] = _m[0] * b[0] + _m[4] * b[1] + _m[8]  * b[2];
  a[1] = _m[1] * b[0] + _m[5] * b[1] + _m[9]  * b[2];
  a[2] = _m[2] * b[0] + _m[6] * b[1] + _m[10] * b[2];
}


//////////////////////////////////////////////////////////////////////////
//
//  Divide all the members by the value.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline SlMatrix4<T> &SlMatrix4<T>::operator /= ( const T &value )
{
  T inv = SL_MATRIX4_ONE / value;

  _m[0] *= inv; _m[4] *= inv; _m[8]  *= inv; _m[12] *= inv;
  _m[1] *= inv; _m[5] *= inv; _m[9]  *= inv; _m[13] *= inv;
  _m[2] *= inv; _m[6] *= inv; _m[10] *= inv; _m[14] *= inv;
  _m[3] *= inv; _m[7] *= inv; _m[11] *= inv; _m[15] *= inv;

  return *this;
}


//////////////////////////////////////////////////////////////////////////
//
//  Transpose this matrix. (There's probably an optimized way to do this).
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::transpose()
{
  T temp;
  SL_SWAP ( _m[1],  _m[4],  temp );
  SL_SWAP ( _m[2],  _m[8],  temp );
  SL_SWAP ( _m[3],  _m[12], temp );
  SL_SWAP ( _m[6],  _m[9],  temp );
  SL_SWAP ( _m[7],  _m[13], temp );
  SL_SWAP ( _m[11], _m[14], temp );
}


//////////////////////////////////////////////////////////////////////////
//
//  Get the determinate of the specified 3 x 3 submatrix.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline T SlMatrix4<T>::_det3 ( int r1, int r2, int r3, int c1, int c2, int c3 ) const
{
  // Convert to the correct indices.
  c1 *= 4;
  c2 *= 4;
  c3 *= 4;

  // Return the answer.
  return
    _m[r1 + c1] * ( _m[r2 + c2] * _m[r3 + c3] - _m[r2 + c3] * _m[r3 + c2] ) -
    _m[r1 + c2] * ( _m[r2 + c1] * _m[r3 + c3] - _m[r2 + c3] * _m[r3 + c1] ) +
    _m[r1 + c3] * ( _m[r2 + c1] * _m[r3 + c2] - _m[r2 + c2] * _m[r3 + c1] );
}


//////////////////////////////////////////////////////////////////////////
//
//  Get the determinate of the matrix.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline T SlMatrix4<T>::det() const
{
  return
    _m[0]  * this->_det3 ( 1, 2, 3, 1, 2, 3 ) -
    _m[4]  * this->_det3 ( 1, 2, 3, 0, 2, 3 ) +
    _m[8]  * this->_det3 ( 1, 2, 3, 0, 1, 3 ) -
    _m[12] * this->_det3 ( 1, 2, 3, 0, 1, 2 );
}


//////////////////////////////////////////////////////////////////////////
//
//  Get the determinate of the upper left 3 x 3 submatrix.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline T SlMatrix4<T>::det3() const
{
  return this->_det3 ( 0, 1, 2, 0, 1, 2 );
}


//////////////////////////////////////////////////////////////////////////
//
//  Get the inverse.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlMatrix4<T>::getInverse ( Matrix4 &M ) const
{
  Real d = this->det();

  if ( 0 == d ) 
    return false;

  M[0]  =   this->_det3 ( 1, 2, 3, 1, 2, 3 );
  M[4]  = - this->_det3 ( 1, 2, 3, 0, 2, 3 );
  M[8]  =   this->_det3 ( 1, 2, 3, 0, 1, 3 );
  M[12] = - this->_det3 ( 1, 2, 3, 0, 1, 2 );

  M[1]  = - this->_det3 ( 0, 2, 3, 1, 2, 3 );
  M[5]  =   this->_det3 ( 0, 2, 3, 0, 2, 3 );
  M[9]  = - this->_det3 ( 0, 2, 3, 0, 1, 3 );
  M[13] =   this->_det3 ( 0, 2, 3, 0, 1, 2 );

  M[2]  =   this->_det3 ( 0, 1, 3, 1, 2, 3 );
  M[6]  = - this->_det3 ( 0, 1, 3, 0, 2, 3 );
  M[10] =   this->_det3 ( 0, 1, 3, 0, 1, 3 );
  M[14] = - this->_det3 ( 0, 1, 3, 0, 1, 2 );

  M[3]  = - this->_det3 ( 0, 1, 2, 1, 2, 3 );
  M[7]  =   this->_det3 ( 0, 1, 2, 0, 2, 3 );
  M[11] = - this->_det3 ( 0, 1, 2, 0, 1, 3 );
  M[15] =   this->_det3 ( 0, 1, 2, 0, 1, 2 );

  M.transpose();
  M /= d;

  return true;
}


//////////////////////////////////////////////////////////////////////////
//
//  Get the rotation.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::getRotationAndScale ( Matrix4 &R ) const
{
  R.setValue ( 
    _m[0], _m[4], _m[8],      0,
    _m[1], _m[5], _m[9],      0,
    _m[2], _m[6], _m[10],     0,
        0,     0,     0,  _m[15] );
}


//////////////////////////////////////////////////////////////////////////
//
//  Return the scale part.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::getScale ( Vec3 &s ) const
{
  s.setValue ( _m[0], _m[5], _m[10] );
}


//////////////////////////////////////////////////////////////////////////
//
//  Return the translation part.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::getTranslation ( Vec3 &t ) const
{
  t.setValue ( _m[12], _m[13], _m[14] );
}


//////////////////////////////////////////////////////////////////////////
//
//  Get the transpose.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::getTranspose ( Matrix4 &M ) const
{
  M.setValue ( 
    _m[0],  _m[1],  _m[2],  _m[3],
    _m[4],  _m[5],  _m[6],  _m[7],
    _m[8],  _m[9],  _m[10], _m[11],
    _m[12], _m[13], _m[14], _m[15] );
}


//////////////////////////////////////////////////////////////////////////
//
//  Get the value.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::getValue ( T m[16] ) const
{
  m[0] = _m[0]; m[4] = _m[4]; m[8]  = _m[8];  m[12] = _m[12];
  m[1] = _m[1]; m[5] = _m[5]; m[9]  = _m[9];  m[13] = _m[13];
  m[2] = _m[2]; m[6] = _m[6]; m[10] = _m[10]; m[14] = _m[14];
  m[3] = _m[3]; m[7] = _m[7]; m[11] = _m[11]; m[15] = _m[15];
}


//////////////////////////////////////////////////////////////////////////
//
//  Get the value.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::getValue ( Array4x4 &m ) const
{
  m[0][0] = _m[0]; m[0][1] = _m[4]; m[0][2] = _m[8];  m[0][3] = _m[12];
  m[1][0] = _m[1]; m[1][1] = _m[5]; m[1][2] = _m[9];  m[1][3] = _m[13];
  m[2][0] = _m[2]; m[2][1] = _m[6]; m[2][2] = _m[10]; m[2][3] = _m[14];
  m[3][0] = _m[3]; m[3][1] = _m[7]; m[3][2] = _m[11]; m[3][3] = _m[15];
}


//////////////////////////////////////////////////////////////////////////
//
//  Get the value.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::getValue ( 
  T &a0, T &a4, T &a8,  T &a12,
  T &a1, T &a5, T &a9,  T &a13,
  T &a2, T &a6, T &a10, T &a14,
  T &a3, T &a7, T &a11, T &a15 ) const
{
  a0 = _m[0]; a4 = _m[4]; a8  = _m[8];  a12 = _m[12];
  a1 = _m[1]; a5 = _m[5]; a9  = _m[9];  a13 = _m[13];
  a2 = _m[2]; a6 = _m[6]; a10 = _m[10]; a14 = _m[14];
  a3 = _m[3]; a7 = _m[7]; a11 = _m[11]; a15 = _m[15];
}


//////////////////////////////////////////////////////////////////////////
//
//  See if they are equal within the given tolerance.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlMatrix4<T>::isEqualTo ( const Matrix4 &M, const Real &tolerance ) const
{
  return 
    ( SL_ABS ( _m[0]  - M._m[0] )  <= tolerance &&
      SL_ABS ( _m[1]  - M._m[1] )  <= tolerance &&
      SL_ABS ( _m[2]  - M._m[2] )  <= tolerance &&
      SL_ABS ( _m[3]  - M._m[3] )  <= tolerance &&

      SL_ABS ( _m[4]  - M._m[4] )  <= tolerance &&
      SL_ABS ( _m[5]  - M._m[5] )  <= tolerance &&
      SL_ABS ( _m[6]  - M._m[6] )  <= tolerance &&
      SL_ABS ( _m[7]  - M._m[7] )  <= tolerance &&

      SL_ABS ( _m[8]  - M._m[8] )  <= tolerance &&
      SL_ABS ( _m[9]  - M._m[9] )  <= tolerance &&
      SL_ABS ( _m[10] - M._m[10] ) <= tolerance &&
      SL_ABS ( _m[11] - M._m[11] ) <= tolerance &&

      SL_ABS ( _m[12] - M._m[12] ) <= tolerance &&
      SL_ABS ( _m[13] - M._m[13] ) <= tolerance &&
      SL_ABS ( _m[14] - M._m[14] ) <= tolerance &&
      SL_ABS ( _m[15] - M._m[15] ) <= tolerance );
}


//////////////////////////////////////////////////////////////////////////
//
//  See if they are equal.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlMatrix4<T>::isEqualTo ( const Matrix4 &M ) const
{
  return ( 
    _m[0] == M[0] && _m[4] == M[4] && _m[8]  == M[8]  && _m[12] == M[12] && 
    _m[1] == M[1] && _m[5] == M[5] && _m[9]  == M[9]  && _m[13] == M[13] && 
    _m[2] == M[2] && _m[6] == M[6] && _m[10] == M[10] && _m[14] == M[14] && 
    _m[3] == M[3] && _m[7] == M[7] && _m[11] == M[11] && _m[15] == M[15] );
}


//////////////////////////////////////////////////////////////////////////
//
//  See if they are not equal within the given tolerance.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlMatrix4<T>::isNotEqualTo ( const Matrix4 &M, const Real &tolerance ) const
{
  return ( false == this->isEqualTo ( M, tolerance ) );
}


//////////////////////////////////////////////////////////////////////////
//
//  See if they are not equal.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlMatrix4<T>::isNotEqualTo ( const Matrix4 &M ) const
{
  return ( false == this->isEqualTo ( M ) );
}


//////////////////////////////////////////////////////////////////////////
//
//  Set this matrix to the identity.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::identity()
{
  _m[0] = 1; _m[4] = 0; _m[8]  = 0; _m[12] = 0;
  _m[1] = 0; _m[5] = 1; _m[9]  = 0; _m[13] = 0;
  _m[2] = 0; _m[6] = 0; _m[10] = 1; _m[14] = 0;
  _m[3] = 0; _m[7] = 0; _m[11] = 0; _m[15] = 1;
}


//////////////////////////////////////////////////////////////////////////
//
//  Invert this matrix.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlMatrix4<T>::invert()
{
  SlMatrix4 M;
  if ( false == this->getInverse ( M ) )
    return false;
  this->setValue ( M );
  return true;
}


//////////////////////////////////////////////////////////////////////////
//
//  See if it's the identity.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlMatrix4<T>::isIdentity() const
{
  return ( 
    _m[0] == 1 && _m[4] == 0 && _m[8]  == 0 && _m[12] == 0 &&
    _m[1] == 0 && _m[5] == 1 && _m[9]  == 0 && _m[13] == 0 &&
    _m[2] == 0 && _m[6] == 0 && _m[10] == 1 && _m[14] == 0 &&
    _m[3] == 0 && _m[7] == 0 && _m[11] == 0 && _m[15] == 1 );
}


//////////////////////////////////////////////////////////////////////////
//
//  See if it's the identity within the tolerence.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlMatrix4<T>::isIdentity ( const Real &tol ) const
{
  const Real CONST_1 ( SL_MATRIX4_ONE );
  return ( 
    SL_ABS ( _m[0]  - CONST_1 ) < tol && 
    SL_ABS ( _m[1]  )           < tol && 
    SL_ABS ( _m[2]  )           < tol && 
    SL_ABS ( _m[3]  )           < tol && 
    SL_ABS ( _m[4]  )           < tol && 
    SL_ABS ( _m[5]  - CONST_1 ) < tol && 
    SL_ABS ( _m[6]  )           < tol && 
    SL_ABS ( _m[7]  )           < tol && 
    SL_ABS ( _m[8]  )           < tol && 
    SL_ABS ( _m[9]  )           < tol && 
    SL_ABS ( _m[10] - CONST_1 ) < tol && 
    SL_ABS ( _m[11] )           < tol && 
    SL_ABS ( _m[12] )           < tol && 
    SL_ABS ( _m[13] )           < tol && 
    SL_ABS ( _m[14] )           < tol && 
    SL_ABS ( _m[15] - CONST_1 ) < tol );
}


//////////////////////////////////////////////////////////////////////////
//
//  See if it's the inverse.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlMatrix4<T>::isInverse ( const Matrix4 &M1 ) const
{
  SlMatrix4 M2 = (*this) * M1;
  return M2.isIdentity();
}


//////////////////////////////////////////////////////////////////////////
//
//  See if it's the inverse (within tolerance).
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlMatrix4<T>::isInverse ( const SlMatrix4 &M1, const T &tolerance ) const
{
  SlMatrix4 M2 = (*this) * M1;
  return M2.isIdentity ( tolerance );
}


//////////////////////////////////////////////////////////////////////////
//
//  Get the matrix that might be orthogonal.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::_getOrthogonalCandidate ( Matrix4 &M ) const
{
  // If this matrix times its transpose is the identity, 
  // then it is orthogonal.

  // Get the transpose.
  SlMatrix4 M1;
  this->getTranspose ( M1 );

  // Multiply this matrix by it's transpose.
  M = (*this) * M1;
}


//////////////////////////////////////////////////////////////////////////
//
//  See if it's orthogonal.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlMatrix4<T>::isOrthogonal() const
{
  // If this matrix times its transpose is the identity, 
  // then it is orthogonal.
  SlMatrix4 M;
  this->_getOrthogonalCandidate ( M );
  return M2.isIdentity();
}


//////////////////////////////////////////////////////////////////////////
//
//  See if it's orthogonal within tolerance.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlMatrix4<T>::isOrthogonal ( const Real &tolerance ) const
{
  // If this matrix times its transpose is the identity, 
  // then it is orthogonal.
  SlMatrix4 M;
  this->_getOrthogonalCandidate ( M );
  return M2.isIdentity ( tolerance );
}


//////////////////////////////////////////////////////////////////////////
//
//  See if it's the transpose.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlMatrix4<T>::isTranspose ( const Matrix4 &M ) const
{
  return ( 
    _m[0] == M[0]  && _m[4] == M[1]  && _m[8]  == M[2]  && _m[12] == M[3]  &&
    _m[1] == M[4]  && _m[5] == M[5]  && _m[9]  == M[6]  && _m[13] == M[7]  &&
    _m[2] == M[8]  && _m[6] == M[9]  && _m[10] == M[10] && _m[14] == M[11] &&
    _m[3] == M[12] && _m[7] == M[13] && _m[11] == M[14] && _m[15] == M[15] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the rotation about an arbitrary axis.
//  radians = an angle in radians
//  axis    = a normalized vector that specifies the axis to rotate about.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::setRotation ( const T &cosine, const T &sine, const Vec3 &axis )
{
  SL_ASSERT ( cosine >= -1 && cosine <= 1 );
  SL_ASSERT ( sine >= -1 && sine <= 1 );

  const Real CONST_1 ( SL_MATRIX4_ONE );
  const Real CONST_0 ( SL_MATRIX4_ZERO );
  const Real &x = axis[0];
  const Real &y = axis[1];
  const Real &z = axis[2];
  Real one_minus_cosine = CONST_1 - cosine;

  _m[0]  = x * x + ( CONST_1 - x * x ) * cosine;
  _m[1]  = x * y * one_minus_cosine + z * sine;
  _m[2]  = x * z * one_minus_cosine - y * sine;
  _m[3]  = CONST_0;

  _m[4]  = y * x * one_minus_cosine - z * sine;
  _m[5]  = y * y + ( CONST_1 - y * y ) * cosine;
  _m[6]  = y * z * one_minus_cosine + x * sine;
  _m[7]  = CONST_0;

  _m[8]  = z * x * one_minus_cosine + y * sine;
  _m[9]  = z * y * one_minus_cosine - x * sine;
  _m[10] = z * z + ( CONST_1 - z * z ) * cosine;
  _m[11] = CONST_0;

  _m[12] = CONST_0;
  _m[13] = CONST_0;
  _m[14] = CONST_0;
  _m[15] = CONST_1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the rotation about an arbitrary axis.
//  radians = an angle in radians
//  axis    = a normalized vector that specifies the axis to rotate about.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::setRotation ( const Real &radians, const Vec3 &axis )
{
  Real cosine = CadKit::getCosine ( radians );
  Real sine = CadKit::getSine ( radians );
  this->setRotation ( cosine, sine, axis );
}


/////////////////////////////////////////////////////////////////////////
//
//  Set the x-axis rotation.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::setRotationX ( const T &radians )
{
  T c = CadKit::getCosine ( radians );
  T s = CadKit::getSine   ( radians );

  _m[0] = 1; _m[4] = 0; _m[8]  =  0; _m[12] = 0;
  _m[1] = 0; _m[5] = c; _m[9]  = -s; _m[13] = 0;
  _m[2] = 0; _m[6] = s; _m[10] =  c; _m[14] = 0;
  _m[3] = 0; _m[7] = 0; _m[11] =  0; _m[15] = 1;
}


/////////////////////////////////////////////////////////////////////////
//
//  Set the y-axis rotation.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::setRotationY ( const T &radians )
{
  T c = CadKit::getCosine ( radians );
  T s = CadKit::getSine   ( radians );

  _m[0] =  c; _m[4] = 0; _m[8]  = s; _m[12] = 0;
  _m[1] =  0; _m[5] = 1; _m[9]  = 0; _m[13] = 0;
  _m[2] = -s; _m[6] = 0; _m[10] = c; _m[14] = 0;
  _m[3] =  0; _m[7] = 0; _m[11] = 0; _m[15] = 1;
}


/////////////////////////////////////////////////////////////////////////
//
//  Set the z-axis rotation.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::setRotationZ ( const T &radians )
{
  T c = CadKit::getCosine ( radians );
  T s = CadKit::getSine   ( radians );

  _m[0] = c; _m[4] = -s; _m[8]  = 0; _m[12] = 0;
  _m[1] = s; _m[5] =  c; _m[9]  = 0; _m[13] = 0;
  _m[2] = 0; _m[6] =  0; _m[10] = 1; _m[14] = 0;
  _m[3] = 0; _m[7] =  0; _m[11] = 0; _m[15] = 1;
}


/////////////////////////////////////////////////////////////////////////
//
//  Set the twist about an arbitrary axis.
//  radians = an angle in radians
//  axis    = a normalized vector that specifies the axis to rotate about.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::rotate ( const Real &radians, const Vec3 &axis )
{
  SlMatrix4 R;
  R.setRotation ( radians, axis );
  this->multRight ( R );
}


/////////////////////////////////////////////////////////////////////////
//
//  Rotate the matrix about the x-axis.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::rotateX ( const Real &radians )
{
  SlMatrix4 R;
  R.setRotationX ( radians );
  this->multRight ( R );
}


/////////////////////////////////////////////////////////////////////////
//
//  Rotate the matrix about the y-axis.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::rotateY ( const Real &radians )
{
  SlMatrix4 R;
  R.setRotationY ( radians );
  this->multRight ( R );
}


/////////////////////////////////////////////////////////////////////////
//
//  Rotate the matrix about the z-axis.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::rotateZ ( const Real &radians )
{
  SlMatrix4 R;
  R.setRotationZ ( radians );
  this->multRight ( R );
}


//////////////////////////////////////////////////////////////////////////
//
//  Set this matrix to rotate the given x-axis to the global x-axis,
//  and the given y-axis to the global y-axis.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::setRotationXY ( const Vec3 &x, const Vec3 &y )
{
  // Get the z-axis.
  Vec3 z = x.cross ( y );

  _m[0] = x[0]; _m[4] = x[1]; _m[8]  = x[2]; _m[12] = 0;
  _m[1] = y[0]; _m[5] = y[1]; _m[9]  = y[2]; _m[13] = 0;
  _m[2] = z[0]; _m[6] = z[1]; _m[10] = z[2]; _m[14] = 0;
  _m[3] =    0; _m[7] =    0; _m[11] =    0; _m[15] = 1;
}


//////////////////////////////////////////////////////////////////////////
//
//  Set this matrix to rotate the given y-axis to the global y-axis,
//  and the given z-axis to the global z-axis.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::setRotationYZ ( const Vec3 &y, const Vec3 &z )
{
  // Get the x-axis.
  Vec3 x = y.cross ( z );

  _m[0] = x[0]; _m[4] = x[1]; _m[8]  = x[2]; _m[12] = 0;
  _m[1] = y[0]; _m[5] = y[1]; _m[9]  = y[2]; _m[13] = 0;
  _m[2] = z[0]; _m[6] = z[1]; _m[10] = z[2]; _m[14] = 0;
  _m[3] =    0; _m[7] =    0; _m[11] =    0; _m[15] = 1;
}


//////////////////////////////////////////////////////////////////////////
//
//  Set this matrix to rotate the given z-axis to the global z-axis,
//  and the given x-axis to the global x-axis.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::setRotationZX ( const Vec3 &z, const Vec3 &x )
{
  // Get the y-axis.
  Vec3 y = z.cross ( x );

  _m[0] = x[0]; _m[4] = x[1]; _m[8]  = x[2]; _m[12] = 0;
  _m[1] = y[0]; _m[5] = y[1]; _m[9]  = y[2]; _m[13] = 0;
  _m[2] = z[0]; _m[6] = z[1]; _m[10] = z[2]; _m[14] = 0;
  _m[3] =    0; _m[7] =    0; _m[11] =    0; _m[15] = 1;
}


//////////////////////////////////////////////////////////////////////////
//
//  Set the translation.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::setTranslation ( const Vec3 &t )
{
  _m[0] = 1; _m[4] = 0; _m[8]  = 0; _m[12] = t[0];
  _m[1] = 0; _m[5] = 1; _m[9]  = 0; _m[13] = t[1];
  _m[2] = 0; _m[6] = 0; _m[10] = 1; _m[14] = t[2];
  _m[3] = 0; _m[7] = 0; _m[11] = 0; _m[15] =    1;
}


//////////////////////////////////////////////////////////////////////////
//
//  Translate the matrix.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::translate ( const Vec3 &t )
{
  SlMatrix4 M;
  M.setTranslation ( t );
  this->multRight ( M );
}


//////////////////////////////////////////////////////////////////////////
//
//  Set the scale.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::setScale ( const Vec3 &s )
{
  _m[0] = s[0]; _m[4] =    0; _m[8]  =    0; _m[12] = 0;
  _m[1] =    0; _m[5] = s[1]; _m[9]  =    0; _m[13] = 0;
  _m[2] =    0; _m[6] =    0; _m[10] = s[2]; _m[14] = 0;
  _m[3] =    0; _m[7] =    0; _m[11] =    0; _m[15] = 1;
}


//////////////////////////////////////////////////////////////////////////
//
//  Scale the matrix.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::scale ( const Vec3 &s )
{
  Matrix4 S;
  S.setScale ( s );
  this->multRight ( S );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Eliminate the rotations from the matrix by rotating it to be 
//  alligned with the global axes.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlMatrix4<T>::billboard()
{
  // Set the constant vector values.
  const Vec3 upVector    ( 0, 1, 0 );
  const Vec3 frontVector ( 0, 0, 1 );
  const Vec3 eyePosition ( 0, 0, 1 );
  const Vec3 upPosition  ( 0, 1, 0 );

  //  
  // First, align the front vector with the camera vector.
  // This will make the billboard face the camera.
  //

  // Get the inverse of this matrix.
  SlMatrix4 temp;
  if ( !this->getInverse ( temp ) ) 
    return false;

  // Get the eye vector.
  Vec3 eyeVector;
  temp.multByUpper3x3 ( eyePosition, eyeVector );
  eyeVector.normalize();

  // We use these below but we have to declare it up here.
  Vec3 axis;

  // If the vectors are not equal (because we do a cross product)...
  if ( eyeVector != frontVector )
  {
    // Get the rotation axis.
    axis.setValue ( eyeVector.cross ( frontVector ) );

    // Sometimes we still get a zero vector even though we checked 
    // (eyeVector != frontVector). This must be from roundoff error.
    if ( !( 0 == axis[0] && 0 == axis[1] && 0 == axis[2] ) )
    {
      // Normalize the axis.
      axis.normalize();

      // Rotate to align the front vector.
      Real radians = CadKit::getArcCosine ( eyeVector.dot ( frontVector ) );
      this->rotate ( - radians, axis );
    }
  }

  //
  // Second, align the up vector.
  // This will make the billboard point along the up vector.
  //

  // Take the inverse of the matrix (again, it changed).
  if ( !this->getInverse ( temp ) ) 
    return false;

  // Get the psuedo eye vector (since the real eye vector is pointing at the camera).
  temp.multByUpper3x3 ( upPosition, eyeVector );
  eyeVector.normalize();

  // If the vectors are not equal (because we do a cross product)...
  if ( eyeVector != upVector )
  {
    // Get the rotation axis.
    axis.setValue ( eyeVector.cross ( upVector ) );

    // Sometimes we still get a zero vector even though we checked 
    // (eyeVector != upVector). This must be from roundoff error.
    if ( !( 0 == axis[0] && 0 == axis[1] && 0 == axis[2] ) )
    {
      // Normalize the axis.
      axis.normalize();

      // Rotate to align the up vector.
      Real radians = CadKit::getArcCosine ( eyeVector.dot ( upVector ) );
      this->rotate ( - radians, axis );
    }
  }

  // All done.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the matrix which eliminate the rotations by rotating it to be 
//  alligned with the global axes.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool SlMatrix4<T>::getBillboard ( Matrix4 &R ) const
{
  R.setValue ( *this );
  return R.billboard();
}


/////////////////////////////////////////////////////////////////////////
//
//  Get this matrix like a call to "glIdentity(); glLookAt();"
//  eye    = your vantage point.
//  center = the place you want to look.
//  up     = a vector that defines "up".
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::getLookAt ( const Vec3 &eye, const Vec3 &center, const Vec3 &up, Matrix4 &M )
{
  Vec3 upVec = up;
  upVec.normalize();

  Vec3 n = eye - center;
  n.normalize();

  Vec3 v = upVec - center;
  Real d = v.dot ( n );
  v -= n * d;
  v.normalize();

  Vec3 u = v.cross(n);

  M[0]  = u[0];
  M[4]  = u[1];
  M[8]  = u[2];

  M[1]  = v[0];
  M[5]  = v[1];
  M[9]  = v[2];
  
  M[2]  = n[0];
  M[6]  = n[1];
  M[10] = n[2];
  
  M[12] = -u.dot ( eye );
  M[13] = -v.dot ( eye );
  M[14] = -n.dot ( eye );
  
  M[3]  = 0;
  M[7]  = 0;
  M[11] = 0;
  M[15] = 1;
}


//////////////////////////////////////////////////////////////////////////
//
//  Get this matrix like a call to "glIdentity(); glFrustum();"
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::getFrustum 
  ( const T &left, const T &right, const T &bottom, const T &top, const T &zNear, const T &zFar, Matrix4 &M )
{
  const Real CONST_2 ( SL_MATRIX4_TWO );

  M[0]  = ( CONST_2 * zNear ) / ( right - left );
  M[1]  = 0;
  M[2]  = 0;
  M[3]  = 0;

  M[4]  = 0;
  M[5]  = ( CONST_2 * zNear ) / ( top - bottom );
  M[6]  = 0;
  M[7]  = 0;

  M[8]  = ( right + left ) / ( right - left );
  M[9]  = ( top + bottom ) / ( top - bottom );
  M[10] = - ( zFar + zNear ) / ( zFar - zNear );
  M[11] = -1;

  M[12] = 0;
  M[13] = 0;
  M[14] = -( CONST_2 * zFar * zNear ) / ( zFar - zNear );
  M[15] = 0;
}


//////////////////////////////////////////////////////////////////////////
//
//  Get this matrix like a call to "glIdentity(); gluPerspective();"
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::getPerspective
  ( const T &fovyInRadians, const T &aspect, const T &zNear, const T &zFar, Matrix4 &M )
{
  Real ymax ( zNear * SL_TANGENT ( fovyInRadians * SL_MATRIX4_HALF ) );
  Real ymin ( -ymax );
  Real xmin ( ymin * aspect );
  Real xmax ( ymax * aspect );
  getFrustum ( xmin, xmax, ymin, ymax, zNear, zFar, M );
}


/////////////////////////////////////////////////////////////////////////
//
//  Set this matrix to be the "lookat" matrix.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::setLookAt ( const Vec3 &eye, const Vec3 &center, const Vec3 &up )
{
  this->getLookAt ( eye, center, up, *this );
}


/////////////////////////////////////////////////////////////////////////
//
//  The "lookat" matrix post-multiplies the current matrix.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::lookAt ( const Vec3 &eye, const Vec3 &center, const Vec3 &up )
{
  SlMatrix4 L;
  this->getLookAt ( eye, center, up, L );
  this->multRight ( L );
}


/////////////////////////////////////////////////////////////////////////
//
//  Set this matrix be a frustum.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::setFrustum ( const T &left, const T &right, const T &bottom, const T &top, const T &zNear, const T &zFar )
{
  this->getFrustum ( left, right, bottom, top, zNear, zFar, *this );
}


/////////////////////////////////////////////////////////////////////////
//
//  Set this matrix be a perspective matrix (a frustum).
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::setPerspective ( const T &fovyInRadians, const T &aspect, const T &zNear, const T &zFar )
{
  this->getPerspective ( fovyInRadians, aspect, zNear, zFar, *this );
}


//////////////////////////////////////////////////////////////////////////
//
//  This matrix gets multiplied on the right by the given matrix.
//  The result becomes this matrix.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline SlMatrix4<T> &SlMatrix4<T>::operator *= ( const Matrix4 &M )
{
  this->multRight ( M );
}


//////////////////////////////////////////////////////////////////////////
//
//  Multiply, C = A * B. Public wrapper around _multiply(), provided for
//  "operator * ()" below.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::multiply 
  ( const SlMatrix4<T> &A, const SlMatrix4<T> &B, SlMatrix4<T> &C )
{
  // Call the other one.
  SlMatrix4<T>::_multiply ( A._m, B._m, C._m );
}


//////////////////////////////////////////////////////////////////////////
//
//  Return the product.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline SlMatrix4<T> operator * ( const SlMatrix4<T> &A, const SlMatrix4<T> &B )
{
  SlMatrix4<T> C;
  SlMatrix4<T>::multiply ( A, B, C );
  return C;
}


//////////////////////////////////////////////////////////////////////////
//
//  Return the product.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec3<T> operator * ( const SlMatrix4<T> &M, const SlVec3<T> &b )
{
  T a[4];

  a[3] = SL_MATRIX4_ONE / ( M[3] * b[0] + M[7] * b[1] + M[11] * b[2] + M[15] );
  a[2] =           a[3] * ( M[2] * b[0] + M[6] * b[1] + M[10] * b[2] + M[14] );
  a[1] =           a[3] * ( M[1] * b[0] + M[5] * b[1] + M[9]  * b[2] + M[13] );
  a[0] =           a[3] * ( M[0] * b[0] + M[4] * b[1] + M[8]  * b[2] + M[12] );

  return SlVec3<T> ( a[0], a[1], a[2] );
}


//////////////////////////////////////////////////////////////////////////
//
//  Return the product.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec4<T> operator * ( const SlMatrix4<T> &M, const SlVec4<T> &b )
{
  return SlVec4<T> ( 
    M[0] * b[0] + M[4] * b[1] + M[8]  * b[2] + M[12] * b[3],
    M[1] * b[0] + M[5] * b[1] + M[9]  * b[2] + M[13] * b[3],
    M[2] * b[0] + M[6] * b[1] + M[10] * b[2] + M[14] * b[3],
    M[3] * b[0] + M[7] * b[1] + M[11] * b[2] + M[15] * b[3] );
}


//////////////////////////////////////////////////////////////////////////
//
//  "Divide" the vector by the matrix.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec4<T> operator / ( const SlVec4<T> &v, const SlMatrix4<T> &M )
{
  SlMatrix4<T> invM;
  SL_VERIFY ( M.getInverse ( invM ) );
  return invM * v;
}


//////////////////////////////////////////////////////////////////////////
//
//  "Divide" the vector by the matrix.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline SlVec3<T> operator / ( const SlVec3<T> &v, const SlMatrix4<T> &M )
{
  SlMatrix4<T> invM;
  SL_VERIFY ( M.getInverse ( invM ) );
  return invM * v;
}


//////////////////////////////////////////////////////////////////////////
//
//  See if they are equal.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator == ( const SlMatrix4<T> &A, const SlMatrix4<T> &B )
{
  return A.isEqualTo ( B );
}


//////////////////////////////////////////////////////////////////////////
//
//  See if they are not equal.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline bool operator != ( const SlMatrix4<T> &A, const SlMatrix4<T> &B )
{
  return A.isNotEqualTo ( B );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access the i'th row and j'th column of the matrix as if it were a 2D 
//  array, like matrix(i,j).
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline T &SlMatrix4<T>::operator () ( const int &i, const int &j )
{
  SL_ASSERT ( i >= 0 && j >= 0 && i < 4 && j < 4 && j * 4 + i < 16 );
  return _m[j * 4 + i]; // This was backwards in the old (non-template) classes.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access the i'th row and j'th column of the matrix as if it were a 2D 
//  array, like matrix(i,j).
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline const T &SlMatrix4<T>::operator () ( const int &i, const int &j ) const
{
  SL_ASSERT ( i >= 0 && j >= 0 && i < 4 && j < 4 && j * 4 + i < 16 );
  return _m[j * 4 + i]; // This was backwards in the old (non-template) classes.
}


//////////////////////////////////////////////////////////////////////////
//
//  Set the matrix from the given quaternion rotation.
//
//////////////////////////////////////////////////////////////////////////

template<class T> inline void SlMatrix4<T>::setFromQuaternion ( const Vec4 &quat )
{
  const Real CONST_1 ( SL_MATRIX4_ONE );
  const Real CONST_2 ( SL_MATRIX4_TWO );

  _m[0]  = CONST_1 - CONST_2 * ( quat[1] * quat[1] + quat[2] * quat[2] );
  _m[1]  = CONST_2           * ( quat[0] * quat[1] - quat[2] * quat[3] );
  _m[2]  = CONST_2           * ( quat[2] * quat[0] + quat[1] * quat[3] );
  _m[3]  = 0;

  _m[4]  = CONST_2           * ( quat[0] * quat[1] + quat[2] * quat[3] );
  _m[5]  = CONST_1 - CONST_2 * ( quat[2] * quat[2] + quat[0] * quat[0] );
  _m[6]  = CONST_2           * ( quat[1] * quat[2] - quat[0] * quat[3] );
  _m[7]  = 0;

  _m[8]  = CONST_2           * ( quat[2] * quat[0] - quat[1] * quat[3] );
  _m[9]  = CONST_2           * ( quat[1] * quat[2] + quat[0] * quat[3] );
  _m[10] = CONST_1 - CONST_2 * ( quat[1] * quat[1] + quat[0] * quat[0] );
  _m[11] = 0;

  _m[12] = 0;
  _m[13] = 0;
  _m[14] = 0;
  _m[15] = 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//	Common types.
//
///////////////////////////////////////////////////////////////////////////////

typedef SlMatrix4<long double> SlMatrix4ld;
typedef SlMatrix4<double>      SlMatrix4d;
typedef SlMatrix4<float>       SlMatrix4f;


}; // namespace CadKit.


#endif // _CADKIT_STANDARD_LIBRARY_4_BY_4_TEMPLATE_MATRIX_CLASS_H_
