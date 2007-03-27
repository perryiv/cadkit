
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

#include "Standard/SlVec4.h"
#include "Standard/SlVec3.h"
#include "Standard/SlVec2.h"


namespace CadKit
{
template<NCSDTA> class NcLine : public NcCreate<NCSDCA>
{
public:

  DECLARE_TYPEDEFS;

  // Create the line.
  static bool create ( const CtrPtType *pt0, 
                       const CtrPtType *pt1, 
                       const IndexType &dimension, 
                       NcCurve<NCSDCA> &curve );

  // Create the 1D line.
  static bool create ( const CtrPtType &pt0, 
                       const CtrPtType &pt1, 
                       NcCurve<NCSDCA> &curve );

  // Create the 2D line.
  static bool create ( const SlVec2<CtrPtType> &pt0, 
                       const SlVec2<CtrPtType> &pt1, 
                       NcCurve<NCSDCA> &curve );

  // Create the 3D line.
  static bool create ( const SlVec3<CtrPtType> &pt0, 
                       const SlVec3<CtrPtType> &pt1, 
                       NcCurve<NCSDCA> &curve );

  // Create the 4D line.
  static bool create ( const SlVec4<CtrPtType> &pt0, 
                       const SlVec4<CtrPtType> &pt1, 
                       NcCurve<NCSDCA> &curve );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Create a line.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcLine<NCSDCA>::create ( 
  const CtrPtType *pt0, 
  const CtrPtType *pt1, 
  const IndexType &dimension,
  NcCurve<NCSDCA> &curve )
{
  NC_CHECK_ARGUMENT ( 0x0 != pt0 );
  NC_CHECK_ARGUMENT ( 0x0 != pt1 );
  NC_CHECK_ARGUMENT ( dimension > 0 );

  // Reset the array sizes.
  NC_VERIFY_ALLOCATION ( curve.resize ( 1, dimension, 2, 2, false ) );

  // Build the knot vector.
  curve.setKnot ( 0, 0 );
  curve.setKnot ( 1, 0 );
  curve.setKnot ( 2, 1 );
  curve.setKnot ( 3, 1 );

  // Fill in the control points.
  for ( IndexType i = 0; i < dimension; ++i )
  {
    curve.setCtrPt ( i, 0, pt0[i] );
    curve.setCtrPt ( i, 1, pt1[i] );
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
  const CtrPtType &pt0, 
  const CtrPtType &pt1,
  NcCurve<NCSDCA> &curve )
{
  return NcLine<NCSDCA>::create ( &pt0, &pt1, 1, curve );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a line.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcLine<NCSDCA>::create (
  const SlVec2<CtrPtType> &pt0, 
  const SlVec2<CtrPtType> &pt1,
  NcCurve<NCSDCA> &curve )
{
  return NcLine<NCSDCA>::create ( pt0.getValue(), pt1.getValue(), 2, curve );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a line.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcLine<NCSDCA>::create (
  const SlVec3<CtrPtType> &pt0, 
  const SlVec3<CtrPtType> &pt1,
  NcCurve<NCSDCA> &curve )
{
  return NcLine<NCSDCA>::create ( pt0.getValue(), pt1.getValue(), 3, curve );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a line.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcLine<NCSDCA>::create (
  const SlVec4<CtrPtType> &pt0, 
  const SlVec4<CtrPtType> &pt1,
  NcCurve<NCSDCA> &curve )
{
  return NcLine<NCSDCA>::create ( pt0.getValue(), pt1.getValue(), 4, curve );
}


}; // namespace CadKit


#endif // _CADKIT_NURBS_CORE_LIBRARY_CREATE_LINE_CLASS_H_
