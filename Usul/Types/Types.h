
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

  typedef unsigned __int64  Uint64;
  typedef unsigned __int32  Uint32;
  typedef unsigned __int16  Uint16;

  typedef __int64           Int64;
  typedef __int32           Int32;
  typedef __int16           Int16;

  typedef float             Float32;
  typedef double            Float64;

#elif defined __SGI
  
  typedef signed char             int8_t;
  typedef unsigned char           uint8_t;
  typedef signed short            int16_t;
  typedef unsigned short          uint16_t;
  typedef signed int              int32_t;
  typedef unsigned int            uint32_t;
  typedef signed long long        int64_t;
  typedef unsigned long long      uint64_t;


  typedef uint64_t            Uint64;
  typedef uint32_t            Uint32;
  typedef uint16_t            Uint16;

  typedef int64_t             Int64;
  typedef int32_t             Int32;
  typedef int16_t             Int16;

  typedef float               Float32;
  typedef double              Float64;
  
  
#else

  #include <stdint.h>

  typedef uint64_t          Uint64;
  typedef uint32_t          Uint32;
  typedef uint16_t          Uint16;

  typedef int64_t           Int64;
  typedef int32_t           Int32;
  typedef int16_t           Int16;

  typedef float             Float32;
  typedef double            Float64;

#endif


}; // namespace Types
}; // namespace Usul


#endif // _USUL_BASIC_TYPES_H_
