
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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

#if _WIN32 || __sgi
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


#endif // _CADKIT_STANDARD_LIBRARY_DECLARE_CONSTANTS_H_
