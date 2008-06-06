
///////////////////////////////////////////////////////////////////////////////
//
//  Example program demonstrating how to use the Nurbs Foundation Library.
//
///////////////////////////////////////////////////////////////////////////////

#include "NFL/Core/Splines/Curve.h"
#include "NFL/Core/Create/Line.h"
#include "NFL/Core/Write/XML.h"

#include <iostream>
#include <stdexcept>


int main ( int, char ** )
{
  try
  {
    // Declare curve.
    typedef NFL::Core::Splines::Curve Curve;
    Curve curve;
    curve.name ( "Example01" );

    // Make it a line.
    const Curve::Vec3 point0 ( 0, 1, 2 );
    const Curve::Vec3 point1 ( 1, 2, 3 );
    NFL::Core::Create::line ( curve, point0, point1 );

    // Print the spline.
    NFL::Core::Write::xml ( curve, std::cout );
  }

  catch ( std::exception &e )
  {
    std::cout << "Exception caught: " << e.what() << std::endl;
  }

  //std::cin.get();
  return 1;
}
