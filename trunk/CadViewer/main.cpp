
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
# include "Signals.h"
#endif

#include "Threads/OpenThreads/Mutex.h"

#include "Usul/Threads/Mutex.h"

#include "VRV/Core/Program.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Set for multi-threaded. This is global because the sooner we set this, 
//  the better. Setting in main() may be too late.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Threads::SetMutexFactory factory ( &Threads::OT::newOpenThreadsMutex );


///////////////////////////////////////////////////////////////////////////////
//
//  Procter & Gamble -- Virtual Reality Viewer.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  #ifdef __GNUC__
  // Register the signal handlers.
  CV::registerSignalHandlers ( argv[0] );
  #endif

  VRV::Core::Program < CV::Application > program ( "CadViewer" );

  // Isolate application run inside this function.
  return program.run ( argc, argv );
}
