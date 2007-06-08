
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "Application.h"
#include "Registry.h"
#include "Exceptions.h"

#ifdef __GNUC__
# include "Signals.h"
#endif

#include "Usul/Threads/Mutex.h"
#include "Usul/Components/Object.h"

#include "Threads/OpenThreads/Mutex.h"

#include "VRV/Core/Exceptions.h"

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
//  This function starts the application and returns when it stops, or if 
//  there is an exception.
//
///////////////////////////////////////////////////////////////////////////////

void runApplication ( int argc, char **argv )
{
  // Instantiate the registry. The sooner the better.
  CV::Registry::instance();
#if 0
  // Check arguments.
  if ( argc < 2 )
  {
    CV::Application::usage ( argv[0], std::cout );
    return;
  }
#endif
  // Populate the registry.
  CV::Registry::instance().read();

  // Put the arguments into a container.
  CV::Application::Args args;
  std::copy ( argv + 1, argv + argc, std::back_inserter ( args ) );

  // Construct an application.
  CV::Application app ( args );

  // Run the application.
  app.run();
}


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

    // Isolate application run inside this function.
    ::runApplication ( argc, argv );

    // Release all libraries that loaded during component creation. 
    // Note: We should be able to safely do this now that all components 
    // should have been destroyed.
    Usul::Components::Object::releaseLibraries();

    // Success.
    result = 0;
  }

  // Catch local exceptions.
  catch ( const VRV::Core::Exceptions::UserInput &e )
  {
    std::cout << "Input Error:\n" << e.what() << std::endl;
    CV::Application::usage ( argv[0], std::cout );
  }

  // Catch XML-reader exceptions.
  catch ( const CV::Exceptions::CorruptRegistry &e )
  {
    std::cout << e.what() << std::endl;
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
