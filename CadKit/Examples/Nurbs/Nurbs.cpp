
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Example program demonstrating how to use the NURBS classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
# pragma warning(disable:4786) // Truncated debug names.
#endif

#include "Standard/SlVec3.h"
#include "Standard/SlVec3IO.h"

#include "Nurbs/Core/NcCurve.h"
#include "Nurbs/Core/NcSurface.h"
#include "Nurbs/Core/NcSplineIO.h"
#include "Nurbs/Core/NcLine.h"
#include "Nurbs/Core/NcCircle.h"

#include <iostream>

using namespace CadKit;


template<NCSDTA, class Vec> void testCurve1 ( const Vec &p1, 
                                              const Vec &p2, 
                                              NcCurve<NCSDCA> &c1, 
                                              NcCurve<NCSDCA> &c2 )
{
  // Print to stdout.
  std::cout << "A01: p1 = " << p1 << std::endl;
  std::cout << "A02: p2 = " << p2 << std::endl;
  std::cout << "A03: c1 = \n" << c1 << std::endl;
  std::cout << "A04: c2 = \n" << c2 << std::endl;
  std::cout << "A05: ( c1 == c2 ) = " << ( c1 == c2 ) << std::endl;

  // Construct the curves.
  NcLine<NCSDCA>::create ( p1, p2, c1 );
  NcLine<NCSDCA>::create ( p1, p2, c2 );
  std::cout << "A06: c1 = \n" << c1 << std::endl;
  std::cout << "A07: c2 = \n" << c2 << std::endl;
  std::cout << "A08: ( c1 == c2 ) = " << ( c1 == c2 ) << std::endl;

  // Make one rational.
  c1.setRational ( true );
  std::cout << "A09: c1 = \n" << c1 << std::endl;
  std::cout << "A10: ( c1 == c2 ) = " << ( c1 == c2 ) << std::endl;
}


template<NCSDTA, class Vec> void testCurve2 ( const Vec &center, 
                                              const C &radius, 
                                              NcCurve<NCSDCA> &c1, 
                                              NcCurve<NCSDCA> &c2 )
{
  // Print to stdout.
  std::cout << "B01: center = " << center << std::endl;
  std::cout << "B02: radius = " << radius << std::endl;
  std::cout << "B03: c1 = \n" << c1 << std::endl;
  std::cout << "B04: c2 = \n" << c2 << std::endl;
  std::cout << "B05: ( c1 == c2 ) = " << ( c1 == c2 ) << std::endl;

  // Construct the curves.
  NcCircle<NCSDCA>::create ( center, radius, c1 );
  NcCircle<NCSDCA>::create ( center, radius, c2 );
  std::cout << "B06: c1 = \n" << c1 << std::endl;
  std::cout << "B07: c2 = \n" << c2 << std::endl;
  std::cout << "B08: ( c1 == c2 ) = " << ( c1 == c2 ) << std::endl;

  // Make one non-rational.
  c1.setRational ( false );
  std::cout << "B09: c1 = \n" << c1 << std::endl;
  std::cout << "B10: ( c1 == c2 ) = " << ( c1 == c2 ) << std::endl;
}


int main ( int argc, char **argv )
{
  // Declare some curves.
  NcCurve<double,double, unsigned int,  unsigned int>   c01;
  NcCurve<double,double, unsigned int,  unsigned int>   c02;
  NcCurve<double,double, unsigned short,short>          c03;
  NcCurve<double,double, unsigned short,short>          c04;
  NcCurve<double,double, unsigned long, long>           c05;
  NcCurve<double,double, unsigned long, long>           c06;
  NcCurve<float, float,  unsigned int,  unsigned int>   c07;
  NcCurve<float, float,  unsigned int,  unsigned int>   c08;
  NcCurve<double,SlVec3d,unsigned long, unsigned short> c09;
  NcCurve<double,SlVec3d,unsigned long, unsigned short> c10;
  NcCurve<float, SlVec3f,unsigned short,unsigned long>  c11;
  NcCurve<float, SlVec3f,unsigned short,unsigned long>  c12;

  // Points used for curve construction.
  SlVec3d p1d ( 1.1,  2.2,  3.3 );
  SlVec3d p2d ( 4.4,  5.5,  6.6 );
  SlVec3f p1f ( 1.1f, 2.2f, 3.3f );
  SlVec3f p2f ( 4.4f, 5.5f, 6.6f );

  // Test the curves.
  std::cout << "Testing curves c01 and c02" << std::endl; testCurve1 ( p1d, p2d, c01, c02 );
  std::cout << "Testing curves c03 and c04" << std::endl; testCurve1 ( p1d, p2d, c03, c04 );
  std::cout << "Testing curves c05 and c06" << std::endl; testCurve1 ( p1d, p2d, c05, c06 );
  std::cout << "Testing curves c07 and c08" << std::endl; testCurve1 ( p1f, p2f, c07, c08 );
  std::cout << "Testing curves c09 and c10" << std::endl; testCurve1 ( p1d, p2d, c09, c10 );
  std::cout << "Testing curves c11 and c12" << std::endl; testCurve1 ( p1f, p2f, c11, c12 );

  // More test for the curves with scalar control point types.
  std::cout << "Testing curves c01 and c02" << std::endl; testCurve2 ( p1d, 2.0,  c01, c02 );
  std::cout << "Testing curves c03 and c04" << std::endl; testCurve2 ( p2d, 3.0,  c03, c04 );
  std::cout << "Testing curves c05 and c06" << std::endl; testCurve2 ( p1d, 4.0,  c05, c06 );
  std::cout << "Testing curves c07 and c08" << std::endl; testCurve2 ( p2f, 5.0f, c07, c08 );

  // Wait for user to press the 'any' key.
  //std::cin.get();

  return 0;
}
