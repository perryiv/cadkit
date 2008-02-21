
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

#include "Threads/OpenThreads/Mutex.h"
#include "Threads/OpenThreads/Thread.h"

#include "Usul/Functions/SafeCall.h"
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

  // Branding.
  const std::string program ( "Helios" );
  const std::string version ( "" );
  const std::string vendor  ( "CadKit" );
  const std::string url     ( "cadkit.sf.net" );
  const std::string icon    ( "helios_sun.png" );
  const std::string splash  ( "splash_screen.png" );
  const std::string plugins ( "../configs/Helios.plugins" );
  const std::string manual  ( "" );

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
  USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "1687235915" );

  // Return the result.
  return result;
}
