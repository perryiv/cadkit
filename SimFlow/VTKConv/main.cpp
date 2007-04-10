
#include "VTKConv.h"

using namespace SimFlow::VTKConv;


void printUsageInformation()
{
	std::cout	<< "Usage: VTKConv.exe --source input_directory --destination output_directory " 
				<< std::endl;
}

int main( int argc, char* argv[] )
{
	std::string source;
	std::string destination;

	if( argc < 2 )
	{
		std::cerr << "ERROR: Insufficient arguments " << std::endl;
		printUsageInformation();		
		std::exit(0);
	}

	for( int i=1; i < argc; ++i )
	{		
		if( std::string( argv[i] ).compare( "--source" ) == 0 )
		{
			source = argv[i + 1];
		}

		if( std::string( argv[i] ).compare( "--destination"  ) == 0 )
		{
			destination = argv[i + 1];
		}

		if( std::string( argv[i] ).compare( "--help"  ) == 0 )
		{
			printUsageInformation();
			std::exit( 0 );
		}	
	}

	// If no argument given for destination source becomes destination as well. 
	if( destination.empty() ) destination = source;

	convVTKAsciiToBinary( source, destination );
}
