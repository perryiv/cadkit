
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgFoxContextImpl: The FOX OpenGL context implementation class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_OPENGL_FOX_RENDERING_CONTEXT_IMPLEMENTATION_H_
#define _CADKIT_SCENEGRAPH_OPENGL_FOX_RENDERING_CONTEXT_IMPLEMENTATION_H_

namespace FX
{
  class FXGLCanvas;
};


namespace CadKit
{
class SgFoxContextImpl
{
public:

  SgFoxContextImpl();
  ~SgFoxContextImpl();

  // Create the context. Pass a pointer to an FXGLCanvas.
  bool                  create ( void *window );

  // make this context current.
  bool                  makeCurrent();

  // Swap the buffer.
  bool                  swapBuffers();

protected:

  FX::FXGLCanvas *_canvas;
};

}; // namespace CadKit

#endif // _CADKIT_SCENEGRAPH_OPENGL_FOX_RENDERING_CONTEXT_IMPLEMENTATION_H_
