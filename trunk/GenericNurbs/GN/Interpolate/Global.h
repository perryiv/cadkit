
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Global interpolation.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_GLOBAL_INTERPOLATION_H_
#define _GENERIC_NURBS_LIBRARY_GLOBAL_INTERPOLATION_H_

#include "GN/Macros/ErrorCheck.h"
#include "GN/MPL/TypeCheck.h"
#include "GN/Math/Matrix.h"


namespace GN {
namespace Interpolate {


///////////////////////////////////////////////////////////////////////////////
//
//  Global interpolation.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template
  <
    class CurveType, 
    class IndependentSequenceType, 
    class DependentContainerType
  >
  struct Global
  {

  };
};


///////////////////////////////////////////////////////////////////////////////
//
//  Global interpolation.
//
///////////////////////////////////////////////////////////////////////////////

template
<
  class CurveType, 
  class IndependentSequenceType, 
  class DependentContainerType
>
void global ( const typename CurveType::SizeType &order,
              const IndependentSequenceType &params,
              const IndependentSequenceType &knots,
              const DependentContainerType &points,
              CurveType &curve )
{
  GN_CAN_BE_CURVE ( CurveType );
  typedef typename CurveType::SplineClass SplineClass;
  typedef typename SplineClass::SizeType SizeType;
  typedef typename SplineClass::SizeContainer SizeContainer;
  typedef typename SplineClass::DependentContainer MatrixContainer;
  typedef typename SplineClass::DependentTester DependentTester;
  typedef typename SplineClass::ErrorCheckerType ErrorCheckerType;
  typedef GN::Math::Matrix < MatrixContainer, DependentTester, ErrorCheckerType > Matrix;

  // Get reference to spline.
  SplineClass &spline = curve.spline();

  // Make sure there is room.
  SizeType dimension ( points.size() );
  SizeType numCtrPts ( params.size() );
  bool rational ( false );
  spline.resize ( dimension, order, numCtrPts, rational );

  // Declare the matrix A in Ax = b.
  Matrix A ( curve.numControlPoints ( 0 ), curve.numControlPoints ( 0 ) );
}


}; // namespace Interpolate
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_GLOBAL_INTERPOLATION_H_
