
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

#ifndef _GENERIC_SCENE_GRAPH_FOX_INTERNAL_PRECOMPILED_HEADERS_H_
#define _GENERIC_SCENE_GRAPH_FOX_INTERNAL_PRECOMPILED_HEADERS_H_

#ifndef COMPILING_GSG_FOX
# error "This header file is only used for compiling the library."
# error "It should not be included in client code."
#endif

#ifdef _MSC_VER                     // VC++

# pragma warning ( disable : 4290 ) // Exception specification ignored.
# pragma warning ( disable : 4786 ) // Truncated debug names.

# define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers.
# define NOMINMAX                   // Do not define min and max as macros.

# include <windows.h>               // Base Windows header file.

# include "fx.h"
# include "fx3d.h"
# include "FXCURCursor.h"

# include "GSG/OpenGL/GlDraw.h"

# include "GSG/Core/Camera.h"
# include "GSG/Core/Viewer.h"
# include "GSG/Core/Update.h"
# include "GSG/Core/FrustumCull.h"
# include "GSG/Core/Sort.h"
# include "GSG/Core/Renderer.h"
# include "GSG/Core/RenderBin.h"

# include <cassert>
# include <memory>

#endif // _WIN32

#endif // _GENERIC_SCENE_GRAPH_FOX_INTERNAL_PRECOMPILED_HEADERS_H_
