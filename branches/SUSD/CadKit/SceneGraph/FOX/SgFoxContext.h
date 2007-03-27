
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgFoxContext: The FOX OpenGL context class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_OPENGL_FOX_RENDERING_CONTEXT_H_
#define _CADKIT_SCENEGRAPH_OPENGL_FOX_RENDERING_CONTEXT_H_

#include "SgFoxApi.h"

#include "SceneGraph/OpenGL/SgGlContext.h"


namespace CadKit
{
class SgFoxContextImpl;

class SG_FOX_API SgFoxContext : public SgGlContext
{
public:

  SgFoxContext();

  // Create the context. Pass a pointer to an HWND.
  virtual bool          create ( void *window );

  // make this context current.
  virtual bool          makeCurrent();

  // Swap the buffer.
  virtual bool          swapBuffers();

protected:

  SgFoxContextImpl *_context;

  virtual ~SgFoxContext();

  SL_DECLARE_REFERENCE_POINTER ( SgFoxContext );
  SL_DECLARE_DYNAMIC_CLASS ( SgFoxContext, 1047759198 );
};

}; // namespace CadKit

#endif // _CADKIT_SCENEGRAPH_OPENGL_FOX_RENDERING_CONTEXT_H_
