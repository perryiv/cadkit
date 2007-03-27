
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

#include "NcSplineData.h"
#include "NcWork.h"


namespace CadKit
{
template<NCSDTCD> class NcSpline : public NcSplineData<NCSDCA>
{
public:

  DECLARE_TYPEDEFS;

  NcSpline();
  NcSpline ( const NcSpline &spline );
  ~NcSpline(){}

  /// Get the work space.
  NcWork<NCSDCA> &              getWork() const { return _work; }

  /// Equality test.
  bool                          isEqual    ( const NcSpline &spline ) const;
  bool                          isEqual    ( const NcSpline &spline, const ParamType &knotTol, const CtrPtType &ctrPtTol ) const;

  /// Inequality test.
  bool                          isNotEqual ( const NcSpline &spline ) const { return false == this->isEqual ( spline ); }
  bool                          isNotEqual ( const NcSpline &spline, const ParamType &knotTol, const CtrPtType &ctrPtTol ) const { return false == this->isEqual ( spline, knotTol, ctrPtTol ); }

  /// Check the state.
  bool                          isAllocated() const { return ( 0 != _numIndepVars && 0 != _numDepVars ); }

  /// Assignment operator.
  NcSpline &                    operator = ( const NcSpline &spline ) { SL_VERIFY ( this->setValue ( spline ) ); return *this; }

  /// Resize the spline.
  bool                          resize ( 
                                  const IndexType &numIndepVars, 
                                  const IndexType &numDepVars, 
                                  const IndexType *order, 
                                  const IndexType *numCtrPts, 
                                  const bool &rational );

  /// Set the value.
  bool                          setValue ( const NcSpline &spline );

protected:

  BitMaskType _flags;
  mutable NcWork<NCSDCA> _work;
};


///////////////////////////////////////////////////////////////////////////////
///
/// Constructor.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline NcSpline<NCSDCA>::NcSpline() : 
  NcSplineData<NCSDCA>(),
  _flags ( 0 )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
///
/// Copy constructor.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline NcSpline<NCSDCA>::NcSpline ( const NcSpline<NCSDCA> &spline ) :
  NcSplineData<NCSDCA> ( spline ),
  _flags ( spline._flags ),
  _work  ( spline._work )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Allocate the arrays and set the integer members.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcSpline<NCSDCA>::resize ( 
  const IndexType &numIndepVars, 
  const IndexType &numDepVars, 
  const IndexType *order, 
  const IndexType *numCtrPts, 
  const bool &rational )
{
  // Call the base class's function.
  NC_VERIFY_ALLOCATION ( NcSplineData<NCSDCA>::resize ( numIndepVars, numDepVars, order, numCtrPts, rational ) );

  // Resize the work space.
  NC_VERIFY_ALLOCATION ( _work.resize ( numIndepVars, order ) );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
///
/// Set the value.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcSpline<NCSDCA>::setValue ( const NcSpline<NCSDCA> &spline )
{
  // Call the base class's function.
  NC_CHECK_EXPRESSION ( NcSplineData<NCSDCA>::setValue ( spline ) );

  // Set the members.
  _flags = spline._flags;

  // Set the work.
  NC_CHECK_EXPRESSION ( _work.setValue ( spline._work ) );

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
///
/// Equality.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcSpline<NCSDCA>::isEqual 
  ( const NcSpline<NCSDCA> &spline, const ParamType &knotTol, const CtrPtType &ctrPtTol ) const
{
  return (
    _flags == spline._flags &&
    true == NcSplineData<NCSDCA>::isEqual ( spline, knotTol, ctrPtTol )
    );
}


///////////////////////////////////////////////////////////////////////////////
///
/// Equality.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcSpline<NCSDCA>::isEqual ( const NcSpline<NCSDCA> &spline ) const
{
  return (
    _flags == spline._flags &&
    true == NcSplineData<NCSDCA>::isEqual ( spline )
    );
}


///////////////////////////////////////////////////////////////////////////////
///
/// Equality. This is not a member.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool operator == ( const NcSpline<NCSDCA> &a, const NcSpline<NCSDCA> &b )
{
  return a.isEqual ( b );
}


///////////////////////////////////////////////////////////////////////////////
///
/// Inequality. This is not a member.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool operator != ( const NcSpline<NCSDCA> &a, const NcSpline<NCSDCA> &b )
{
  return a.isNotEqual ( b );
}


}; // namespace CadKit


#endif // _CADKIT_NURBS_CORE_LIBRARY_SPLINE_CLASS_H_
