
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

#define _CADKIT_NURBS_CORE_CHECK_FUNCTION_ARGUMENTS
#define _CADKIT_NURBS_CORE_CHECK_MEMORY_ALLOCATION
#define _CADKIT_NURBS_CORE_CHECK_FUNCTION_RESULTS
#define _CADKIT_SL_VEC3_DECLARE_SCALAR_MEMBERS

#include "Standard/SlVec3.h"
#include "Standard/SlVec3IO.h"
#include "Standard/SlArrayPtr.h"

#include "Nurbs/Core/NcCurve.h"
#include "Nurbs/Core/NcSurface.h"
#include "Nurbs/Core/NcSplineIO.h"
#include "Nurbs/Core/NcLine.h"
#include "Nurbs/Core/NcCircle.h"
#include "Nurbs/Core/NcFindSpan.h"
#include "Nurbs/Core/NcBasisFunctions.h"
#include "Nurbs/Core/NcEvaluate.h"

#include <iostream>

using namespace CadKit;


template<NCSDTA> void testEvaluation ( NcCurve<NCSDCA> &curve )
{
  // Typedefs.
  typedef NcCurve<NCSDCA> Curve;
  typedef typename Curve::Parameter Parameter;
  typedef typename Curve::ControlPoint ControlPoint;
  typedef typename Curve::Index Index;

  // Find the first and last knot.
  Parameter firstU ( curve.getFirstKnot() );
  Parameter lastU  ( curve.getLastKnot() );
  std::cout << "C01: First knot = " << firstU << std::endl;
  std::cout << "C02: Last knot  = " << lastU << std::endl;

	// For loop speed.
  Index numParams ( 25 );
  Index order ( curve.getOrder() );
  Index dimension ( curve.getDimension() ), j, span;
	Parameter diffU ( lastU - firstU ), u, sum;
  SlArrayPtr<Parameter,Index> N ( new Parameter[order] );
  SlArrayPtr<ControlPoint,Index> pt ( new ControlPoint[dimension] );

  // Loop through the parameters.
  for ( Index i = 0; i < numParams; ++i )
  {
		u = static_cast<Parameter> ( i ) 
      / static_cast<Parameter> ( numParams - 1 );
		u = firstU + u * diffU;

    // Print the span.
    span = Nurbs::findSpan ( curve, u );
    std::cout << "C03: i = " << i << ", u = " << u << ", span = " << span << std::endl;

    // Print the basis functions.
    NcBasisFunctions<NCSDCA>::basis ( curve, u, span, N );
    sum = 0;
    std::cout << "C04: N[" << i << "] = ( ";
    for ( j = 0; j < order; ++j )
    {
      std::cout << N[j] << ( ( order - 1 == j ) ? " )" : ", " );
      sum += N[j];
    }
    std::cout << ", sum = " << sum << ", sum - 1 = " << sum - 1 << std::endl;

    // Print the point.
    NcEvaluate<NCSDCA>::evaluate ( curve, u, pt );
    std::cout << "C05: pt[" << i << "] = ( ";
    for ( j = 0; j < dimension; ++j )
    {
      std::cout << pt[j] << ( ( dimension - 1 == j ) ? " )\n" : ", " );
    }
  }
}


