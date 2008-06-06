
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functions for printing vectors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_PRINT_VECTOR_H_
#define _USUL_PRINT_VECTOR_H_

#include <iomanip>


namespace Usul {
namespace Print {


///////////////////////////////////////////////////////////////////////////////
//
//  Print a vector.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class StringType_, 
  typename ElementType_, 
  class StreamType_
> 
inline void vector ( const StringType_ &message, 
                     ElementType_ v0, 
                     ElementType_ v1, 
                     ElementType_ v2, 
                     ElementType_ v3, 
                     StreamType_ &out, 
                     int width = 10,
                     int precision = 4 )
{
  int w ( width );
  int p ( precision );

  out << message;
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << v0;
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << v1;
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << v2;
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << v3;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Print a vector.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class StringType_, 
  typename ElementType_, 
  class StreamType_
> 
inline void vector ( const StringType_ &message, 
                     ElementType_ v0, 
                     ElementType_ v1, 
                     ElementType_ v2, 
                     StreamType_ &out, 
                     int width = 10,
                     int precision = 4 )
{
  int w ( width );
  int p ( precision );

  out << message;
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << v0;
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << v1;
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << v2;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Print a vector.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class StringType_, 
  typename ElementType_, 
  class StreamType_
> 
inline void vector ( const StringType_ &message, 
                     ElementType_ v0, 
                     ElementType_ v1, 
                     StreamType_ &out, 
                     int width = 10,
                     int precision = 4 )
{
  int w ( width );
  int p ( precision );

  out << message;
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << v0;
  out << std::setw ( w ) << std::setprecision ( p ) << std::fixed << v1;
}


} // namespace Print
} // namespace Usul


#endif // _USUL_PRINT_VECTOR_H_
