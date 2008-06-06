
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  NcSplineData.h: Class containing the spline definition. For internal use.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_NURBS_CORE_LIBRARY_SPLINE_DATA_CLASS_H_
#define _CADKIT_NURBS_CORE_LIBRARY_SPLINE_DATA_CLASS_H_

#include "NcInternalMacros.h"
#include "NcDefine.h"

#include <functional>


namespace CadKit
{
template<NCSDTCD> class NcSplineData
{
public:

  DECLARE_TYPEDEFS;

  /// Constructors/destructor.
  NcSplineData();
  NcSplineData ( const NcSplineData &sd );
  ~NcSplineData();

  /// Rational flag.
  const bool &                  isRational() const { return _rational; }
  void                          setRational ( const bool &state );

  /// Get integer members.
  const IndexType &             getNumIndepVars()   const { return _numIndepVars; }
  const IndexType &             getNumDepVars()     const { return _numDepVars; }
  const IndexType &             getDimension()      const { return _dimension; }
  const IndexType &             getTotalNumCtrPts() const { return _totalNumCtrPts; }
  const IndexType &             getTotalNumKnots()  const { return _totalNumKnots; }

  /// Get integer values.
  const IndexType &             getNumCtrPts ( const IndexType &whichIndepVar ) const { return _numCtrPts[whichIndepVar]; }
  const IndexType &             getOrder     ( const IndexType &whichIndepVar ) const { return _order[whichIndepVar]; }
  const IndexType &             getDegree    ( const IndexType &whichIndepVar ) const { return _degree[whichIndepVar]; }
  const IndexType &             getNumKnots  ( const IndexType &whichIndepVar ) const { return _numKnots[whichIndepVar]; }

  /// Get integer arrays as const.
  const IndexType *             getNumCtrPts() const { return _numCtrPts; }
  const IndexType *             getOrder()     const { return _order; }
  const IndexType *             getDegree()    const { return _degree; }
  const IndexType *             getNumKnots()  const { return _numKnots; }

  /// Get data values.
  const ParamType &             getKnot   ( const IndexType &whichIndepVar, const IndexType &whichKnot )  const { return _knots[whichIndepVar][whichKnot]; }
  const CtrPtType &             getCtrPt  ( const IndexType &whichDepVar,   const IndexType &whichCtrPt ) const { return _ctrPts[whichDepVar][whichCtrPt]; }
  const CtrPtType &             getWeight ( const IndexType &whichWeight )                                const { return _weights[whichWeight]; }

  /// Set data values.
  void                          setKnot   ( const IndexType &whichIndepVar, const IndexType &whichKnot,   const ParamType &knot )  { _knots[whichIndepVar][whichKnot] = knot; }
  void                          setCtrPt  ( const IndexType &whichDepVar,   const IndexType &whichCtrPt,  const CtrPtType &ctrPt ) { _ctrPts[whichDepVar][whichCtrPt] = ctrPt; }
  void                          setWeight ( const IndexType &whichWeight,   const CtrPtType &weight )                              { _weights[whichWeight] = weight; }

  /// Get data arrays as const.
  const ParamType *             getAllKnots()  const { return _allKnots; }
  const CtrPtType *             getAllCtrPts() const { return _allCtrPts; }

  /// Get data arrays as const.
  const ParamType *             getKnots ( const IndexType &whichIndepVar ) const { return _knots[whichIndepVar]; }
  const CtrPtType *             getCtrPts ( const IndexType &whichDepVar )  const { return _ctrPts[whichDepVar]; }

  /// Get data-pointer arrays as const.
  const ParamType **            getKnots()  const { return const_cast<const ParamType **> ( _knots ); }
  const CtrPtType **            getCtrPts() const { return const_cast<const CtrPtType **> ( _ctrPts ); }

  /// Get shortcut to the _weights.
  const CtrPtType *             getWeights() const { return _weights; }

  /// Get the number of bytes for the types.
  static unsigned int           getNumBytesIndexType()        { return sizeof ( IndexType ); }
  static unsigned int           getNumBytesParamType()    { return sizeof ( ParamType ); }
  static unsigned int           getNumBytesCtrPtType() { return sizeof ( CtrPtType ); }

  /// Get the first and last knot.
  const ParamType &             getFirstKnot ( const IndexType &whichIndepVar ) const { return this->getKnot ( whichIndepVar, 0 ); }
  const ParamType &             getLastKnot  ( const IndexType &whichIndepVar ) const { return this->getKnot ( whichIndepVar, this->getNumKnots ( whichIndepVar ) - 1 ); }

  /// Assignment operator.
  NcSplineData &                operator = ( const NcSplineData &sd ) { SL_VERIFY ( this->setValue ( sd ) ); return *this; }

  /// Resize the spline data.
  bool                          resize ( 
                                  const IndexType &numIndepVars, 
                                  const IndexType &numDepVars, 
                                  const IndexType *order, 
                                  const IndexType *numCtrPts, 
                                  const bool &rational );

  /// Set the value.
  bool                          setValue ( const NcSplineData &sd, bool makeCopy );

  /// Validate.
  bool                          isValid() const;
  static bool                   isValid ( 
                                  const IndexType &numIndepVars, 
                                  const IndexType &numDepVars, 
                                  const IndexType *order, 
                                  const IndexType *numCtrPts );

  /// Confirm parametric range.
  bool                          isInRange ( const IndexType &whichIndepVar, const ParamType &u ) const;

  /// Equality test.
  bool                          isEqual    ( const NcSplineData &spline ) const;
  bool                          isEqual    ( const NcSplineData &spline, const ParamType &knotTol, const CtrPtType &ctrPtTol ) const;

  /// Inequality test.
  bool                          isNotEqual ( const NcSplineData &spline ) const { return false == this->isEqual ( spline ); }
  bool                          isNotEqual ( const NcSplineData &spline, const ParamType &knotTol, const CtrPtType &ctrPtTol ) const { return false == this->isEqual ( spline, knotTol, ctrPtTol ); }

protected:

  /// Rational flag.
  bool _rational;

  /// Integers.
  IndexType _numIndepVars;
  IndexType _numDepVars;
  IndexType _dimension;
  IndexType _totalNumCtrPts;
  IndexType _totalNumKnots;

  /// Integer arrays.
  IndexType *_numCtrPts;
  IndexType *_order;
  IndexType *_degree;
  IndexType *_numKnots;

  /// Data arrays.
  ParamType *_allKnots;
  CtrPtType *_allCtrPts;

  /// Data-pointer arrays.
  ParamType **_knots;
  CtrPtType **_ctrPts;

  /// Shortcut to the _weights.
  CtrPtType *_weights;

  /// Delete the internal arrays.
  void                          _delete();

  /// Test the equality of the integers.
  bool                          _isEqualIntegers ( const NcSplineData<NCSDCA> &spline ) const;
};


///////////////////////////////////////////////////////////////////////////////
///
/// Constructor initializer list.
///
///////////////////////////////////////////////////////////////////////////////

#define _NC_CONSTRUCTOR_INITIALIZER_LIST \
  _rational       ( false ), \
  _numIndepVars   ( 0 ), \
  _numDepVars     ( 0 ), \
  _dimension      ( 0 ), \
  _totalNumCtrPts ( 0 ), \
  _totalNumKnots  ( 0 ), \
  _numCtrPts      ( 0x0 ), \
  _order          ( 0x0 ), \
  _degree         ( 0x0 ), \
  _numKnots       ( 0x0 ), \
  _allKnots       ( 0x0 ), \
  _allCtrPts      ( 0x0 ), \
  _knots          ( 0x0 ), \
  _ctrPts         ( 0x0 ), \
  _weights        ( 0x0 )


///////////////////////////////////////////////////////////////////////////////
///
/// Constructor.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline NcSplineData<NCSDCA>::NcSplineData() :
  _NC_CONSTRUCTOR_INITIALIZER_LIST
{
  // Empty.
}


///////////////////////////////////////////////////////////////////////////////
///
/// Copy constructor.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline NcSplineData<NCSDCA>::NcSplineData ( const NcSplineData<NCSDCA> &sd ) :
  _NC_CONSTRUCTOR_INITIALIZER_LIST
{
  SL_VERIFY ( this->setValue ( sd, true ) );
}


///////////////////////////////////////////////////////////////////////////////
///
/// Destructor.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline NcSplineData<NCSDCA>::~NcSplineData()
{
  this->_delete();
}


///////////////////////////////////////////////////////////////////////////////
///
/// Delete the internal arrays.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline void NcSplineData<NCSDCA>::_delete()
{
  IndexAllocator ia;
  ia.deallocate ( _numCtrPts, _numIndepVars );
  ia.deallocate ( _order,     _numIndepVars );
  ia.deallocate ( _degree,    _numIndepVars );
  ia.deallocate ( _numKnots,  _numIndepVars );

  ParameterPointerAllocator ppa;
  ppa.deallocate ( _knots, _numIndepVars );

  ControlPointPointerAllocator cppa;
  cppa.deallocate ( _ctrPts, _numDepVars );

  ParameterAllocator pa;
  pa.deallocate ( _allKnots, _totalNumKnots );

  ControlPointAllocator ca;
  ca.deallocate ( _allCtrPts, static_cast<IndexType>(_totalNumCtrPts * _numDepVars) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the given spline configuration is valid.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcSplineData<NCSDCA>::isValid ( 
  const IndexType &numIndepVars, 
  const IndexType &numDepVars, 
  const IndexType *order, 
  const IndexType *numCtrPts )
{
  if ( numIndepVars < CADKIT_NURBS_MINIMUM_NUMBER_OF_INDEPENDENT_VARIABLES )
    return false;

  if ( numDepVars < CADKIT_NURBS_MINIMUM_NUMBER_OF_DEPENDENT_VARIABLES )
    return false;

  if ( 0x0 == numCtrPts )
    return false;

  if ( 0x0 == order )
    return false;

  for ( IndexType i = 0; i < numIndepVars; ++i )
  {
    if ( numCtrPts[i] < CADKIT_NURBS_MINIMUM_NUMBER_OF_CONTROL_POINTS )
      return false;

    if ( order[i] < CADKIT_NURBS_MINIMUM_ORDER )
      return false;
  }

  // It's ok.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
///
/// See if the current spline configuration is valid.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcSplineData<NCSDCA>::isValid() const
{
  // Call the other one.
  if ( false == this->isValid ( _numIndepVars, _numDepVars, _order, _numCtrPts ) )
    return false;

  // TODO.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
///
/// Set the rational flag. All this does is set the flag. If the spline 
/// really is rational (i.e., it has weights) and you call this with false, 
/// the weights will be ignored and the homogeneous control points will be 
/// treated like regular coordinates. If the spline is nonrational and you 
/// call this with true, then weights will be assumed. If the spline never 
/// was rational to begin with, and you call this with true, chances are your 
/// program will crash some time after because internally there will be 
/// instances where it will walk off the end of an array. Only call this for 
/// splines that are originally rational. To determine if a spline was/is 
/// actually rational see if the number of dependent variables is equal to 
/// the dimension.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline void NcSplineData<NCSDCA>::setRational ( const bool &state )
{
  NC_CHECK_THIS ( this->isValid() );

  // Set the flag.
  _rational = state;

  // If we are rational then set the weights to point to the start of the 
  // last dependent variable. If not then we set it to NULL.
  _weights = ( _rational ) ? &_allCtrPts[static_cast<IndexType>(_totalNumCtrPts * _dimension)] : 0x0;
}


///////////////////////////////////////////////////////////////////////////////
///
/// Test the equality of the integers.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcSplineData<NCSDCA>::_isEqualIntegers ( const NcSplineData<NCSDCA> &spline ) const
{
  return (
    _rational       == spline._rational &&
    _numIndepVars   == spline._numIndepVars &&
    _numDepVars     == spline._numDepVars &&
    _dimension      == spline._dimension &&
    _totalNumCtrPts == spline._totalNumCtrPts &&
    _totalNumKnots  == spline._totalNumKnots &&
    true == std::equal ( _numCtrPts, _numCtrPts + _numIndepVars, spline._numCtrPts ) &&
    true == std::equal ( _order,     _order     + _numIndepVars, spline._order     ) &&
    true == std::equal ( _degree,    _degree    + _numIndepVars, spline._degree    ) &&
    true == std::equal ( _numKnots,  _numKnots  + _numIndepVars, spline._numKnots  )
    );
}


///////////////////////////////////////////////////////////////////////////////
///
/// Predicate functor to test equality within a given tolerance.
///
///////////////////////////////////////////////////////////////////////////////

template <typename T> class IsEqualWithinTolerance : public std::binary_function<T,T,bool>
{
public:
  IsEqualWithinTolerance ( const T &tol ) : _tol ( tol ){}
  bool operator() ( const T &a, const T &b ) const
  {
    return ( a < b ) ? ( b - a ) < _tol : ( a - b ) < _tol;
  }
private:
  T _tol;
};


///////////////////////////////////////////////////////////////////////////////
///
/// Predicate functor to test equality within a given tolerance.
///
///////////////////////////////////////////////////////////////////////////////

template <> class IsEqualWithinTolerance<SlVec3d> : public std::binary_function<SlVec3d,SlVec3d,bool>
{
public:
  IsEqualWithinTolerance ( const SlVec3d &tol ) : _tol ( tol ){}
  bool operator() ( const SlVec3d &a, const SlVec3d &b ) const
  {
    return 
      ( ( a[0] < b[0] ) ? ( b[0] - a[0] ) < _tol[0] : ( a[0] - b[0] ) < _tol[0] ) &&
      ( ( a[1] < b[1] ) ? ( b[1] - a[1] ) < _tol[1] : ( a[1] - b[1] ) < _tol[1] ) &&
      ( ( a[2] < b[2] ) ? ( b[2] - a[2] ) < _tol[2] : ( a[2] - b[2] ) < _tol[2] );
  }
private:
  SlVec3d _tol;
};


///////////////////////////////////////////////////////////////////////////////
///
/// Predicate functor to test equality within a given tolerance.
///
///////////////////////////////////////////////////////////////////////////////

template <> class IsEqualWithinTolerance<SlVec3f> : public std::binary_function<SlVec3f,SlVec3f,bool>
{
public:
  IsEqualWithinTolerance ( const SlVec3f &tol ) : _tol ( tol ){}
  bool operator() ( const SlVec3f &a, const SlVec3f &b ) const
  {
    return 
      ( ( a[0] < b[0] ) ? ( b[0] - a[0] ) < _tol[0] : ( a[0] - b[0] ) < _tol[0] ) &&
      ( ( a[1] < b[1] ) ? ( b[1] - a[1] ) < _tol[1] : ( a[1] - b[1] ) < _tol[1] ) &&
      ( ( a[2] < b[2] ) ? ( b[2] - a[2] ) < _tol[2] : ( a[2] - b[2] ) < _tol[2] );
  }
private:
  SlVec3f _tol;
};


//TODO
//Since the client may want to pass a floating point number as the control point
//tolerance even when the control point type is a SlVec3d, you may want to move 
//the isEqual functions out of this class and into their own template class (or 
//namespace) and have overloaded template functions that can handle all cases. 
//Move these predicate functors to a more appropriate place too. Might want to 
//make something similar to CADKIT_DECLARE_CONSTANT_CLASS_FLOAT and friends.


///////////////////////////////////////////////////////////////////////////////
///
/// Equality.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcSplineData<NCSDCA>::isEqual 
  ( const NcSplineData<NCSDCA> &spline, const ParamType &knotTol, const CtrPtType &ctrPtTol ) const
{
  IndexType total ( _totalNumCtrPts * _numDepVars );
  IsEqualWithinTolerance<CtrPtType> functor ( ctrPtTol );
  return (
    true == this->_isEqualIntegers ( spline ) &&
    true == std::equal ( _allKnots,  _allKnots + _totalNumKnots, spline._allKnots,  IsEqualWithinTolerance<ParamType> ( knotTol ) ) &&
    true == std::equal ( _allCtrPts, _allCtrPts + total,         spline._allCtrPts, functor )
    );
}


///////////////////////////////////////////////////////////////////////////////
///
/// Equality.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcSplineData<NCSDCA>::isEqual ( const NcSplineData<NCSDCA> &spline ) const
{
  IndexType total ( _totalNumCtrPts * _numDepVars );
  return (
    true == this->_isEqualIntegers ( spline ) &&
    true == std::equal ( _allKnots,  _allKnots  + _totalNumKnots, spline._allKnots  ) &&
    true == std::equal ( _allCtrPts, _allCtrPts + total,          spline._allCtrPts )
    );
}


///////////////////////////////////////////////////////////////////////////////
///
/// Equality. This is not a member.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool operator == ( const NcSplineData<NCSDCA> &a, const NcSplineData<NCSDCA> &b )
{
  return a.isEqual ( b );
}


///////////////////////////////////////////////////////////////////////////////
///
/// Inequality. This is not a member.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool operator != ( const NcSplineData<NCSDCA> &a, const NcSplineData<NCSDCA> &b )
{
  return a.isNotEqual ( b );
}


///////////////////////////////////////////////////////////////////////////////
///
/// Confirm parametric range.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcSplineData<NCSDCA>::isInRange ( 
  const IndexType &whichIndepVar, 
  const ParamType &u ) const
{
  return (
    u >= this->getFirstKnot ( whichIndepVar ) &&
    u <= this->getLastKnot ( whichIndepVar ) 
    );
}


///////////////////////////////////////////////////////////////////////////////
///
/// Resize the given array if needed.
///
///////////////////////////////////////////////////////////////////////////////

template<class AllocatorType, class SizeType, class ArrayType> inline bool _resize ( 
  const SizeType &currentSize, 
  const SizeType &newSize, 
  ArrayType *&array,
  AllocatorType &allocator )
{
  SL_ASSERT ( newSize > 0 );
  SL_ASSERT ( ( array == 0x0 ) == ( currentSize == 0 ) );

  // If the new size is smaller (or the same) as the current size, and the 
  // given array is already allocated, then just return.
  if ( newSize <= currentSize && array != 0x0 )
    return true;

  // Allocate the new array.
  ArrayType *newArray = allocator.allocate ( newSize );

  // Check allocation.
  NC_CHECK_ALLOCATION ( newArray != 0x0 );

  // Delete the current array using the allocator.
  if ( array != 0x0 )
    allocator.deallocate ( array, currentSize );

  // Assign the given array to the new array.
  array = newArray;

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Allocate the arrays and set the integer members.
//
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcSplineData<NCSDCA>::resize ( 
  const IndexType &numIndepVars, 
  const IndexType &numDepVars, 
  const IndexType *order, 
  const IndexType *numCtrPts, 
  const bool &rational )
{
  NC_CHECK_ARGUMENT ( numIndepVars >= CADKIT_NURBS_MINIMUM_NUMBER_OF_INDEPENDENT_VARIABLES );
  NC_CHECK_ARGUMENT ( numDepVars >= CADKIT_NURBS_MINIMUM_NUMBER_OF_DEPENDENT_VARIABLES );
  NC_CHECK_ARGUMENT ( 0x0 != numCtrPts );
  NC_CHECK_ARGUMENT ( 0x0 != order );

#ifdef _CADKIT_NURBS_CORE_CHECK_FUNCTION_ARGUMENTS

  for ( IndexType ii = 0; ii < numIndepVars; ++ii )
  {
    SL_ASSERT ( numCtrPts[ii] >= CADKIT_NURBS_MINIMUM_NUMBER_OF_CONTROL_POINTS );
    SL_ASSERT ( order[ii] >= CADKIT_NURBS_MINIMUM_ORDER );
  }

#endif

  // Figure out the totals.
  IndexType totalNumCtrPts ( 1 );
  IndexType totalNumKnots ( 0 ), i;

  for ( i = 0; i < numIndepVars; ++i )
  {
    totalNumKnots += ( numCtrPts[i] + order[i] );
    totalNumCtrPts *= numCtrPts[i];
  }

  // For readability.
  IndexType oldCtrPtsSize ( static_cast<IndexType>(_totalNumCtrPts * _numDepVars) );
  IndexType newCtrPtsSize ( static_cast<IndexType>( totalNumCtrPts *  numDepVars) );

  // Declare the allocators.
  IndexAllocator ia;
  ParameterPointerAllocator ppa;
  ControlPointPointerAllocator cppa;
  ParameterAllocator pa;
  ControlPointAllocator cpa;

  // Resize the arrays if needed.
  NC_VERIFY_ALLOCATION ( true == CadKit::_resize ( _numIndepVars,  numIndepVars,  _numCtrPts, ia ) );
  NC_VERIFY_ALLOCATION ( true == CadKit::_resize ( _numIndepVars,  numIndepVars,  _order,     ia ) );
  NC_VERIFY_ALLOCATION ( true == CadKit::_resize ( _numIndepVars,  numIndepVars,  _degree,    ia ) );
  NC_VERIFY_ALLOCATION ( true == CadKit::_resize ( _numIndepVars,  numIndepVars,  _numKnots,  ia ) );
  NC_VERIFY_ALLOCATION ( true == CadKit::_resize ( _numIndepVars,  numIndepVars,  _knots,     ppa ) );
  NC_VERIFY_ALLOCATION ( true == CadKit::_resize ( _numDepVars,    numDepVars,    _ctrPts,    cppa ) );
  NC_VERIFY_ALLOCATION ( true == CadKit::_resize ( _totalNumKnots, totalNumKnots, _allKnots,  pa ) );
  NC_VERIFY_ALLOCATION ( true == CadKit::_resize ( oldCtrPtsSize,  newCtrPtsSize, _allCtrPts, cpa ) );

  // Set these integer members.
  _rational       = rational;
  _numIndepVars   = numIndepVars;
  _numDepVars     = numDepVars;
  _dimension      = ( _rational ) ? numDepVars - 1 : numDepVars;
  _totalNumCtrPts = totalNumCtrPts;

  // Initialize.
  _totalNumKnots = 0;

  // For each independent variable...
  for ( i = 0; i < _numIndepVars; ++i )
  {
    // Set all the integer arrays.
    _numCtrPts[i] = numCtrPts[i];
    _order[i]     = order[i];
    _degree[i]    = order[i] - 1;
    _numKnots[i]  = numCtrPts[i] + order[i];

    // Set the pointers to the individual knot vectors.
    _knots[i] = &_allKnots[_totalNumKnots];

    // Increment the total.
    _totalNumKnots += _numKnots[i];
  }

  // We increment the new one in the above loop. Make sure it ends up being 
  // what it is supposed to be.
  SL_ASSERT ( _totalNumKnots == totalNumKnots );

  // If we are rational then set the weights to point to the start of the 
  // last dependent variable. If not then we set it to NULL.
  _weights = ( _rational ) ? &_allCtrPts[static_cast<IndexType>(_totalNumCtrPts * _dimension)] : 0x0;

  // Set the pointers to the individual control points (i.e., pointer 
  // to the x's, pointer to the y's, etc).
  for ( i = 0; i < _numDepVars; ++i ) 
    _ctrPts[i] = &_allCtrPts[static_cast<IndexType>(_totalNumCtrPts * i)];

  // It worked.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
///
/// Set the value. Make a hard copy if the flag says to.
///
///////////////////////////////////////////////////////////////////////////////

template<NCSDTA> inline bool NcSplineData<NCSDCA>::setValue ( const NcSplineData<NCSDCA> &sd, bool makeCopy )
{
  NC_CHECK_ARGUMENT ( sd.isValid() );

  // If you are not making a "hard copy" (i.e., pointer assignments).
  if ( false == makeCopy )
  {
    // Assignments of the array-pointers will automatically transfer ownership.
    // If there is a current internal array it will get deleted.
    _rational        = sd._rational;
    _numIndepVars    = sd._numIndepVars;
    _numDepVars      = sd._numDepVars;
    _dimension       = sd._dimension;
    _totalNumCtrPts  = sd._totalNumCtrPts;
    _totalNumKnots   = sd._totalNumKnots;
    _numCtrPts       = sd._numCtrPts;
    _order           = sd._order;
    _degree          = sd._degree;
    _numKnots        = sd._numKnots;
    _allKnots        = sd._allKnots;
    _allCtrPts       = sd._allCtrPts;
    _knots           = sd._knots;
    _ctrPts          = sd._ctrPts;
    _weights         = sd._weights;

    // It worked.
    return true;
  }

  //
  // If we get to here then we are making a "hard copy".
  //

  // Resize the spline.
  NC_VERIFY_ALLOCATION ( this->resize ( sd._numIndepVars, sd._numDepVars, sd._order, sd._numCtrPts, sd._rational ) );

  // Should be true.
  SL_ASSERT ( _rational       == sd._rational );
  SL_ASSERT ( _numIndepVars   == sd._numIndepVars );
  SL_ASSERT ( _numDepVars     == sd._numDepVars );
  SL_ASSERT ( _dimension      == sd._dimension );
  SL_ASSERT ( _totalNumCtrPts == sd._totalNumCtrPts );
  SL_ASSERT ( _totalNumKnots  == sd._totalNumKnots );
  SL_ASSERT ( CadKit::isEqual ( _numCtrPts, sd._numCtrPts, _numIndepVars ) );
  SL_ASSERT ( CadKit::isEqual ( _order,     sd._order,     _numIndepVars ) );
  SL_ASSERT ( CadKit::isEqual ( _degree,    sd._degree,    _numIndepVars ) );
  SL_ASSERT ( CadKit::isEqual ( _numKnots,  sd._numKnots,  _numIndepVars ) );
  SL_ASSERT ( CadKit::isEqual ( _knots,     sd._knots,     _numIndepVars ) );
  SL_ASSERT ( CadKit::isEqual ( _ctrPts,    sd._ctrPts,    _numDepVars ) );

  // Copy the _knots.
  for ( i = 0; i < _totalNumKnots; ++i )
    _allKnots[i] = sd._allKnots[i];

  // Copy all the control points (including the _weights).
  IndexType total ( _totalNumCtrPts * _numDepVars );
  for ( i = 0; i < total; ++i )
    _allCtrPts[i] = sd._allCtrPts[i];

  // It worked.
  return true;
}


}; // namespace CadKit


#endif // _CADKIT_NURBS_CORE_LIBRARY_SPLINE_DATA_CLASS_H_
