///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Mike Jackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#include "Helios/Cocoa/Views/CocoaGLContext.h"

using namespace Helios::Cocoa::Views;

///////////////////////////////////////////////////////////////////////////////
//
//  Usul/Interfaces/IUnknown interface Implementation
//
///////////////////////////////////////////////////////////////////////////////


//  This implements the ref/unref methods
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( CocoaGLContext , CocoaGLContext::BaseClass );

// -----------------------------------------------------------------------------
//    queryInterface method
// -----------------------------------------------------------------------------
Usul::Interfaces::IUnknown* CocoaGLContext::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
    case Usul::Interfaces::IUnknown::IID:
    case Usul::Interfaces::IOpenGLContext::IID:
      return static_cast < Usul::Interfaces::IOpenGLContext* > ( this );
    default:
      return 0x0;
  };
}

//////////////////////////////////////////////////////////////////////////////
//
//    Usul/Interfaces/IOpenGLContext Interface Implementation
//
///////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
//    Constructor that takes an NSOpenGLContext and retains it
// -----------------------------------------------------------------------------
CocoaGLContext::CocoaGLContext( NSOpenGLContext *context)
{
  this->_context = context;
  [ _context retain];
}

// -----------------------------------------------------------------------------
//    Destructor
// -----------------------------------------------------------------------------
CocoaGLContext::~CocoaGLContext()
{
  [ _context release];
  this->_context = 0x0;
}

// -----------------------------------------------------------------------------
//    Makes the OpenGLConext the Current Context
// -----------------------------------------------------------------------------
void    CocoaGLContext::makeCurrent()
{
  [ _context makeCurrentContext];
}

// -----------------------------------------------------------------------------
//    Swaps the OpenGLBuffers
// -----------------------------------------------------------------------------
void    CocoaGLContext::swapBuffers()
{
  [_context flushBuffer];
}


bool CocoaGLContext::isContextThread() const
{
return true;
}

///////////////////////////////////////////////////////////////////////////////
//
//    Usul IUnknown Interface
//
///////////////////////////////////////////////////////////////////////////////


