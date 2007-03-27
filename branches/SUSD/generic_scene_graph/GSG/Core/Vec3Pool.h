
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

  GSG_DECLARE_REFERENCED ( Vec3Pool );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Vec3Pool, Referenced );
  typedef SeqVec3::size_type size_type;
  typedef SeqVec3::iterator iterator;
  typedef SeqVec3::const_iterator const_iterator;
  typedef SeqVec3::reference reference;
  typedef SeqVec3::const_reference const_reference;
  typedef SeqVec3::value_type value_type;

  explicit Vec3Pool();
  Vec3Pool ( const Vec3Pool &pool );

  // Access to the values.
  const value_type &  value ( size_type i ) const;
  void                value ( size_type i, const value_type &v );

  // Get the values.
  const SeqVec3 &     values() const { return _values; }
  SeqVec3 &           values()       { return _values; }

  // Get a pointer to the internal data.
  const value_type *  ptr() const { return ( _values.empty() ) ? 0x0 : &(_values[0]); }

protected:

  virtual ~Vec3Pool();

private:

  SeqVec3 _values;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_VEC_3_POOL_H_
