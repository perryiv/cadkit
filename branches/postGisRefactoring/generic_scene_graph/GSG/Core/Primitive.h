
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A single vertex primitive.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_PRIMITIVE_H_
#define _GENERIC_SCENE_GRAPH_CORE_PRIMITIVE_H_

#include "GSG/Core/Referenced.h"


namespace GSG {


class GSG_CORE_EXPORT Primitive : public Referenced
{
public:

  GSG_DECLARE_REFERENCED ( Primitive );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Primitive, Referenced );
  typedef Indices::size_type size_type;

  explicit Primitive();
  Primitive ( Index start, Index size );
  Primitive ( const Primitive &prim );

  // The start of the primitive.
  Index                   start() const { return _start; }
  void                    start ( Index s );

  // The number of vertices in the primitive.
  Index                   size() const { return _size; }
  void                    size ( Index s );

protected:

  virtual ~Primitive();

private:

  Index _start;
  Index _size;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_PRIMITIVE_H_
