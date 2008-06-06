
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

/////////////////////////////////////////////////////////////////////////////
//
//  SgGlWinRenderingContext: The base renderer class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_OPENGL_WINDOWS_RENDERING_CONTEXT_H_
#define _CADKIT_SCENEGRAPH_OPENGL_WINDOWS_RENDERING_CONTEXT_H_

#include "SgGlRenderingContext.h"


namespace CadKit
{
class SG_GL_API SgGlWinRenderingContext : public SgGlRenderingContext
{
public:

  SgGlWinRenderingContext();

  // Create the context. Pass a pointer to an HWND.
  virtual bool          create ( void *window );

  // make this context current.
  virtual bool          makeCurrent();

  // Swap the buffer.
  virtual bool          swapBuffers() const;

protected:

  HWND _window;
  HDC _deviceContext;
  HGLRC _glRenderingContext;

  virtual ~SgGlWinRenderingContext();

  bool                  _createContext ( HWND window );
  static HGLRC          _createOpenGLRenderingContext ( HDC deviceContext );

  bool                  _deleteContext();
  bool                  _deleteOpenGLRenderingContext();

  static SlInt32        _getBestPixelFormatDesc ( HDC deviceContext, PIXELFORMATDESCRIPTOR &pixelFormatDescriptor );
  static HDC            _getDeviceContext ( HWND window );

  bool                  _releaseDeviceContext();

  bool                  _selectSystemFont();
  void                  _setPointersToNull();
  static bool           _setupPixelFormat ( HDC deviceContext );

  SL_DECLARE_REFERENCE_POINTER ( SgGlWinRenderingContext );
  SL_DECLARE_DYNAMIC_CLASS ( SgGlWinRenderingContext, 1032911989 );
};

}; // namespace CadKit

#endif // _CADKIT_SCENEGRAPH_OPENGL_WINDOWS_RENDERING_CONTEXT_H_
