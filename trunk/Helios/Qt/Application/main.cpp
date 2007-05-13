
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helios with Qt user-interface.
//
///////////////////////////////////////////////////////////////////////////////

#include "QtGui/QApplication"

#include "Helios/Qt/Core/MainWindow.h"

#include "Threads/OpenThreads/Mutex.h"

#include "Usul/CommandLine/Arguments.h"
#include "Usul/Components/Manager.h"
#include "Usul/Exceptions/Thrower.h"
#include "Usul/File/Temp.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/IO/Redirect.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Trace/Print.h"

#include <fstream>


///////////////////////////////////////////////////////////////////////////////
//
//  Clean up after the application.
//
///////////////////////////////////////////////////////////////////////////////

namespace Program
{
  void clean()
  {
    // Release all plugins.
    Usul::Components::Manager::instance().clear ( &std::cout );

    // Unset the mutex factory.
    Usul::Threads::Mutex::createFunction ( 0x0 );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper object to clean up after the application.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  struct Clean
  {
    ~Clean()
    {
      Usul::Functions::safeCall ( &Program::clean, "2129726739" );
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the application.
//
///////////////////////////////////////////////////////////////////////////////

namespace Program
{
  void run ( int argc, char **argv, int &result )
  {
    // Set for multi-threaded.
    Usul::Threads::Mutex::createFunction ( &Threads::OT::newOpenThreadsMutex );

    // Set command-line arguments.
    Usul::CommandLine::Arguments::instance().set ( argc, argv );

    // Vendor and program name.
    const std::string program ( "Helios" );
    const std::string vendor ( "CadKit Project" );
    const std::string url ( "cadkit.sf.net" );

    // Redirect standard out and error to a file.
    const std::string tempDir ( Usul::File::Temp::directory ( true ) );
    const std::string output ( tempDir + program + ".out" );
    Usul::IO::Redirect redirect ( output, true, true );

    // Send trace output here. Comment this out for stdout.
    const std::string traceFile ( tempDir + program + ".csv" );
    std::ofstream traceStream ( traceFile.c_str() );
    Usul::Trace::Print::init ( &traceStream );

    // We want the above objects to live longer than the application.
    {
      // For cleaning.
      Helper::Clean clean;

      // Declare application.
      QApplication app ( argc, argv );

      // Declare the main window.
      CadKit::Helios::Core::MainWindow mw ( vendor, url, program );

      // Show the main window.
      mw.show();

      // Call quit when the last window closes. (Not sure if this is needed...)
      app.connect ( &app, SIGNAL ( lastWindowClosed() ), &app, SLOT ( quit() ) );

      // Run the application.
      result = app.exec();
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  // Initialize the result.
  int result ( 1 );

  // Run the application.
  Usul::Functions::safeCallV1V2R3 ( &Program::run, argc, argv, result, "4083709520" );

  // Return the result.
  return result;
}
