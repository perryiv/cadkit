
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlMatrix44IO: Input/output functions for SlMatrix44.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_4_BY_4_TEMPLATE_MATRIX_IO_FUNCTIONS_H_
#define _CADKIT_STANDARD_LIBRARY_4_BY_4_TEMPLATE_MATRIX_IO_FUNCTIONS_H_

#include "SlMatrix44.h"

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

template<class T> inline void write ( const SlMatrix44<T> &M, std::ostream &out )
{
  out << M[0] << " " << M[4] << " " << M[8]  << " " << M[12] << "\n";
  out << M[1] << " " << M[5] << " " << M[9]  << " " << M[13] << "\n";
  out << M[2] << " " << M[6] << " " << M[10] << " " << M[14] << "\n";
  out << M[3] << " " << M[7] << " " << M[11] << " " << M[15];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Input helper function.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void read ( SlMatrix44<T> &M, std::istream &in )
{
  in >> M[0] >> M[4] >> M[8]  >> M[12];
  in >> M[1] >> M[5] >> M[9]  >> M[13];
  in >> M[2] >> M[6] >> M[10] >> M[14];
  in >> M[3] >> M[7] >> M[11] >> M[15];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Output.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void write ( const SlMatrix44<T> &M, FILE *fp, const char *format )
{
  SL_ASSERT ( fp );

  fprintf ( fp, format, M[0] );
  fprintf ( fp, format, M[4] );
  fprintf ( fp, format, M[8] );
  fprintf ( fp, format, M[12] );
  fprintf ( fp, "\n" );

  fprintf ( fp, format, M[1] );
  fprintf ( fp, format, M[5] );
  fprintf ( fp, format, M[9] );
  fprintf ( fp, format, M[13] );
  fprintf ( fp, "\n" );
  
  fprintf ( fp, format, M[2] );
  fprintf ( fp, format, M[6] );
  fprintf ( fp, format, M[10] );
  fprintf ( fp, format, M[14] );
  fprintf ( fp, "\n" );
  
  fprintf ( fp, format, M[3] );
  fprintf ( fp, format, M[7] );
  fprintf ( fp, format, M[11] );
  fprintf ( fp, format, M[15] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Input.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline void read ( SlMatrix44<T> &M, FILE *fp, const char *format )
{
  SL_ASSERT ( fp );
  SL_ASSERT ( 0 ); // Untested. If it works then take this assert out.

  fscanf ( fp, format, &(M[0]) );
  fscanf ( fp, format, &(M[4]) );
  fscanf ( fp, format, &(M[8]) );
  fscanf ( fp, format, &(M[12]) );

  fscanf ( fp, format, &(M[1]) );
  fscanf ( fp, format, &(M[5]) );
  fscanf ( fp, format, &(M[9]) );
  fscanf ( fp, format, &(M[13]) );

  fscanf ( fp, format, &(M[2]) );
  fscanf ( fp, format, &(M[6]) );
  fscanf ( fp, format, &(M[10]) );
  fscanf ( fp, format, &(M[14]) );

  fscanf ( fp, format, &(M[3]) );
  fscanf ( fp, format, &(M[7]) );
  fscanf ( fp, format, &(M[11]) );
  fscanf ( fp, format, &(M[15]) );
}


}; // namespace CadKit.


///////////////////////////////////////////////////////////////////////////////
//
//  Output operator. Keep in global namespace.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline std::ostream &operator << ( std::ostream &out, const CadKit::SlMatrix44<T> &M )
{
  CadKit::write ( M, out );
  return out;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Input operator. Keep in global namespace.
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline std::istream &operator >> ( std::istream &in, const CadKit::SlMatrix44<T> &M )
{
  CadKit::read ( M, in );
  return in;
}


#endif // _CADKIT_STANDARD_LIBRARY_4_BY_4_TEMPLATE_MATRIX_IO_FUNCTIONS_H_
