
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  NcSplineData.h: Class containing the dpline definition. For internal use.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_NURBS_CORE_LIBRARY_SPLINE_DATA_CLASS_H_
#define _CADKIT_NURBS_CORE_LIBRARY_SPLINE_DATA_CLASS_H_

#include "NcInternalMacros.h"
#include "NcDefine.h"

#include "Standard/SlArrayPtr.h"


namespace CadKit
{
template<NCSDTCD> class NcSplineData
{
public:

  DECLARE_TYPEDEFS;

  /// Rational flag.
  bool rational;

  /// Integers.
  IndexType numIndepVars;
  IndexType numDepVars;
  IndexType dimension;
  IndexType totalNumCtrPts;
  IndexType totalNumKnots;

  /// Integer arrays.
  SlArrayPtr<IndexType> numCtrPts;
  SlArrayPtr<IndexType> order;
  SlArrayPtr<IndexType> degree;
  SlArrayPtr<IndexType> numKnots;

  /// Data arrays.
  SlArrayPtr<ParameterType> allKnots;
  SlArrayPtr<ControlPointType> allCtrPts;

  /// Data-pointer arrays.
  SlArrayPtr<ParameterType *> knots;
  SlArrayPtr<ControlPointType *> *ctrPts;

  /// Shortcut to the weights.
  ControlPointType *weights;

  /// Constructors/destructor.
  NcSplineData();
  NcSplineData ( const NcSplineData &spline );
  ~NcSplineData(){}

  /// Assignment operator.
  NcSplineData &                operator = ( const NcSplineData &sd ) { this->setValue ( sd ); return *this; }

  /// Resize the spline data.
  bool                          resize ( const IndexType &numIndepVars, 
                                         const IndexType &numDepVars, 
                                         const IndexType *order, 
                                         const IndexType *numCtrPts, 
                                         const bool &rational );

  /// Set the value.
  void                          setValue ( const NcSplineData &sd, bool makeCopy );

protected:

  IndexType *                   _allocateIndexArray ( const IndexType &num ) const;
  ParameterType *               _allocateParameterArray ( const IndexType &num ) const;
  ControlPointType *            _allocateControlPointArray ( const IndexType &num ) const;

  IndexType *                   _resize ( const IndexType &oldSize, const IndexType &newSize, IndexType *array );
  ParameterType *               _resize ( const IndexType &oldSize, const IndexType &newSize, ParameterType *array );
  ControlPointType *            _resize ( const IndexType &oldSize, const IndexType &newSize, ControlPointType *array );
};


///////////////////////////////////////////////////////////////////////////////
///
///  Constructor.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline NcSplineData<NCSDCA>::NcSplineData() :
  rational ( false ),
  numIndepVars ( 0 ),
  numDepVars ( 0 ),
  dimension ( 0 ),
  totalNumCtrPts ( 0 ),
  totalNumKnots ( 0 ),
  numCtrPts ( 0x0 ),
  order ( 0x0 ),
  degree ( 0x0 ),
  numKnots ( 0x0 ),
  allKnots ( 0x0 ),
  allCtrPts ( 0x0 ),
  knots ( 0x0 ),
  ctrPts ( 0x0 ),
  weights ( 0x0 )
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
///
///  Copy constructor.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline NcSplineData<NCSDCA>::NcSplineData ( const NcSplineData<NCSDCA> &sd ) :
  rational ( false ),
  numIndepVars ( 0 ),
  numDepVars ( 0 ),
  dimension ( 0 ),
  totalNumCtrPts ( 0 ),
  totalNumKnots ( 0 ),
  numCtrPts ( 0x0 ),
  order ( 0x0 ),
  degree ( 0x0 ),
  numKnots ( 0x0 ),
  allKnots ( 0x0 ),
  allCtrPts ( 0x0 ),
  knots ( 0x0 ),
  ctrPts ( 0x0 ),
  weights ( 0x0 )
{
  SL_VERIFY ( this->setValue ( sd, true ) );
}


///////////////////////////////////////////////////////////////////////////////
///
///  Allocate the array.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline IndexType *NcSplineData<NCSDCA>::_allocateIndexArray ( const IndexType &num ) const
{
  SL_ASSERT ( num > 0 );
  IndexAllocator allocator;
  IndexType *array = allocator.allocate ( num );
  SL_ASSERT ( array );
  return array;
}


///////////////////////////////////////////////////////////////////////////////
///
///  Allocate the array.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline ParameterType *NcSplineData<NCSDCA>::_allocateParameterArray ( const IndexType &num ) const
{
  SL_ASSERT ( num > 0 );
  ParameterAllocator allocator;
  ParameterType *array = allocator.allocate ( num );
  SL_ASSERT ( array );
  return array;
}


///////////////////////////////////////////////////////////////////////////////
///
///  Allocate the array.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline ControlPoint *NcSplineData<NCSDCA>::_allocateControlPointArray ( const IndexType &num ) const
{
  SL_ASSERT ( num > 0 );
  ControlPointAllocator allocator;
  ControlPoint *array = allocator.allocate ( num );
  SL_ASSERT ( array );
  return array;
}


///////////////////////////////////////////////////////////////////////////////
///
///  Resize the array if needed.
///
///////////////////////////////////////////////////////////////////////////////

template<class SizeType, class ArrayType> inline IndexType *NcSplineData<NCSDCA>::_resize ( 
  const IndexType &oldSize, 
  const IndexType &newSize, 
  IndexType *array )
{
  SL_ASSERT ( newSize > 0 );
  if ( newSize > oldSize )
    return this->_allocateIndexArray
}


///////////////////////////////////////////////////////////////////////////////
///
///  Set the value. Make a hard copy if the flag says to.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcSplineData<NCSDCA>::setValue ( const NcSplineData<NCSDCA> &sd, bool makeCopy )
{
  // If you are not making a "hard copy" (i.e., pointer assignments).
  if ( false == makeCopy )
  {
    // Assignments of the array-pointers will automatically transfer ownership.
    // If there is a current internal array it will get deleted.
    rational        = sd.rational;
    numIndepVars    = sd.numIndepVars;
    numDepVars      = sd.numDepVars;
    dimension       = sd.dimension;
    totalNumCtrPts  = sd.totalNumCtrPts;
    totalNumKnots   = sd.totalNumKnots;
    numCtrPts       = sd.numCtrPts;
    order           = sd.order;
    degree          = sd.degree;
    numKnots        = sd.numKnots;
    allKnots        = sd.allKnots;
    allCtrPts       = sd.allCtrPts;
    knots           = sd.knots;
    ctrPts          = sd.ctrPts;
    weights         = sd.weights;

    // It worked.
    return true;
  }

  //
  // If we get to here then we are making a "hard copy".
  //

  // Resize the arrays if needed.
  allKnots = this->_resize ( totalNumKnots, sd.totalNumKnots, (ParameterAllocator *) 0x0 );

  // If the number of knots is different...
  if ( totalNumKnots != sd.totalNumKnots )
  {
    // Allocate the new array.
    allKnots = this->_allocateIndexArray ( sd.totalNumKnots );
  }

  // If the number of control points is different then delete and allocate.
  if ( totalNumCtrPts != sd.totalNumCtrPts )
  {
    // Delete it if it exist.
    if ( allCtrPts ) delete [] allCtrPts;

    // Allocate the new array.
    allCtrPts = new SlFloat64[sd.totalNumCtrPts * sd.numDepVars];

    if ( !allCtrPts )
    {
      SL_PUSH_ERROR ( SL_FAILED_TO_ALLOCATE_MEMORY, "In NcSplineData::setValue(), allocation of new control points failed" );
      return SL_FALSE;
    }
  }

  // If the number of independent variables is different then delete and allocate.
  if ( numIndepVars != sd.numIndepVars )
  {
    // Delete if already exist.
    if ( numCtrPts ) delete [] numCtrPts;
    if ( order ) delete [] order;
    if ( degree ) delete [] degree;
    if ( numKnots ) delete [] numKnots;
    if ( knots ) delete [] knots;

    // Allocate the new arrays.
    numCtrPts = new SlInt32[sd.numIndepVars];
    order = new SlInt32[sd.numIndepVars];
    degree = new SlInt32[sd.numIndepVars];
    numKnots = new SlInt32[sd.numIndepVars];
    knots = new SlFloat64 *[sd.numIndepVars];

    if ( !numCtrPts || !order || !degree || !numKnots || !knots )
    {
      SL_PUSH_ERROR ( SL_FAILED_TO_ALLOCATE_MEMORY, "In NcSplineData::setValue(), allocation of new arrays failed" );
      return SL_FALSE;
    }
  }

  // If the number of dependent variables is different then delete and allocate.
  if ( numDepVars != sd.numDepVars )
  {
    // Delete if already exist.
    if ( ctrPts ) delete [] ctrPts;

    // Allocate the new one.
    ctrPts = new SlFloat64 *[sd.numDepVars];

    if ( !ctrPts )
    {
      SL_PUSH_ERROR ( SL_FAILED_TO_ALLOCATE_MEMORY, "In NcSplineData::setValue(), allocation of new arrays failed" );
      return SL_FALSE;
    }
  }

  // Set the integer members.
  rational = sd.rational;
  numIndepVars = sd.numIndepVars;
  numDepVars = sd.numDepVars;
  dimension = sd.dimension;
  totalNumCtrPts = sd.totalNumCtrPts;

  // Initialize.
  totalNumKnots = 0;

  // For each independent variable...
  for ( SlInt32 i=0; i<numIndepVars; i++ )
  {
    // Set all the integer arrays.
    numCtrPts[i] = sd.numCtrPts[i];
    order[i] = sd.order[i];
    degree[i] = sd.degree[i];
    numKnots[i] = sd.numKnots[i];

    // Set the pointers to the individual knot vectors.
    knots[i] = &allKnots[totalNumKnots];

    // Increment the total.
    totalNumKnots += numKnots[i];
  }

  // We increment the new one in the above loop. Make sure it end up being 
  // what it is supposed to be.
  SL_ASSERT ( totalNumKnots == sd.totalNumKnots );

  // Copy the knots.
  for ( i=0; i<totalNumKnots; i++ ) allKnots[i] = sd.allKnots[i];

  // Copy all the control points (including the weights).
  SlInt32 total = totalNumCtrPts * numDepVars;
  for ( i=0; i<total; i++ ) allCtrPts[i] = sd.allCtrPts[i];

  // If we are rational then set the weights to point to the start of the 
  // last dependent variable. If not then we set it to NULL.
  weights = ( rational ) ? &allCtrPts[totalNumCtrPts * dimension] : 0x0;

  // Set the pointers to the individual control points (i.e., pointer 
  // to the x's, pointer to the y's, etc).
  for ( i=0; i<numDepVars; i++ ) ctrPts[i] = &allCtrPts[totalNumCtrPts * i];

  // All is good.
  return true;
}


}; // namespace CadKit


#endif // _CADKIT_NURBS_CORE_LIBRARY_SPLINE_DATA_CLASS_H_
