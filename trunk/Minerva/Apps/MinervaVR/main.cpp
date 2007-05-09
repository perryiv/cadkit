
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "MinervaVR.h"

#include "vrj/Kernel/Kernel.h"

#include "Usul/CommandLine/Arguments.h"
#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Threads/SetThreadFactory.h"
#include "Usul/Trace/Print.h"

#include "Threads/OpenThreads/Mutex.h"
#include "Threads/OpenThreads/Thread.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Set for multi-threaded. This is global because the sooner we set this, 
//  the better. Setting in main() may be too late.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Threads::SetMutexFactory  mutexFactory  ( &Threads::OT::newOpenThreadsMutex );
Usul::Threads::SetThreadFactory threadFactory ( &Threads::OT::newOpenThreadsThread );

int main( int argc, char* argv[] )
{
  // Set the trace stream.
#ifdef _DEBUG
  Usul::Trace::Print::init ( &std::cerr );
#endif

  // Set the command line arguments.
  Usul::CommandLine::Arguments::instance().set ( argc, argv );

  // Add some plugin extensions.
#ifdef _DEBUG
  Usul::Components::Manager::instance().addPluginExtension ( "plugd" );
#else
  Usul::Components::Manager::instance().addPluginExtension ( "plug" );
#endif
 
  // Get the directory where the application lives.
  const std::string dir ( Usul::CommandLine::Arguments::instance().directory() );
   
  Usul::Components::Manager::instance().addDirectory( dir );
   
  // Load all plugins.
  Usul::Components::Manager::instance().load ( Usul::Interfaces::IPlugin::IID );

  std::cerr << "Loading plugins...." << std::endl;

  // Feedback about plugins.
  const Usul::Components::Manager::Strings names ( Usul::Components::Manager::instance().names() );
  std::cout << "Found " << names.size() << " plugins: ";
  std::copy ( names.begin(), names.end(), std::ostream_iterator<std::string> ( std::cout, "; " ) );
  std::cout << std::endl;

  vrj::Kernel* kernel		= vrj::Kernel::instance();
  MinervaVR* application	= new MinervaVR(kernel, argc, argv);	
  
  // Number of argumets should be more than 1 :: Needs atleast one config file ::
  if( argc < 2 )
  {
    std::cerr << " Test: " << std::endl;
    //printUsageInformation();
    std::exit(1);
  }

  for( int i = 1; i < argc; ++i )
  {
    kernel->loadConfigFile( argv[i] );
  }

  kernel->start();
  kernel->setApplication( application );
  kernel->waitForKernelStop();

  return 0;
}


