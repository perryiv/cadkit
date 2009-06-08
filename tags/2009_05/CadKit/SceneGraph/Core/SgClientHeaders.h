
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
//  Clients include this header before others from this library.
//
//////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_HEADERS_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_HEADERS_H_

#ifdef _WIN32
# pragma warning(disable:4786) // Truncated debug names.
#endif

// Needed headers.
#include "Standard/SlClientHeaders.h"

// Nodes.
#include "SgAllNodes.h"

// Visitors.
#include "SgGetBoundingBox.h"
#include "SgGetRayIntersection.h"
#include "SgScreenRenderer.h"
#include "SgOffScreenRenderer.h"
#include "SgGetCount.h"

// Windows.
#include "SgViewer.h"

// Misc.
#include "SgDefine.h"
#include "SgDraw.h"
#include "SgMessage.h"

#endif // _CADKIT_SCENEGRAPH_CORE_LIBRARY_HEADERS_H_
