
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Clients include this header before others from this library.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_OPENGL_HEADERS_H_
#define _CADKIT_SCENEGRAPH_OPENGL_HEADERS_H_

#ifdef _WIN32
# pragma warning(disable:4786) // Truncated debug names.
#endif

// Needed headers.
#include "SceneGraph/Core/SgClientHeaders.h"

// Headers from this library.
#include "SgGlRenderer.h"
#include "SgGlFeedback.h"
#include "SgGlConstants.h"
#include "SgGlContext.h"

#ifdef _WIN32
# include "SgGlWinContext.h"
#else
  TODO
#endif

#endif // _CADKIT_SCENEGRAPH_OPENGL_HEADERS_H_
