
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
//  into sub-parts. An integer vector stores the indices of the partitions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_TEMPLATE_PARTITIONED_VECTOR_CLASS_H_
#define _CADKIT_STANDARD_LIBRARY_TEMPLATE_PARTITIONED_VECTOR_CLASS_H_

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <vector>
#endif


namespace CadKit
{
template <class IndexType, class DataType> class SlPartitionedVector
{
public:

  typedef std::vector<IndexType> Indices;
  typedef std::vector<DataType> Data;

  void                  clear() { _indices.clear(); _data.clear(); }

  const Indices &       getIndices() const { return _indices; }
  Indices &             getIndices()       { return _indices; }

  const Data &          getData() const { return _data; }
  Data &                getData()       { return _data; }

  SlPartitionedVector(){}
  ~SlPartitionedVector(){}

protected:

  Indices _indices;
  Data _data;
};


}; // namespace CadKit


#endif // _CADKIT_STANDARD_LIBRARY_TEMPLATE_PARTITIONED_VECTOR_CLASS_H_
