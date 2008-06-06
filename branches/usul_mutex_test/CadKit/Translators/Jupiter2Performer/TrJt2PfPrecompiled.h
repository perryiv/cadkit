
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  TrJt2PfPrecompiled.h: The precompiled header file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_JUPITER_TO_PERFORMER_LIBRARY_INTERNAL_PRECOMPILED_HEADERS_H_
#define _CADKIT_JUPITER_TO_PERFORMER_LIBRARY_INTERNAL_PRECOMPILED_HEADERS_H_

#ifdef _WIN32
# pragma warning(disable:4251) // See note 3ce3a140-2edf-11d4-98a4-0040054c86c7.
# pragma warning(disable:4786) // Truncated debug names.
# define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers.
# define NOMINMAX              // Do not define min and max as macros.
# include <windows.h>          // Base Windows header file.
#endif // _WIN32

// Needed headers.
#ifdef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Database/Jupiter/DbJtClientHeaders.h"
# include <fstream> 
# include <iostream>
# include <iomanip>
# include <memory>
# include <map>
# include "TrJt2PfPerformer.h"
#endif

#endif // _CADKIT_JUPITER_TO_PERFORMER_LIBRARY_INTERNAL_PRECOMPILED_HEADERS_H_
