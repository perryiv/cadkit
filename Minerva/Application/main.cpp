
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Minerva program.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Core/Program.h"

#include "Threads/OpenThreads/Mutex.h"
#include "Threads/OpenThreads/Thread.h"

#include "Usul/Functions/SafeCall.h"
#include "Usul/System/DateTime.h"
#include "Usul/Trace/Print.h"

#include <string>


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  // Initialize the result.
  int result ( 1 );

	// Get the current time.
  ::tm time ( Usul::System::DateTime::local() );

  // Convert it to a string.
  const unsigned int size ( 1024 );
  char buffer[size];
  ::memset ( buffer, '\0', size );
  ::strftime ( buffer, size - 1, "%y.%m.%d", &time );

  // Branding.
  const std::string program ( "Minerva" );
	const std::string version ( buffer );  // Version is YY.MM.DD
  const std::string vendor  ( "CadKit" );
  const std::string url     ( "www.minerva-gis.org" );
  const std::string icon    ( "minerva_icon.png" );
  const std::string splash  ( "minerva_splash.png" );
  const std::string plugins ( "Minerva.plugins" );
  const std::string manual  ( "../docs/minerva.html" );

  // Other configurations.
  const unsigned int jobManagerThreadPoolSize ( 4 );

  try
  {
    // Default trace behavior.
    Usul::Trace::Print::printing ( false );

    // Optional trace filters.
    Usul::Trace::Print::addFilter ( true, "Usul::Threads::Pool" );

    // Run the application.
    CadKit::Helios::Core::Program::run (
      argc, argv, 
      &Threads::OT::newOpenThreadsMutex, 
      &Threads::OT::newOpenThreadsThread, 
      program, version, vendor, url, icon, splash, plugins, manual,
      jobManagerThreadPoolSize,
      result );
  }

  // Catch exceptions.
  USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "1192223129" );

  // Return the result.
  return result;
}
