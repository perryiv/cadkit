
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 4004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for vector pools.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_VEC_POOL_BASE_H_
#define _GENERIC_SCENE_GRAPH_CORE_VEC_POOL_BASE_H_

#include "GSG/Core/Referenced.h"


namespace GSG {


class GSG_CORE_EXPORT VecPool : public Referenced
{
public:

  GSG_DECLARE_LOCAL_TYPEDEFS ( VecPool, Referenced );
  typedef Indices::size_type size_type;
  typedef Indices::iterator iterator;
  typedef Indices::const_iterator const_iterator;
  typedef Indices::reference reference;
  typedef Indices::const_reference const_reference;
  typedef Indices::value_type value_type;

  // Access to the indices.
  value_type          index ( size_type i ) const;
  void                index ( size_type i, const value_type &v );

  // Get the indices.
  const Indices &     indices() const { return _indices; }
  Indices &           indices()       { return _indices; }

protected:

  explicit VecPool();
  VecPool ( const VecPool &pool );
  virtual ~VecPool();

private:

  Indices _indices;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_VEC_POOL_BASE_H_
