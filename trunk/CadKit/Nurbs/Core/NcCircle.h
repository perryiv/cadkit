
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


namespace CadKit
{
template<NCSDTA> class NcCircle : public NcCreate<NCSDCA>
{
public:

  DECLARE_TYPEDEFS;

  // Create the 2D circle.
  static bool create ( const SlVec2<CtrPtType> &center, 
                       const CtrPtType &radius, 
                       NcCurve<NCSDCA> &curve );

  // Create the 3D circle.
  static bool create ( const SlVec3<CtrPtType> &center, 
                       const SlVec3<CtrPtType> &normal, 
                       const CtrPtType &radius, 
                       NcCurve<NCSDCA> &curve );
protected:

  static bool _create ( const IndexType &dimension, 
                        NcCurve<NCSDCA> &curve );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Create a unit circle in the x-y plane at the origin.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcCircle<NCSDCA>::_create ( 
  const IndexType &dimension,
  NcCurve<NCSDCA> &curve )
{
  NC_CHECK_ARGUMENT ( 2 == dimension || 3 == dimension );

  // Reset the array sizes.
  NC_VERIFY_ALLOCATION ( curve.resize ( 1, dimension + 1, 3, 9, true ) );

  // Build the knot vector.
  curve.setKnot (  0, 0 );
  curve.setKnot (  1, 0 );
  curve.setKnot (  2, 0 );
  curve.setKnot (  3, 0.25f );
  curve.setKnot (  4, 0.25f );
  curve.setKnot (  5, 0.5f );
  curve.setKnot (  6, 0.5f );
  curve.setKnot (  7, 0.75f );
  curve.setKnot (  8, 0.75f );
  curve.setKnot (  9, 1 );
  curve.setKnot ( 10, 1 );
  curve.setKnot ( 11, 1 );

  // Fill in the x control points.
  curve.setCtrPt ( 0, 0, 1 );
  curve.setCtrPt ( 0, 1, SL_INV_SQRT_2_F );
  curve.setCtrPt ( 0, 2, 0 );
  curve.setCtrPt ( 0, 3, -SL_INV_SQRT_2_F );
  curve.setCtrPt ( 0, 4, -1 );
  curve.setCtrPt ( 0, 5, -SL_INV_SQRT_2_F );
  curve.setCtrPt ( 0, 6, 0 );
  curve.setCtrPt ( 0, 7, SL_INV_SQRT_2_F );
  curve.setCtrPt ( 0, 8, 1 );

  // Fill in the y control points.
  curve.setCtrPt ( 1, 0, 0 );
  curve.setCtrPt ( 1, 1, SL_INV_SQRT_2_F );
  curve.setCtrPt ( 1, 2, 1 );
  curve.setCtrPt ( 1, 3, SL_INV_SQRT_2_F );
  curve.setCtrPt ( 1, 4, 0 );
  curve.setCtrPt ( 1, 5, -SL_INV_SQRT_2_F );
  curve.setCtrPt ( 1, 6, -1 );
  curve.setCtrPt ( 1, 7, -SL_INV_SQRT_2_F );
  curve.setCtrPt ( 1, 8, 0 );

  // Initialize the index for the weights.
  IndexType w ( 2 );

  // If we are 3D...
  if ( 3 == dimension )
  {
    // Fill in the z control points.
    curve.setCtrPt ( 2, 0, 0 );
    curve.setCtrPt ( 2, 1, 0 );
    curve.setCtrPt ( 2, 2, 0 );
    curve.setCtrPt ( 2, 3, 0 );
    curve.setCtrPt ( 2, 4, 0 );
    curve.setCtrPt ( 2, 5, 0 );
    curve.setCtrPt ( 2, 6, 0 );
    curve.setCtrPt ( 2, 7, 0 );
    curve.setCtrPt ( 2, 8, 0 );

    // Reset the index for the weights.
    w = 3;
  }

  // Fill in the weights.
  curve.setCtrPt ( w, 0, 1 );
  curve.setCtrPt ( w, 1, SL_INV_SQRT_2_F );
  curve.setCtrPt ( w, 2, 1 );
  curve.setCtrPt ( w, 3, SL_INV_SQRT_2_F );
  curve.setCtrPt ( w, 4, 1 );
  curve.setCtrPt ( w, 5, SL_INV_SQRT_2_F );
  curve.setCtrPt ( w, 6, 1 );
  curve.setCtrPt ( w, 7, SL_INV_SQRT_2_F );
  curve.setCtrPt ( w, 8, 1 );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a circle.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcCircle<NCSDCA>::create (
  const SlVec2<CtrPtType> &center, 
  const CtrPtType &radius,
  NcCurve<NCSDCA> &curve )
{
  // Create the unit circle.
  NC_VERIFY_RESULTS ( true == NcCircle<NCSDCA>::_create ( 2, curve ) );

  // Transform the curve. TODO.

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a circle.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcCircle<NCSDCA>::create (
  const SlVec3<CtrPtType> &center, 
  const SlVec3<CtrPtType> &normal,
  const CtrPtType &radius,
  NcCurve<NCSDCA> &curve )
{
  // Create the unit circle.
  NC_VERIFY_RESULTS ( true == NcCircle<NCSDCA>::_create ( 3, curve ) );
/*
  TODO

  // Scale it.
  NcTransform<NCSDCA>::scale ( SlVec3d ( radius, radius, 1.0 ), curve );

  // Allign the circle with the given normal and start vector. 
  // We create a matrix that will rotate the normal to the global 
  // z-axis, and the startVec to the global x-axis.
  SlMatrix4d M;
  M.setRotationZX ( normal, startVec );

  // We actually want to rotate the opposite way. The matrix is 
  // orthogonal so we can just transpose to get the inverse.
  M.transpose();

  // Transform it.
  NcTransform<NCSDCA>::transform ( M, curve );

  // Translate it.
  NcTransform<NCSDCA>::translate ( center, curve );
*/
  // It worked.
  return true;
}


}; // namespace CadKit


#endif // _CADKIT_NURBS_CORE_LIBRARY_CREATE_CIRCLE_CLASS_H_
