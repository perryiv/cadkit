
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
#include "GN/Algorithms/Copy.h"
#include "GN/Evaluate/Point.h"
#include "GN/Tessellate/Bisect.h"
#include "GN/Interpolate/Global.h"

#include <iostream>
#include <iomanip>
#include <list>


///////////////////////////////////////////////////////////////////////////////
//
//  Turn output on/off here.
//
///////////////////////////////////////////////////////////////////////////////

bool global_output_flag ( false );
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
  typedef typename SplineType::ControlPointType ControlPointType;
  typedef typename SplineType::Vec3 Vec3;
  typedef typename SplineType::Vec2 Vec2;

  OUTPUT << "<testCircle>\n";

  Vec3 center3D ( 1, 2, 3 );
  Vec2 center2D ( 1, 2 );
  ControlPointType radius ( 10 );

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
  typedef typename SplineType::ControlPointType ControlPointType;
  typedef typename SplineType::Vec3 Vec3;

  OUTPUT << "<testSphere>\n";

  Vec3 center ( 1, 2, 3 );
  ControlPointType radius ( 10 );

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
  typedef typename SplineType::UIntType UIntType;
  typedef typename SplineType::KnotType Parameter;
  typedef typename SplineType::WorkSpace BasisFunctions;

  OUTPUT << "<testBasisFunctions>\n";

  UIntType numIndepVars ( s.numIndepVars() );
  UIntType numParams ( 100 );
  BasisFunctions &N = s.work().basis;

  for ( UIntType i = 0; i < numIndepVars; ++i )
  {
    UIntType order ( s.order ( i ) );
    N.accommodate ( order );
    for ( UIntType j = 0; j < numParams; ++j )
    {
      Parameter u ( Parameter ( j ) / Parameter ( numParams - 1 ) );
      UIntType span ( GN::Algorithms::findKnotSpan ( s, i, u ) );
      GN::Algorithms::basisFunctions ( s, i, span, u, N );

      OUTPUT << "indep = " << i << ", u = " << u;
      for ( UIntType k = 0; k < order; ++k )
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
  typedef typename SplineType::UIntType UIntType;
  typedef typename SplineType::KnotType Parameter;
  typedef typename SplineType::Vector Point;
  GN_CAN_BE_CURVE ( SplineType );

  OUTPUT << "<testCurvePoint>\n";

  UIntType numIndepVars ( s.numIndepVars() );
  UIntType numPoints ( 100 );
  UIntType dimension ( s.dimension() );
  Point pt;
  pt.resize ( dimension );

  for ( UIntType i = 0; i < numPoints; ++i )
  {
    Parameter u ( Parameter ( i ) / Parameter ( numPoints - 1 ) );
    GN::Evaluate::point ( s, u, pt );

    OUTPUT << "u = " << u;
    for ( UIntType d = 0; d < dimension; ++d )
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
  typedef typename SplineType::KnotType Parameter;
  typedef typename SplineType::ErrorCheckerType ErrorCheckerType;
  typedef typename SplineType::ControlPointArgument ToleranceType;

  GN_CAN_BE_CURVE ( SplineType );

  OUTPUT << "<testTessellation>\n";

  ToleranceType tolerance ( 0.0001f );

  ::confirm ( s );
  std::list<Parameter> u1;
  GN::Tessellate::bisect ( s, tolerance, u1 );

  ::confirm ( s );
  std::vector<Parameter> u2;
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
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test interpolating points.
//
///////////////////////////////////////////////////////////////////////////////

template < class SplineType > void inline testInterpolation ( SplineType &s )
{
  typedef typename SplineType::UIntType UIntType;
  typedef typename SplineType::KnotType Parameter;
  typedef typename SplineType::ControlPointType DataPointType;
  typedef typename SplineType::ControlPointContainer DataContainer;
  typedef typename SplineType::KnotContainer::value_type ParamContainer;
  GN_CAN_BE_CURVE ( SplineType );

  OUTPUT << "<testInterpolation>\n";

  // Scalar data.
  const UIntType numDataPts ( 6 );
  const UIntType dimension ( 3 );
  const UIntType order ( 4 );
  const Parameter power ( GN::Algorithms::Constants::CENTRIPETAL_FIT );

  // Data points.
  const DataPointType data[ dimension * numDataPts ] =
  {
    0, 1, 2, 3, 4, 5, // x
    1, 2, 1, 2, 1, 2, // y
    0, 0, 0, 0, 0, 0  // z
  };

  // Copy to a container.
  DataContainer points;
  GN::Algorithms::copy ( data, dimension, numDataPts, points );

  // Make the parameters.
  ParamContainer params;
  GN::Algorithms::parameterize ( s, points, dimension, order, power, params );

  // Interpolate.
  GN::Interpolate::global ( s, points, params, dimension, order );

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
  typedef typename SplineType::UIntType UIntType;
  typedef typename SplineType::KnotType KnotType;

  OUTPUT << "<testFindingKnotSpan>\n";

  UIntType indep ( s.numIndepVars() );
  UIntType numParams ( 100 );
  std::string indent1 ( "  " );
  std::string indent2 ( indent1 + indent1 );
  char buffer[1024];

  OUTPUT << indent1 << "<spline>\n";
  OUTPUT << indent2 << "<string>" << s.getStringData() << "</string>\n";
  OUTPUT << indent2 << "<address>" << &s << "</address>\n";
  OUTPUT << indent1 << "</spline>\n";

  for ( UIntType i = 0; i < indep; ++i )
  {
    OUTPUT << indent1 << "<independent which = \"" << i << "\">\n";

    for ( UIntType j = 0; j < numParams; ++j )
    {
      KnotType u ( KnotType ( j ) / KnotType ( numParams - 1 ) );
      UIntType span ( GN::Algorithms::findKnotSpan ( s, i, u ) );
      ::sprintf ( buffer, "%20.15f", u );
      OUTPUT << indent2 << "<test j=\"" << j << "\">u = " << buffer << " span = " << span << "</test>\n";
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

template < class SplineType > void inline printKnotMultiplicity ( const SplineType &s, typename SplineType::KnotType u )
{
  char buffer[1024];
  ::sprintf ( buffer, "<multiplicity_test> u = %6.4f  multiplicity = %d </multiplicity_test>\n", u, s.knotMultiplicity ( 0, u ) );
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
  typedef typename SplineClass::KnotType KnotType;
  
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
  typename SplineType::UIntType whichIndepVar,
  typename SplineType::KnotType knot,
  typename SplineType::UIntType numTimes )
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
  typedef typename SplineClass::KnotType KnotType;
  typedef typename SplineClass::UIntType UIntType;

  // Make a copy.
  SplineClass s ( spline );

  OUTPUT << "<testQueryKnotInsertion>\n";

  UIntType numIndepVars ( s.numIndepVars() );
  std::string reason;
  std::string indent1 ( "  " );

  for ( UIntType i = 0; i < numIndepVars; ++i )
  {
    OUTPUT << indent1 << "<independent i=\"" << i << "\">\n";
    UIntType numKnots ( s.numKnots ( i ) );
    UIntType degree ( s.degree ( i ) );

    for ( UIntType j = 0; j < numKnots; ++j )
    {
      for ( UIntType k = 1; k <= degree; ++k )
      {
        KnotType knot ( s.knot ( i, j ) );
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
  ::testLine ( c );
  ::testCircle ( c );
  ::testCurvePoint ( c );
  ::testTessellation ( c );
  ::testInterpolation ( c );
  ::testSpline ( c );
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
  ::testDtNurbsCarray ( s );
  ::testSpline ( s );
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
