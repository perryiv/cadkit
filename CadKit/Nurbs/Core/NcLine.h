
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  NcLine.h: Class to construct a NURBS curve as a line.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_NURBS_CORE_LIBRARY_CREATE_LINE_CLASS_H_
#define _CADKIT_NURBS_CORE_LIBRARY_CREATE_LINE_CLASS_H_

#include "NcCreate.h"
#include "NcCurve.h"


namespace CadKit
{
template<NCSDTA> class NcLine : public NcCreate<NCSDCA>
{
public:

  DECLARE_TYPEDEFS;

  // Create the line.
  static bool create ( const ControlPointType *pt1, 
                       const ControlPointType *pt2, 
                       const IndexType &dimension, 
                       NcCurve<NCSDCA> &curve );

  // Create the 1D line.
  static bool create ( const ControlPointType &pt1, 
                       const ControlPointType &pt2, 
                       NcCurve<NCSDCA> &curve );

  // Create the 2D line.
  static bool create ( const SlVec2<ControlPointType> &pt1, 
                       const SlVec2<ControlPointType> &pt2, 
                       NcCurve<NCSDCA> &curve );

  // Create the 3D line.
  static bool create ( const SlVec3<ControlPointType> &pt1, 
                       const SlVec3<ControlPointType> &pt2, 
                       NcCurve<NCSDCA> &curve );

  // Create the 4D line.
  static bool create ( const SlVec4<ControlPointType> &pt1, 
                       const SlVec4<ControlPointType> &pt2, 
                       NcCurve<NCSDCA> &curve );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Create a line.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcLine<NCSDCA>::create ( 
  const ControlPointType *pt1, 
  const ControlPointType *pt2, 
  const IndexType &dimension,
  NcCurve<NCSDCA> &curve )
{
  NC_CHECK_ARGUMENT ( NULL != pt1 );
  NC_CHECK_ARGUMENT ( NULL != pt2 );
  NC_CHECK_ARGUMENT ( dimension > 0 );

  // Reset the array sizes.
  if ( false == curve.resize ( 1, dimension, 2, 2, false ) )
    return false;

  // Build the knot vector.
  ParameterArray &knots = curve.getKnotVector();
  knots(0) = 0;
  knots(1) = 0;
  knots(2) = 1;
  knots(3) = 1;

  // Fill in the control points.
  ControlPointArray &ctrPts = curve.getControlPoints();
  for ( IndexType i = 0; i < dimension; ++i )
  {
    ctrPts(i,0) = pt1[i];
    ctrPts(i,1) = pt2[i];
  }

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a line.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcLine<NCSDCA>::create (
  const ControlPointType &pt1, 
  const ControlPointType &pt2,
  NcCurve<NCSDCA> &curve )
{
  return NcLine<NCSDCA>::create ( &pt1, &pt2, 1, curve );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a line.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcLine<NCSDCA>::create (
  const SlVec2<ControlPointType> &pt1, 
  const SlVec2<ControlPointType> &pt2,
  NcCurve<NCSDCA> &curve )
{
  return NcLine<NCSDCA>::create ( pt1.getValue(), pt2.getValue(), 2, curve );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a line.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcLine<NCSDCA>::create (
  const SlVec3<ControlPointType> &pt1, 
  const SlVec3<ControlPointType> &pt2,
  NcCurve<NCSDCA> &curve )
{
  return NcLine<NCSDCA>::create ( pt1.getValue(), pt2.getValue(), 3, curve );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a line.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcLine<NCSDCA>::create (
  const SlVec4<ControlPointType> &pt1, 
  const SlVec4<ControlPointType> &pt2,
  NcCurve<NCSDCA> &curve )
{
  return NcLine<NCSDCA>::create ( pt1.getValue(), pt2.getValue(), 4, curve );
}


}; // namespace CadKit


#endif // _CADKIT_NURBS_CORE_LIBRARY_CREATE_LINE_CLASS_H_
