
////////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//  SlTypedefs: Useful typedefs.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_TYPEDEFS_H_
#define _CADKIT_STANDARD_LIBRARY_TYPEDEFS_H_

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <wchar.h>
# ifdef _SGI_NATIVE_COMPILER
#  include <sgidefs.h>
# endif
#endif

namespace CadKit
{
//
// Character types.
//

typedef char                SlChar;
typedef unsigned char       SlUchar;


//
// Wide character.
// Note: wchar_t is defined differently on the various compiler, 
// and (at least with SGI's CC) with the various compile options.
//

typedef wchar_t             SlWchar;


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

#elif _SGI_NATIVE_COMPILER

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

