
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

#include "SgGlPrecompiled.h"
#include "SgGlFoxRenderer.h"
#include "SgGlConstants.h"
#include "SgGlDefine.h"

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

SL_IMPLEMENT_CLASS(SgGlFoxRenderer,SgScreenRenderer);


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SgGlFoxRenderer::SgGlFoxRenderer() : SgScreenRenderer(),
  _canvas ( 0x0 )
{
  SL_PRINT2 ( "SgGlFoxRenderer::SgGlFoxRenderer(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SgGlFoxRenderer::~SgGlFoxRenderer()
{
  SL_PRINT2 ( "SgGlFoxRenderer::~SgGlFoxRenderer(), this = %X\n", this );
  // TODO. Do I have to delete anything?
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the window to render on.
//
///////////////////////////////////////////////////////////////////////////////

SlBool SgGlFoxRenderer::setWindow ( void *window )
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

SlBool SgGlFoxRenderer::_makeThisContextCurrent() const
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

SlBool SgGlFoxRenderer::_swapBuffers() const
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

SlResult SgGlFoxRenderer::getCanvasSize ( SlInt32 &width, SlInt32 &height ) const
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
