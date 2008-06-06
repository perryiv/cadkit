
///////////////////////////////////////////////////////////////////////////////
//
//  Example program demonstrating how to use the NURBS classes.
//
///////////////////////////////////////////////////////////////////////////////

#include "GN/Config/GmtlConfig.h"

#include "Examples/Common/test.h"


int main ( int, char ** )
{
  // GMTL-based configuration types.
  typedef GN::Config::GmtlConfig<long double> GmtlConfigld;
  typedef GN::Config::GmtlConfig<double>      GmtlConfigd;
  typedef GN::Config::GmtlConfig<float>       GmtlConfigf;

  try
  {
    // Start the XML output.
    std::cout << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n<example>\n";

    // Run the tests on the configurations.
    Test<GmtlConfigld>::run();
    Test<GmtlConfigd>::run();
    Test<GmtlConfigf>::run();

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
