
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//  SlMatrixNN: An NxN matrix class.
//
//////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_N_BY_N_TEMPLATE_MATRIX_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_N_BY_N_TEMPLATE_MATRIX_CLASS_H_

#include "SlDefine.h"
#include "SlTypedefs.h"


namespace CadKit
{
class SlMatrixNN
{
public:

  SlMatrixNN() : _arrayLength ( 0 ), _size ( 0 ), _m ( 0x0 ){}
  SlMatrixNN ( const SlInt32 size );
  SlMatrixNN ( const SlInt32 size, const SlFloat64 *m, bool makeCopy );
  SlMatrixNN ( const SlMatrixNN &copyMe );
  virtual ~SlMatrixNN();

  // Make the matrix identity.
  void                identity();

  // Make the matrix all zeros.
  void                zero();

  // Perform an LU decomposition.
  void                lud();

  // This function assumes that the matrix was decompused using lud(). 
  // Pass in the vector "b" in the equation Ax = b. The vector "x" will overwrite "b".
  void                forwardBackward ( SlFloat64 *b );

  // Get the internal array length.
  SlInt32                getArrayLength() const { return _arrayLength; }

  // Get the size.
  SlInt32                getSize() const { return _size; }

  // Get the internal array.
  const SlFloat64 *          getValue() const { return _m; }
  SlFloat64 *              getValue() { return _m; }

  // Convert to SlFloat64 *
                    operator     SlFloat64 *()     { return _m; }
                    operator const SlFloat64 *() const { return _m; }

  // Access the internal array.
  SlFloat64 &              operator [] ( SlInt32 i )    { SL_ASSERT ( i >= 0 && i < _arrayLength ); return _m[i]; }
  const SlFloat64 &          operator [] ( SlInt32 i ) const { SL_ASSERT ( i >= 0 && i < _arrayLength ); return _m[i]; }

  // Assignment operator.
  SlMatrixNN &            operator = ( const SlMatrixNN &copyMe );

  // Access the i'th row and j'th column of the matrix (as if it were a 2D array).
  const SlFloat64 &          operator () ( SlInt32 i, SlInt32 j ) const { SL_ASSERT ( i >= 0 && j >= 0 && i < _size && j < _size ); return _m[i*_size+j]; }
  SlFloat64 &              operator () ( SlInt32 i, SlInt32 j )       { SL_ASSERT ( i >= 0 && j >= 0 && i < _size && j < _size ); return _m[i*_size+j]; }

  // Set the size of the matrix. This will destroy an existing matrix.
  bool                setSize ( const SlInt32 size );

  // Set the value. Pass true for "makeCopy" if you want to make a hardcopy. 
  // Pass false if you want a pointer assignment. If you pass false then this 
  // class instance will "own" the array (i.e., it will delete it when appropriate).
  // The array has to be at least (size * size) in length.
  bool                setValue ( const SlInt32 size, const SlFloat64 *m, bool makeCopy );
  bool                setValue ( const SlMatrixNN &copyMe );

  // Return the size of the matrix.
  SlInt32                size() const { return _size; }

  // Transpose the matrix.
  void                    transpose();

protected:

  SlInt32 _arrayLength;
  SlInt32 _size;
  SlFloat64 *_m;

