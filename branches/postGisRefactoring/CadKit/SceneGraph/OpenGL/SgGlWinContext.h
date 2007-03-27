
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgGlWinContext: The Win32 context class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_OPENGL_WINDOWS_RENDERING_CONTEXT_H_
#define _CADKIT_SCENEGRAPH_OPENGL_WINDOWS_RENDERING_CONTEXT_H_

#include "SgGlContext.h"


namespace CadKit
{
class SG_GL_API SgGlWinContext : public SgGlContext
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

  HWND _window;
  HDC _deviceContext;
  HGLRC _glContext;

  virtual ~SgGlWinContext();

  bool                  _createContext ( HWND window );
  static HGLRC          _createOpenGLContext ( HDC deviceContext );

  bool                  _deleteContext();
  bool                  _deleteOpenGLContext();

  static SlInt32        _getBestPixelFormatDesc ( HDC deviceContext, PIXELFORMATDESCRIPTOR &pixelFormatDescriptor );
  static HDC            _getDeviceContext ( HWND window );

  bool                  _releaseDeviceContext();

  bool                  _selectSystemFont();
  void                  _setPointersToNull();
  static bool           _setupPixelFormat ( HDC deviceContext );

  SL_DECLARE_REFERENCE_POINTER ( SgGlWinContext );
  SL_DECLARE_DYNAMIC_CLASS ( SgGlWinContext, 1032912127 );
};

}; // namespace CadKit

#endif // _CADKIT_SCENEGRAPH_OPENGL_WINDOWS_RENDERING_CONTEXT_H_
