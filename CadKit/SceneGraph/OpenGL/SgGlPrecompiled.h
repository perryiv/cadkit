
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgGlPrecompiled.h: The precompiled header file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_OPENGL_INTERNAL_PRECOMPILED_HEADERS_H_
#define _CADKIT_SCENEGRAPH_OPENGL_INTERNAL_PRECOMPILED_HEADERS_H_

#ifdef _WIN32
# pragma warning(disable:4251) // See note 3ce3a140-2edf-11d4-98a4-0040054c86c7.
# pragma warning(disable:4786) // Truncated debug names.
# define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers.
# define NOMINMAX              // Do not define min and max as macros.
# include <windows.h>          // Base Windows header file.
#endif // _WIN32

// Needed headers.
#ifdef _CADKIT_USE_PRECOMPILED_HEADERS
# define CADKIT_DEFINE_SL_LINE_3_MATRIX_FUNCTIONS
# include "SceneGraph/Core/SgClientHeaders.h"
# include <memory>
#endif

#endif // _CADKIT_SCENEGRAPH_OPENGL_INTERNAL_PRECOMPILED_HEADERS_H_
