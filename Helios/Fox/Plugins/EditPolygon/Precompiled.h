
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The precompiled header file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DELETION_PRIMITIVE_INTERNAL_PRECOMPILED_HEADERS_H_
#define _DELETION_PRIMITIVEE_INTERNAL_PRECOMPILED_HEADERS_H_

#ifndef _COMPILING_DELETION_PRIMITIVE
# error "This header file is only used for compiling the library."
# error "It should not be included in client code."
#endif

#ifdef _MSC_VER                     // VC++
# pragma warning ( disable : 4290 ) // Exception specification ignored.
# pragma warning ( disable : 4786 ) // Truncated debug names.
# pragma warning ( disable : 4312 ) // Cast to type of greater size.
# pragma warning ( disable : 4244 ) // Conversion from ... possible loss of data.
# define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers.
# define NOMINMAX                   // Do not define min and max as macros.
# include <windows.h>
#endif // _MSC_VER

#endif // _DELETION_PRIMITIVE_INTERNAL_PRECOMPILED_HEADERS_H_