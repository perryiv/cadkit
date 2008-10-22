
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  MxN matrix class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_M_BY_N_MATRIX_CLASS_H_
#define _GENERIC_NURBS_LIBRARY_M_BY_N_MATRIX_CLASS_H_

#include "GN/Macros/ErrorCheck.h"
#include "GN/Math/Absolute.h"

#include <stdexcept>
#include <limits>
#include <sstream>


namespace GN {
namespace Math {


///////////////////////////////////////////////////////////////////////////////
//
//  MxN matrix class.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class SizeType_,
  class ContainerType_,   // Should behave as a vector of vectors.
  class SizeContainer_,
  class ValueTester_,
  class ErrorCheckerType
>
class Matrix
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Useful typedefs.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef SizeType_ SizeType;
  typedef ContainerType_ ContainerType;
  typedef SizeContainer_ SizeContainer;
  typedef typename ContainerType::value_type VectorType;
  typedef typename VectorType::value_type ValueType;
  typedef ValueTester_ ValueTester;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Standard container adaptability.
  //
  /////////////////////////////////////////////////////////////////////////////

  typedef ValueType value_type;
  typedef SizeType size_type;


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Default constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Matrix() : _m()
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Matrix ( SizeType rows, SizeType columns ) : _m()
  {
    this->resize ( rows, columns );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Copy constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  Matrix ( const Matrix &m ) : _m ( m )
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Template constructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Array2D > Matrix ( const Array2D &array, SizeType rows, SizeType columns ) : _m()
  {
    this->set ( array, rows, columns );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Destructor.
  //
  /////////////////////////////////////////////////////////////////////////////

  ~Matrix()
  {
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //	Return the number of rows.
  //
  /////////////////////////////////////////////////////////////////////////////

  SizeType rows() const
  {
    return _m.size();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //	Return the number of columns.
  //
  /////////////////////////////////////////////////////////////////////////////

  SizeType columns() const
  {
    return ( this->rows() > 0 ) ? _m[0].size() : 0;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //	See if the matrix is empty.
  //
  /////////////////////////////////////////////////////////////////////////////

  bool empty() const
  {
    return _m.empty();
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //	Resize the matrix.
  //
  /////////////////////////////////////////////////////////////////////////////

  void resize ( SizeType rows, SizeType columns )
  {
    _m.resize ( rows );
    for ( typename ContainerType::iterator i = _m.begin(); i != _m.end(); ++i )
      i->resize ( columns );
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //	Set the matrix.
  //
  /////////////////////////////////////////////////////////////////////////////

  void set ( const Matrix &m )
  {
    // Make room.
    SizeType r ( m.rows() );
    _m.resize ( r );

    // Loop through the rows...
    for ( SizeType i = 0; i < r; ++i )
    {
      // Assign the entire column.
      _m[i] = m[i];
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //	Set the matrix.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Array2D > void set ( const Array2D &a, SizeType r, SizeType c )
  {
    // Make room.
    _m.resize ( r );

    // Loop through the rows...
    for ( SizeType i = 0; i < r; ++i )
    {
      // Assign the entire column.
      _m[i].assign ( a.at(i).begin(), a.at(i).end() );
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //	Set all elements to the given value.
  //
  /////////////////////////////////////////////////////////////////////////////

  void set ( const ValueType &value )
  {
    // Get the existing size.
    SizeType r ( this->rows() );
    SizeType c ( this->columns() );

    // Loop through the rows.
    for ( SizeType i = 0; i < r; ++i )
    {
      // Assign the entire column.
      _m[i].assign ( c, value );
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Assignment operator.
  //
  /////////////////////////////////////////////////////////////////////////////

  Matrix &operator = ( const Matrix &m )
  {
    this->set ( m );
    return *this;
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Access with an offset.
  //
  /////////////////////////////////////////////////////////////////////////////

  ValueType &operator () ( SizeType i, SizeType j, SizeType first = 0 )
  {
    GN_ERROR_CHECK ( i - first < this->rows() );
    GN_ERROR_CHECK ( j - first < this->columns() );
    return _m[i-first][j-first];
  }
  const ValueType &operator () ( SizeType i, SizeType j, SizeType first = 0 ) const
  {
    GN_ERROR_CHECK ( i - first < this->rows() );
    GN_ERROR_CHECK ( j - first < this->columns() );
    return _m[i-first][j-first];
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Perform LU decomposition. Adapted from "Numerical Recipes in C".
  //  See: http://www.library.cornell.edu/nr/bookcpdf/c2-3.pdf
  //
  /////////////////////////////////////////////////////////////////////////////

  void luDecomp ( SizeContainer &index )
  {
    SizeType imax, i, j, k;
    SizeType n ( this->rows() );
    ValueType big ( 0 ), dum, sum, temp, d ( 1 );
    const ValueType zero ( static_cast < ValueType > ( 0 ) );
    const ValueType one  ( static_cast < ValueType > ( 1 ) );

    // Only makes sense for square matrices.
    GN_ERROR_CHECK ( this->columns() == n );

    // Stores scaling for each row. Size for first index of one.
    VectorType vv;
    vv.resize ( n + 1 );

    // Make sure this is big enough.
    index.resize ( n );

    // Loop over rows to get scaling.
    for ( i = 1; i <= n; ++i )
    {
      big = zero;
      for ( j = 1; j <= n; ++j )
      {
        temp = GN::Math::absolute ( (*this)(i,j,1) );
        if ( temp > big )
          big = temp;
      }
      if ( zero == big )
        throw std::runtime_error ( "Error 1268204037: trying to LU-decompose a singular matrix" );
      vv[i] = one / big;
    }

    // Loop over columns of Crout’s method.
    for ( j = 1; j <= n; ++j )
    {
      for ( i = 1; i < j; ++i )
      {
        sum = (*this)(i,j,1);
        for ( k = 1; k < i; ++k )
          sum -= (*this)(i,k,1) * (*this)(k,j,1);
        (*this)(i,j,1) = sum;
      }

      // Search for largest pivot element.
      big = zero;
      for ( i = j; i <= n; ++i )
      {
        sum = (*this)(i,j,1);
        for ( k = 1; k < j; ++k )
          sum -= (*this)(i,k,1) * (*this)(k,j,1);
        (*this)(i,j,1) = sum;
        dum = vv[i] * GN::Math::absolute ( sum );
        if ( dum >= big )
        {
          big = dum;
          imax = i;
        }
      }

      // See if row interchange is needed.
      if ( j != imax )
      {
        for ( k = 1; k <= n; ++k )
        {
          dum = (*this)(imax,k,1);
          (*this)(imax,k,1) = (*this)(j,k,1);
          (*this)(j,k,1) = dum;
        }

        // Change the sign of d and scale factor.
        d = -d;
        vv[imax] = vv[j];
      }

      index[j-1] = imax;
      if ( zero == (*this)(j,j,1) )
        (*this)(j,j,1) = std::numeric_limits<ValueType>::min();

      if ( j != n )
      {
        dum = one / (*this)(j,j,1);
        for ( i = j + 1; i <= n; ++i )
          (*this)(i,j,1) *= dum;
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  LU back substitution. Assumes this matrix is already LU-decomposed.
  //  If the equation is Ax=b, b returns holding x.
  //  See: http://www.library.cornell.edu/nr/bookcpdf/c2-3.pdf
  //
  /////////////////////////////////////////////////////////////////////////////

  void luSolve ( const SizeContainer &index, VectorType &b ) const
  {
    SizeType i, ii ( 0 ), ip, j;
    SizeType n ( this->rows() );
    ValueType sum;

    // Check state.
    GN_ERROR_CHECK ( this->columns() == n );
    GN_ERROR_CHECK ( b.size() == n );

    for ( i = 1; i <= n; ++i )
    {
      ip = index[i-1];
      sum = b[ip-1];
      b[ip-1] = b[i-1];
      if ( ii )
        for ( j = ii; j <= i - 1; ++j )
          sum -= (*this)(i,j,1) * b[j-1];
      else if ( sum )
        ii = i;
      b[i-1] = sum;
    }

    for ( i = n; i >= 1; --i )
    {
      sum = b[i-1];
      for ( j = i + 1; j <= n; ++j )
        sum -= (*this)(i,j,1) * b[j-1];
      b[i-1] = sum / (*this)(i,i,1);
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //	Apply the functor to each element.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class Functor > void apply ( Functor f )
  {
    // Loop through the rows.
    SizeType r ( this->rows() );
    for ( SizeType i = 0; i < r; ++i )
    {
      // Apply to every element in this row.
      std::for_each ( _m[i].begin(), _m[i].end(), f );
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //	Print to the stream.
  //
  /////////////////////////////////////////////////////////////////////////////

  template < class OutputStream > void print ( const std::string &format, OutputStream &out )
  {
    // Shortcuts.
    SizeType r ( this->rows() );
    SizeType c ( this->columns() );

    // Should be big enough...
    char buf[1024];

    // Loop through the rows.
    for ( SizeType i = 0; i < r; ++i )
    {
      // Loop through the columns.
      for ( SizeType j = 0; j < c; ++j )
      {
        ::sprintf ( buf, format.c_str(), _m[i][j] );
        out << buf;
      }

      // Next line.
      out << '\n';
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  //
  //  Data members.
  //
  /////////////////////////////////////////////////////////////////////////////

private:

  ContainerType _m;
};


}; // namespace Math
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_KNOT_INSERTION_ALGORITHM_H_
