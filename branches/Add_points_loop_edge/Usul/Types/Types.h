
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
  typedef int                 Int32;
  typedef short               Int16;
  typedef char                Int8;

  typedef unsigned __int64    Uint64;
  typedef unsigned int        Uint32;
  typedef unsigned short      Uint16;
  typedef unsigned char       Uint8;

  typedef float               Float32;
  typedef double              Float64;

#elif __SGI
  
  typedef long long           Int64;
  typedef int                 Int32;
  typedef short               Int16;
  typedef char                Int8;

  typedef unsigned long long  Uint64;
  typedef unsigned int        Uint32;
  typedef unsigned short      Uint16;
  typedef unsigned char       Uint8;

  typedef float               Float32;
  typedef double              Float64;

#elif __APPLE__ 

  // These were taken from the MacTypes.h located in 
  // /System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/
  //     CarbonCore.framework/Versions/A/Headers/
  typedef unsigned char       Uint8;
  typedef signed char         Int8;
  
  typedef unsigned short      Uint16;
  typedef signed short        Int16;

  typedef unsigned int                    Uint32;
  typedef signed int                      Int32;

  #if defined(_MSC_VER) && !defined(__MWERKS__) && defined(_M_IX86)
    typedef   signed __int64                Int64;
    typedef unsigned __int64                Uint64;
  #else
    typedef   signed long long              Int64;
    typedef unsigned long long              Uint64;
  #endif
  
  typedef float               Float32;
  typedef double              Float64;  

#elif __linux 

  //TODO Figure out if this is correct.... 
  typedef long long           Int64;
  typedef int                 Int32;
  typedef short               Int16;
  typedef char                Int8;
  
  typedef unsigned long long  Uint64;
  typedef unsigned int        Uint32;
  typedef unsigned short      Uint16;
  typedef unsigned char       Uint8;
  
  typedef float               Float32;
  typedef double              Float64;
  
#else

  #include <stdint.h>

  typedef int64_t             Int64;
  typedef int32_t             Int32;
  typedef int16_t             Int16;
  typedef int8_t              Int8;

  typedef uint64_t            Uint64;
  typedef uint32_t            Uint32;
  typedef uint16_t            Uint16;
  typedef uint8_t             Uint8;

  typedef float               Float32;
  typedef double              Float64;

#endif


} // namespace Types
} // namespace Usul


#endif // _USUL_BASIC_TYPES_H_
