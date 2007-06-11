
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
#include "Threads/OpenThreads/Thread.h"

#include "Usul/CommandLine/Arguments.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Components/Loader.h"
#include "Usul/Components/Manager.h"
#include "Usul/Console/Feedback.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Threads/Manager.h"
#include "Usul/Trace/Trace.h"

#include "VRV/Core/Exceptions.h"

#include "XmlTree/Document.h"

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
  Usul::Threads::Manager::instance().factory ( &Threads::OT::newOpenThreadsThread );

  std::ofstream trace ( "trace.csv" );
  Usul::Trace::Print::init ( &trace );
  Usul::CommandLine::Arguments::instance().set ( argc, argv );

	// Console Feedback.
	Usul::Console::Feedback::RefPtr feedback ( new Usul::Console::Feedback );
	
  // Load the plugins.
  Usul::Components::Loader < XmlTree::Document > loader;
  loader.parse ( CV::Config::filename ( "registry" ) );
  loader.load ( feedback->queryInterface ( Usul::Interfaces::IUnknown::IID ) );

  // Use 10 threads.
  Usul::Jobs::Manager::instance().poolResize ( 10 );

  // Print what we found.
  Usul::Components::Manager::instance().print ( std::cout );

  // Put the arguments into a container.
  CV::Application::Args args;
  std::copy ( argv + 1, argv + argc, std::back_inserter ( args ) );

  // Construct an application.
  CV::Application app ( args );

  // Run the application.
  app.run();

  // The job manager has a thread-pool.
  Usul::Jobs::Manager::destroy();

  // There should not be any threads running.
  Usul::Threads::Manager::instance().wait();

  // Set the mutex factory to null so that we can find late uses of it.
  Usul::Threads::Mutex::createFunction ( 0x0 );

  // Clean up the thread manager.
  Usul::Threads::Manager::destroy();

  // Release all libraries that loaded during component creation. 
  // Note: We should be able to safely do this now that all components 
  // should have been destroyed.
  Usul::Components::Manager::instance().clear( &std::cout );
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

    // Success.
    result = 0;
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
