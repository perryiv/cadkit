
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

  GSG_DECLARE_REFERENCED ( Vec4Pool );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Vec4Pool, Referenced );
  typedef SeqVec4::size_type size_type;
  typedef SeqVec4::iterator iterator;
  typedef SeqVec4::const_iterator const_iterator;
  typedef SeqVec4::reference reference;
  typedef SeqVec4::const_reference const_reference;
  typedef SeqVec4::value_type value_type;

  explicit Vec4Pool();
  Vec4Pool ( const Vec4Pool &pool );

  // Access to the values.
  const value_type &  value ( size_type i ) const;
  void                value ( size_type i, const value_type &v );

  // Get the values.
  const SeqVec4 &     values() const { return _values; }
  SeqVec4 &           values()       { return _values; }

protected:

  virtual ~Vec4Pool();

private:

  SeqVec4 _values;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_VEC_4_POOL_H_
