
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
#include "Usul/IO/StreamSink.h"
#include "Usul/Strings/Format.h"
#include "Usul/Threads/Manager.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Named.h"
#include "Usul/Trace/Print.h"

#include <fstream>
#include <sstream>


///////////////////////////////////////////////////////////////////////////////
//
//  Need this to live longer than anything.
//
///////////////////////////////////////////////////////////////////////////////

namespace Program
{
  typedef Usul::IO::StreamSink StreamSink;
  StreamSink::RefPtr sink ( static_cast<StreamSink *> ( 0x0 ) );
}


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

    // Unset the mutex factory to null so that we can find late uses of it.
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

    // Get temporary directory and make sure it exists.
    const std::string tempDir ( Usul::File::Temp::directory ( true ) + "/" + program + "/" );
    Usul::File::make ( tempDir );

    // Redirect standard out and error to a file.
    const std::string output ( tempDir + program + ".out" );
    Usul::File::remove ( output, false );
    Program::sink = Program::StreamSink::RefPtr ( new Usul::IO::StreamSink ( output ) );

    // Print welcome message.
    std::ostringstream out;
    out << "Welcome to " << program << std::endl;
    out << "Built on " << Usul::Strings::formatDate ( __DATE__ ) << " at " << __TIME__ << std::endl;

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

        // Main window needs to live longer than the stream listener.
        {
          // Add and remove main window as listener to stream sink.
          StreamSink::ScopedListener listener ( *sink, &mw );

          // Now we can print this to stdout. Note: this isn't fool-proof. 
          // If some other code printed to stdout or stderr before the sink's 
          // listener was set, it will go to the file but not the text window.
          std::cout << out.str() << std::flush;

          // Load the plugins.
          mw.addPluginFile ( mw.defautPluginFile() );
          mw.loadPlugins();
          mw.printPlugins();

          // Hide the splash screen and show the main window.
          mw.hideSplashScreen();
          mw.show();

          // Run the application.
          result = app.exec();
        }
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
