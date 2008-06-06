
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The precompiled header file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_FOX_VIEWS_INTERNAL_PRECOMPILED_HEADERS_H_
#define _OSG_FOX_VIEWS_INTERNAL_PRECOMPILED_HEADERS_H_

#ifndef _COMPILING_OSG_FOX_VIEWS
# error "This header file is only used for compiling the library."
# error "It should not be included in client code."
#endif

#ifdef _MSC_VER                     // VC++

# pragma warning ( disable : 4290 ) // Exception specification ignored.
# pragma warning ( disable : 4786 ) // Truncated debug names.

# define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers.
# define NOMINMAX                   // Do not define min and max as macros.

# include <windows.h>               // Base Windows header file.
# include <shellapi.h>              // For DragAcceptFiles().

#endif // _WIN32

#endif // _OSG_FOX_VIEWS_INTERNAL_PRECOMPILED_HEADERS_H_
