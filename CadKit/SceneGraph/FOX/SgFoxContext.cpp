
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgFoxContext: The FOX OpenGL context class.
//
///////////////////////////////////////////////////////////////////////////////

#include "SgFoxContext.h"
#include "SgFoxContextImpl.h"

#include "Standard/SlAssert.h"

using namespace CadKit;

SL_IMPLEMENT_DYNAMIC_CLASS ( SgFoxContext, SgGlContext );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SgFoxContext::SgFoxContext() : SgGlContext(), 
  _context ( new SgFoxContextImpl )
{
  SL_ASSERT ( 0x0 != _context );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SgFoxContext::~SgFoxContext()
{
  // VC++ doesn't like an auto-pointer here.
  delete _context;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the context.
//
///////////////////////////////////////////////////////////////////////////////

bool SgFoxContext::create ( void *window )
{
  SL_ASSERT ( this && _context && window );

  // Delegate.
  return _context->create ( window );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make this class's rendering context the current one.
//
///////////////////////////////////////////////////////////////////////////////

bool SgFoxContext::makeCurrent()
{
  SL_ASSERT ( this && _context );

  // If we are already current then just return.
  if ( true == this->isCurrent() )
    return true;

  // Delegate.
  if ( false == _context->makeCurrent() )
    return false;

  // Call the base class's function.
  return SgGlContext::makeCurrent();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Swap the buffers.
//
///////////////////////////////////////////////////////////////////////////////

bool SgFoxContext::swapBuffers()
{
  // Make sure this context is current.
  if ( false == this->makeCurrent() )
    return false;

  // Delegate.
  return _context->swapBuffers();
}
