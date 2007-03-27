
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlMatrixNN: An NxN matrix class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_N_BY_N_TEMPLATE_MATRIX_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_N_BY_N_TEMPLATE_MATRIX_CLASS_H_

#include "SlAssert.h"
#include "SlArrayPtr.h"
#include "SlConstants.h"
#include "SlSwap.h"

#include <algorithm>


namespace CadKit
{
template<class T, class I = unsigned int> class SlMatrixNN
{
public:

  typedef T Real;
  typedef I Index;

  SlMatrixNN() : _length ( 0 ), _size ( 0 ), _m ( 0x0 ){}
  SlMatrixNN ( const I &size );
  SlMatrixNN ( const I &size, T *m );
  SlMatrixNN ( const SlMatrixNN &copyMe );
  virtual ~SlMatrixNN();

  // Attach to the given pointer.
  void                    attach ( const I &size, T *m );

  // Clear the internal array.
  void                    clear() { this->attach ( 0, 0x0 ); }

  // Detach from the internal pointer. Returns the internal pointer. 
  // The caller is responsible for deleting it.
  T *                     detach();

  // This function assumes that the matrix was decompused using ludecomp(). 
  // Pass in the vector "b" in the equation Ax = b. The vector "x" will overwrite "b".
  void                    forwardBackward ( T *b ) const;

  // Get the size of the matrix. The size of an N x N matrix is N.
  const I &               getSize() const { return _size; }

  // Get the internal array.
  const T *               getValue() const { return _m; }
  T *                     getValue()       { return _m; }

  // Make the matrix identity.
  void                    identity();

  // Invert the matrix.
  bool                    invert();

  // Perform an in-place LU decomposition.
  void                    ludecomp();

  // Typecast operators.
  operator       T *()       { return _m; }
  operator const T *() const { return _m; }

  // Access the internal array.
  T &                     operator [] ( const I &i )       { SL_ASSERT ( i >= 0 && i < _length ); return _m[i]; }
  const T &               operator [] ( const I &i ) const { SL_ASSERT ( i >= 0 && i < _length ); return _m[i]; }

  // Assignment operator.
  SlMatrixNN &            operator = ( const SlMatrixNN &copyMe );

  // Access the i'th row and j'th column of the matrix (as if it were a 2D array).
  const T &               operator () ( const I &i, const I &j ) const;
  T &                     operator () ( const I &i, const I &j );

  // Set the size of the matrix. Pass true for "preserve" to copy the existing 
  // contents (as much as will fit) into the new matrix.
  bool                    setSize ( const I &size, bool preserve );

  // Set the value. The array has to be at least (size * size) in length.
  bool                    setValue ( const I &size, const T *m );
  bool                    setValue ( const SlMatrixNN &copyMe );

  // Transpose the matrix.
  void                    transpose();

  // Make the matrix all zeros.
  void                    zero();

protected:

  I _length;
  I _size;
  SlArrayPtr<T,I> _m;

  void                    _divideColumn ( const I &bsize, T *a, T *diag ) const;

  void                    _factorDiagonal ( const I &bsize, T *a ) const;
  
  void                    _getValue ( const I &n, const T &alpha, T *a, T *b ) const;
  
  void                    _modifyRow ( const I &bsize, T *a, T *diag ) const;
  void                    _modifyRowColumn ( const I &bsize, T *a, T *row, T *col ) const;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize list, for convenience.
//
///////////////////////////////////////////////////////////////////////////////

#define SL_MATRIX_NN_INITIALIZER_LIST _length ( 0 ), _size ( 0 ), _m ( 0x0 )


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline SlMatrixNN<T,I>::SlMatrixNN ( const I &size ) :
  SL_MATRIX_NN_INITIALIZER_LIST
{
  SL_VERIFY ( this->setSize ( size, false ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline SlMatrixNN<T,I>::SlMatrixNN ( const I &size, T *m ) :
  SL_MATRIX_NN_INITIALIZER_LIST
{
  this->attach ( size, m );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline SlMatrixNN<T,I>::SlMatrixNN ( const SlMatrixNN &copyMe ) :
  SL_MATRIX_NN_INITIALIZER_LIST
{
  SL_VERIFY ( this->setValue ( copyMe ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline SlMatrixNN<T,I>::~SlMatrixNN()
{
  // Nothing to delete, _m is a smart-pointer.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access the i'th row and j'th column of the matrix (as if it were a 
//  2D array).
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline const T &SlMatrixNN<T,I>::operator () ( const I &i, const I &j ) const 
{
  SL_ASSERT ( i >= 0 && j >= 0 && i < _size && j < _size );
  return _m[i*_size+j];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access the i'th row and j'th column of the matrix (as if it were a 
//  2D array).
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline T &SlMatrixNN<T,I>::operator () ( const I &i, const I &j )
{
  SL_ASSERT ( i >= 0 && j >= 0 && i < _size && j < _size );
  return _m[i*_size+j];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the matrix to identity.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline void SlMatrixNN<T,I>::identity()
{
  I i, j, temp;
  T *element = _m;

  for ( i = 0; i < _size; ++i )
  {
    temp = _size * i;

    for ( j = 0; j < _size; ++j )
    {
      *element = ( i == j ) ? 1 : 0;
      ++element;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Invert the matrix.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline bool SlMatrixNN<T,I>::invert()
{
  SL_ASSERT ( 0 ); // TODO.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the matrix to zero.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline void SlMatrixNN<T,I>::zero()
{
  T *element = _m;
  T *end = _m + _length;

  while ( start != end )
  {
    *element = 0;
    ++element;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline bool SlMatrixNN<T,I>::setSize ( const I &size, bool preserve )
{
  // Trivial case.
  if ( size == _size ) 
    return true;

  // Another trivial case.
  if ( 0 == size )
  {
    this->clear();
    return true;
  }

  // The new length.
  I length ( size * size );

  // Allocate.
  SlArrayPtr<T> m ( new T[length] );

  // Check allocation.
  if ( m.isNull() )
    return false;

  // Copy what we can into the new array if we are supposed to.
  if ( true == preserve )
  {
    I min ( std::min ( _length, length ) );
    SL_ASSERT ( 0 ); // Just added, is this right?
    std::copy ( _m.get(), _m.get() + min, m.get() );
  }

  // Assign the members.
  _length = length;
  _size = size;
  _m = m; // Smart-pointer assignment, not a leak.

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline bool SlMatrixNN<T,I>::setValue ( const SlMatrixNN &copyMe )
{
  // Call the other one.
  return this->setValue ( copyMe.getSize(), copyMe.getValue() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Attach to the given pointer.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline void SlMatrixNN<T,I>::attach ( const I &size, T *m )
{
  SL_ASSERT ( ( NULL != m && 0 < size ) || ( 0 == size && NULL == m ) );

  // Assign the members.
  _length = size * size;
  _size = size;
  _m = m; // Smart-pointer deletion of old array, if any.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Detach from the internal pointer.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline T *SlMatrixNN<T,I>::detach()
{
  // Initialize.
  _length = _size = 0;

  // Return the internal pointer, releasing ownership.
  return _m.detach();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline bool SlMatrixNN<T,I>::setValue ( const I &size, const T *m )
{
  SL_ASSERT ( ( NULL != m && 0 < size ) || ( 0 == size && NULL == m ) );

  // Set the size.
  if ( false == this->setSize ( size, false ) )
    return false;

  // Should be true.
  SL_ASSERT ( size * size == _length );
  SL_ASSERT ( size == _size );

  // Copy the values.
  SL_ASSERT ( 0 ); // Just added, is this right?
  std::copy ( m, m + _length, _m.get() );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment operator.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline SlMatrixNN<T,I> &SlMatrixNN<T,I>::operator = ( const SlMatrixNN &copyMe )
{
  SL_VERIFY ( this->setValue ( copyMe ) );
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the LU Decomposition.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline void SlMatrixNN<T,I>::_getValue ( const I &n, const T &alpha, T *a, T *b ) const
{
  for ( I i = 0; i < n; ++i )
    a[i] += alpha * b[i];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Factor the diagonal.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline void SlMatrixNN<T,I>::_factorDiagonal ( const I &bsize, T *a ) const
{
  I j, k;
  T alpha;
  const T CONST_0 ( SlConstants<T>::zero() );

  for ( k = 0; k < bsize; ++k )
  {
    // Modify subsequent columns.
    for ( j = k+1; j < bsize; ++j )
    {
      // Should not divide by zero.
      SL_ASSERT ( CONST_0 != a[_size*k+k] );

      a[_size*j+k] /= a[_size*k+k];

      // Check for a finite number (could happen if denominator is close to zero).
      SL_ASSERT ( CadKit::isFinite ( a[_size*j+k] ) );

      alpha = -a[_size*j+k];
      this->_getValue ( &a[_size*j+(k+1)], &a[_size*k+(k+1)], bsize-k-1, alpha );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Divide column by diagonal.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline void SlMatrixNN<T,I>::_divideColumn ( const I &bsize, T *a, T *diag ) const
{
  I j, k;
  T alpha;

  for ( k = 0; k < bsize; ++k )
  {
    for ( j = k+1; j < bsize; ++j )
    {
      alpha = -diag[_size*j+k];
      this->_getValue ( &a[_size*j], &a[_size*k], bsize, alpha );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Modify row using diagonal.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline void SlMatrixNN<T,I>::_modifyRow ( const I &bsize, T *a, T *diag ) const
{
  I j, k;
  T alpha;
  const T CONST_0 ( SlConstants<T>::zero() );

  for ( k = 0; k < bsize; ++k )
  {
    for ( j = 0; j < bsize; ++j )
    {
      // Should not divide by zero.
      SL_ASSERT ( diag[_size*k+k] != CONST_0 );

      a[_size*j+k] /= diag[_size*k+k];

      // Check for a finite number (could happen if denominator is close to zero).
      SL_ASSERT ( CadKit::isFinite ( a[_size*j+k] ) );

      alpha = -a[_size*j+k];
      this->_getValue ( &a[_size*j+(k+1)], &diag[_size*k+(k+1)], bsize-k-1, alpha );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Modify subsequent rows and columns.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline void SlMatrixNN<T,I>::_modifyRowColumn ( const I &bsize, T *a, T *row, T *col ) const
{
  I j, k;
  T alpha;

  for ( k = 0; k < bsize; ++k )
  {
    for ( j = 0; j < bsize; ++j )
    {
      alpha = -col[_size*j+k];
      this->_getValue ( &a[_size*j], &row[_size*k], bsize, alpha );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Perform an in-place LU decomposition.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline void SlMatrixNN<T,I>::ludecomp()
{
  SL_ASSERT ( _m && _size > 0 && _length == _size * _size );

  I i, j, k;

  // Loop over diagonal.
  for ( i = 0; i < _size; ++i )
  {
    // Factor diagonal.
    this->_factorDiagonal ( &_m[_size*i+i], 1 );

    // Divide column by diagonal.
    for ( j = i+1; j < _size; ++j )
      this->_divideColumn ( &_m[_size*i+j], &_m[_size*i+i], 1 );

    // Modify row using diagonal.
    for ( j = i+1; j < _size; ++j )
      this->_modifyRow ( &_m[_size*j+i], &_m[_size*i+i], 1 );

    // Modify subsequent rows and columns.
    for ( j = i+1; j < _size; ++j )
      for ( k = i+1; k < _size; ++k )
        this->_modifyRowColumn ( &_m[_size*j+k], &_m[_size*i+k], &_m[_size*j+i], 1 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Do the forward reduction and back substitution.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline void SlMatrixNN<T,I>::forwardBackward ( T *b ) const
{
  SL_ASSERT ( b );
  SL_ASSERT ( _m && _size > 0 && _length == _size * _size );

  I i, j;
  T sum;
  const T CONST_0 ( SlConstants<T>::zero() );

  // Forward reduction.
  for ( i = 0; i < _size; ++i )
  {
    sum = CONST_0;

    for ( j = 0; j < i; ++j )
      sum += _m[_size*i+j]*b[j];

    b[i] -= sum;
  }

  // Backward substitution.
  for ( i = _size-1; i >= 0; i-- )
  {
    sum = CONST_0;

    for ( j = _size-1; j > i; j-- )
      sum += _m[_size*i+j]*b[j];

    b[i] -= sum;

    // Do not divide by zero.
    SL_ASSERT ( CONST_0 != _m[_size*i+i] );

    b[i] /= _m[_size*i+i];

    // Check for a finite number (could happen if denominator is close to zero).
    SL_ASSERT ( CadKit::isFinite ( b[i] ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//	Transpose the matrix.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline void SlMatrixNN<T,I>::transpose()
{
  T temp;
  I i, j;
  for ( i = 0; i < _size; ++i )
    for ( j = i; j < _size; ++j )
      CadKit::swap ( (*this)(i,j), (*this)(j,i), temp );
}


///////////////////////////////////////////////////////////////////////////////
//
//	Common types.
//
///////////////////////////////////////////////////////////////////////////////

typedef SlMatrixNN<long double> SlMatrixNNld;
typedef SlMatrixNN<double>      SlMatrixNNd;
typedef SlMatrixNN<float>       SlMatrixNNf;


}; // namespace CadKit


#endif // _CADKIT_STANDARD_LIBRARY_N_BY_N_TEMPLATE_MATRIX_CLASS_H_
