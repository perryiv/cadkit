
///////////////////////////////////////////////////////////////////////////////
//
//  Example program demonstrating how to use the NURBS classes.
//
///////////////////////////////////////////////////////////////////////////////

#include "GN/Config/OsgConfig.h"

// Cull out tests that declare splines on the stack.
#define NO_STACK_DECLARATIONS

#include "Examples/Common/test.h"

#include "osg/ref_ptr"


template < class ConfigType > struct OsgTest
{
  static void run()
  {
    // Declare spline types.
    typedef GN::Splines::Spline<ConfigType>  Spline;
    typedef GN::Splines::Curve<ConfigType>   Curve;
    typedef GN::Splines::Surface<ConfigType> Surface;

    // Declare spline variables.
    osg::ref_ptr<Spline> spline ( new Spline );
    osg::ref_ptr<Curve> curve ( new Curve );
    osg::ref_ptr<Surface> surface ( new Surface );

    // Run the tests.
    ::Test<ConfigType>::run ( *spline, *curve, *surface );
  }
};


int main ( int, char ** )
{
  // GMTL-based configuration types.
  typedef GN::Config::OsgConfig<long double> OsgConfigld;
  typedef GN::Config::OsgConfig<double>      OsgConfigd;
  typedef GN::Config::OsgConfig<float>       OsgConfigf;

  try
  {
    // Start the XML output.
    std::cout << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n<example>\n";

    // Run the tests on the configurations.
    OsgTest<OsgConfigld>::run();
    OsgTest<OsgConfigd>::run();
    OsgTest<OsgConfigf>::run();

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
