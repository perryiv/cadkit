
///////////////////////////////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//  SlDeclareConst: For declaring constant numbers on different platforms.
//
//////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_DECLARE_CONSTANTS_H_
#define _CADKIT_STANDARD_LIBRARY_DECLARE_CONSTANTS_H_


//////////////////////////////////////////////////////////////////////////
//
// Macros used for declaring integer constants.
//
//////////////////////////////////////////////////////////////////////////

#if _WIN32 || _SGI_NATIVE_COMPILER
# define SL_CONST_INT64(n)  n
# define SL_CONST_UINT64(n) n
# define SL_CONST_INT32(n)  n
# define SL_CONST_UINT32(n) n
#elif __GNUC__
# ifdef _64_BIT_MACHINE
#  define SL_CONST_INT64(n)  n##L
#  define SL_CONST_UINT64(n) n##UL
# else // 32 bit machines.
#  define SL_CONST_INT64(n)  n##LL
#  define SL_CONST_UINT64(n) n##ULL
# endif // 64/32 bit linux.
# define SL_CONST_UINT32(n) n##U
# define SL_CONST_UINT16(n) n##U
# define SL_CONST_UINT8(n)  n##U
#else
TODO
#endif


//////////////////////////////////////////////////////////////////////////
//
// Macros used for floating-point constants.
//
//////////////////////////////////////////////////////////////////////////


#endif // _CADKIT_STANDARD_LIBRARY_DECLARE_CONSTANTS_H_
