
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
//  SgglFoxRenderer: The renderer class.
//
///////////////////////////////////////////////////////////////////////////////

#include "SgglPrecompiled.h"
#include "SgglFoxRenderer.h"
#include "SgglConstants.h"
#include "SgglDefine.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlString.h"
# include "Standard/SlFunctions.h"
# include "Sg/SgDefine.h"
# include "fox/fxver.h"
# include "fox/fxdefs.h"
# include "fox/FXStream.h"
# include "fox/FXObject.h"
# include "fox/FXId.h"
# include "fox/FXDrawable.h"
# include "fox/FXWindow.h"
# include "fox/FXCanvas.h"
# include "fox/FXComposite.h"
# include "fox/FXGLVisual.h"
# include "fox/FXGLCanvas.h"
#endif

using namespace CadKit;

SL_IMPLEMENT_CLASS(SgglFoxRenderer,SgScreenRenderer);


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SgglFoxRenderer::SgglFoxRenderer() : SgScreenRenderer(),
  _canvas ( 0x0 )
{
  SL_PRINT2 ( "SgglFoxRenderer::SgglFoxRenderer(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SgglFoxRenderer::~SgglFoxRenderer()
{
  SL_PRINT2 ( "SgglFoxRenderer::~SgglFoxRenderer(), this = %X\n", this );
  // TODO. Do I have to delete anything?
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the window to render on.
//
///////////////////////////////////////////////////////////////////////////////

SlBool SgglFoxRenderer::setWindow ( void *window )
{
  SL_ASSERT ( this );
  SL_ASSERT ( window );
  SL_ASSERT ( _canvas == 0x0 );
  SG_CHECK_POINTER ( "TODO_REPLACE_WITH_UUID", SL_FALSE, this );
  SG_CHECK_POINTER ( "TODO_REPLACE_WITH_UUID", SL_FALSE, window );

  // Set the canvas.
  _canvas = (FXGLCanvas *) window;

  // Set our default OpenGL settings.
  return ( this->setOpenGLDefaults() == SL_OK ) ? SL_TRUE : SL_FALSE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make this class's rendering context the current one.
//
///////////////////////////////////////////////////////////////////////////////

SlBool SgglFoxRenderer::_makeThisContextCurrent() const
{
  SL_ASSERT ( this && _canvas );

  // Make our canvas the current context.
  return ( _canvas->makeCurrent() == TRUE ) ? SL_TRUE : SL_FALSE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Swap the buffers.
//
///////////////////////////////////////////////////////////////////////////////

SlBool SgglFoxRenderer::_swapBuffers() const
{
  SL_ASSERT ( this && _canvas );

  // Tell the canvas to swap buffers.
  _canvas->swapBuffers();

  // Leave this function as a boolean return because the Win32 API 
  // function ::SwapBuffers() returns a boolean, so some day I may 
  // change FXGLCanvas::swapBuffers() to do the same.
  return SL_TRUE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the size of the canvas.
//
///////////////////////////////////////////////////////////////////////////////

SlResult SgglFoxRenderer::getCanvasSize ( SlInt32 &width, SlInt32 &height ) const
{
  SL_ASSERT ( this );
  SL_ASSERT ( _canvas );
  SG_CHECK_POINTER ( "TODO_REPLACE_WITH_UUID", SL_NULL_POINTER, this );
  SG_CHECK_POINTER ( "TODO_REPLACE_WITH_UUID", SL_NULL_POINTER, _canvas );

  // Get the size of the canvas.
  width  = _canvas->getWidth();
  height = _canvas->getHeight();

  // It worked.
  return SL_OK;
}
