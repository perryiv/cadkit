
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

#include "Threads/OpenThreads/Thread.h"

#include "Usul/CommandLine/Arguments.h"
#include "Usul/Components/Loader.h"
#include "Usul/Components/Manager.h"
#include "Usul/Console/Feedback.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Threads/Manager.h"
#include "Usul/Trace/Trace.h"
#include "Usul/System/Host.h"

#include "XmlTree/Document.h"

namespace VRV {
namespace Core {

template < class Application >
class Program
{
public:
  Program() : 
    _trace ( 0x0 )
  {
    std::string filename ( Usul::System::Host::name() + "_trace.csv" );
    _trace = new std::ofstream ( filename.c_str() );

    // Set the Thread factory.
    Usul::Threads::Manager::instance().factory ( &Threads::OT::newOpenThreadsThread );

    // Set the trace output file.
    Usul::Trace::Print::init ( _trace );

    // Use 2 threads.
    Usul::Jobs::Manager::instance().poolResize ( 2 );
  }

  ~Program()
  {
    // Set the mutex factory to null so that we can find late uses of it.
    Usul::Threads::Mutex::createFunction ( 0x0 );

    // Clean up the thread manager.
    Usul::Threads::Manager::destroy();

    // Release all libraries that loaded during component creation. 
    // Note: We should be able to safely do this now that all components 
    // should have been destroyed.
    Usul::Components::Manager::instance().clear( &std::cout );

    // Delete ostream.
    if ( 0x0 != _trace )
      delete _trace;
  }

  int run ( int argc, char ** argv )
  {
    // Set the arguments.
    Usul::CommandLine::Arguments::instance().set ( argc, argv );

    // Console Feedback.
    Usul::Console::Feedback::RefPtr feedback ( new Usul::Console::Feedback );

    // Load the plugins.
    Usul::Components::Loader < XmlTree::Document > loader;
    loader.parse ( CV::Config::filename ( "registry" ) );
    loader.load ( feedback->queryInterface ( Usul::Interfaces::IUnknown::IID ) );

    // Print what we found.
    Usul::Components::Manager::instance().print ( std::cout );

    // Put the arguments into a container.
    typename Application::Args args;
    std::copy ( argv + 1, argv + argc, std::back_inserter ( args ) );

    // Construct an application.
    Application app ( args );

    // Run the application.
    app.run();

    // Wait for all jobs to finish
    Usul::Jobs::Manager::instance().wait();

    // The job manager has a thread-pool.
    Usul::Jobs::Manager::destroy();

    // There should not be any threads running.
    Usul::Threads::Manager::instance().wait();

    // Successs.
    return 0;
  }

private:
  std::ofstream *_trace;
};

}
}

#endif // __VRV_CORE_PROGRAM_H__
