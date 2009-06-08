
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//  SlPrecompiled.h: The precompiled header file.
//
//////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_INTERNAL_PRECOMPILED_HEADERS_H_
#define _CADKIT_STANDARD_LIBRARY_INTERNAL_PRECOMPILED_HEADERS_H_

#ifdef _WIN32
# pragma warning(disable:4786) // We know the debug names are truncated to 255 characters.
# define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers.
# define NOMINMAX              // Do not define min and max as macros.
# include <windows.h>          // Base Windows header file.
#endif // _WIN32

// Needed headers.
#ifdef _CADKIT_USE_PRECOMPILED_HEADERS
# include <stdio.h>
# include <math.h>
# include <memory> // For std::pair
# include <vector>
#endif // _CADKIT_USE_PRECOMPILED_HEADERS

#endif // _CADKIT_STANDARD_LIBRARY_INTERNAL_PRECOMPILED_HEADERS_H_
