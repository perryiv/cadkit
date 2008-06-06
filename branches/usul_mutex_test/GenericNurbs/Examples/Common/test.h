
///////////////////////////////////////////////////////////////////////////////
//
//  Test functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_TESTS_H_
#define _GENERIC_NURBS_TESTS_H_

#include "GN/Splines/Curve.h"
#include "GN/Splines/Surface.h"
#include "GN/Create/Circle.h"
#include "GN/Create/Line.h"
#include "GN/Create/Sphere.h"
#include "GN/Transform/Matrix.h"
#include "GN/Write/XML.h"
#include "GN/Convert/DtNurbs.h"
#include "GN/Algorithms/FindSpan.h"
#include "GN/Algorithms/InsertKnot.h"
#include "GN/Algorithms/BasisFunctions.h"
#include "GN/Algorithms/Parameterize.h"
#include "GN/Algorithms/KnotVector.h"
#include "GN/Algorithms/Copy.h"
#include "GN/Evaluate/Point.h"
#include "GN/Tessellate/Bisect.h"
#include "GN/Interpolate/Global.h"

#include <iostream>
#include <iomanip>
#include <list>
#include <limits>


///////////////////////////////////////////////////////////////////////////////
//
//  Turn output on/off here.
//
///////////////////////////////////////////////////////////////////////////////

bool global_output_flag ( true );
#define OUTPUT if ( ::global_output_flag ) std::cout


