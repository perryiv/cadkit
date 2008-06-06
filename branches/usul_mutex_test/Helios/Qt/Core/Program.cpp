
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helios program class. Handles startup, shutdown, and branding.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Core/Program.h"
#include "Helios/Qt/Core/Application.h"
#include "Helios/Qt/Core/MainWindow.h"

#include "Usul/App/Application.h"
#include "Usul/Convert/Convert.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Components/Manager.h"
#include "Usul/Errors/AssertPolicy.h"
#include "Usul/Errors/CompositePolicy.h"
#include "Usul/Errors/ThrowingPolicy.h"
#include "Usul/Errors/InvalidParameter.h"
#include "Usul/Errors/PureVirtualCall.h"
#include "Usul/Exceptions/Thrower.h"
#include "Usul/File/Contents.h"
#include "Usul/File/Make.h"
#include "Usul/File/Path.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/IO/Redirect.h"
#include "Usul/IO/StreamSink.h"
#include "Usul/Registry/Database.h"
#include "Usul/Strings/Case.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/Environment.h"
#include "Usul/Threads/Manager.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Named.h"
#include "Usul/Trace/Print.h"
#include "Usul/User/Directory.h"

#ifdef __GNUC__
# include "Usul/Errors/Signals.h"
#endif

#include <fstream>
#include <sstream>

using namespace CadKit::Helios::Core;


