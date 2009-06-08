
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Modflow viewer program.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Core/Program.h"

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

  // Branding.
  const std::string program ( "Nimue" );
  const std::string version ( Usul::System::DateTime::version ( __DATE__ ) + " (beta)" );
  const std::string vendor  ( "Nimue" );
  const std::string url     ( "" );
  const std::string icon    ( "nimue_icon.png" );
  const std::string splash  ( "nimue_splash.png" );
  const std::string plugins ( "Nimue.plugins" );
  const std::string manual  ( "NimueManual.pdf" );

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
      &Threads::OT::newOpenThreadsThread, 
      program, version, vendor, url, icon, splash, plugins, manual,
      jobManagerThreadPoolSize,
      result );
  }

  // Catch exceptions.
  USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "4260630360" );

  // Return the result.
  return result;
}
