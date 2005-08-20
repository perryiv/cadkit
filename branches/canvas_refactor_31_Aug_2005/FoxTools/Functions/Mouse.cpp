
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Mouse functions.
//
///////////////////////////////////////////////////////////////////////////////

#include "FoxTools/Functions/Mouse.h"
#include "FoxTools/Headers/App.h"

#include "Usul/Bits/Bits.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Get the mouse state from the event.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int FoxTools::Functions::mouseState ( const FX::FXEvent *event )
{
  unsigned int state ( 0 );

  if ( event )
  {
    if ( Usul::Bits::has<int, int> ( event->state, FX::LEFTBUTTONMASK ) )
      state = Usul::Bits::add<int, int> ( state, FX::LEFTBUTTONMASK );

    if ( Usul::Bits::has<int, int> ( event->state, FX::MIDDLEBUTTONMASK ) )
      state = Usul::Bits::add<int, int> ( state, FX::MIDDLEBUTTONMASK );

    if ( Usul::Bits::has<int, int> ( event->state, FX::RIGHTBUTTONMASK ) )
      state = Usul::Bits::add<int, int> ( state, FX::RIGHTBUTTONMASK );
  }

  return state;
}
