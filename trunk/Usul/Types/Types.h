
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Basic types.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_BASIC_TYPES_H_
#define _USUL_BASIC_TYPES_H_


namespace Usul {
namespace Types {


#ifdef _MSC_VER // Visual C++

  typedef __int64             Int64;
  typedef __int32             Int32;
  typedef __int16             Int16;

  typedef unsigned __int64    Uint64;
  typedef unsigned __int32    Uint32;
  typedef unsigned __int16    Uint16;

  typedef float               Float32;
  typedef double              Float64;

#elif defined __SGI
  
  typedef long long           Int64;
  typedef int                 Int32;
  typedef short               Int16;

  typedef unsigned long long  Uint64;
  typedef unsigned int        Uint32;
  typedef unsigned short      Uint16;

  typedef float               Float32;
  typedef double              Float64;
  
#else

  #include <stdint.h>

  typedef int64_t             Int64;
  typedef int32_t             Int32;
  typedef int16_t             Int16;

  typedef uint64_t            Uint64;
  typedef uint32_t            Uint32;
  typedef uint16_t            Uint16;

  typedef float               Float32;
  typedef double              Float64;

#endif


}; // namespace Types
}; // namespace Usul


#endif // _USUL_BASIC_TYPES_H_
