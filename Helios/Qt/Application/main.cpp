
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
#include "Usul/Strings/Format.h"
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

    // Branding.
    const std::string program ( "Helios" );
    const std::string vendor ( "CadKit" );
    const std::string url ( "cadkit.sf.net" );
    const std::string icon ( "helios_sun.png" );
    Usul::App::Application::instance().splashImage ( "splash_screen.jpg" );

    // Get temporary directory nd make sure it exists.
    const std::string tempDir ( Usul::File::Temp::directory ( true ) + "/" + program + "/" );
    Usul::File::make ( tempDir );

    // Redirect standard out and error to a file. This is a leak by design.
    const std::string output ( tempDir + program + ".out" );
    Program::redirect = new Usul::IO::Redirect ( output, true, true );

    // Print welcome message.
    std::cout << "Welcome to " << program << std::endl;
    std::cout << "Built on " << Usul::Strings::formatDate ( __DATE__ ) << " at " << __TIME__ << std::endl;

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

      // Call quit when the last window closes. (Not sure if this is needed...)
      app.connect ( &app, SIGNAL ( lastWindowClosed() ), &app, SLOT ( quit() ) );

      // We want the application to live longer than the main window.
      {
        // Declare the main window.
        CadKit::Helios::Core::MainWindow mw ( vendor, url, program, icon, output, true );

        // Load the plugins.
        mw.addPluginFile ( mw.defautPluginFile() );
        mw.loadPlugins();
        mw.printPlugins();

        // Hide the splash screen and show the main window.
        mw.hideSplashScreen();
        mw.show();

        // Force an update of the text window.
        mw.updateTextWindow();

        // Run the application.
        result = app.exec();
      }
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
