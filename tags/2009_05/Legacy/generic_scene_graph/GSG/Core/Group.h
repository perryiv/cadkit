
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A group node.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_GROUP_H_
#define _GENERIC_SCENE_GRAPH_CORE_GROUP_H_

#include "GSG/Core/Node.h"


namespace GSG {

class Visitor;


class GSG_CORE_EXPORT Group : public Node
{
public:

  GSG_DECLARE_REFERENCED ( Group );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Group, Node );
  typedef GSG_BIDIRECTIONAL_CONTAINER < Node::ValidPtr > Nodes;
  typedef Nodes::size_type size_type;
  typedef Nodes::iterator iterator;
  typedef Nodes::const_iterator const_iterator;
  typedef Nodes::reference reference;
  typedef Nodes::const_reference const_reference;

  explicit Group();
  Group ( const Group &group );

  // Clear (remove) all the children.
  void                    clear();

  // Erase (remove) the children indicated by the iterators.
  void                    erase ( iterator first, iterator last );

  // Iterators to the nodes.
  const_iterator          begin() const;
  iterator                begin();
  const_iterator          end() const;
  iterator                end();

  // Add a primitive.
  void                    append ( Node *node );
  void                    prepend ( Node *node );
  void                    insert ( iterator beforeMe, Node *node );

  // Access to the first and last child.
  reference               front();
  const_reference         front() const;
  reference               back();
  const_reference         back() const;

  // The number of children.
  size_type               size() const;

  // Update this node.
  virtual void            update();

  // Calculate the bounding sphere.
  virtual void            calculateBoundingSphere();

protected:

  virtual ~Group();

  GSG_DECLARE_ACCEPT ( Group );

private:

  Nodes _nodes;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_GROUP_H_