  void                    _factorDiagonal ( SlFloat64 *a, SlInt32 bsize );
  void                    _divideColumn ( SlFloat64 *a, SlFloat64 *diag, SlInt32 bsize );
  void                    _modifyRow ( SlFloat64 *a, SlFloat64 *diag, SlInt32 bsize );
  void                    _modifyRowColumn ( SlFloat64 *a, SlFloat64 *row, SlFloat64 *col, SlInt32 bsize );
  void                    _getValue ( SlFloat64 *a, SlFloat64 *b, SlInt32 n, SlFloat64 alpha );
};


//////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
//////////////////////////////////////////////////////////////////////////

SlMatrixNN::SlMatrixNN ( const SlInt32 size )
{
  SL_ASSERT ( size > 0 );

  _size = size;
  _arrayLength = size * size;

  _m = new SlFloat64 [_arrayLength];
  SL_ASSERT ( _m );
}


//////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
//////////////////////////////////////////////////////////////////////////

SlMatrixNN::SlMatrixNN ( const SlInt32 size, const SlFloat64 *m, bool makeCopy )
{
  SL_ASSERT ( size > 0 );
  SL_ASSERT ( m );

  _size = size;
  _arrayLength = size * size;

  if ( makeCopy )
  {
    _m = new SlFloat64 [_arrayLength];
    SL_ASSERT ( _m );
    for ( SlInt32 i = 0; i < _arrayLength; i++ ) _m[i] = m[i];
  }

  else _m = (SlFloat64 *) m;
}


//////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
//////////////////////////////////////////////////////////////////////////

SlMatrixNN::SlMatrixNN ( const SlMatrixNN &copyMe )
{
  SL_ASSERT ( copyMe._size > 0 );
  SL_ASSERT ( copyMe._arrayLength == copyMe._size * copyMe._size );
  SL_ASSERT ( copyMe._m );

  _size = copyMe._size;
  _arrayLength = _size * _size;

  _m = new SlFloat64 [copyMe._arrayLength];
  SL_ASSERT ( _m );

  _size = copyMe._size;
  _arrayLength = copyMe._arrayLength;

  for ( SlInt32 i = 0; i < _arrayLength; i++ ) _m[i] = copyMe._m[i];
}


//////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
//////////////////////////////////////////////////////////////////////////

SlMatrixNN::~SlMatrixNN()
{
  // May not have allocated.
  if ( _m ) delete [] _m;
}


//////////////////////////////////////////////////////////////////////////
//
//  Initialize the matrix to identity.
//
//////////////////////////////////////////////////////////////////////////

inline void SlMatrixNN::identity()
{
    SlInt32 i, j;
    
  for ( i = 0; i < _size; ++i )
  {
    for ( j = 0; j < _size; ++j )
    {
      _m[_size*i+j] = ( i == j ) ? 1.0 : 0.0;
    }
  }
}


//////////////////////////////////////////////////////////////////////////
//
//  Initialize the matrix to zero.
//
//////////////////////////////////////////////////////////////////////////

inline void SlMatrixNN::zero()
{
  for ( SlInt32 i = 0; i < _arrayLength; ++i ) _m[i] = 0.0;
}


//////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
//////////////////////////////////////////////////////////////////////////

bool SlMatrixNN::setSize ( SlInt32 size )
{
  SL_ASSERT ( size > 0 );

  // Trivial case.
  if ( size == _size ) return true;

  // Assign the integer members.
  _size = size;
  _arrayLength = size * size;

  // Allocate.
  _m = new SlFloat64 [_arrayLength];

  // Check allocation.
  if ( !_m ) _size = _arrayLength = 0;

  // Returns true if allocated ok.
  return ( _m != 0x0 );
}


//////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
//////////////////////////////////////////////////////////////////////////

bool SlMatrixNN::setValue ( const SlMatrixNN &copyMe )
{
  SL_ASSERT ( copyMe._size > 0 );
  SL_ASSERT ( copyMe._arrayLength == copyMe._size * copyMe._size );
  SL_ASSERT ( copyMe._m );

  // Set the size (allocates a new array if needed).
  if ( !this->setSize ( copyMe._size ) ) return false;
  SL_ASSERT ( _size == copyMe._size && _arrayLength == copyMe._arrayLength && _m );

  // Copy the values.
  for ( SlInt32 i = 0; i < _arrayLength; i++ ) _m[i] = copyMe._m[i];

  // All is good.
  return true;
}


//////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
//////////////////////////////////////////////////////////////////////////

bool SlMatrixNN::setValue ( const SlInt32 size, const SlFloat64 *m, bool makeCopy )
{
  SL_ASSERT ( size > 0 );
  SL_ASSERT ( m );

  // If you're supposed to make a copy.
  if ( makeCopy )
  {
    // Set the size (allocates a new array if needed).
    if ( !this->setSize ( size ) ) return false;
    SL_ASSERT ( _size == size && _arrayLength == size * size && _m );

    // Copy the values.
    for ( SlInt32 i = 0; i < _arrayLength; i++ ) _m[i] = m[i];

    // All is good.
    return true;
  }

  // Otherwise, just assign the pointers.
  else 
  {
    if ( _m ) delete [] _m;
    _m = (SlFloat64 *) m;
    _size = size;
    _arrayLength = size * size;
    return true;
  }
}


//////////////////////////////////////////////////////////////////////////
//
//  Assignment operator.
//
//////////////////////////////////////////////////////////////////////////

SlMatrixNN &SlMatrixNN::operator = ( const SlMatrixNN &copyMe )
{
  SL_VERIFY ( this->setValue ( copyMe ) );
  return *this;
}


//////////////////////////////////////////////////////////////////////////
//
//  Get the LU Decomposition.
//
//////////////////////////////////////////////////////////////////////////

inline void SlMatrixNN::_getValue ( SlFloat64 *a, SlFloat64 *b, SlInt32 n, SlFloat64 alpha )
{
  for ( SlInt32 i = 0; i < n; i++ )
    a[i] += alpha * b[i];
}


//////////////////////////////////////////////////////////////////////////
//
//  Factor the diagonal.
//
//////////////////////////////////////////////////////////////////////////

inline void SlMatrixNN::_factorDiagonal ( SlFloat64 *a, SlInt32 bsize )
{
  SlInt32 j, k;
  SlFloat64 alpha;

  for ( k = 0; k < bsize; k++ )
  {
    // Modify subsequent columns.
    for ( j = k+1; j < bsize; j++ )
    {
      // Should not divide by zero.
      SL_ASSERT ( a[_size*k+k] != 0.0 );

      a[_size*j+k] /= a[_size*k+k];

      // Check for a finite number (could happen if denominator is close to zero).
      SL_ASSERT ( CadKit::isFinite ( a[_size*j+k] ) );

      alpha = -a[_size*j+k];
      this->_getValue ( &a[_size*j+(k+1)], &a[_size*k+(k+1)], bsize-k-1, alpha );
    }
  }
}


//////////////////////////////////////////////////////////////////////////
//
//  Divide column by diagonal.
//
//////////////////////////////////////////////////////////////////////////

inline void SlMatrixNN::_divideColumn ( SlFloat64 *a, SlFloat64 *diag, SlInt32 bsize )
{
  SlInt32 j, k;
  SlFloat64 alpha;

  for ( k = 0; k < bsize; k++ )
  {
    for ( j = k+1; j < bsize; j++ )
    {
      alpha = -diag[_size*j+k];
      this->_getValue ( &a[_size*j], &a[_size*k], bsize, alpha );
    }
  }
}


//////////////////////////////////////////////////////////////////////////
//
//  Modify row using diagonal.
//
//////////////////////////////////////////////////////////////////////////

inline void SlMatrixNN::_modifyRow ( SlFloat64 *a, SlFloat64 *diag, SlInt32 bsize )
{
  SlInt32 j, k;
  SlFloat64 alpha;

  for ( k = 0; k < bsize; k++ )
  {
    for ( j = 0; j < bsize; j++ )
    {
      // Should not divide by zero.
      SL_ASSERT ( diag[_size*k+k] != 0.0 );

      a[_size*j+k] /= diag[_size*k+k];

      // Check for a finite number (could happen if denominator is close to zero).
      SL_ASSERT ( CadKit::isFinite ( a[_size*j+k] ) );

      alpha = -a[_size*j+k];
      this->_getValue ( &a[_size*j+(k+1)], &diag[_size*k+(k+1)], bsize-k-1, alpha );
    }
  }
}


//////////////////////////////////////////////////////////////////////////
//
//  Modify subsequent rows and columns.
//
//////////////////////////////////////////////////////////////////////////

inline void SlMatrixNN::_modifyRowColumn ( SlFloat64 *a, SlFloat64 *row, SlFloat64 *col, SlInt32 bsize )
{
  SlInt32 j, k;
  SlFloat64 alpha;

  for ( k = 0; k < bsize; k++ )
  {
    for ( j = 0; j < bsize; j++ )
    {
      alpha = -col[_size*j+k];
      this->_getValue ( &a[_size*j], &row[_size*k], bsize, alpha );
    }
  }
}


//////////////////////////////////////////////////////////////////////////
//
//  Get the LU Decomposition.
//
//////////////////////////////////////////////////////////////////////////

void SlMatrixNN::lud()
{
  SL_ASSERT ( _m && _size > 0 && _arrayLength == _size * _size );

  SlInt32 i, j, k;

  // Loop over diagonal.
  for ( i = 0; i < _size; i++ )
  {
    // Factor diagonal.
    this->_factorDiagonal ( &_m[_size*i+i], 1 );

    // Divide column by diagonal.
    for ( j = i+1; j < _size; j++ )
      this->_divideColumn ( &_m[_size*i+j], &_m[_size*i+i], 1 );

    // Modify row using diagonal.
    for ( j = i+1; j < _size; j++ )
      this->_modifyRow ( &_m[_size*j+i], &_m[_size*i+i], 1 );

    // Modify subsequent rows and columns.
    for ( j = i+1; j < _size; j++ )
      for ( k = i+1; k < _size; k++ )
        this->_modifyRowColumn ( &_m[_size*j+k], &_m[_size*i+k], &_m[_size*j+i], 1 );
  }
}


//////////////////////////////////////////////////////////////////////////
//
//  Do the forward reduction and back substitution.
//
//////////////////////////////////////////////////////////////////////////

void SlMatrixNN::forwardBackward ( SlFloat64 *b )
{
  SL_ASSERT ( b );
  SL_ASSERT ( _m && _size > 0 && _arrayLength == _size * _size );

  SlInt32 i, j;
  SlFloat64 sum;

  // Forward substitution.

  for ( i = 0; i < _size; i++ )
  {
    sum = 0.0;

    for ( j = 0; j < i; j++ )
      sum += _m[_size*i+j]*b[j];

    b[i] -= sum;
  }

  // Backward substitution.

  for ( i = _size-1; i >= 0; i-- )
  {
    sum = 0.0;

    for ( j = _size-1; j > i; j-- )
      sum += _m[_size*i+j]*b[j];

    b[i] -= sum;

    // Do not divide by zero.
    SL_ASSERT ( _m[_size*i+i] != 0.0 );

    b[i] /= _m[_size*i+i];

    // Check for a finite number (could happen if denominator is close to zero).
    SL_ASSERT ( CadKit::isFinite ( b[i] ) );
  }
}


}; // namespace CadKit


#endif // _CADKIT_STANDARD_LIBRARY_N_BY_N_TEMPLATE_MATRIX_CLASS_H_
