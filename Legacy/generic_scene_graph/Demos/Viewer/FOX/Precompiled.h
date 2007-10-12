
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The precompiled header file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_DEMO_FOX_INTERNAL_PRECOMPILED_HEADERS_H_
#define _GENERIC_SCENE_GRAPH_DEMO_FOX_INTERNAL_PRECOMPILED_HEADERS_H_

#ifdef _MSC_VER                     // VC++

# pragma warning ( disable : 4290 ) // Exception specification ignored.
# pragma warning ( disable : 4786 ) // Truncated debug names.
# pragma warning ( disable : 4312 ) // Cast to type of greater size

# define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers.
# define NOMINMAX                   // Do not define min and max as macros.

# include <windows.h>               // Base Windows header file.

# include "fx.h"

# include "GSG/FOX/MdiChildWindow.h"

# include "GSG/Core/Factory.h"
# include "GSG/Core/Shape.h"

# include <memory>

#endif // _MSC_VER

#endif // _GENERIC_SCENE_GRAPH_DEMO_FOX_INTERNAL_PRECOMPILED_HEADERS_H_
