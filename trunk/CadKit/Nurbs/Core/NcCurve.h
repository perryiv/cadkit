
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  NcCurve.h: NURBS curve class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_NURBS_CORE_LIBRARY_CURVE_CLASS_H_
#define _CADKIT_NURBS_CORE_LIBRARY_CURVE_CLASS_H_

#include "NcSpline.h"

#include "Standard/SlVec2.h"
#include "Standard/SlVec3.h"
#include "Standard/SlVec4.h"


namespace CadKit
{
template<NCSDTCD> class NcCurve : public NcSpline<NCSDCA>
{
public:

  DECLATE_TYPEDEFS;

  NcCurve(){}
  NcCurve ( const NcCurve<NCSDCA> &sd ) : NcSpline<NCSDCA> ( sd ){}
  ~NcCurve(){}

  // Assignment operator.
  NcCurve &                     operator = ( const NcCurve &sd ) { this->setValue ( sd ); return *this; }

  // Set the value.
  void                          setValue ( const NcSpline<NCSDCA> &spline ) { return _sd.setData ( spline._sd ); }

  // Resize the curve.
  bool                          resize ( const IndexType &numIndepVars, const IndexType &numDepVars, const IndexType &order, const IndexType &numCtrPts, const bool &rational );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Resize the curve.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcCurve<NCSDCA>::resize (
  const IndexType &numIndepVars, 
  const IndexType &numDepVars, 
  const IndexType &order, 
  const IndexType &numCtrPts, 
  const bool &rational )
{
  return NcSpline<NCSDCA>::resize ( numIndepVars, numDepVars, &order, &numCtrPts, rational );
}


}; // namespace CadKit


#endif // _CADKIT_NURBS_CORE_LIBRARY_CURVE_CLASS_H_
