
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  OpenGL rendering context.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_OPENGL_RENDERING_CONTEXT_H_
#define _GENERIC_SCENE_GRAPH_OPENGL_RENDERING_CONTEXT_H_

#include "GSG/OpenGL/OglDll.h"

#include "GSG/Core/Referenced.h"


namespace GSG {


class GSG_OPENGL_EXPORT GlContext : public Referenced
{
public:

  GSG_DECLARE_LOCAL_TYPEDEFS ( GlContext, Referenced );

  // Make this context the current one.
  virtual void        makeCurrent() = 0;

  // Swap the buffers.
  virtual void        swapBuffers() = 0;

  // Is this context the current one?
  virtual bool        isCurrent() const;

protected:

  GlContext();
  GlContext ( const GlContext & );
  virtual ~GlContext();
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_OPENGL_RENDERING_CONTEXT_H_
