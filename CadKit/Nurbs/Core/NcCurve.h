
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

  DECLARE_TYPEDEFS;

  NcCurve(){}
  NcCurve ( const NcCurve<NCSDCA> &sd ) : NcSpline<NCSDCA> ( sd ){}
  ~NcCurve(){}

  /// Find the span in the knot vector given the parameter.
  IndexType                     findSpan ( const ParameterType &u ) const { return this->findSpan ( u, this->getDegree() ); }
  IndexType                     findSpan ( const ParameterType &u, const IndexType &low ) const { return NcSpline<NCSDCA>::findSpan ( 0, u, low ); }

  /// Get the degree.
  IndexType                     getDegree() const { return NcSpline<NCSDCA>::getDegree ( 0 ); }

  /// Get the first knot.
  ParameterType                 getFirstKnot() const { return NcSpline<NCSDCA>::getFirstKnot ( 0 ); }

  /// Get the last knot.
  ParameterType                 getLastKnot() const { return NcSpline<NCSDCA>::getLastKnot ( 0 ); }

  /// Assignment operator.
  NcCurve &                     operator = ( const NcCurve &sd ) { this->setValue ( sd ); return *this; }

  /// Set the value.
  void                          setValue ( const NcSpline<NCSDCA> &spline ) { return _sd.setData ( spline._sd ); }

  /// Resize the curve.
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