///////////////////////////////////////////////////////////////////////////////
//
//  Need this to live longer than anything.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  // For stdout and stderr re-direction.
  typedef Usul::IO::StreamSink StreamSink;
  StreamSink::RefPtr sink ( static_cast<StreamSink *> ( 0x0 ) );

  // Trapping errors.
  typedef Usul::Errors::ThrowingPolicy < std::runtime_error > ThrowingPolicy;
  typedef Usul::Errors::CompositePolicy < Usul::Errors::AssertPolicy, ThrowingPolicy > CompositePolicy;
  Usul::Errors::PureVirtualCall < CompositePolicy > pureCallAction;
  Usul::Errors::InvalidParameter < CompositePolicy > invalidParameterAction;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean up after the application.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void cleanup()
  {
    // Release all plugins.
    Usul::Components::Manager::instance().clear ( &std::cout );

    // Destroy the thread manager.
    Usul::Threads::Manager::destroy();

    // Clear the map of named threads.
    Usul::Threads::Named::clear();

    // Clear the registry.
    Usul::Registry::Database::destroy();
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
      Usul::Functions::safeCall ( &Helper::cleanup, "2129726739" );
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper object to clean up after the application.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  std::string getPluginFileName ( const std::string &persistantDir, const std::string &userFile, const std::string &defaultFile )
  {
    // Try user's file as is.
    if ( true == Usul::Predicates::FileExists::test ( userFile ) )
    {
      return userFile;
    }

    // Try pre-pending persistant directory.
    std::string pluginFile ( persistantDir + userFile );
    if ( true == Usul::Predicates::FileExists::test ( pluginFile ) )
    {
      return pluginFile;
    }

    // Try same directory as default file.
    pluginFile = Usul::File::directory ( defaultFile, true ) + userFile;
    if ( true == Usul::Predicates::FileExists::test ( pluginFile ) )
    {
      return pluginFile;
    }

    // Otherwise, return the default.
    return defaultFile;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the application.
//
///////////////////////////////////////////////////////////////////////////////

void Program::run ( int argc, char **argv,
                    Usul::Threads::Manager::FactoryFunction *threadFactory,
                    const std::string &program, 
                    const std::string &version, 
                    const std::string &vendor, 
                    const std::string &url, 
                    const std::string &icon, 
                    const std::string &splash,
                    const std::string &plugins,
                    const std::string &manual,
                    unsigned int poolSize,
                    int &result )
{
  #ifdef __GNUC__
  // Register the signal handlers.
  Usul::Errors::registerSignalHandlers ( argv[0] );
  #endif

  // Set thread factories.
  Usul::Threads::Manager::instance().factory ( threadFactory );

  // Set command-line arguments.
  Usul::CommandLine::Arguments::instance().set ( argc, argv );

  // Make job manager.
  Usul::Jobs::Manager::init ( poolSize, true );

  // Set splash image.
  Usul::App::Application::instance().splashImage ( splash );
  
  // Set information.
  Usul::App::Application::instance().vendor ( vendor );
  Usul::App::Application::instance().program ( program );

  // Get persistant directory and make sure it exists.
  const std::string persistantDir ( Usul::User::Directory::vendor ( vendor, true ) + program + "/" );
  Usul::File::make ( persistantDir );

  // Redirect standard out and error to a file.
  const std::string output ( persistantDir + "stdout.txt" );
  Usul::File::remove ( output, false );
  Helper::sink = Helper::StreamSink::RefPtr ( new Usul::IO::StreamSink ( output ) );

  // Print welcome message.
  std::ostringstream out;
  out << "Welcome to " << program << ' ' << version << std::endl;
  out << "Built on " << Usul::Strings::formatDate ( __DATE__ ) << " at " << __TIME__ << std::endl;

  // Send trace output here.
  const std::string traceFile ( persistantDir + "trace.csv" );
  std::ofstream traceStream ( traceFile.c_str() );
  Usul::Trace::Print::stream ( &traceStream );

  // Set job manager's log file.
  const std::string logFile ( persistantDir + "jobs.csv" );
  Usul::Jobs::Manager::instance().log ( new Usul::File::Log ( logFile, false ) );

  // Initialize singleton of named threads.
  Usul::Threads::Named::instance().set ( Usul::Threads::Names::MAIN );

  // We want the above objects to live longer than the application.
  {
    // For cleaning.
    Helper::Clean clean;

    // Declare application.
    CadKit::Helios::Core::Application app ( argc, argv );

    // Call quit when the last window closes. (Not sure if this is needed...)
    app.connect ( &app, SIGNAL ( lastWindowClosed() ), &app, SLOT ( quit() ) );

    // We want the application to live longer than the main window.
    {
      // Declare the main window.
      CadKit::Helios::Core::MainWindow mw ( vendor, url, program, manual, icon, output, out.str(), true );

      // Main window needs to live longer than the stream listener.
      {
        // Add and remove main window as listener to stream sink.
        Helper::StreamSink::ScopedListener listener ( *Helper::sink, &mw );

        // Now we can print this to stdout. Note: this isn't fool-proof. 
        // If some other code printed to stdout or stderr before the sink's 
        // listener was set, it will go to the file but not the text window.
        std::cout << out.str() << std::flush;

        // Print additional info if we are supposed to.
        const std::string printDebugEnvName ( Usul::Strings::upperCase ( program ) + "_PRINT_DEBUG_INFO" );
        const std::string printDebugEnvValue ( Usul::System::Environment::get ( printDebugEnvName ) );
        if ( true == Usul::Convert::Type<std::string,bool>::convert ( printDebugEnvValue ) ) 
        {
          std::cout << "Text output: " << output << std::endl;
          std::cout << "Debug trace: " << traceFile << std::endl;
          std::cout << "Settings file: " << mw.settingsFileName() << std::flush;
        }

        // Load the plugins.
        mw.addPluginFile ( Helper::getPluginFileName ( persistantDir, plugins, mw.defautPluginFile() ) );
        mw.loadPlugins();
        mw.printPlugins();
        mw.initPlugins();

        // Restore the main window's state.
        mw.restoreDockWindows();

        // Hide the splash screen and show the main window.
        mw.hideSplashScreen();
        mw.show();

        // Parse the command-line.
        mw.parseCommandLine ( argc, argv );

        // Run the application.
        result = app.exec();
      }
    }
  }
}
