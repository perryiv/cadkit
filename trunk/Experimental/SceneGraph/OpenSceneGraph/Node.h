
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The node class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SCENE_GRAPH_OSG_NODE_CLASS_H_
#define _SCENE_GRAPH_OSG_NODE_CLASS_H_

#include "SceneGraph/OpenSceneGraph/Object.h"

#include "osg/Node"
#include "osg/ref_ptr"


namespace SceneGraph {
namespace OSG {


class Node : 
  public SceneGraph::OSG::Object,
  public Usul::Interfaces::SceneGraph::IAcceptVisitorModifier,
  public Usul::Interfaces::SceneGraph::IAcceptVisitorNonModifier,
  public Usul::Interfaces::SceneGraph::IChild
{
public:

  // Typedefs.
  typedef SceneGraph::OSG::Object BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::SceneGraph::IAcceptVisitorModifier IAcceptVisitorModifier;
  typedef Usul::Interfaces::SceneGraph::IAcceptVisitorNonModifier IAcceptVisitorNonModifier;
  typedef Usul::Interfaces::SceneGraph::IChild IChild;
  typedef Usul::Interfaces::SceneGraph::IVisitorNonModifier<osg::Node> IVisitorNonModifier;
  typedef Usul::Interfaces::SceneGraph::IVisitorModifier<osg::Node> IVisitorModifier;
  typedef osg::ref_ptr<osg::Node> NodePtr;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Node );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Usul::Interfaces::SceneGraph::IAcceptVisitorModifier
  virtual void                  acceptVisitorModifier ( IUnknown::RefPtr );

  // Usul::Interfaces::SceneGraph::IAcceptVisitorNonModifier
  virtual void                  acceptVisitorNonModifier ( IUnknown::RefPtr ) const;

  // Usul::Interfaces::SceneGraph::IChild
  virtual unsigned int          getNumParents() const;
  virtual IUnknown::RefPtr      getParent ( unsigned int ) const;

  // Get the node.
  virtual const NodePtr         node() const;
  virtual NodePtr               node();

protected:

  // Construction.
  Node ( NodePtr );

  // Use reference counting.
  virtual ~Node();

  void                          _destroy();

private:

  // Do not copy.
  Node ( const Node & );
  Node &operator = ( const Node & );

  NodePtr _node;
};


} // namespace OSG
} // namespace SceneGraph


#endif // _SCENE_GRAPH_OSG_NODE_CLASS_H_
