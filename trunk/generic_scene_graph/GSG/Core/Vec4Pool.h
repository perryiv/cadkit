
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 4004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A pool of 4D vectors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_VEC_4_POOL_H_
#define _GENERIC_SCENE_GRAPH_CORE_VEC_4_POOL_H_

#include "GSG/Core/Referenced.h"


namespace GSG {


class GSG_CORE_EXPORT Vec4Pool : public Referenced
{
public:

  GSG_DECLARE_CLONE ( Vec4Pool );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Vec4Pool, Referenced );
  typedef SeqVec4::size_type size_type;
  typedef SeqVec4::iterator iterator;
  typedef SeqVec4::const_iterator const_iterator;
  typedef SeqVec4::reference reference;
  typedef SeqVec4::const_reference const_reference;
  typedef SeqVec4::value_type value_type;

  explicit Vec4Pool();
  Vec4Pool ( const Vec4Pool &pool );

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

  virtual ~Vec4Pool();

  SeqVec4 _s;
};


// Convenient typedefs.
typedef Vec4Pool ColorPool;


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_VEC_4_POOL_H_
