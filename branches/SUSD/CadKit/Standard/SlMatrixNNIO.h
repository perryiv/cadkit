
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlMatrixNNIO: Input/output functions for SlMatrixNN.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_N_BY_N_TEMPLATE_MATRIX_IO_FUNCTIONS_H_
#define _CADKIT_STANDARD_LIBRARY_N_BY_N_TEMPLATE_MATRIX_IO_FUNCTIONS_H_

#include "SlMatrixNN.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <stdio.h>
# include <iostream>
#endif


namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  Output helper function.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline void write ( const SlMatrixNN<T,I> &M, std::ostream &out )
{
  I size ( M.getSize() ), i, j;

  for ( i = 0; i < size; ++i )
  {
    for ( j = 0; j < size; ++j )
    {
      out << M(i,j) << " ";
    }

    out << "\n";
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Input helper function. Note: we could read the matrix into the array 
//  using the bracket operator, but iff the internal array is row-major. 
//  This way is independent of that implementation detail.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline void read ( SlMatrixNN<T,I> &M, std::istream &in )
{
  I size ( M.getSize() ), i, j;

  for ( i = 0; i < size; ++i )
  {
    for ( j = 0; j < size; ++j )
    {
      in >> M(i,j);
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Output.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline void write ( const SlMatrixNN<T,I> &M, FILE *fp, const char *format )
{
  SL_ASSERT ( fp );

  I size ( M.getSize() ), i, j;

  for ( i = 0; i < size; ++i )
  {
    for ( j = 0; j < size; ++j )
    {
      fprintf ( fp, format, M(i,j) );
    }

    fprintf ( fp, "\n" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Input.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline void read ( SlMatrixNN<T,I> &M, FILE *fp, const char *format )
{
  SL_ASSERT ( fp );
  SL_ASSERT ( 0 ); // Untested. If it works then take this assert out.

  I size ( M.getSize() ), i, j;
  T temp;

  for ( i = 0; i < size; ++i )
  {
    for ( j = 0; j < size; ++j )
    {
      fscanf ( fp, format, &temp );
      M(i,j) = temp;
    }
  }
}


}; // namespace CadKit.


///////////////////////////////////////////////////////////////////////////////
//
//  Output operator. Keep in global namespace.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline std::ostream &operator << ( std::ostream &out, const CadKit::SlMatrixNN<T,I> &M )
{
  CadKit::write ( M, out );
  return out;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Input operator. Keep in global namespace.
//
///////////////////////////////////////////////////////////////////////////////

template<class T, class I> inline std::istream &operator >> ( std::istream &in, const CadKit::SlMatrixNN<T,I> &M )
{
  CadKit::read ( M, in );
  return in;
}


#endif // _CADKIT_STANDARD_LIBRARY_N_BY_N_TEMPLATE_MATRIX_IO_FUNCTIONS_H_
