
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  NcSpline.h: Base class for all NURBS entities.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_NURBS_CORE_LIBRARY_SPLINE_CLASS_H_
#define _CADKIT_NURBS_CORE_LIBRARY_SPLINE_CLASS_H_

#include "NcInternalMacros.h"

#include "Standard/SlPartitionedVector.h"
#include "Standard/SlAssert.h"
#include "Standard/SlBitmask.h"


namespace CadKit
{
template<NCSDTCD> class NcSpline
{
public:

  DECLATE_TYPEDEFS;

  enum // Possible flags.
  {
    RATIONAL = 0x0001,
  };

  NcSpline();
  NcSpline ( const NcSpline &spline );
  ~NcSpline(){}

  // Get the control point.
  ControlPointType              getControlPoint ( const IndexType &whichDepVar, const IndexType &whichControlPoint ) const { return _ctrPts ( whichDepVar, whichControlPoint ); }

  // Get the control points.
  const ControlPointArray &     getControlPoints() const { return _ctrPts; }
  ControlPointArray &           getControlPoints()       { return _ctrPts; }

  // Get the degree.
  IndexType                     getDegree ( const IndexType &whichIndepVar ) const { return ( this->getOrder ( whichIndepVar ) - 1 ); }

  // Get the dimension.
  IndexType                     getDimension() const { return ( this->isRational() ) ? this->getNumDepVars() - 1 : this->getNumDepVars(); }

  // Get the knot.
  ParameterType                 getKnot ( const IndexType &whichIndepVar, const IndexType &whichKnot ) const { return _knots ( whichIndepVar, whichKnot ); }

  // Get the knot vector.
  const ParameterArray &        getKnotVector() const { return _knots; }
  ParameterArray &              getKnotVector()       { return _knots; }

  // Get the number of bytes for the types.
  static unsigned int           getNumBytesIndexType()        { return sizeof ( I ); }
  static unsigned int           getNumBytesParameterType()    { return sizeof ( P ); }
  static unsigned int           getNumBytesControlPointType() { return sizeof ( C ); }

  // Get the number of control points.
  IndexType                     getNumControlPoints ( const IndexType &whichIndepVar ) const;

  // Get the number of dependent variables.
  const IndexType &             getNumDepVars() const { return _numDepVars; }

  // Get the number of independent variables.
  const IndexType &             getNumIndepVars() const { return _numIndepVars; }

  // Get the number of knots.
  IndexType                     getNumKnots ( const IndexType &whichIndepVar ) const { return ( this->getNumControlPoints ( whichIndepVar ) + this->getOrder ( whichIndepVar ) ); }

  // Get the order.
  IndexType                     getOrder ( const IndexType &whichIndepVar ) const;

  // Get the total number of control points.
  IndexType                     getTotalNumControlPoints() const;

  // Get the total number of knots.
  IndexType                     getTotalNumKnots() const { return _knots.getData().size(); }

  // Equality test.
  bool                          isEqual    ( const NcSpline &spline ) const;
  bool                          isEqual    ( const NcSpline &spline, const ParameterType &knotTol, const ControlPointType &ctrPtTol ) const;

  // Inequality test.
  bool                          isNotEqual ( const NcSpline &spline ) const { return false == this->isEqual ( spline ); }
  bool                          isNotEqual ( const NcSpline &spline, const ParameterType &knotTol, const ControlPointType &ctrPtTol ) const { return false == this->isEqual ( spline, knotTol, ctrPtTol ); }

  // See if the given spline configuration is ok.
  bool                          isOk ( const IndexType &numIndepVars, const IndexType &numDepVars, const IndexType *order, const IndexType *numCtrPts, const bool &rational );

  // Is it rational?
  bool                          isRational() const { return this->hasFlags ( RATIONAL ); }

  // Assignment operator.
  NcSpline &                    operator = ( const NcSpline &spline ) { this->setValue ( spline ); return *this; }

  // Resize the spline data.
  bool                          resize ( const IndexType &numIndepVars, const IndexType &numDepVars, const IndexType *order, const IndexType *numCtrPts, const bool &rational );

  // Set the rational flag.
  void                          setRational ( const bool &state );

  // Set the value.
  void                          setValue ( const NcSpline &spline );

protected:

  BitMaskType _flags;

  IndexType _numIndepVars;
  IndexType _numDepVars;

  IndexArray _order;
  IndexArray _numCtrPts;

  ParameterArray    _knots;
  ControlPointArray _ctrPts;

  SL_DECLARE_BITMASK_FUNCTIONS ( Flags, BitMaskType, _flags );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline NcSpline<NCSDCA>::NcSpline() :
  _flags        ( 0 ),
  _numIndepVars ( 0 ),
  _numDepVars   ( 0 )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline NcSpline<NCSDCA>::NcSpline ( const NcSpline<NCSDCA> &spline ) :
  _flags        ( spline._flags ),
  _numIndepVars ( spline._numIndepVars ),
  _numDepVars   ( spline._numDepVars ),
  _order        ( spline._order ),
  _numCtrPts    ( spline._numCtrPts ),
  _knots        ( spline._knots ),
  _ctrPts       ( spline._ctrPts )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline void NcSpline<NCSDCA>::setValue ( const NcSpline<NCSDCA> &spline )
{
  _flags           = spline._flags;
  _numIndepVars    = spline._numIndepVars;
  _numDepVars      = spline._numDepVars;
  _order           = spline._order;
  _numCtrPts       = spline._numCtrPts;
  _knots.setValue  ( spline._knots );
  _ctrPts.setValue ( spline._ctrPts );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of control points.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline I NcSpline<NCSDCA>::getNumControlPoints ( const I &whichIndepVar ) const
{
  SL_ASSERT ( whichIndepVar == 0 || whichIndepVar > 0 ); // Make g++ happy.
  SL_ASSERT ( whichIndepVar < _numIndepVars );
  return _numCtrPts[whichIndepVar];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the order.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline I NcSpline<NCSDCA>::getOrder ( const I &whichIndepVar ) const
{
  SL_ASSERT ( whichIndepVar == 0 || whichIndepVar > 0 ); // Make g++ happy.
  SL_ASSERT ( whichIndepVar < _numIndepVars );
  return _order[whichIndepVar];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the total number of control points.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline I NcSpline<NCSDCA>::getTotalNumControlPoints() const
{
  return ( _ctrPts.getData().empty() ) ? 0 : ( _ctrPts.getData().size() ) / this->getNumDepVars();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the rational flag.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline void NcSpline<NCSDCA>::setRational ( const bool &state )
{
  if ( state )
    this->addFlags ( RATIONAL );
  else
    this->removeFlags ( RATIONAL );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the given spline configuration is ok.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcSpline<NCSDCA>::isOk (
  const IndexType &numIndepVars, 
  const IndexType &numDepVars, 
  const IndexType *order, 
  const IndexType *numCtrPts, 
  const bool &rational )
{
  // TODO.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Resize the spline data.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcSpline<NCSDCA>::resize (
  const IndexType &numIndepVars, 
  const IndexType &numDepVars, 
  const IndexType *order, 
  const IndexType *numCtrPts, 
  const bool &rational )
{
  NC_CHECK_ARGUMENT ( true == this->isOk ( numIndepVars, numDepVars, order, numCtrPts, rational ) );

  // Set the rational flag.
  this->setRational ( rational );

  // Set these members.
  _numIndepVars = numIndepVars;
  _numDepVars   = numDepVars;

  // Resize the integer arrays.
  _order.resize     ( numIndepVars );
  _numCtrPts.resize ( numIndepVars );

  // Check allocation.
  NC_CHECK_ALLOCATION ( numIndepVars == _order.size() );
  NC_CHECK_ALLOCATION ( numIndepVars == _numCtrPts.size() );

  // Get shortcuts to the knot indices.
  IndexArray &knotVectorStart = _knots.getIndices();

  // Resize the indices of the partitioned array.
  knotVectorStart.resize ( numIndepVars );

  // Check allocation.
  NC_CHECK_ALLOCATION ( numIndepVars == knotVectorStart.size() );

  // Needed below.
  IndexType i ( 0 ), totalNumKnots ( 0 ), totalNumCtrPts ( 1 );

  // Loop through the independent variables.
  for ( i = 0; i < numIndepVars; ++i )
  {
    // Set these array elements.
    _order[i]     = order[i];
    _numCtrPts[i] = numCtrPts[i];

    // Set the index for the start of this knot vector.
    knotVectorStart[i] = totalNumKnots;

    // Update the total number of knots thus far.
    totalNumKnots += ( _numCtrPts[i] + _order[i] );

    // Update the total number of control points thus far.
    totalNumCtrPts *= _numCtrPts[i];
  }

  // Resize the knots.
  _knots.getData().resize ( totalNumKnots );

  // Check allocation.
  NC_CHECK_ALLOCATION ( totalNumKnots == _knots.getData().size() );

  // Get shortcuts to the control point indices.
  IndexArray &ctrPtsStart = _ctrPts.getIndices();

  // Resize the indices of the partitioned array.
  ctrPtsStart.resize ( numDepVars );

  // Check allocation.
  NC_CHECK_ALLOCATION ( numDepVars == ctrPtsStart.size() );

  // Loop through the dependent variables.
  for ( i = 0; i < numDepVars; ++i )
  {
    // Set the index for the start of this dependent variable.
    ctrPtsStart[i] = totalNumCtrPts * i;
  }

  // Resize the control points.
  IndexType size ( totalNumCtrPts * numDepVars );
  _ctrPts.getData().resize ( size );

  // Check allocation.
  NC_CHECK_ALLOCATION ( size == _ctrPts.getData().size() );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Equality.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcSpline<NCSDCA>::isEqual ( const NcSpline<NCSDCA> &spline ) const
{
  return (
    _flags          == spline._flags &&
    _numIndepVars   == spline._numIndepVars &&
    _numDepVars     == spline._numDepVars &&
    _order          == spline._order &&
    _numCtrPts      == spline._numCtrPts &&
    _knots.isEqual  ( spline._knots ) &&
    _ctrPts.isEqual ( spline._ctrPts )
    );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Equality.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcSpline<NCSDCA>::isEqual 
  ( const NcSpline<NCSDCA> &spline, const ParameterType &knotTol, const ControlPointType &ctrPtTol ) const
{
  return (
    _flags          == spline._flags &&
    _numIndepVars   == spline._numIndepVars &&
    _numDepVars     == spline._numDepVars &&
    _order          == spline._order &&
    _numCtrPts      == spline._numCtrPts &&
    _knots.isEqual  ( spline._knots, knotTol ) &&
    _ctrPts.isEqual ( spline._ctrPts, ctrPtTol )
    );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Equality.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool operator == ( const NcSpline<NCSDCA> &a, const NcSpline<NCSDCA> &b )
{
  return a.isEqual ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Inequality.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool operator != ( const NcSpline<NCSDCA> &a, const NcSpline<NCSDCA> &b )
{
  return a.isNotEqual ( b );
}


}; // namespace CadKit


#endif // _CADKIT_NURBS_CORE_LIBRARY_SPLINE_CLASS_H_
