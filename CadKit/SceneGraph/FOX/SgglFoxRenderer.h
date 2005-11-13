
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgglFoxRenderer: The base renderer class.
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
class SG_GL_API SgglFoxRenderer : public SgScreenRenderer
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

  SgglFoxRenderer();
  virtual ~SgglFoxRenderer();

  SlBool                  _makeThisContextCurrent() const;

  SlBool                  _swapBuffers() const;

  SG_DECLARE_VISITOR(SgglFoxRenderer,0x00005003);
};
};

#endif

