
///////////////////////////////////////////////////////////////////////////////
//
//  Example program demonstrating how to use the NURBS classes.
//
///////////////////////////////////////////////////////////////////////////////

#include "GN/Config/UsulConfig.h"

#include "Examples/Common/test.h"


int main ( int, char ** )
{
  // Usul-based configuration types.
  typedef GN::Config::UsulConfig<long double> UsulConfigld;
  typedef GN::Config::UsulConfig<double>      UsulConfigd;
  typedef GN::Config::UsulConfig<float>       UsulConfigf;

  try
  {
    // Start the XML output.
    std::cout << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n<example>\n";

    // Run the tests on the configurations.
    Test<UsulConfigld>::run();
    Test<UsulConfigd>::run();
    Test<UsulConfigf>::run();

    // End the XML output.
    std::cout << "</example>\n";
  }

  catch ( std::exception &e )
  {
    std::cout << "Exception caught: " << e.what() << std::endl;
  }

  //std::cin.get();
  return 1;
}
