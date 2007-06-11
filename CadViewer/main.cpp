
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "Application.h"
#include "ConfigFiles.h"

#ifdef __GNUC__
# include "Signals.h"
#endif

#include "Threads/OpenThreads/Mutex.h"

#include "Usul/Threads/Mutex.h"

#include "VRV/Core/Exceptions.h"
#include "VRV/Core/Program.h"

#include <iostream>
#include <string>
#include <algorithm>

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
  // Initialize the return value.
  int result ( 1 );

  // Be safe...
  try
  {
    #ifdef __GNUC__
    // Register the signal handlers.
    CV::registerSignalHandlers ( argv[0] );
    #endif

    VRV::Core::Program < CV::Application > program;

    // Isolate application run inside this function.
    result = program.run ( argc, argv );
  }

  // Catch local exceptions.
  catch ( const VRV::Core::Exceptions::UserInput &e )
  {
    std::cout << "Input Error:\n" << e.what() << std::endl;
    CV::Application::usage ( argv[0], std::cout );
  }

  // Catch standard exceptions.
  catch ( const std::exception &e )
  {
    std::cout << "Exception caught:\n" << e.what() << std::endl;
  }

  // Catch all other exceptions.
  catch ( ... )
  {
    std::cout << "Unknown exception caught" << std::endl;
  }

  // Return the result.
  return result;
}
