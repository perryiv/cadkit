
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
#include "NcEvaluate.h"


namespace CadKit
{
template<NCSDTCD> class NcCurve : public NcSpline<NCSDCA>
{
public:

  DECLARE_TYPEDEFS;

  NcCurve(){}
  NcCurve ( const NcCurve<NCSDCA> &sd ) : NcSpline<NCSDCA> ( sd ){}
  ~NcCurve(){}

  /// Calculate the basis functions.
  void                          basisFunctions ( const ParameterType &u, ParameterType *N ) const { NcSpline<NCSDCA>::basisFunctions ( 0, u, N ); }
  void                          basisFunctions ( const ParameterType &u, const IndexType &span, ParameterType *N ) const { NcSpline<NCSDCA>::basisFunctions ( 0, u, span, N ); }

  /// Evaluate the point.
  void                          evaluate ( const ParameterType &u, ControlPointType *pt );

  /// Find the span in the knot vector given the parameter.
  IndexType                     findSpan ( const ParameterType &u ) const { return this->findSpan ( u, this->getDegree() ); }
  IndexType                     findSpan ( const ParameterType &u, const IndexType &low ) const { return NcSpline<NCSDCA>::findSpan ( 0, u, low ); }

  /// Get the degree.
  IndexType                     getDegree() const { return NcSpline<NCSDCA>::getDegree ( 0 ); }

  /// Get the first knot.
  ParameterType                 getFirstKnot() const { return NcSpline<NCSDCA>::getFirstKnot ( 0 ); }

  /// Get the last knot.
  ParameterType                 getLastKnot() const { return NcSpline<NCSDCA>::getLastKnot ( 0 ); }

  /// Get the order.
  IndexType                     getOrder() const { return NcSpline<NCSDCA>::getOrder ( 0 ); }

  // See if the parameter is in range.
  bool                          isInRange ( const ParameterType &u ) const;

  /// Assignment operator.
  NcCurve &                     operator = ( const NcCurve &sd ) { this->setValue ( sd ); return *this; }

  /// Set the value.
  void                          setValue ( const NcSpline<NCSDCA> &spline ) { return _sd.setData ( spline._sd ); }

  /// Resize the curve.
  bool                          resize ( const IndexType &numIndepVars, const IndexType &numDepVars, const IndexType &order, const IndexType &numCtrPts, const bool &rational ) { return NcSpline<NCSDCA>::resize ( numIndepVars, numDepVars, &order, &numCtrPts, rational ); }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Evaluate the point.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline void NcCurve<NCSDCA>::evaluate ( const ParameterType &u, ControlPointType *pt )
{
  SL_ASSERT ( NULL != pt );
  SL_ASSERT ( this->isInRange ( u ) );

  // Get the order.
  const IndexType order ( this->getOrder() );

  // Get the span.
  IndexType span = this->findSpan ( u );

  // Get the basis functions.
  ParameterType *N = _work.getBasisFunctions ( 0 );
  this->basisFunctions ( u, span, N );

  // Call the function to evaluate a point.
  NcEvaluate<NCSDCA>::evaluate ( order, span, u, N, pt );
}


}; // namespace CadKit


#endif // _CADKIT_NURBS_CORE_LIBRARY_CURVE_CLASS_H_
