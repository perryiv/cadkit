
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgGlWinContext: The Win32 OpenGL context class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_OPENGL_WIN32_RENDERING_CONTEXT_H_
#define _CADKIT_SCENEGRAPH_OPENGL_WIN32_RENDERING_CONTEXT_H_

#include "SgGlWinApi.h"

#include "SceneGraph/OpenGL/SgGlContext.h"

#include "Standard/SlAutoPtr.h"


namespace CadKit
{
class SgGlWinContextImpl;

class SG_WIN_API SgGlWinContext : public SgGlContext
{
public:

  SgGlWinContext();

  // Create the context. Pass a pointer to an HWND.
  virtual bool          create ( void *window );

  // make this context current.
  virtual bool          makeCurrent();

  // Swap the buffer.
  virtual bool          swapBuffers();

protected:

  SlAutoPtr<SgGlWinContextImpl> _context;

  virtual ~SgGlWinContext();

  SL_DECLARE_REFERENCE_POINTER ( SgGlWinContext );
  SL_DECLARE_DYNAMIC_CLASS ( SgGlWinContext, 1045536926 );
};

}; // namespace CadKit

#endif // _CADKIT_SCENEGRAPH_OPENGL_WIN32_RENDERING_CONTEXT_H_
