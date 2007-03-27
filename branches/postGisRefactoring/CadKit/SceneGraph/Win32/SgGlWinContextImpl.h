
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgGlWinContextImpl: The Win32 OpenGL context implementation class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_OPENGL_WIN32_RENDERING_CONTEXT_IMPLEMENTATION_H_
#define _CADKIT_SCENEGRAPH_OPENGL_WIN32_RENDERING_CONTEXT_IMPLEMENTATION_H_

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# define NOMINMAX // Do not define min and max as macros.
# include <windows.h>
#endif


namespace CadKit
{
class SgGlWinContextImpl
{
public:

  SgGlWinContextImpl();
  ~SgGlWinContextImpl();

  // Create the context. Pass a pointer to an HWND.
  bool                  create ( void *window );

  // make this context current.
  bool                  makeCurrent();

  // Swap the buffer.
  bool                  swapBuffers();

protected:

  HWND _window;
  HDC _deviceContext;
  HGLRC _glContext;

  bool                  _createContext ( HWND window );
  static HGLRC          _createOpenGLContext ( HDC deviceContext );

  bool                  _deleteContext();
  bool                  _deleteOpenGLContext();

  static int            _getBestPixelFormatDesc ( HDC deviceContext, PIXELFORMATDESCRIPTOR &pixelFormatDescriptor );
  static HDC            _getDeviceContext ( HWND window );

  bool                  _releaseDeviceContext();

  bool                  _selectSystemFont();
  void                  _setPointersToNull();
  static bool           _setupPixelFormat ( HDC deviceContext );
};

}; // namespace CadKit

#endif // _CADKIT_SCENEGRAPH_OPENGL_WIN32_RENDERING_CONTEXT_H_
