
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
#include "GSG/Core/Primitive.h"
#include "GSG/Core/Attributes.h"


namespace GSG {


class GSG_CORE_EXPORT Shape : public Node
{
public:

  GSG_DECLARE_CLONE ( Shape );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Shape, Node );
  typedef GSG_SEQUENCE_CONTAINER_TYPE < Primitive::ValidPtr > Primitives;
  typedef Primitives::size_type size_type;
  typedef Primitives::iterator iterator;
  typedef Primitives::const_iterator const_iterator;
  typedef Primitives::reference reference;
  typedef Primitives::const_reference const_reference;

  enum Binding
  {
    UNKNOWN,
    PER_VERTEX,
    PER_PRIMITIVE,
    PER_SHAPE
  };

  explicit Shape();
  Shape ( const Shape &g );

  // Iterators to the primitives.
  const_iterator          begin() const;
  iterator                begin();
  const_iterator          end() const;
  iterator                end();

  // Access to the first and last primitive.
  reference               front();
  const_reference         front() const;
  reference               back();
  const_reference         back() const;

  // Add a primitive.
  void                    append ( Primitive *prim );
  void                    prepend ( Primitive *prim );
  void                    insert ( iterator beforeMe, Primitive *prim );

  // Get the bindings.
  Binding                 vertexBinding() const;
  Binding                 normalBinding() const;
  Binding                 colorBinding()  const;

  // Set the bindings.
  void                    vertexBinding ( Binding vb );
  void                    normalBinding ( Binding nb );
  void                    colorBinding  ( Binding cb );

  // Get/set the attributes.
  const Attributes *      attributes() const;
  Attributes *            attributes();
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

  Primitives _primitives;
  Binding _vb;
  Binding _nb;
  Binding _cb;
  Attributes::Ptr _attributes;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_SHAPE_H_
