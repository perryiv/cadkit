
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Commands/RenderLoop.h"
#include "Usul/Interfaces/IRenderLoop.h"

using namespace Usul::Commands;

USUL_IMPLEMENT_COMMAND ( RenderLoop );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RenderLoop::RenderLoop ( const std::string& name, Usul::Interfaces::IUnknown *caller ) : 
BaseClass ( caller )
{
  this->text ( name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

RenderLoop::~RenderLoop ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void RenderLoop::_execute ()
{
  Usul::Interfaces::IRenderLoop::QueryPtr rl ( this->caller () );

  if ( rl.valid () )
    rl->renderLoop ( !rl->renderLoop () );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the check.
//
///////////////////////////////////////////////////////////////////////////////

bool RenderLoop::updateCheck () const
{
  Usul::Interfaces::IRenderLoop::QueryPtr rl ( const_cast < Usul::Interfaces::IUnknown * > ( this->caller () ) );
  return rl.valid () ? rl->renderLoop () : false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the enabled state.
//
///////////////////////////////////////////////////////////////////////////////

bool RenderLoop::updateEnable () const
{
  Usul::Interfaces::IRenderLoop::QueryPtr rl ( const_cast < Usul::Interfaces::IUnknown * > ( this->caller () ) );
  return rl.valid ();
}
