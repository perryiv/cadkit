
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlPartitionedVector: Encapsulated a vector (array) that is partitioned
//  into sub-parts. An integer vector stores the partitions. 
//
//  Note: it is left up to the client to decide what the indices mean.
//  Wether that be the size of each partition, or the starting index of each 
//  partition, or whatever.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_TEMPLATE_PARTITIONED_VECTOR_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_TEMPLATE_PARTITIONED_VECTOR_CLASS_H_

#include "SlTestEquality.h"
#include "SlZero.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <vector>
#endif

#define SLPVTA class IndexType, class DataType
#define SLPVCA IndexType, DataType


namespace CadKit
{
template <SLPVTA> class SlPartitionedVector
{
public:

  typedef std::vector<IndexType> IndexArray;
  typedef std::vector<DataType> DataArray;

  SlPartitionedVector(){}
  SlPartitionedVector ( const SlPartitionedVector &pv ) : _indices ( pv._indices ), _data ( pv._data ){}
  ~SlPartitionedVector(){}

  // Clear both vectors.
  void                    clear() { _indices.clear(); _data.clear(); }

  // Get the vector of indices. This class makes no assumption as to what 
  // these indices mean, if anything.
  const IndexArray &      getIndices() const { return _indices; }
  IndexArray &            getIndices()       { return _indices; }

  // Get the data vector.
  const DataArray &       getData() const { return _data; }
  DataArray &             getData()       { return _data; }

  // Get a pointer to the start of the data.
  const DataType *        getDataPointer ( const IndexType &offset ) const { return this->_getDataPointer ( offset ); }
  DataType *              getDataPointer ( const IndexType &offset );

  // Equality test.
  bool                    isEqual    ( const SlPartitionedVector &pv ) const;
  bool                    isEqual    ( const SlPartitionedVector &pv, const DataType &tolerance ) const;

  // Inequality test.
  bool                    isNotEqual ( const SlPartitionedVector &pv )                            const { return false == this->isEqual ( sd ); }
  bool                    isNotEqual ( const SlPartitionedVector &pv, const DataType &tolerance ) const { return false == this->isEqual ( sd, tolerance ); }

  // Assignment operator.
  SlPartitionedVector &   operator = ( const SlPartitionedVector &pv ) { this->setValue ( pv ); return *this; }

  // Parenthesis operator, for accessing as a single array.
  const DataType &        operator() ( const IndexType &element ) const;
  DataType &              operator() ( const IndexType &element );

  // Parenthesis operator, for accessing the individual partitions.
  const DataType &        operator() ( const IndexType &partition, const IndexType &element ) const;
  DataType &              operator() ( const IndexType &partition, const IndexType &element );

  // Set the value.
  void                    setValue ( const SlPartitionedVector &pv ) { _indices = pv._indices; _data = pv._data; }

protected:

  IndexArray _indices;
  DataArray _data;

  const DataType *        _getDataPointer ( const IndexType &offset ) const;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Are the two vectors equal?
//
///////////////////////////////////////////////////////////////////////////////

template<class T> inline bool isEqual ( const std::vector<T> &v1, const std::vector<T> &v2, const T &tol )
{
  if ( v1.size() != v2.size() )
    return false;

  std::vector<T>::const_iterator i1 = v1.begin();
  std::vector<T>::const_iterator i2 = v2.begin();

  while ( i1 != v1.end() )
  {
    if ( false == CadKit::isEqual ( *i1, *i2, tol ) )
      return false;

    ++i1;
    ++i2;
  }

  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are they equal?
//
///////////////////////////////////////////////////////////////////////////////

template<SLPVTA> inline bool SlPartitionedVector<SLPVCA>::isEqual ( const SlPartitionedVector<SLPVCA> &pv ) const
{
  return ( _indices == pv._indices && _data == pv._data );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are they equal?
//
///////////////////////////////////////////////////////////////////////////////

template<SLPVTA> inline bool SlPartitionedVector<SLPVCA>::isEqual ( const SlPartitionedVector<SLPVCA> &pv, const DataType &tol ) const
{
  return ( _indices == pv._indices && true == CadKit::isEqual ( _data, pv._data, tol ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Equality.
//
///////////////////////////////////////////////////////////////////////////////

template<SLPVTA> inline bool operator == ( const SlPartitionedVector<SLPVCA> &a, const SlPartitionedVector<SLPVCA> &b )
{
  return a.isEqual ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Inequality.
//
///////////////////////////////////////////////////////////////////////////////

template<SLPVTA> inline bool operator != ( const SlPartitionedVector<SLPVCA> &a, const SlPartitionedVector<SLPVCA> &b )
{
  return a.isNotEqual ( b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parenthesis operator, for accessing as a single array.
//
///////////////////////////////////////////////////////////////////////////////

template<SLPVTA> inline const DataType &SlPartitionedVector<SLPVCA>::operator() ( const IndexType &element ) const
{
  SL_ASSERT ( element == 0 || element > 0 ); // Make g++ happy.
  SL_ASSERT ( element < _data.size() );
  return _data[element];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parenthesis operator, for accessing as a single array.
//
///////////////////////////////////////////////////////////////////////////////

template<SLPVTA> inline DataType &SlPartitionedVector<SLPVCA>::operator() ( const IndexType &element )
{
  SL_ASSERT ( element == 0 || element > 0 ); // Make g++ happy.
  SL_ASSERT ( element < _data.size() );
  return _data[element];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parenthesis operator, for accessing the individual partitions.
//
///////////////////////////////////////////////////////////////////////////////

template<SLPVTA> inline const DataType &SlPartitionedVector<SLPVCA>::operator() ( const IndexType &partition, const IndexType &element ) const
{
  SL_ASSERT ( partition == 0 || partition > 0 ); // Make g++ happy.
  SL_ASSERT ( partition < _indices.size() );

  // Get the index.
  IndexType index = _indices[partition] + element;

  // Should be true.
  SL_ASSERT ( index == 0 || index > 0 ); // Make g++ happy for unsigned types.
  SL_ASSERT ( index < _data.size() );

  return _data[index];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parenthesis operator, for accessing the individual partitions.
//
///////////////////////////////////////////////////////////////////////////////

template<SLPVTA> inline DataType &SlPartitionedVector<SLPVCA>::operator() ( const IndexType &partition, const IndexType &element )
{
  SL_ASSERT ( partition == 0 || partition > 0 ); // Make g++ happy.
  SL_ASSERT ( partition < _indices.size() );

  // Get the index.
  IndexType index = _indices[partition] + element;

  // Should be true.
  SL_ASSERT ( index == 0 || index > 0 ); // Make g++ happy for unsigned types.
  SL_ASSERT ( index < _data.size() );

  return _data[index];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get a pointer to the start of the data.
//
///////////////////////////////////////////////////////////////////////////////

template<SLPVTA> inline const DataType *SlPartitionedVector<SLPVCA>::_getDataPointer ( const IndexType &offset ) const
{
  SL_ASSERT ( offset == 0 || offset > 0 ); // Make g++ happy.
  SL_ASSERT ( offset < _data.size() );

  // Return the address at the requested offset.
  return &(_data[offset]);
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get a pointer to the start of the data. This one is unsafe.
//
///////////////////////////////////////////////////////////////////////////////

template<SLPVTA> inline DataType *SlPartitionedVector<SLPVCA>::getDataPointer ( const IndexType &offset )
{
  return ( const_cast<DataType *> ( this->_getDataPointer ( offset ) ) );
}


}; // namespace CadKit


#endif // _CADKIT_STANDARD_LIBRARY_TEMPLATE_PARTITIONED_VECTOR_CLASS_H_
