
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functions for printing a matrix.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_PRINT_MATRIX_H_
#define _USUL_PRINT_MATRIX_H_

#include <iomanip>


namespace Usul {
namespace Print {


///////////////////////////////////////////////////////////////////////////////
//
//  Print a matrix. It assumes the array "T" has 16 elements in column-major 
//  order (like OpenGL's matrices).
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class StringType_, 
  typename ElementType_, 
  class StreamType_
> 
inline void matrix ( const StringType_ &message, 
                     const ElementType_ *m, 
                     StreamType_ &out, 
                     int width = 10,
                     int precision = 4 )
{
  const int w ( width );
  const int p ( precision );

  out << message;
  out << "\n\t";
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m[0];
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m[4];
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m[8];
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m[12];
  out << "\n\t";
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m[1];
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m[5];
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m[9];
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m[13];
  out << "\n\t";
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m[2];
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m[6];
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m[10];
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m[14];
  out << "\n\t";
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m[3];
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m[7];
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m[11];
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << m[15];
}


} // namespace Print
} // namespace Usul


#endif // _USUL_PRINT_MATRIX_H_
