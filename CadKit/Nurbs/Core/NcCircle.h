
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  NcCircle.h: Class to construct a NURBS curve as a circle.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_NURBS_CORE_LIBRARY_CREATE_CIRCLE_CLASS_H_
#define _CADKIT_NURBS_CORE_LIBRARY_CREATE_CIRCLE_CLASS_H_

#include "NcCreate.h"
#include "NcCurve.h"

#include "Standard/SlConstants.h"


namespace CadKit
{
template<NCSDTA> class NcCircle : public NcCreate<NCSDCA>
{
public:

  DECLARE_TYPEDEFS;

  // Create the 2D circle.
  static bool create ( const SlVec2<ControlPointType> &center, 
                       const ControlPointType &radius, 
                       NcCurve<NCSDCA> &curve );

  // Create the 3D circle.
  static bool create ( const SlVec3<ControlPointType> &center, 
                       const ControlPointType &radius, 
                       NcCurve<NCSDCA> &curve );
protected:

  static bool _create ( const ControlPointType *center, 
                        const ControlPointType &radius, 
                        const IndexType &dimension, 
                        NcCurve<NCSDCA> &curve );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Create a circle.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcCircle<NCSDCA>::_create ( 
  const ControlPointType *center, 
  const ControlPointType &radius, 
  const IndexType &dimension,
  NcCurve<NCSDCA> &curve )
{
  NC_CHECK_ARGUMENT ( NULL != center );
  NC_CHECK_ARGUMENT ( 2 == dimension || 3 == dimension );

  // Reset the array sizes.
  if ( false == curve.resize ( 1, dimension + 1, 3, 9, true ) )
    return false;

  // Build the knot vector.
  ParameterArray &knots = curve.getKnotVector();
  knots(0)  = 0;
  knots(1)  = 0;
  knots(2)  = 0;
  knots(3)  = 0.25f;
  knots(4)  = 0.25f;
  knots(5)  = 0.5f;
  knots(6)  = 0.5f;
  knots(7)  = 0.75f;
  knots(8)  = 0.75f;
  knots(9)  = 1;
  knots(10) = 1;
  knots(11) = 1;

  // Fill in the x control points.
  ControlPointArray &ctrPts = curve.getControlPoints();
  ctrPts(0,0) =  1;
  ctrPts(0,1) =  SL_INV_SQRT_2_F;
  ctrPts(0,2) =  0;
  ctrPts(0,3) = -SL_INV_SQRT_2_F;
  ctrPts(0,4) = -1;
  ctrPts(0,5) = -SL_INV_SQRT_2_F;
  ctrPts(0,6) =  0;
  ctrPts(0,7) =  SL_INV_SQRT_2_F;
  ctrPts(0,8) =  1;

  // Fill in the y control points.
  ctrPts(1,0) =  0;
  ctrPts(1,1) =  SL_INV_SQRT_2_F;
  ctrPts(1,2) =  1;
  ctrPts(1,3) =  SL_INV_SQRT_2_F;
  ctrPts(1,4) =  0;
  ctrPts(1,5) = -SL_INV_SQRT_2_F;
  ctrPts(1,6) = -1;
  ctrPts(1,7) = -SL_INV_SQRT_2_F;
  ctrPts(1,8) =  0;

  // Initialize the index for the weights.
  IndexType w ( 2 );

  // If we are 3D...
  if ( 3 == dimension )
  {
    // Fill in the z control points.
    ctrPts(2,0) = 0;
    ctrPts(2,1) = 0;
    ctrPts(2,2) = 0;
    ctrPts(2,3) = 0;
    ctrPts(2,4) = 0;
    ctrPts(2,5) = 0;
    ctrPts(2,6) = 0;
    ctrPts(2,7) = 0;
    ctrPts(2,8) = 0;

    // Reset the index for the weights.
    w = 3;
  }

  // Fill in the weights.
  ctrPts(w,0) = 1;
  ctrPts(w,1) = SL_INV_SQRT_2_F;
  ctrPts(w,2) = 1;
  ctrPts(w,3) = SL_INV_SQRT_2_F;
  ctrPts(w,4) = 1;
  ctrPts(w,5) = SL_INV_SQRT_2_F;
  ctrPts(w,6) = 1;
  ctrPts(w,7) = SL_INV_SQRT_2_F;
  ctrPts(w,8) = 1;

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a circle.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcCircle<NCSDCA>::create (
  const SlVec2<ControlPointType> &center, 
  const ControlPointType &radius,
  NcCurve<NCSDCA> &curve )
{
  return NcCircle<NCSDCA>::_create ( center.getValue(), radius, 2, curve );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a circle.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcCircle<NCSDCA>::create (
  const SlVec3<ControlPointType> &center, 
  const ControlPointType &radius,
  NcCurve<NCSDCA> &curve )
{
  return NcCircle<NCSDCA>::_create ( center.getValue(), radius, 3, curve );
}


}; // namespace CadKit


#endif // _CADKIT_NURBS_CORE_LIBRARY_CREATE_CIRCLE_CLASS_H_
