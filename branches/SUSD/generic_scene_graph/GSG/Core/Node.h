
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all scene graph nodes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_SCENE_GRAPH_CORE_NODE_H_
#define _GENERIC_SCENE_GRAPH_CORE_NODE_H_

#include "GSG/Core/Referenced.h"


namespace GSG {


class Visitor;


class GSG_CORE_EXPORT Node : public Referenced
{
public:

  GSG_DECLARE_LOCAL_TYPEDEFS ( Node, Referenced );

  // Update this node.
  virtual void            update();

  // Bounding sphere.
  virtual void            calculateBoundingSphere() = 0;
  const BoundingSphere &  boundingSphere() const;
  void                    boundingSphere ( const BoundingSphere &b );

protected:

  explicit Node();
  Node ( const Node &node );
  virtual ~Node();

  // Accept the visitor.
  virtual void            _accept ( Visitor &v ) = 0;

  // Friends.
  friend class Visitor;

private:

  BoundingSphere _bound;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_NODE_H_
