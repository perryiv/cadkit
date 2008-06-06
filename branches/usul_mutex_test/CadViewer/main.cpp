
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "Application.h"

#ifdef __GNUC__
# include "Usul/Errors/Signals.h"
#endif

#include "VRV/Core/Program.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Procter & Gamble -- Virtual Reality Viewer.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  #ifdef __GNUC__
  // Register the signal handlers.
  Usul::Errors::registerSignalHandlers ( argv[0] );
  #endif

  VRV::Core::Program < CV::Application > program ( "CadViewer" );

  // Isolate application run inside this function.
  return program.run ( argc, argv );
}