template<NCSDTA, class Vec> void testCurve1 ( 
  const Vec &p1, 
  const Vec &p2, 
  NcCurve<NCSDCA> &c1, 
  NcCurve<NCSDCA> &c2 )
{
  // Typedefs.
  typedef NcCurve<NCSDCA> Curve;
  typedef typename Curve::Parameter Parameter;
  typedef typename Curve::ControlPoint ControlPoint;

  Parameter knotTol ( SlConstants<Parameter>::zero() );
  ControlPoint ctrPtTol ( SlConstants<ControlPoint>::zero() );

  // Print to stdout.
  std::cout << "A01: p1 = " << p1 << std::endl;
  std::cout << "A02: p2 = " << p2 << std::endl;
  std::cout << "A03: c1 = \n" << c1 << std::endl;
  std::cout << "A04: c2 = \n" << c2 << std::endl;
  std::cout << "A05: ( c1 == c2 ) = " << ( c1 == c2 ) << std::endl;
  std::cout << "A06: ( c1.isEqual ( c2 ) = " << ( c1.isEqual ( c2, knotTol, ctrPtTol ) ) << std::endl;

  // Construct the curves.
  NcLine<NCSDCA>::create ( p1, p2, c1 );
  NcLine<NCSDCA>::create ( p1, p2, c2 );
  std::cout << "A07: c1 = \n" << c1 << std::endl;
  std::cout << "A08: c2 = \n" << c2 << std::endl;
  std::cout << "A09: ( c1 == c2 ) = " << ( c1 == c2 ) << std::endl;
  std::cout << "A10: ( c1.isEqual ( c2 ) = " << ( c1.isEqual ( c2, knotTol, ctrPtTol ) ) << std::endl;

  // Make one rational.
  c1.setRational ( true );
  std::cout << "A11: c1 = \n" << c1 << std::endl;
  std::cout << "A12: ( c1 == c2 ) = " << ( c1 == c2 ) << std::endl;
  std::cout << "A13: ( c1.isEqual ( c2 ) = " << ( c1.isEqual ( c2, knotTol, ctrPtTol ) ) << std::endl;

  // Test the evaluation algorithms.
  testEvaluation ( c1 );
  testEvaluation ( c2 );
}


template<NCSDTA, class Vec> void testCurve2 ( 
  const Vec &center, 
  const Vec &normal, 
  const CtrPtType &radius, 
  NcCurve<NCSDCA> &c1, 
  NcCurve<NCSDCA> &c2 )
{
  // Print to stdout.
  std::cout << "B01: center = " << center << std::endl;
  std::cout << "B02: normal = " << normal << std::endl;
  std::cout << "B03: radius = " << radius << std::endl;
  std::cout << "B04: c1 = \n" << c1 << std::endl;
  std::cout << "B05: c2 = \n" << c2 << std::endl;
  std::cout << "B06: ( c1 == c2 ) = " << ( c1 == c2 ) << std::endl;

  // Construct the curves.
  NcCircle<NCSDCA>::create ( center, normal, radius, c1 );
  NcCircle<NCSDCA>::create ( center, normal, radius, c2 );
  std::cout << "B07: c1 = \n" << c1 << std::endl;
  std::cout << "B08: c2 = \n" << c2 << std::endl;
  std::cout << "B09: ( c1 == c2 ) = " << ( c1 == c2 ) << std::endl;

  // Make one non-rational.
  c1.setRational ( false );
  std::cout << "B10: c1 = \n" << c1 << std::endl;
  std::cout << "B11: ( c1 == c2 ) = " << ( c1 == c2 ) << std::endl;

  // Test the evaluation algorithms.
  testEvaluation ( c1 );
  testEvaluation ( c2 );
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

  // The normals.
  SlVec3d n1d ( p1d ); n1d.normalize();
  SlVec3d n2d ( p2d ); n2d.normalize();
  SlVec3f n1f ( p1f ); n1f.normalize();

  // Test the curves.
  std::cout << "Testing curves c01 and c02" << std::endl; testCurve1 ( p1d, p2d, c01, c02 );
  std::cout << "Testing curves c03 and c04" << std::endl; testCurve1 ( p1d, p2d, c03, c04 );
  std::cout << "Testing curves c05 and c06" << std::endl; testCurve1 ( p1d, p2d, c05, c06 );
  std::cout << "Testing curves c07 and c08" << std::endl; testCurve1 ( p1f, p2f, c07, c08 );
  std::cout << "Testing curves c09 and c10" << std::endl; testCurve1 ( p1d, p2d, c09, c10 );
  std::cout << "Testing curves c11 and c12" << std::endl; testCurve1 ( p1f, p2f, c11, c12 );

  // More tests for the curves with scalar control point types.
  std::cout << "Testing curves c01 and c02" << std::endl; testCurve2 ( p1d, n1d, 2.0,  c01, c02 );
  std::cout << "Testing curves c03 and c04" << std::endl; testCurve2 ( p2d, n2d, 3.0,  c03, c04 );
  std::cout << "Testing curves c05 and c06" << std::endl; testCurve2 ( p1d, n1d, 4.0,  c05, c06 );
  std::cout << "Testing curves c07 and c08" << std::endl; testCurve2 ( p1f, n1f, 5.0f, c07, c08 );

  // Wait for user to press the 'any' key.
  //std::cin.get();

  return 0;
}
