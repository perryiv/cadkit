
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  The FOX OpenGL context class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_FOX_BINDING_RENDERING_CONTEXT_H_
#define _GENERIC_SCENE_GRAPH_FOX_BINDING_RENDERING_CONTEXT_H_

#include "GSG/FOX/FoxDll.h"

#include "GSG/OpenGL/GlContext.h"


namespace FX { class FXGLCanvas; };


namespace GSG {
namespace FOX {


class GSG_FOX_EXPORT Context : public GlContext
{
public:

  GSG_DECLARE_REFERENCED ( Context );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Context, GlContext );

  Context ( FXGLCanvas *canvas );
  Context ( const Context & ); // Do not copy.

  // Make this context current.
  virtual void        makeCurrent();

  // Swap the buffer.
  virtual void        swapBuffers();

protected:

  virtual ~Context();

private:

  FX::FXGLCanvas *_canvas;
};


}; // namespace FOX
}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_FOX_BINDING_RENDERING_CONTEXT_H_
