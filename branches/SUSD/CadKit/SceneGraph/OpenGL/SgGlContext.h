
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgGlContext: The rendering context class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_OPENGL_RENDERING_CONTEXT_H_
#define _CADKIT_SCENEGRAPH_OPENGL_RENDERING_CONTEXT_H_

#include "SgGlApi.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlRefBase.h"
#endif
  

namespace CadKit
{
class SG_GL_API SgGlContext : public SlRefBase
{
public:

  SgGlContext();

  // Create the context. Pass a pointer to the native window 
  // (i.e., HWND or Widget) to draw on.
  virtual bool                    create ( void *window ) = 0;

  // Is this context current?
  bool                            isCurrent() { return ( this == _current ); }

  // Make this context the current one.
  virtual bool                    makeCurrent() = 0;

  // Swap the buffers.
  virtual bool                    swapBuffers() = 0;

protected:

  static SgGlContext *_current;

  virtual ~SgGlContext();

  SL_DECLARE_REFERENCE_POINTER ( SgGlContext );
  SL_DECLARE_CLASS ( SgGlContext, 1032912208 );
};

}; // namespace CadKit

#endif // _CADKIT_SCENEGRAPH_OPENGL_RENDERING_CONTEXT_H_
