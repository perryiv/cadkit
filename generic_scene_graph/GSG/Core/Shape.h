
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
#include "GSG/Core/Attributes.h"


namespace GSG {


class GSG_CORE_EXPORT Shape : public Node
{
public:

  GSG_DECLARE_CLONE ( Shape );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Shape, Node );
  typedef GSG_BIDIRECTIONAL_CONTAINER < PrimitiveSet::ValidPtr > Sets;
  typedef Sets::size_type size_type;
  typedef Sets::iterator iterator;
  typedef Sets::const_iterator const_iterator;
  typedef Sets::reference reference;
  typedef Sets::const_reference const_reference;

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
  const Attributes *      attributes() const { return _attributes.get(); }
  Attributes *            attributes()       { return _attributes.get(); }
  void                    attributes ( Attributes *a );

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

  Sets _sets;
  Attributes::Ptr _attributes;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_SHAPE_H_
