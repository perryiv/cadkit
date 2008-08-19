
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helios with Qt user-interface.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Core/Program.h"

#include "Threads/OpenThreads/Thread.h"

#include "Usul/Functions/SafeCall.h"
#include "Usul/System/DateTime.h"
#include "Usul/System/Environment.h"
#include "Usul/Trace/Print.h"
#include "Usul/Convert/Convert.h"

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

  // Branding.
  const std::string program ( "Helios" );
  const std::string version ( Usul::System::DateTime::version ( __DATE__ ) );
  const std::string vendor  ( "CadKit" );
  const std::string url     ( "cadkit.sf.net" );
  const std::string icon    ( "helios_sun.png" );
  const std::string splash  ( "splash_screen.png" );
  const std::string plugins ( "Helios.plugins" );
  const std::string manual  ( "" );


  // Other configurations.
  // check for the existance of the thread pool size environment variable
  const std::string numThreads ( Usul::System::Environment::get( "HELIOS_JOB_MANAGER_THREAD_POOL_SIZE" ) );
  const unsigned int jobManagerThreadPoolSize ( ( numThreads.empty() ? 4 : Usul::Convert::Type< std::string, unsigned int >::convert( numThreads ) ) );

  try
  {
    // Default trace behavior.
    Usul::Trace::Print::printing ( false );

    // Optional trace filters.
    Usul::Trace::Print::addFilter ( true, "Usul::Threads::Pool" );

    // Run the application.
    CadKit::Helios::Core::Program::run (
      argc, argv, 
      &Threads::OT::newOpenThreadsThread, 
      program, version, vendor, url, icon, splash, plugins, manual,
      jobManagerThreadPoolSize,
      result );
  }

  // Catch exceptions.
  USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "1687235915" );

  // Return the result.
  return result;
}
