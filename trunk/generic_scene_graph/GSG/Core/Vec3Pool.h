
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 3003, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A pool of 3D vectors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_VEC_3_POOL_H_
#define _GENERIC_SCENE_GRAPH_CORE_VEC_3_POOL_H_

#include "GSG/Core/Referenced.h"


namespace GSG {


class GSG_CORE_EXPORT Vec3Pool : public Referenced
{
public:

  GSG_DECLARE_CLONE ( Vec3Pool );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Vec3Pool, Referenced );
  typedef SeqVec3::size_type size_type;
  typedef SeqVec3::iterator iterator;
  typedef SeqVec3::const_iterator const_iterator;
  typedef SeqVec3::reference reference;
  typedef SeqVec3::const_reference const_reference;
  typedef SeqVec3::value_type value_type;

  explicit Vec3Pool();
  Vec3Pool ( const Vec3Pool &pool );

  // Iterators to the vertices.
  const_iterator      begin() const;
  iterator            begin();
  const_iterator      end() const;
  iterator            end();

  bool                empty() const { return 0 == this->size(); }

  void                resize ( size_type s );

  const value_type &  operator [] ( size_type i ) const;
  value_type &        operator [] ( size_type i );

  size_type           operator [] ( const value_type &v );

  size_type           size() const;

protected:

  virtual ~Vec3Pool();

  SeqVec3 _s;
  MapVec3 _m;
};


// Convenient typedefs.
typedef Vec3Pool VertexPool;
typedef Vec3Pool NormalPool;


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_VEC_3_POOL_H_
