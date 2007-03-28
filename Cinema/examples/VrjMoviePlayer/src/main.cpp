//
//
// Disable useless warnings
#ifdef WIN32
	#pragma warning( disable : 4290 4251 4267 4275 4244 4312)
#endif

// VRJ includes
#include "MyApp.h"

// Juggler include 
#include <vrj/Kernel/Kernel.h>

int main( int argc, char* argv[] )
{
	vrj::Kernel* kernel		= vrj::Kernel::instance();
	MyApp* application	= new MyApp(kernel, argc, argv);	
	
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


