
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A shape node.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_SHAPE_H_
#define _GENERIC_SCENE_GRAPH_CORE_SHAPE_H_

#include "GSG/Core/Node.h"
#include "GSG/Core/PrimitiveSet.h"
#include "GSG/Core/AttributeSet.h"


namespace GSG {


class GSG_CORE_EXPORT Shape : public Node
{
public:

  GSG_DECLARE_REFERENCED ( Shape );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Shape, Node );
  typedef GSG_BIDIRECTIONAL_CONTAINER < PrimitiveSet::ValidPtr > PrimitiveSets;
  typedef PrimitiveSets::size_type size_type;
  typedef PrimitiveSets::iterator iterator;
  typedef PrimitiveSets::const_iterator const_iterator;
  typedef PrimitiveSets::reference reference;
  typedef PrimitiveSets::const_reference const_reference;

  explicit Shape();
  Shape ( const Shape &g );

  // Iterators to the primitive sets.
  const_iterator          begin() const { return _sets.begin(); }
  iterator                begin()       { return _sets.begin(); }
  const_iterator          end() const   { return _sets.end(); }
  iterator                end()         { return _sets.end(); }

  // Add a primitive set.
  void                    append ( PrimitiveSet *ps );
  void                    prepend ( PrimitiveSet *ps );
  void                    insert ( iterator beforeMe, PrimitiveSet *ps );

  // Get/set the attributes.
  const AttributeSet *    attributes() const { return _attributes.get(); }
  AttributeSet *          attributes()       { return _attributes.get(); }
  void                    attributes ( AttributeSet *a );

  // The number of primitives.
  size_type               size() const;

  // Update this node.
  virtual void            update();

  // Calculate the bounding sphere.
  virtual void            calculateBoundingSphere();

protected:

  virtual ~Shape();

  GSG_DECLARE_ACCEPT ( Shape );

private:

  PrimitiveSets _sets;
  AttributeSet::Ptr _attributes;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_SHAPE_H_
