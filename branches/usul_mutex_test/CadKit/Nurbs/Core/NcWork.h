
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  NcWork.h: Work space for a spline.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_NURBS_CORE_LIBRARY_WORK_CLASS_H_
#define _CADKIT_NURBS_CORE_LIBRARY_WORK_CLASS_H_

#include "NcInternalMacros.h"

#include <vector>
#include <algorithm>


namespace CadKit
{
template<NCSDTCD> class NcWork
{
public:

  DECLARE_TYPEDEFS;

  NcWork();
  NcWork ( const NcWork &work );
  ~NcWork(){}

  /// Get the basis function work space.
  ParamType *                   getBasisFunctionsLeft  ( const IndexType &whichIndepVar );
  ParamType *                   getBasisFunctionsRight ( const IndexType &whichIndepVar );
  ParamType *                   getBasisFunctions      ( const IndexType &whichIndepVar );

  /// Assignment operator.
  NcWork &                      operator = ( const NcWork &work ) { SL_VERIFY ( this->setValue ( work ) ); return *this; }

  /// Resize the work space.
  bool                          resize ( const IndexType &numIndepVars, const IndexType *order );

  /// Set the value.
  bool                          setValue ( const NcWork &work );

protected:

  IndexType _maxOrder;
  std::vector<ParamType> _left;  // Use when calculating blending functions.
  std::vector<ParamType> _right; // Use when calculating blending functions.
  std::vector<ParamType> _basis; // Blending functions.
};


///////////////////////////////////////////////////////////////////////////////
///
///  Constructor.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline NcWork<NCSDCA>::NcWork() :
  _maxOrder ( 0 )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
///
///  Copy constructor.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline NcWork<NCSDCA>::NcWork ( const NcWork<NCSDCA> &work ) :
  _maxOrder ( work._maxOrder ),
  _left     ( work._left ),
  _right    ( work._right ),
  _basis    ( work._basis )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
///
///  Set the value.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcWork<NCSDCA>::setValue ( const NcWork<NCSDCA> &work )
{
  _maxOrder = work._maxOrder;
  _left     = work._left;
  _right    = work._right;
  _basis    = work._basis;

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
///
///  Get the basis function work space.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline ParamType *NcWork<NCSDCA>::getBasisFunctionsLeft ( const IndexType &whichIndepVar )
{
  SL_ASSERT ( 0 == whichIndepVar || 0 < whichIndepVar );
  SL_ASSERT ( static_cast<std::vector<ParamType>::size_type> ( whichIndepVar * _maxOrder + _maxOrder ) <= _left.size() );

  return &(_left[whichIndepVar * _maxOrder]);
}


///////////////////////////////////////////////////////////////////////////////
///
///  Get the basis function work space.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline ParamType *NcWork<NCSDCA>::getBasisFunctionsRight ( const IndexType &whichIndepVar )
{
  SL_ASSERT ( 0 == whichIndepVar || 0 < whichIndepVar );
  SL_ASSERT ( static_cast<std::vector<ParamType>::size_type> ( whichIndepVar * _maxOrder + _maxOrder ) <= _right.size() );

  return &(_right[whichIndepVar * _maxOrder]);
}


///////////////////////////////////////////////////////////////////////////////
///
///  Get the basis function work space.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline ParamType *NcWork<NCSDCA>::getBasisFunctions ( const IndexType &whichIndepVar )
{
  SL_ASSERT ( 0 == whichIndepVar || 0 < whichIndepVar );
  SL_ASSERT ( static_cast<std::vector<ParamType>::size_type> ( whichIndepVar * _maxOrder + _maxOrder ) <= _basis.size() );

  return &(_basis[whichIndepVar * _maxOrder]);
}


///////////////////////////////////////////////////////////////////////////////
///
///  Resize the work space.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcWork<NCSDCA>::resize ( const IndexType &numIndepVars, const IndexType *order )
{
  SL_ASSERT ( numIndepVars > 0 );
  SL_ASSERT ( order );

  // Figure out the maximum order.
  _maxOrder = *(std::max_element ( order, order + numIndepVars ));
  SL_ASSERT ( _maxOrder > 1 );

  // The new size.
  const IndexType newSize ( numIndepVars * _maxOrder );

  // Resize the left and right basis function storage arrays.
  _left.resize  ( newSize );
  _right.resize ( newSize );
  _basis.resize ( newSize );

  // Check allocation.
  NC_CHECK_ALLOCATION ( newSize == _left.size() );
  NC_CHECK_ALLOCATION ( newSize == _right.size() );
  NC_CHECK_ALLOCATION ( newSize == _basis.size() );

  // It worked.
  return true;
}


}; // namespace CadKit


#endif // _CADKIT_NURBS_CORE_LIBRARY_WORK_CLASS_H_
