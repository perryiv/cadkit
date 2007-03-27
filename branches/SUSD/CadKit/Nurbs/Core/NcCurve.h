
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


namespace CadKit
{
template<NCSDTCD> class NcCurve : public NcSpline<NCSDCA>
{
public:

  DECLARE_TYPEDEFS;

  NcCurve(){}
  NcCurve ( const NcCurve<NCSDCA> &sd ) : NcSpline<NCSDCA> ( sd ){}
  ~NcCurve(){}

  /// Get the degree.
  const IndexType &             getDegree() const { return NcSpline<NCSDCA>::getDegree ( 0 ); }

  /// Get the first knot.
  const ParamType &             getFirstKnot() const { return NcSpline<NCSDCA>::getFirstKnot ( 0 ); }

  /// Get the knots.
  const ParamType *             getKnots() const { return NcSpline<NCSDCA>::getKnots ( 0 ); }

  /// Get the last knot.
  const ParamType &             getLastKnot() const { return NcSpline<NCSDCA>::getLastKnot ( 0 ); }

  /// Get the order.
  const IndexType &             getOrder() const { return NcSpline<NCSDCA>::getOrder ( 0 ); }

  // See if the parameter is in range.
  bool                          isInRange ( const ParamType &u ) const { return NcSpline<NCSDCA>::isInRange ( 0, u ); }

  /// Assignment operator.
  NcCurve &                     operator = ( const NcCurve &curve ) { SL_VERIFY ( this->setValue ( curve ) ); return *this; }

  /// Set the value.
  bool                          setValue ( const NcCurve<NCSDCA> &curve ) { return NcSpline<NCSDCA>::setData ( curve ); }

  /// Resize the curve.
  bool                          resize ( const IndexType &numIndepVars, const IndexType &numDepVars, const IndexType &order, const IndexType &numCtrPts, const bool &rational ) { return NcSpline<NCSDCA>::resize ( numIndepVars, numDepVars, &order, &numCtrPts, rational ); }

  /// Set the knot.
  void                          setKnot   ( const IndexType &whichKnot, const ParamType &knot ) { NcSpline<NCSDCA>::setKnot ( 0, whichKnot, knot ); }
};


}; // namespace CadKit


#endif // _CADKIT_NURBS_CORE_LIBRARY_CURVE_CLASS_H_
