
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgFoxContextImpl: The FOX OpenGL context implementation class.
//
///////////////////////////////////////////////////////////////////////////////

#include "SgFoxContextImpl.h"

#ifdef _WIN32
# define NOMINMAX // Fox includes <windows.h>
#endif

#include <fx.h>
#include <fx3d.h>

#include "Standard/SlAssert.h"
#include "Standard/SlPrint.h"

using namespace CadKit;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SgFoxContextImpl::SgFoxContextImpl() :
  _canvas ( 0x0 )
{
  SL_PRINT2 ( "SgFoxContextImpl::SgFoxContextImpl(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SgFoxContextImpl::~SgFoxContextImpl()
{
  SL_PRINT2 ( "SgFoxContextImpl::~SgFoxContextImpl(), this = %X\n", this );
  // TODO. Do I have to delete anything?
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the context. Pass a pointer to an FXGLCanvas.
//
///////////////////////////////////////////////////////////////////////////////

bool SgFoxContextImpl::create ( void *window )
{
  SL_ASSERT ( this );
  SL_ASSERT ( window );
  SL_ASSERT ( _canvas == 0x0 );

  // Set the canvas.
  _canvas = (FXGLCanvas *) window;
  SL_ASSERT ( _canvas->isMemberOf ( FXMETACLASS ( FXGLCanvas ) ) );

  // Nothing to create here. The FXGLCanvas create's its own context.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make this class's rendering context the current one.
//
///////////////////////////////////////////////////////////////////////////////

bool SgFoxContextImpl::makeCurrent()
{
  SL_ASSERT ( this && _canvas );

  // Make our canvas the current context.
  return ( TRUE == _canvas->makeCurrent() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Swap the buffers.
//
///////////////////////////////////////////////////////////////////////////////

bool SgFoxContextImpl::swapBuffers()
{
  SL_ASSERT ( this && _canvas );

  // Tell the canvas to swap buffers.
  _canvas->swapBuffers();

  // It worked.
  return true;
}
