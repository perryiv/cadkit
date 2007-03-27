
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A pool of 2D vectors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_VEC_2_POOL_H_
#define _GENERIC_SCENE_GRAPH_CORE_VEC_2_POOL_H_

#include "GSG/Core/Referenced.h"


namespace GSG {


class GSG_CORE_EXPORT Vec2Pool : public Referenced
{
public:

  GSG_DECLARE_REFERENCED ( Vec2Pool );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Vec2Pool, Referenced );
  typedef SeqVec2::size_type size_type;
  typedef SeqVec2::iterator iterator;
  typedef SeqVec2::const_iterator const_iterator;
  typedef SeqVec2::reference reference;
  typedef SeqVec2::const_reference const_reference;
  typedef SeqVec2::value_type value_type;

  explicit Vec2Pool();
  Vec2Pool ( const Vec2Pool &pool );

  // Access to the values.
  const value_type &  value ( size_type i ) const;
  void                value ( size_type i, const value_type &v );

  // Get the values.
  const SeqVec2 &     values() const { return _values; }
  SeqVec2 &           values()       { return _values; }

protected:

  virtual ~Vec2Pool();

private:

  SeqVec2 _values;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_VEC_2_POOL_H_
