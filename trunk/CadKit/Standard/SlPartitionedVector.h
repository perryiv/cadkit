
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
//  into sub-parts.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_TEMPLATE_PARTITIONED_VECTOR_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_TEMPLATE_PARTITIONED_VECTOR_CLASS_H_

#include "SlTestEquality.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <vector>
#endif

#define SLPVTA class IndexType, class SizeType, class DataType
#define SLPVCA IndexType, SizeType, DataType


namespace CadKit
{
template <SLPVTA> class SlPartitionedVector
{
public:

  typedef std::vector<SizeType>  SizeArray;
  typedef std::vector<IndexType> StartsArray;
  typedef std::vector<DataType>  DataArray;

  SlPartitionedVector(){}
  SlPartitionedVector ( const SlPartitionedVector &pv ) : _sizes ( pv._sizes ), _starts ( pv._starts ), _data ( pv._data ){}
  ~SlPartitionedVector(){}

  // Clear both vectors.
  void                    clear() { _sizes.clear(); _starts.clear(); _data.clear(); }

  // Get the vector of starting places for the partitions.
  const StartsArray &     getStarts() const { return _starts; }
  StartsArray &           getStarts()       { return _starts; }

  // Get the vector of sizes.
  const SizeArray &       getSizes() const { return _sizes; }
  SizeArray &             getSizes()       { return _sizes; }

  // Get the data vector.
  const DataArray &       getData() const { return _data; }
  DataArray &             getData()       { return _data; }

  // Get a pointer to the start of the data.
  const DataType *        getDataPointer ( const SizeType &offset ) const { return this->_getDataPointer ( offset ); }
  DataType *              getDataPointer ( const SizeType &offset );

  // Equality test.
  bool                    isEqual    ( const SlPartitionedVector &pv ) const;
  bool                    isEqual    ( const SlPartitionedVector &pv, const DataType &tolerance ) const;

  // Inequality test.
  bool                    isNotEqual ( const SlPartitionedVector &pv )                            const { return false == this->isEqual ( this->sd ); }
  bool                    isNotEqual ( const SlPartitionedVector &pv, const DataType &tolerance ) const { return false == this->isEqual ( this->sd, tolerance ); }

  // Assignment operator.
  SlPartitionedVector &   operator = ( const SlPartitionedVector &pv ) { this->setValue ( pv ); return *this; }

  // Parenthesis operator, for accessing as a single array.
  const DataType &        operator() ( const SizeType &element ) const;
  DataType &              operator() ( const SizeType &element );

  // Parenthesis operator, for accessing the individual partitions.
  const DataType &        operator() ( const SizeType &partition, const SizeType &element ) const;
  DataType &              operator() ( const SizeType &partition, const SizeType &element );

  // Set the value.
  void                    setValue ( const SlPartitionedVector &pv ) { _sizes = pv._sizes; _starts = pv._starts; _data = pv._data; }

  // Return the size of the partition.
  unsigned int            size ( unsigned int partition ) const { return ( _sizes.empty() ) ? 0 : _sizes[partition]; }

protected:

  SizeArray _sizes;
  StartsArray _starts;
  DataArray _data;

  const DataType *        _getDataPointer ( const SizeType &offset ) const;
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

  typename std::vector<T>::const_iterator i1 = v1.begin();
  typename std::vector<T>::const_iterator i2 = v2.begin();

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
  return ( _sizes == pv._sizes && _starts == pv._starts && _data == pv._data );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are they equal?
//
///////////////////////////////////////////////////////////////////////////////

template<SLPVTA> inline bool SlPartitionedVector<SLPVCA>::isEqual ( const SlPartitionedVector<SLPVCA> &pv, const DataType &tol ) const
{
  return ( _sizes == pv._sizes && _starts == pv._starts && true == CadKit::isEqual ( _data, pv._data, tol ) );
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

template<SLPVTA> inline const DataType &SlPartitionedVector<SLPVCA>::operator() ( const SizeType &element ) const
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

template<SLPVTA> inline DataType &SlPartitionedVector<SLPVCA>::operator() ( const SizeType &element )
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

template<SLPVTA> inline const DataType &SlPartitionedVector<SLPVCA>::operator() ( const SizeType &partition, const SizeType &element ) const
{
  SL_ASSERT ( partition == 0 || partition > 0 ); // Make g++ happy.
  SL_ASSERT ( partition < _starts.size() );

  // Get the index.
  SizeType index = _starts[partition] + element;

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

template<SLPVTA> inline DataType &SlPartitionedVector<SLPVCA>::operator() ( const SizeType &partition, const SizeType &element )
{
  SL_ASSERT ( partition == 0 || partition > 0 ); // Make g++ happy.
  SL_ASSERT ( partition < _starts.size() );

  // Get the index.
  SizeType index = _starts[partition] + element;

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

template<SLPVTA> inline const DataType *SlPartitionedVector<SLPVCA>::_getDataPointer ( const SizeType &offset ) const
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

template<SLPVTA> inline DataType *SlPartitionedVector<SLPVCA>::getDataPointer ( const SizeType &offset )
{
  return ( const_cast<DataType *> ( this->_getDataPointer ( offset ) ) );
}


}; // namespace CadKit


#endif // _CADKIT_STANDARD_LIBRARY_TEMPLATE_PARTITIONED_VECTOR_CLASS_H_
