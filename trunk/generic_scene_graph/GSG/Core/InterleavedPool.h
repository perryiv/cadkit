
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 4004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A pool of interleaved values.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_INTERLEAVED_POOL_H_
#define _GENERIC_SCENE_GRAPH_CORE_INTERLEAVED_POOL_H_

#include "GSG/Core/Referenced.h"


namespace GSG {


class GSG_CORE_EXPORT InterleavedPool : public Referenced
{
public:

  GSG_DECLARE_REFERENCED ( InterleavedPool );
  GSG_DECLARE_LOCAL_TYPEDEFS ( InterleavedPool, Referenced );
  typedef GSG_RANDOM_ACCESS_CONTAINER < float > Values; // Has to be float.
  typedef Values::size_type size_type;
  typedef Values::iterator iterator;
  typedef Values::const_iterator const_iterator;
  typedef Values::reference reference;
  typedef Values::const_reference const_reference;
  typedef Values::value_type value_type;

  // Describes what the interleaved values are.
  enum
  {
    NORMALS = 00000001,
    COLORS3 = 00000002,
    COLORS4 = 00000004
  };

  explicit InterleavedPool();
  InterleavedPool ( const InterleavedPool &pool );

  // Get/set the vertices.
  void                vertex ( size_type row, Real  v0, Real  v1, Real  v2 );
  void                vertex ( size_type row, Real &v0, Real &v1, Real &v2 ) const;

  // Get/set the normals.
  void                normal ( size_type row, Real  n0, Real  n1, Real  n2 );
  void                normal ( size_type row, Real &n0, Real &n1, Real &n2 ) const;

  // Get/set the colors.
  void                color ( size_type row, Real  c0, Real  c1, Real  c2, Real  c3 );
  void                color ( size_type row, Real  c0, Real  c1, Real  c2 );
  void                color ( size_type row, Real &c0, Real &c1, Real &c2, Real &c3 ) const;
  void                color ( size_type row, Real &c0, Real &c1, Real &c2 ) const;

  // Get/set an individual value.
  const_reference     value ( size_type index ) const;
  void                value ( size_type index, value_type v );

  // Get the values.
  const Values &      values() const { return _values; }
  Values &            values()       { return _values; }

  // What the values are.
  UnsignedInteger     contains() const { return _contains; }
  void                contains ( UnsignedInteger bits );

  // Get a pointer to the internal data.
  const value_type *  ptr() const { return ( _values.empty() ) ? 0x0 : &(_values[0]); }

  // The number of rows and columns in the "2D" array.
  size_type           numColumns() const { return _vertexOffset + 3; }
  size_type           numRows()    const { return _values.size() / this->numColumns(); }
  void                numRows ( size_type rows );

  // Reserve the number of rows.
  void                reserveRows ( size_type rows );

  // How many rows are allocated.
  size_type           rowsReserved() const { return _values.capacity() / this->numColumns(); }

protected:

  virtual ~InterleavedPool();

private:

  Values _values;
  UnsignedInteger _contains;
  size_type _vertexOffset;
  size_type _normalOffset;
  size_type _colorOffset;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_INTERLEAVED_POOL_H_
