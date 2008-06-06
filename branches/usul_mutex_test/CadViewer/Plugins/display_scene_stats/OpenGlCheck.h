
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Compile-time checking of OpenGL implementation.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_PLUGIN_DISPLAY_SCENE_STATS_OPEN_GL_CHECK_H_
#define _CV_PLUGIN_DISPLAY_SCENE_STATS_OPEN_GL_CHECK_H_

#include "CompileGuard.h"

#ifdef _WIN32
# include <windows.h>
#endif

#include "Usul/Headers/OpenGL.h"

#include "Usul/MPL/StaticAssert.h"


namespace CV {
namespace Plugins {
namespace Detail {


struct PrimitiveModes
{
  // Figure out the min, max, and difference.
  enum
  {
    MIN_MODE  = GL_POINTS,
    MAX_MODE  = GL_POLYGON,
    NUM_MODES = MAX_MODE - MIN_MODE + 1,
  };

  PrimitiveModes()
  {
    // Make sure the mode-constants are consecutive.
    USUL_STATIC_ASSERT ( 1 + GL_POINTS          == GL_LINES );
    USUL_STATIC_ASSERT ( 1 + GL_LINES           == GL_LINE_LOOP );
    USUL_STATIC_ASSERT ( 1 + GL_LINE_LOOP       == GL_LINE_STRIP );
    USUL_STATIC_ASSERT ( 1 + GL_LINE_STRIP      == GL_TRIANGLES );
    USUL_STATIC_ASSERT ( 1 + GL_TRIANGLES       == GL_TRIANGLE_STRIP );
    USUL_STATIC_ASSERT ( 1 + GL_TRIANGLE_STRIP  == GL_TRIANGLE_FAN );
    USUL_STATIC_ASSERT ( 1 + GL_TRIANGLE_FAN    == GL_QUADS );
    USUL_STATIC_ASSERT ( 1 + GL_QUADS           == GL_QUAD_STRIP );
    USUL_STATIC_ASSERT ( 1 + GL_QUAD_STRIP      == GL_POLYGON );
  }
};


}; // namespace Detail
}; // namespace Plugins
}; // namespace CV


#endif // _CV_PLUGIN_DISPLAY_SCENE_STATS_OPEN_GL_CHECK_H_
