
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
#include "Usul/Documents/Manager.h"
#include "Usul/Errors/CompositePolicy.h"
#include "Usul/Errors/ThrowingPolicy.h"
#include "Usul/Errors/InvalidParameter.h"
#include "Usul/Errors/PureVirtualCall.h"
#include "Usul/Factory/ObjectFactory.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/File/Path.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Registry/Database.h"
#include "Usul/Threads/Manager.h"
#include "Usul/Trace/Trace.h"
#include "Usul/System/Host.h"

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
    Usul::Jobs::Manager::init( 5, false );
  }


  ~Program()
  {
    // Wait for all jobs to finish
    Usul::Jobs::Manager::instance().wait();

    // Clear any documents.
    this->_clearDocuments();

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

    // Save the registry.
    XmlTree::RegistryIO::write ( Usul::App::Application::instance().configFile ( "settings" ) );

    // Clear the registry.
    Usul::Registry::Database::destroy();

    // Set the mutex factory to null so that we can find late uses of it.
    Usul::Threads::Mutex::createFunction ( 0x0 );

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

    // Successs.
    return 0;
  }


  void _loadPlugins()
  {
    // Console Feedback.
    Usul::Console::Feedback::RefPtr feedback ( new Usul::Console::Feedback );

    // Load the plugins.
    Usul::Components::Loader < XmlTree::Document > loader;
    loader.parse ( Usul::App::Application::instance().configFile ( "registry" ) );
    loader.load ( feedback->queryInterface ( Usul::Interfaces::IUnknown::IID ) );
  }

  //  Clear all the documents.
  void _clearDocuments()
  {
    // Typedefs.
    typedef Usul::Documents::Manager::Documents  Documents;
    typedef Usul::Documents::Document            Document;

    {
      // Make a copy because as the documents are closing, they remove themselves from the document manager.
      Documents copy ( Usul::Documents::Manager::instance().documents() );

      // Tell the remaining open documents that the application is about to close.
      // This allows the document to clean up any circular references.
      for ( Documents::iterator i = copy.begin(); i != copy.end(); ++i )
      {
        // Grab the document in a smart pointer so it doesn't get deleted out from under us.
        Document::RefPtr doc ( *i );
        doc->applicationClosing( 0x0 );
      }
    }

    // Clear documents.
    Usul::Documents::Manager::instance().documents().clear();

    // Reset the document manager.
    Usul::Documents::Manager::reset();
  }

  // Read the registry.
  void _readRegistryFile ()
  {
    // Need to revisit file name and location..
    XmlTree::RegistryIO::read ( Usul::App::Application::instance().configFile ( "settings" ) );
  }

private:
  std::ofstream *_trace;
};

}
}

#endif // __VRV_CORE_PROGRAM_H__
