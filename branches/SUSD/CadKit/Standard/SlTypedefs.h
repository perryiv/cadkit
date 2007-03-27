
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlTypedefs: Useful typedefs.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_TYPEDEFS_H_
#define _CADKIT_STANDARD_LIBRARY_TYPEDEFS_H_

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <wchar.h>
# ifdef __sgi
#  include <sgidefs.h>
# endif
#endif

namespace CadKit
{
// Note: I removed SlChar, SlUchar, and SlWchar because they do not gain us 
// anything, unlike the numerical types below.

//
// Unambiguous numerical types.
//

#ifdef _WIN32

typedef char                SlInt8;
typedef unsigned char       SlUint8;
typedef short               SlInt16;
typedef unsigned short      SlUint16;
typedef int                 SlInt32;
typedef unsigned int        SlUint32;
typedef __int64             SlInt64;
typedef unsigned __int64    SlUint64;
typedef float               SlFloat32;
typedef double              SlFloat64;

#elif __GNUC__

typedef char                SlInt8;
typedef unsigned char       SlUint8;
typedef short               SlInt16;
typedef unsigned short      SlUint16;
typedef int                 SlInt32;
typedef unsigned int        SlUint32;
typedef long long           SlInt64;
typedef unsigned long long  SlUint64;
typedef float               SlFloat32;
typedef double              SlFloat64;

#elif __sgi

// Note: with the various compile options available on irix 
// (i.e., -n32 and -64), there is no guarantee that an "int" 
// is 32 bits. So we use unambigious types from <sgidefs.h>
typedef char                SlInt8;
typedef unsigned char       SlUint8;
typedef short               SlInt16;   // TODO, is this right for -64?
typedef unsigned short      SlUint16;  // TODO, is this right for -64?
typedef __int32_t           SlInt32;
typedef __uint32_t          SlUint32;
typedef __int64_t           SlInt64;
typedef __uint64_t          SlUint64;
typedef float               SlFloat32; // TODO, is this right for -64?
typedef double              SlFloat64; // TODO, is this right for -64?

#else

TODO

#endif


//
// Other Data Type Typedefs.
//

typedef SlUint8             SlByte;  // One byte, an 8 bit unsigned number.
typedef SlUint16            SlWord;  // One word, a 16 bit unsigned number.
typedef SlUint32            SlDword; // A double word - 32 bit unsigned number.
typedef SlUint64            SlResult;
};

#endif

