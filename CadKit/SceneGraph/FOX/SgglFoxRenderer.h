
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgGlFoxRenderer: The FOX renderer class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_OPENGL_FOX_BASE_RENDERER_VISITOR_H_
#define _CADKIT_SCENEGRAPH_OPENGL_FOX_BASE_RENDERER_VISITOR_H_

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
  #include "Sg/SgScreenRenderer.h"
#endif

class FXGLCanvas;

namespace CadKit
{
class SG_GL_API SgGlFoxRenderer : public SgScreenRenderer
{
public:

  // Get the canvas.
  const FXGLCanvas *      getCanvas() const { return _canvas; }
  FXGLCanvas *            getCanvas()       { return _canvas; }

  // Get the size of the canvas.
  SlResult                getCanvasSize ( SlInt32 &width, SlInt32 &height ) const;

  // Set the OpenGL default settings.
  virtual SlResult        setOpenGLDefaults() = 0;

  // Set the FXGLCanvas to render on. Have to call this before you do 
  // anything else with this class.
  virtual SlBool          setWindow ( void *window ); // Make sure this is an FXGLCanvas!

protected:

  FXGLCanvas *_canvas;

  SgGlFoxRenderer();
  virtual ~SgGlFoxRenderer();

  SlBool                  _makeThisContextCurrent() const;

  SlBool                  _swapBuffers() const;

  SG_DECLARE_VISITOR(SgGlFoxRenderer,0x00005003);
};
};

#endif

