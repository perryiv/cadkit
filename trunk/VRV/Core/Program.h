
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_CORE_PROGRAM_H__
#define __VRV_CORE_PROGRAM_H__

#include "VRV/Core/Exceptions.h"

#include "Threads/OpenThreads/Thread.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/App/Application.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Components/Loader.h"
#include "Usul/Components/Manager.h"
#include "Usul/Console/Feedback.h"
#include "Usul/DLL/LibraryPool.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Errors/CompositePolicy.h"
#include "Usul/Errors/ThrowingPolicy.h"
#include "Usul/Errors/InvalidParameter.h"
#include "Usul/Errors/PureVirtualCall.h"
#include "Usul/Exceptions/Exception.h"
#include "Usul/Factory/ObjectFactory.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/File/Path.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/Registry/Database.h"
#include "Usul/Threads/Manager.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Signals/Actions.h"
#include "Usul/Signals/Handler.h"
#include "Usul/System/Host.h"
#include "Usul/User/Directory.h"

#ifdef __GNUC__
# include "Usul/Errors/Signals.h"
#endif

#include "XmlTree/Document.h"
#include "XmlTree/RegistryIO.h"

#include <fstream>

namespace VRV {
namespace Core {


///////////////////////////////////////////////////////////////////////////////
//
//  Need this to live longer than anything.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  // Trapping errors.
  typedef Usul::Errors::ThrowingPolicy < std::runtime_error > ThrowingPolicy;
  typedef Usul::Errors::CompositePolicy < Usul::Errors::AssertPolicy, ThrowingPolicy > CompositePolicy;
  Usul::Errors::PureVirtualCall < CompositePolicy > pureCallAction;
  Usul::Errors::InvalidParameter < CompositePolicy > invalidParameterAction;
 
  // Trapping signals.
  typedef Usul::Signals::Actions::PrintMessage PrintMessage;
  typedef Usul::Signals::Actions::PrintStackTrace PrintStackTrace;
  typedef Usul::Signals::Actions::Pair<PrintMessage,PrintStackTrace> PrintPair;
  typedef Usul::Signals::Actions::Abort Throw;
  typedef Usul::Signals::Actions::Pair<PrintPair,Throw> SignalAction;
  USUL_DECLARE_SIGNAL_HANDLER ( SignalAction, SegmentationViolation );
  USUL_DECLARE_SIGNAL_HANDLER ( SignalAction, AbortSignal );
  USUL_DECLARE_SIGNAL_HANDLER ( SignalAction, FloatingPointException  );
}


template < class Application >
class Program
{
public:
  Program( const std::string& program = "VRV" ) : 
    _trace ( 0x0 )
  {
    std::string filename ( Usul::File::fullPath ( Usul::System::Host::name() + "_trace.csv" ) );
    _trace = new std::ofstream ( filename.c_str() );

    // Set the vendor and program.
    Usul::App::Application::instance().vendor ( "CadKit" );
    Usul::App::Application::instance().program ( program );

    // Set the Thread factory.
    Usul::Threads::Manager::instance().factory ( &Threads::OT::newOpenThreadsThread );

    // Set the trace output file.
    Usul::Trace::Print::stream ( _trace );

    // Default trace behavior.
    Usul::Trace::Print::printing ( false );

    // Use 5 threads.
    Usul::Jobs::Manager::init( "Job Manager Singleton", 5, false );
  }


  ~Program()
  {
    // Wait for all jobs to finish
    Usul::Jobs::Manager::instance().wait();

    // Delete the job manager.
    Usul::Jobs::Manager::destroy();

    // There should not be any threads running.
    Usul::Threads::Manager::instance().wait();

    // Delete all thread objects.
    Usul::Threads::Manager::instance().purge();

    // Clear the object factory.
    Usul::Factory::ObjectFactory::instance().clear();

    // Release all libraries that loaded during component creation. 
    // Note: We should be able to safely do this now that all components 
    // should have been destroyed.
    Usul::Components::Manager::instance().clear ( &std::cout );

    // Clear the library pool.
    Usul::DLL::LibraryPool::instance().clear( &std::cout );

    // Clear the registry.
    Usul::Registry::Database::destroy();

    // Clean up the thread manager.
    Usul::Threads::Manager::destroy();

    // Delete ostream.
    if ( 0x0 != _trace )
      delete _trace;
  }


  int run ( int argc, char ** argv )
  {
    // Initialize the return value.
    int result ( 1 );

    // Be safe...
    try
    {
      // Isolate application run inside this function.
      result = this->_run ( argc, argv );
    }

    // Catch local exceptions.
    catch ( const VRV::Core::Exceptions::UserInput &e )
    {
      std::cout << "Input Error:\n" << e.what() << std::endl;
      Application::usage ( argv[0], std::cout );
    }

    // Catch standard exceptions.
    catch ( const std::exception &e )
    {
      std::cout << "Exception caught:\n" << e.what() << std::endl;
    }

    // Catch all other exceptions.
    catch ( ... )
    {
      std::cout << "Unknown exception caught" << std::endl;
    }

    return result;
  }

protected:

  int _run ( int argc, char ** argv )
  {
    // Set the arguments.
    Usul::CommandLine::Arguments::instance().set ( argc, argv );

    // Read the registry.
    Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Program::_readRegistryFile ), "3499560042" );

    // Load the plugins. Not a big deal if it throws.
    Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Program::_loadPlugins ), "1701127221" );

    // Print what we found.
    Usul::Components::Manager::instance().print ( std::cout );

    // Construct an application.
    Application app;

    // Run the application.
    app.run();

    // Save the registry.
    XmlTree::RegistryIO::write ( this->_registryFileName(), Usul::Registry::Database::instance() );

    // Clean up.
    app.cleanup();

    // Successs.
    return 0;
  }


  void _loadPlugins()
  {
    // Console Feedback.
    Usul::Console::Feedback::RefPtr feedback ( new Usul::Console::Feedback );

    // Load the plugins.
    Usul::Components::Loader < XmlTree::Document > loader;
    loader.parse ( this->_pluginFileName() );
    loader.load ( feedback->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
  }


  // Read the registry.
  void _readRegistryFile ()
  {
    // Need to revisit file name and location.
    XmlTree::RegistryIO::read ( this->_registryFileName(), Usul::Registry::Database::instance() );
  }

  // Return the registry file.
  std::string _registryFileName() const
  {
    const std::string vendor  ( Usul::App::Application::instance().vendor()  );
    const std::string program ( Usul::App::Application::instance().program() );

    return Usul::User::Directory::program ( vendor, program ) + "/settings.xml";
  }

  // Return the plugin file name.
  std::string _pluginFileName() const
  {
    std::string name ( "registry" );
    {
      std::string filename ( Usul::CommandLine::Arguments::instance().directory() + "/" + name + ".xml" );
      if( Usul::Predicates::FileExists::test ( filename ) )
        return filename;
    }

    const std::string vendor  ( Usul::App::Application::instance().vendor()  );
    const std::string program ( Usul::App::Application::instance().program() );
  
    return Usul::User::Directory::program ( vendor, program ) + "/" + name + ".xml";

  }

private:
  std::ofstream *_trace;
};

}
}

#endif // __VRV_CORE_PROGRAM_H__
