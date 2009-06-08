
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
//  SgGlWinRenderingContext: The renderer class.
//
/////////////////////////////////////////////////////////////////////////////

#include "SgGlPrecompiled.h"
#include "SgGlWinRenderingContext.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlAssert.h"
# include "Standard/SlPrint.h"
#endif

using namespace CadKit;

SL_IMPLEMENT_DYNAMIC_CLASS(SgGlWinRenderingContext,SgGlRenderingContext);


/////////////////////////////////////////////////////////////////////////////
//
//  Macro for constructors.
//
/////////////////////////////////////////////////////////////////////////////

#define SG_GL_INITIALIZE_DATA_MEMBERS \
  _window ( NULL ), \
  _deviceContext ( NULL ), \
  _glRenderingContext ( NULL )


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgGlWinRenderingContext::SgGlWinRenderingContext() : SgGlRenderingContext(), SG_GL_INITIALIZE_DATA_MEMBERS
{
  SL_PRINT ( "SgGlWinRenderingContext::SgGlWinRenderingContext(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

SgGlWinRenderingContext::~SgGlWinRenderingContext()
{
  SL_PRINT ( "SgGlWinRenderingContext::~SgGlWinRenderingContext(), this = %X\n", this );

  // Delete the context if we have one.
  if ( _window ) SL_VERIFY ( this->_deleteContext() );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Create the context.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGlWinRenderingContext::create ( void *theWindow )
{
  SL_ASSERT ( this );

  // Convert to an HWND.
  HWND window = static_cast<HWND>(theWindow);

  // Make sure it's a valid window.
  if ( FALSE == ::IsWindow ( window ) )
  {
    SL_ASSERT ( 0 );
    return false;
  }

  // If we already have a window...
  if ( _window )
  {
    // Delete the current context.
    if ( false == this->_deleteContext() ) 
      return false;
  }

  // If the client passed a good pointer...
  if ( window ) 
  {
    // Then create the new context.
    if ( false == this->_createContext ( window ) ) 
      return false;

    // Make the system font the new device context's selected font.
    if ( false == this->_selectSystemFont() )
    {
      // If it didn't work then delete the context.
      SL_VERIFY ( this->_deleteContext() );

      // It didn't work.
      return false;
    }
  }

  // If we get this far then we did good.
  return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the pointers to null.
//
/////////////////////////////////////////////////////////////////////////////

void SgGlWinRenderingContext::_setPointersToNull()
{
  _window = NULL;
  _deviceContext = NULL;
  _glRenderingContext = NULL;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Delete the OpenGL rendering context.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGlWinRenderingContext::_deleteContext()
{
  SL_ASSERT ( this && _window && _deviceContext && _glRenderingContext );

  // Initialize this.
  bool result = true;

  // Delete the current OpenGL rendering context.
  if ( false == this->_deleteOpenGLRenderingContext() ) 
    result = false;

  // Release the device context.
  if ( false == this->_releaseDeviceContext() ) 
    result = false;

  // Reset to NULL.
  this->_setPointersToNull();

  // Return the result.
  return result;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Release the device context.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGlWinRenderingContext::_releaseDeviceContext()
{
  SL_ASSERT ( this && _window && _deviceContext && !_glRenderingContext );

  // Initialize this.
  bool result = true;

  // Release the device context.
  if ( ::ReleaseDC ( _window, _deviceContext ) != 1 )
  {
    SL_ASSERT ( 0 ); // Why are we here?
    result = false;
  }

  // Either way, do this.
  _deviceContext = NULL;

  // Return the result.
  return result;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Delete the OpenGL rendering context.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGlWinRenderingContext::_deleteOpenGLRenderingContext()
{
  SL_ASSERT ( this && _window && _deviceContext && _glRenderingContext );

  // Initialize this.
  bool result = true;

  // Delete the current OpenGL rendering context.
  if ( FALSE == ::wglDeleteContext ( _glRenderingContext ) )
  {
    SL_ASSERT ( 0 ); // Why are we here?
    result = false;
  }

  // Either way, do this.
  _glRenderingContext = NULL;

  // Return the result.
  return result;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Make the system font the device context's selected font.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGlWinRenderingContext::_selectSystemFont()
{
  SL_ASSERT ( this && _window && _deviceContext && _glRenderingContext );

  // Select the font.
  if ( !::SelectObject ( _deviceContext, ::GetStockObject ( DEFAULT_GUI_FONT ) ) )
  {
    SL_ASSERT ( 0 ); // Why are we here?
    return false;
  }

  // It worked.
  return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  This will create the OpenGL rendering context. Do not pass NULL.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGlWinRenderingContext::_createContext ( HWND window )
{
  SL_ASSERT ( this && window && !_window );

  // Get the devise context and make sure we got a good one
  _deviceContext = _getDeviceContext ( window );
  if ( !_deviceContext ) 
    return false;

  // Set the pixel format.
    if ( !_setupPixelFormat ( _deviceContext ) )
  {
    SL_ASSERT ( 0 ); // Why are we here?

    // Release the device context.
    SL_VERIFY ( this->_releaseDeviceContext() );

    // Reset pointers.
    this->_setPointersToNull();

    return false;
  }

  // Create the OpenGL rendering context.
  _glRenderingContext = _createOpenGLRenderingContext ( _deviceContext );

  // Make sure it created.
  if ( !_glRenderingContext )
  {
    // Release the device context.
    SL_VERIFY ( this->_releaseDeviceContext() );

    // Reset pointers.
    this->_setPointersToNull();

    return false;
  }

  // Set our pointer.
  _window = window;

  // If we get this far then it worked.
  return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get the device context from the window.
//
/////////////////////////////////////////////////////////////////////////////

HDC SgGlWinRenderingContext::_getDeviceContext ( HWND window )
{
  SL_ASSERT ( window );

  // Get the devise context.
  HDC deviceContext = ::GetDC ( window );
  SL_ASSERT ( deviceContext );

  // Return the device context (which may be NULL).
  return deviceContext;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Create the OpenGL rendering context from the device context.
//
/////////////////////////////////////////////////////////////////////////////

HGLRC SgGlWinRenderingContext::_createOpenGLRenderingContext ( HDC deviceContext )
{
  SL_ASSERT ( deviceContext );

  // Create the OpenGL rendering context.
  HGLRC glRenderingContext = ::wglCreateContext ( deviceContext );
  SL_ASSERT ( glRenderingContext );

  // Return the rendering context (which may be NULL).
  return glRenderingContext;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set up the pixel format for a device context. It trys to pick the best 
//  one available to your OpenGL driver.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGlWinRenderingContext::_setupPixelFormat ( HDC deviceContext )
{
  SL_ASSERT ( deviceContext );

  // Declare the pixel format descriptor.
  PIXELFORMATDESCRIPTOR pixelFormatDescriptor;

  // Search all rendering modes available to this HDC,
  // it returns the best pixelformat found.
  int pixformat = _getBestPixelFormatDesc ( deviceContext, pixelFormatDescriptor );

  // If _getBestPixelFormatDesc() failed, (when pixformat == 0)
  // then have Windows choose a pixel format that is "close" 
  // (yeah right). Basically, if _getBestPixelFormatDesc() fails, 
  // then there probably isn't a mode that we'll like.  :( 
  // This just lets our application to continue - hell, _maybe_ 
  // it will look "ok".
  if ( pixformat == 0 )
  {
    memset ( &pixelFormatDescriptor, 0, sizeof ( PIXELFORMATDESCRIPTOR ) );
    pixelFormatDescriptor.nSize = sizeof ( PIXELFORMATDESCRIPTOR );
    pixelFormatDescriptor.nVersion = 1;
    pixelFormatDescriptor.dwFlags = 
      PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_BITMAP | 
      PFD_GENERIC_ACCELERATED | PFD_SWAP_EXCHANGE  | PFD_DOUBLEBUFFER;
    pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
    pixelFormatDescriptor.cColorBits = 24;
    pixelFormatDescriptor.cDepthBits = 16;
    pixelFormatDescriptor.iLayerType = PFD_MAIN_PLANE; // Ignored.

    // Let Windows choose one "closest" to what we want, 
    // that your OpenGL driver supports.
    pixformat = ::ChoosePixelFormat ( deviceContext, &pixelFormatDescriptor );
  };

  // If Windows couldn't even cough up a "reasonably close" matching
  // pixelformat descriptor, then bail out. 
  if ( 0 == pixformat ) 
    return false;

  // We have a valid format descriptor so set the format descriptor for the HDC.
  return ( TRUE == ::SetPixelFormat ( deviceContext, pixformat, &pixelFormatDescriptor ) );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Search all rendering modes available to this HDC. There are some hard 
//  coded constraints in here that could be put into a dialog box or 
//  .rc/.ini file. Basically this "should" cycle through all available pixel 
//  format descriptors available on the OpenGL driver and pick the best one.
//
//  Returns 0 on failure.
//  Returns the pixelformat ID on success.
//
/////////////////////////////////////////////////////////////////////////////

int SgGlWinRenderingContext::_getBestPixelFormatDesc ( HDC deviceContext, PIXELFORMATDESCRIPTOR &pixelFormatDescriptor )
{
  SL_ASSERT ( deviceContext );

  // Flags...
  int iPixelFormat = 1;  
  int result = 1;
  int worstButCloseFormat = 0, wantedFormat = 0, accelFormat = 0;
  int depthSize = 0;
  
  // Aliases, so I can do a debug on the values...
  const int opengl = PFD_SUPPORT_OPENGL;
  const int rgba = PFD_TYPE_RGBA;
  const int doublebuffer = PFD_DOUBLEBUFFER;
  const int accelerated = PFD_GENERIC_ACCELERATED;
  const int colorindex = PFD_TYPE_COLORINDEX;

  // Get the 1st pixel format supported by the video driver.
  result = ::DescribePixelFormat ( deviceContext, iPixelFormat, sizeof ( PIXELFORMATDESCRIPTOR ), &pixelFormatDescriptor );

  // While we can still query, find the best pixel format.
  while ( 0 != result )
  {  
    // The pixel format HAS to have this stuff...
    if ( ( pixelFormatDescriptor.dwFlags & doublebuffer ) == doublebuffer &&
         ( pixelFormatDescriptor.iPixelType & rgba ) == rgba &&
         ( pixelFormatDescriptor.iPixelType & colorindex ) != colorindex &&
         ( pixelFormatDescriptor.dwFlags & opengl ) == opengl &&
         ( pixelFormatDescriptor.cColorBits >= 15 ) )
    {
      // If we found a larger z depth (within our requested range), 
      // then use it instead.
      if ( pixelFormatDescriptor.cDepthBits > depthSize && 
           pixelFormatDescriptor.cDepthBits <= 16 ) // TODO, why 16?
      {
        // Save the base format that's found.
        wantedFormat = iPixelFormat;

        // Save the best Zdepth value so far for comparison later.
        depthSize = pixelFormatDescriptor.cDepthBits;
      }

      // Otherwise, save it here.
      else 
        worstButCloseFormat = iPixelFormat;

      // It would be great to have this stuff, so grab the index if found.
      if ( ( pixelFormatDescriptor.dwFlags & accelerated ) == accelerated ) 
        accelFormat = iPixelFormat;
    }

    // Increment the pixel format number.
    ++iPixelFormat;
    
    // Get the next pixel format available from the video driver.
    result = ::DescribePixelFormat ( deviceContext, iPixelFormat, sizeof ( PIXELFORMATDESCRIPTOR ), &pixelFormatDescriptor );
  } 

  // We want it accelerated most of all.
  if ( 0 != accelFormat ) 
    return accelFormat;

  // We'll take this one next.
  if ( 0 != wantedFormat )
    return wantedFormat;

  // Otherwise return the "worst, but close" format
  return worstButCloseFormat;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Make this class's rendering context the current one.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGlWinRenderingContext::makeCurrent()
{
  SL_ASSERT ( this && _window && _deviceContext && _glRenderingContext );

  // If we are not already the current context...
  if ( _glRenderingContext != ::wglGetCurrentContext() )
  {
    // Try to set the context.
    if ( FALSE == ::wglMakeCurrent ( _deviceContext, _glRenderingContext ) )
    {
      SL_ASSERT ( 0 ); // Why couldn't we set the context?
      return false;
    }
  }

  // If we get to here then this should be true.
  SL_ASSERT ( _glRenderingContext == ::wglGetCurrentContext() );
  SL_ASSERT ( _deviceContext == ::wglGetCurrentDC() );

  // It worked.
  return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Swap the buffers.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGlWinRenderingContext::swapBuffers() const
{
  return ( TRUE == ::SwapBuffers ( ::wglGetCurrentDC() ) );
}
