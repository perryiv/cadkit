//
//
// Disable useless warnings
#ifdef WIN32
	#pragma warning( disable : 4290 4251 4267 4275 4244 4312)
#endif

// VRJ includes
#include "MinervaVR.h"

// Juggler include 
#include <vrj/Kernel/Kernel.h>

#include "Usul/CommandLine/Arguments.h"
#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IPlugin.h"

#include "Threads/OpenThreads/Mutex.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Set for multi-threaded. This is global because the sooner we set this, 
//  the better. Setting in main() may be too late.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Threads::SetMutexFactory factory ( &Threads::OT::newOpenThreadsMutex );

int main( int argc, char* argv[] )
{
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


