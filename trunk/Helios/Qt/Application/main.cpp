
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

#include "QtGui/QApplication"

#include "Helios/Qt/Core/MainWindow.h"

#include "Threads/OpenThreads/Mutex.h"
#include "Threads/OpenThreads/Thread.h"

#include "Usul/App/Application.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Components/Manager.h"
#include "Usul/Exceptions/Thrower.h"
#include "Usul/File/Make.h"
#include "Usul/File/Temp.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/IO/Redirect.h"
#include "Usul/Threads/Manager.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Named.h"
#include "Usul/Threads/Manager.h"
#include "Usul/Trace/Print.h"

#include <fstream>
#include <sstream>


///////////////////////////////////////////////////////////////////////////////
//
//  Need this to live longer than anything.
//
///////////////////////////////////////////////////////////////////////////////

namespace Program { Usul::IO::Redirect *redirect ( 0x0 ); }


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

    // Destroy the thread manager.
    Usul::Threads::Manager::destroy();

    // Clear the map of named threads.
    Usul::Threads::Named::clear();
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
//  Format data string.
//
///////////////////////////////////////////////////////////////////////////////

std::string formatBuildDate ( const std::string &date )
{
  if ( 11 != date.size() )
    return date;

  const std::string year  ( date.begin() + 7, date.end() );
  const std::string month ( date.begin() + 0, date.begin() + 3 );
  const std::string day   ( date.begin() + 4, date.begin() + 6 );
  std::ostringstream out;
  out << day << '-' << month << '-' << year;
  return out.str();
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
    // Set for multi-threaded using OpenThreads.
    Usul::Threads::Mutex::createFunction       ( &Threads::OT::newOpenThreadsMutex );
    Usul::Threads::Manager::instance().factory ( &Threads::OT::newOpenThreadsThread );

    // Set command-line arguments.
    Usul::CommandLine::Arguments::instance().set ( argc, argv );

    // Vendor and program name.
    const std::string program ( "Helios" );
    const std::string vendor ( "CadKit" );
    const std::string url ( "cadkit.sf.net" );

    // Get temporary directory nd make sure it exists.
    const std::string tempDir ( Usul::File::Temp::directory ( true ) + "/" + program + "/" );
    Usul::File::make ( tempDir );

    // Redirect standard out and error to a file. This is a leak by design.
    const std::string output ( tempDir + program + ".out" );
    Program::redirect = new Usul::IO::Redirect ( output, true, true );

    // Print welcome message.
    std::cout << "Welcome to " << program << std::endl;
    std::cout << "Built on " << ::formatBuildDate ( __DATE__ ) << " at " << __TIME__ << std::endl;

    // Send trace output here. Comment this out for stdout.
    const std::string traceFile ( tempDir + program + ".csv" );
    std::ofstream traceStream ( traceFile.c_str() );
    Usul::Trace::Print::init ( &traceStream );

    // Initialize singleton of named threads.
    Usul::Threads::Named::instance().set ( Usul::Threads::Names::MAIN );

    // We want the above objects to live longer than the application.
    {
      // For cleaning.
      Helper::Clean clean;

      // Declare application.
      QApplication app ( argc, argv );

      // Set the splash screen.
      Usul::App::Application::instance().splashImage( "splash_screen.jpg" );

      // Declare the main window.
      CadKit::Helios::Core::MainWindow mw ( vendor, url, program );

      // Call quit when the last window closes. (Not sure if this is needed...)
      app.connect ( &app, SIGNAL ( lastWindowClosed() ), &app, SLOT ( quit() ) );

      // Show the main window.
      mw.show();

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
