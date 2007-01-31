
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

#ifndef _FOX_CURSORS_INTERNAL_PRECOMPILED_HEADERS_H_
#define _FOX_CURSORS_INTERNAL_PRECOMPILED_HEADERS_H_

#ifndef COMPILING_FOX_CURSORS
# error "This header file is only used for compiling the library."
# error "It should not be included in client code."
#endif

#ifdef _MSC_VER                     // VC++

# pragma warning ( disable : 4290 ) // Exception specification ignored.
# pragma warning ( disable : 4786 ) // Truncated debug names.

# define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers.
# define NOMINMAX                   // Do not define min and max as macros.

# include <windows.h>               // Base Windows header file.
# include <limits>
# include <sstream>
# include <list>
# include <string>
# include <stdexcept>
# include <stack>
# include <set>

# include "fx.h"
# include "fxkeys.h"
# include "fx3d.h"
# include "FXCURCursor.h"
# include "FXPNGIcon.h"
# include "FXGIFIcon.h"

#endif // _WIN32

#endif // _OPEN_SCENE_GRAPH_FOX_INTERNAL_PRECOMPILED_HEADERS_H_
