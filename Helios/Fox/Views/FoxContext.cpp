
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Fox/Views/FoxContext.h"
#include "Helios/Fox/Views/FoxCanvas.h"

using namespace OsgFox::Views;


void FoxContext::makeCurrent()
{
  _canvas.makeCurrent();
}

void FoxContext::swapBuffers() 
{ 
  _canvas.swapBuffers(); 
}

void FoxContext::ref ()
{
  BaseClass::ref();
}

void FoxContext::unref ( bool allowDeletion )
{
  BaseClass::unref( allowDeletion );
}
  
Usul::Interfaces::IUnknown* FoxContext::queryInterface ( unsigned long iid )
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

