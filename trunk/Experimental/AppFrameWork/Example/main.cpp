
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Gets application framework going.
//
///////////////////////////////////////////////////////////////////////////////

#include "AppFrameWork/Core/Application.h"

#include "Threads/OpenThreads/Mutex.h"

#include "Usul/Threads/Mutex.h"
#include "Usul/CommandLine/Arguments.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Set for multi-threaded. This is global because the sooner we set this, 
//  the better. Setting in main() may be too late.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Threads::SetMutexFactory factory ( &Threads::OT::newMutex );


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  // Set command-line arguments.
  Usul::CommandLine::Arguments::instance().set ( argc, argv );

  // Shortcut to the application.
  AFW::Core::Application &app ( AFW::Core::Application::instance() );

  // Set name.
  app.name ( "Helios" );

  // Run the application.
  const bool result ( app.run() );

  // Tell the application to clean up.
  app.cleanup();

  // Return the result.
  return ( ( result ) ? 1 : 0 );
}