///////////////////////////////////////////////////////////////////////////////
//
//  Confirm the validity of the spline and write it to an XML stream.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > void inline confirm ( const SplineType &s )
{
  s.check();

  if ( global_output_flag )
  {
    std::string indent ( "  " );
    GN::Write::xml ( s, indent, std::cout );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test making a circle.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > void inline testCircle ( SplineType &s )
{
  typedef typename SplineType::DependentType DependentType;
  typedef typename SplineType::Vec3 Vec3;
  typedef typename SplineType::Vec2 Vec2;

  OUTPUT << "<testCircle>\n";

  Vec3 center3D ( 1, 2, 3 );
  Vec2 center2D ( 1, 2 );
  DependentType radius ( 10 );

  GN::Create::circle ( s, center3D, radius );
  ::confirm ( s );

  GN::Create::circle ( s, center2D, radius );
  ::confirm ( s );

  OUTPUT << "</testCircle>\n";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test making a line.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > void inline testLine ( SplineType &s )
{
  typedef typename SplineType::Vec3 Vec3;
  typedef typename SplineType::Vec2 Vec2;

  OUTPUT << "<testLine>\n";

  Vec3 start3D ( 1, 2, 3 ), end3D ( 2, 3, 4 );
  Vec2 start2D ( 1, 2 ),    end2D ( 2, 3 );

  GN::Create::line ( s, start3D, end3D );
  ::confirm ( s );

  GN::Create::line ( s, start2D, end2D );
  ::confirm ( s );

  OUTPUT << "</testLine>\n";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test transforming a spline using a 4x4 transformation matrix.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > void inline testTransform ( SplineType &s )
{
  typedef typename SplineType::Vec3 Vec3;
  typedef typename SplineType::Matrix44 Matrix44;
  typedef typename SplineType::Translation Translation;
  typedef typename SplineType::Scale Scale;

  OUTPUT << "<testTransform>\n";

  s.dimension ( 3 );
  ::confirm ( s );

  Matrix44 trans, scale;
  Translation::make ( trans, Vec3 ( 1, 2, 3 ) );
  Scale::make       ( scale, Vec3 ( 1, 2, 3 ) );

  GN::Transform::matrix ( s, trans );
  ::confirm ( s );

  GN::Transform::matrix ( s, scale );
  ::confirm ( s );

  OUTPUT << "</testTransform>\n";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test making a sphere.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > void inline testSphere ( SplineType &s )
{
  typedef typename SplineType::DependentType DependentType;
  typedef typename SplineType::Vec3 Vec3;

  OUTPUT << "<testSphere>\n";

  Vec3 center ( 1, 2, 3 );
  DependentType radius ( 10 );

  GN::Create::sphere ( s, center, radius );
  ::confirm ( s );

  OUTPUT << "</testSphere>\n";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test calculating basis function coefficients.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > void inline testBasisFunctions ( const SplineType &s )
{
  typedef typename SplineType::SizeType SizeType;
  typedef typename SplineType::IndependentType IndependentType;
  typedef typename SplineType::WorkSpace BasisFunctions;

  OUTPUT << "<testBasisFunctions>\n";

  SizeType numIndepVars ( s.numIndepVars() );
  SizeType numParams ( 100 );

  for ( SizeType i = 0; i < numIndepVars; ++i )
  {
    BasisFunctions &N = s.work ( i ).basis;
    SizeType order ( s.order ( i ) );
    N.accommodate ( order );
    OUTPUT << "order = " << order << ", num knots = " << s.numKnots ( i ) << '\n';
    for ( SizeType j = 0; j < numParams; ++j )
    {
      IndependentType u ( IndependentType ( j ) / IndependentType ( numParams - 1 ) );
      SizeType span ( GN::Algorithms::findKnotSpan ( s, i, u ) );
      GN::Algorithms::basisFunctions ( s, i, span, u, N );

      OUTPUT << "indep = " << i << ", u = " << u << ", span = " << span;
      for ( SizeType k = 0; k < order; ++k )
      {
        OUTPUT << ", N[" << k << "] = " << N[k];
      }
      OUTPUT << std::endl;
    }
  }

  OUTPUT << "</testBasisFunctions>\n";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test evaluating curve points.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > void inline testCurvePoint ( const SplineType &s )
{
  typedef typename SplineType::SizeType SizeType;
  typedef typename SplineType::IndependentType IndependentType;
  typedef typename SplineType::Vector Point;
  typedef typename SplineType::ErrorCheckerType ErrorCheckerType;
  GN_CAN_BE_CURVE ( SplineType );

  OUTPUT << "<testCurvePoint>\n";

  const SizeType numIndepVars ( s.numIndepVars() );
  GN_ERROR_CHECK ( 1 == numIndepVars );
  const SizeType numPoints ( 100 );
  const SizeType dimension ( s.dimension() );
  Point pt;
  pt.resize ( dimension );

  for ( SizeType i = 0; i < numPoints; ++i )
  {
    IndependentType u ( IndependentType ( i ) / IndependentType ( numPoints - 1 ) );
    GN::Evaluate::point ( s, u, pt );

    OUTPUT << "u = " << u;
    for ( SizeType d = 0; d < dimension; ++d )
    {
      OUTPUT << ", pt[" << d << "] = " << pt[d];
    }
    OUTPUT << std::endl;
  }

  OUTPUT << "</testCurvePoint>\n";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test tessellating a curve.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > void inline testTessellation ( const SplineType &s )
{
  typedef typename SplineType::IndependentType IndependentType;
  typedef typename SplineType::ErrorCheckerType ErrorCheckerType;
  typedef typename SplineType::DependentArgument DependentArgument;

  GN_CAN_BE_CURVE ( SplineType );

  OUTPUT << "<testTessellation>\n";

  DependentArgument tolerance ( 0.0001f );

  ::confirm ( s );
  std::list<IndependentType> u1;
  GN::Tessellate::bisect ( s, tolerance, u1 );

  ::confirm ( s );
  std::vector<IndependentType> u2;
  GN::Tessellate::bisect ( s, tolerance, u2 );

  GN_ERROR_CHECK ( !u1.empty() );
  GN_ERROR_CHECK ( !u2.empty() );
  GN_ERROR_CHECK ( u1.size() == u2.size() );
  GN_ERROR_CHECK ( std::equal ( u1.begin(), u1.end(), u2.begin() ) );

  OUTPUT << "</testTessellation>\n";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test evaluating surface points.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > void inline testSurfacePoint ( const SplineType &s )
{
  typedef typename SplineType::SizeType SizeType;
  typedef typename SplineType::IndependentType IndependentType;
  typedef typename SplineType::Vector Point;
  GN_CAN_BE_SURFACE ( SplineType );

  const std::size_t bufSize ( 1023 );
  char buffer[bufSize+1];

  // Determine format string for maximum precision.
  std::pair<std::string,std::string> format;
  {
    {
      const int precision ( std::numeric_limits<IndependentType>::digits10 );
      std::ostringstream out;
      out << "u = %0." << precision << "f, v = %0." << precision << "f";
      format.first = out.str();
    }
    {
      typedef typename SplineType::DependentType DependentType;
      const int precision ( std::numeric_limits<DependentType>::digits10 );
      std::ostringstream out;
      out << ", pt[%d] = %0." << precision << "f";
      format.second = out.str();
    }
  }

  OUTPUT << "<testSurfacePoint>\n";

  SizeType numIndepVars ( s.numIndepVars() );
  if ( 2 == numIndepVars )
  {
    SizeType numPointsU ( 20 );
    SizeType numPointsV ( 20 );
    SizeType dimension ( s.dimension() );
    Point pt;
    pt.resize ( dimension );

    for ( SizeType i = 0; i < numPointsU; ++i )
    {
      IndependentType u ( IndependentType ( i ) / IndependentType ( numPointsU - 1 ) );
      for ( SizeType j = 0; j < numPointsV; ++j )
      {
        IndependentType v ( IndependentType ( j ) / IndependentType ( numPointsV - 1 ) );
        GN::Evaluate::point ( s, u, v, pt );

        if ( global_output_flag )
        {
          GN_FORMAT_STRING_2 ( buffer, bufSize, format.first.c_str(), u, v );
          OUTPUT << buffer;
          for ( SizeType d = 0; d < dimension; ++d )
          {
            GN_FORMAT_STRING_2 ( buffer, bufSize, format.second.c_str(), d, pt[d] );
            OUTPUT << buffer;
          }
          OUTPUT << std::endl;
        }
      }
    }
  }
  OUTPUT << "</testSurfacePoint>\n";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test interpolating points.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > void inline testInterpolation ( SplineType &s )
{
  typedef typename SplineType::SizeType SizeType;
  typedef typename SplineType::IndependentType IndependentType;
  typedef typename SplineType::IndependentSequence IndependentSequence;
  typedef typename SplineType::DependentType DependentType;
  typedef typename SplineType::DependentContainer DependentContainer;
  typedef typename SplineType::Power PowerFunctor;
  typedef typename SplineType::ErrorCheckerType ErrorCheckerType;
  typedef GN::Algorithms::Parameterize < IndependentSequence, DependentContainer, PowerFunctor, ErrorCheckerType > Parameterize;
  typedef GN::Algorithms::KnotVector < IndependentSequence, ErrorCheckerType > KnotVectorBuilder;
  GN_CAN_BE_CURVE ( SplineType );

  OUTPUT << "<testInterpolation>\n";

  // Scalar data.
  const SizeType numDataPts ( 6 );
  const SizeType dimension ( 3 );
  const SizeType order ( 4 );
  const IndependentType power ( GN::Algorithms::Constants::CENTRIPETAL_FIT );

  // Data points.
  const DependentType data[dimension][numDataPts] =
  {
    { 0, 1, 2, 3, 4, 5 }, // x
    { 1, 2, 1, 2, 1, 2 }, // y
    { 0, 0, 0, 0, 0, 0 }  // z
  };

  // Copy to a container.
  DependentContainer points;
  GN::Algorithms::copy2dTo2d ( data, dimension, numDataPts, points );

  // Make the parameters.
  IndependentSequence params;
  Parameterize::fit ( points, power, params );

  // Make the knot vector. Size it for interpolation.
  IndependentSequence knots;
  knots.resize ( numDataPts + order );
  KnotVectorBuilder::build ( params, order, knots );

  // Interpolate.
  GN::Interpolate::global ( order, params, knots, points, s );

  // Again.
  GN::Interpolate::global ( order, points, power, s );

  // Check.
  ::confirm ( s );

  OUTPUT << "</testInterpolation>\n";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test making a copy of the spline.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > void inline testCopying ( const SplineType &s )
{
  typedef typename SplineType::ErrorCheckerType ErrorCheckerType;

  OUTPUT << "<testCopying>\n";

  SplineType copy1 ( s );
  GN_ERROR_CHECK ( copy1 == s );
  ::confirm ( s );
  ::confirm ( copy1 );

  SplineType copy2 = copy1;
  GN_ERROR_CHECK ( copy2 == s );
  ::confirm ( copy1 );
  ::confirm ( copy2 );

  OUTPUT << "</testCopying>\n";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test setting the spline from a DT_NURBS C-array.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > void inline testDtNurbsCarray ( SplineType &s )
{
  OUTPUT << "<testDtNurbsCarray>\n";

  const double Carray[] = 
  { 
    2, // Two independent variables, u & v.
   -4, // Four dependent variables. Negative indicates rational, 
       // so the last ones are interpreted as weights.
    4, // The order (degree+1) in the u-direction.
    3, // The order (degree+1) in the v-direction.
    5, // Number of control points in the u-direction.
    6, // Number of control points in the v-direction.
    0, 0, // Internal flags for DT_NURBS, set to zero and ignore otherwise. 
          // There is one for each independent variable.
    0, 0, 0, 0,    0.5, 1,    1, 1, 1, // First knot vector.
    0, 0, 0, 0.25, 0.5, 0.75, 1, 1, 1, // Second knot vector.
    0, 1, 2, 3, 4, // First row of x-values in control-point grid.
    5, 6, 7, 8, 9, // Second row of x-values in control-point grid.
    0, 1, 2, 3, 4, // Third...
    5, 6, 7, 8, 9, 
    0, 1, 2, 3, 4, 
    5, 6, 7, 8, 40, 
    0, 1, 2, 3, 4, // First row of y-values in control-point grid.
    5, 6, 7, 8, 9, // Second...
    0, 1, 2, 3, 4, 
    5, 6, 7, 8, 9, 
    0, 1, 2, 3, 4, 
    5, 6, 7, 8, 41, 
    0, 1, 2, 3, 4, // First row of z-values in control-point grid.
    5, 6, 7, 8, 9, 
    0, 1, 2, 3, 4, 
    5, 6, 7, 8, 9, 
    0, 1, 2, 3, 4, 
    5, 6, 7, 8, 42,
    1, 1, 1, 1, 1,  // First row of weights in control-point grid.
    1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 
    1, 1, 1, 1, 43
  };

  GN::Convert::DtNurbs::set ( Carray, s );
  ::confirm ( s );

  OUTPUT << "</testDtNurbsCarray>\n";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test finding the knot-span. This function is called indirectly when you 
//  evaluate a spline.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > void inline testFindingKnotSpan ( const SplineType &s )
{
  typedef typename SplineType::SizeType SizeType;
  typedef typename SplineType::IndependentType IndependentType;

  const std::size_t bufSize ( 1023 );
  char buffer[bufSize+1];

#ifdef _MSC_VER

  // Determine format string for maximum precision.
  std::string format;
  {
    const int precision ( std::numeric_limits<IndependentType>::digits10 );
    std::ostringstream out;
    out << "%0." << precision << "f";
    format = out.str();
  }

#endif

  OUTPUT << "<testFindingKnotSpan>\n";

  SizeType indep ( s.numIndepVars() );
  SizeType numParams ( 100 );
  std::string indent1 ( "  " );
  std::string indent2 ( indent1 + indent1 );

  OUTPUT << indent1 << "<spline>\n";
  OUTPUT << indent2 << "<string>" << s.getStringData() << "</string>\n";
  OUTPUT << indent2 << "<address>" << &s << "</address>\n";
  OUTPUT << indent1 << "</spline>\n";

  for ( SizeType i = 0; i < indep; ++i )
  {
    OUTPUT << indent1 << "<independent which = \"" << i << "\">\n";

    for ( SizeType j = 0; j < numParams; ++j )
    {
      IndependentType u ( IndependentType ( j ) / IndependentType ( numParams - 1 ) );
      SizeType span ( GN::Algorithms::findKnotSpan ( s, i, u ) );
      OUTPUT << indent2 << "<test j=\"" << j << "\">u = ";
      #ifdef _MSC_VER
      ::memset ( buffer, '\0', bufSize );
      GN_FORMAT_STRING_1 ( buffer, bufSize, format.c_str(), u );
      OUTPUT << buffer;
      #else
      OUTPUT << u;
      #endif
      OUTPUT << " span = " << span << "</test>\n";
    }

    OUTPUT << indent1 << "</independent>\n";
  }

  OUTPUT << "</testFindingKnotSpan>\n";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Print the knot multiplicity.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > void inline printKnotMultiplicity ( const SplineType &s, typename SplineType::IndependentType u )
{
  const unsigned int bufSize ( 1023 );
  char buffer[bufSize + 1];
  GN_FORMAT_STRING_2 ( buffer, bufSize, "<multiplicity_test> u = %6.4f  multiplicity = %d </multiplicity_test>\n", u, s.knotMultiplicity ( 0, u ) );
  OUTPUT << buffer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test finding the knot multiplicity.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > void inline testKnotMultiplicity ( const SplineType &spline )
{
  typedef typename SplineType::SplineClass SplineClass;
  typedef typename SplineClass::IndependentType IndependentType;
  
  const SplineClass &s = spline;

  OUTPUT << "<testKnotMultiplicity>\n";
  ::printKnotMultiplicity ( s, 0.00f );
  ::printKnotMultiplicity ( s, 0.25f );
  ::printKnotMultiplicity ( s, 0.50f );
  ::printKnotMultiplicity ( s, 0.75f );
  ::printKnotMultiplicity ( s, 1.00f );
  ::printKnotMultiplicity ( s, 0.37f );
  ::printKnotMultiplicity ( s, 2.00f );
  OUTPUT << "</testKnotMultiplicity>\n";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test inserting knots.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > void inline printQueryKnotInsertion ( 
  const SplineType &s, 
  typename SplineType::SizeType whichIndepVar,
  typename SplineType::IndependentType knot,
  typename SplineType::SizeType numTimes )
{
  std::string reason;
  std::string indent ( "    " );
  bool result ( GN::Algorithms::canInsertKnot ( s, whichIndepVar, knot, numTimes, reason ) );
  OUTPUT << indent << "<inserting_knot value=\"" << knot << "\">";
  if ( result )
    OUTPUT << "Allowed " << numTimes << " time(s).";
  else
    OUTPUT << "Not permitted " << numTimes << " times. " << reason;
  OUTPUT << "</inserting_knot>\n";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test inserting knots.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > void inline testQueryKnotInsertion ( const SplineType &spline )
{
  typedef typename SplineType::SplineClass SplineClass;
  typedef typename SplineClass::IndependentType IndependentType;
  typedef typename SplineClass::SizeType SizeType;

  // Make a copy.
  SplineClass s ( spline );

  OUTPUT << "<testQueryKnotInsertion>\n";

  SizeType numIndepVars ( s.numIndepVars() );
  std::string reason;
  std::string indent1 ( "  " );

  for ( SizeType i = 0; i < numIndepVars; ++i )
  {
    OUTPUT << indent1 << "<independent i=\"" << i << "\">\n";
    SizeType numKnots ( s.numKnots ( i ) );
    SizeType degree ( s.degree ( i ) );

    for ( SizeType j = 0; j < numKnots; ++j )
    {
      for ( SizeType k = 1; k <= degree; ++k )
      {
        IndependentType knot ( s.knot ( i, j ) );
        printQueryKnotInsertion ( s, i, knot,         k );
        printQueryKnotInsertion ( s, i, knot + 1e-4f, k );
        printQueryKnotInsertion ( s, i, knot - 1e-4f, k );
      }
    }
    OUTPUT << indent1 << "</independent>\n";
  }

  OUTPUT << "</testQueryKnotInsertion>\n";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the spline through several tests.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > void inline testSpline ( SplineType &s )
{
  ::testTransform ( s );
  ::testFindingKnotSpan ( s );
  ::testKnotMultiplicity ( s );
  ::testBasisFunctions ( s );
#ifndef NO_STACK_DECLARATIONS
  ::testCopying ( s );
  ::testQueryKnotInsertion ( s );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the curve through several tests.
//
///////////////////////////////////////////////////////////////////////////////

template < class CurveType > void inline testCurve ( CurveType &c )
{
#if 1
  ::testLine ( c );
  ::testCircle ( c );
  ::testCurvePoint ( c );
  ::testTessellation ( c );
  ::testInterpolation ( c );
  ::testSpline ( c );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the surface through several tests.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > void inline testSurface ( SplineType &s )
{
  ::testSphere ( s );
  ::testSurfacePoint ( s );
#if 1
  ::testDtNurbsCarray ( s );
  ::testSpline ( s );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Struct to declare splines and run the tests.
//
///////////////////////////////////////////////////////////////////////////////

template < class ConfigType > struct Test
{
  template 
  <
    class SplineType, 
    class CurveType,
    class SurfaceType
  >
  static void run ( SplineType &spline, CurveType &curve, SurfaceType &surface )
  {
    // Set string data. This is important when printing.
    spline.setStringData  ( "spline" );
    curve.setStringData   ( "curve" );
    surface.setStringData ( "surface" );

    // Test the curve methods on both the general spline and curve.
    ::testCurve ( spline );
    ::testCurve ( curve );

    // Test the surface methods on both the general spline and surface.
    ::testSurface ( spline );
    ::testSurface ( surface );
  }
  static void run()
  {
    // Declare spline types.
    typedef GN::Splines::Spline<ConfigType>  Spline;
    typedef GN::Splines::Curve<ConfigType>   Curve;
    typedef GN::Splines::Surface<ConfigType> Surface;

    // Declare spline variables.
    Spline spline;
    Curve curve;
    Surface surface;

    // Run the tests.
    run ( spline, curve, surface );
  }
};


#endif // _GENERIC_NURBS_TESTS_H_
