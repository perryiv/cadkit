
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
#include "Usul/IO/Redirect.h"
#include "Usul/File/Remove.h"
#include "Usul/File/Path.h"
#include "Usul/System/DateTime.h"

#include <iostream>


///////////////////////////////////////////////////////////////////////////////
//
//  Set for multi-threaded. This is global because the sooner we set this, 
//  the better. Setting in main() may be too late.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Threads::SetMutexFactory factory ( &Threads::OT::newMutex );


///////////////////////////////////////////////////////////////////////////////
//
//  Redirect stdout and stderr.
//
///////////////////////////////////////////////////////////////////////////////

Usul::IO::Redirect redirect ( Usul::File::fullPath ( "Helios.log" ), true, false );


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  // Set command-line arguments.
  Usul::CommandLine::Arguments::instance().set ( argc, argv );

  // Application name.
  const std::string name ( "Helios" );

  std::cout << name << " started: " << Usul::System::DateTime::now() << std::endl;

  // Shortcut to the application.
  AFW::Core::Application &app ( AFW::Core::Application::instance() );

  // Set name.
  app.name ( "Helios" );

  // Set the name of the redirected output file.
  app.redirect ( redirect.file() );

  // Run the application.
  const bool result ( app.run() );

  // Tell the application to clean up.
  app.cleanup();

  std::cout << name << " stopped: " << Usul::System::DateTime::now() << std::endl;

  // Return the result.
  return ( ( result ) ? 1 : 0 );
}
