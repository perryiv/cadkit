
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Node class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "Node.h"
#include "Factory.h"

#include "Usul/Cast/Cast.h"

#include "Performer/pf/pfNode.h"

using namespace SAL;
using namespace SAL::Performer;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Node, Node::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Node::Node ( ::pfNode *n ) : BaseClass(),
  _node ( n )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Node::~Node()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Node::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case SAL::Interfaces::INode::IID:
    return static_cast<SAL::Interfaces::INode*>(this);
  case SAL::Interfaces::IPerformer::IID:
    return static_cast<SAL::Interfaces::IPerformer*>(this);
  case SAL::Interfaces::IBoundingSphereFloat::IID:
    return static_cast<SAL::Interfaces::IBoundingSphereFloat*>(this);
  case Usul::Interfaces::IUnknown::IID:
    return static_cast<Usul::Interfaces::IUnknown*>(static_cast<SAL::Interfaces::INode*>(this));
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name.
//
///////////////////////////////////////////////////////////////////////////////

void Node::getNodeName ( std::string &name ) const
{
  name = _node->getName();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name.
//
///////////////////////////////////////////////////////////////////////////////

void Node::setNodeName ( const std::string &name )
{
  _node->setName ( name.c_str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the pointer to the base class for all Performer objects.
//
///////////////////////////////////////////////////////////////////////////////

const ::pfMemory *Node::getPfMemory() const
{
  return _node;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the pointer to the base class for all Performer objects.
//
///////////////////////////////////////////////////////////////////////////////

::pfMemory *Node::getPfMemory()
{
  return _node;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the center.
//
///////////////////////////////////////////////////////////////////////////////

void Node::boundingSphereCenter ( Usul::Math::Vec3f &center ) const
{
  ::pfSphere sphere;
  ::pfNode *node = const_cast < ::pfNode * > ( _node.get() );
  node->getBound ( &sphere );
  const pfVec3 &c = sphere.center;
  center.set ( c[0], c[1], c[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the radius.
//
///////////////////////////////////////////////////////////////////////////////

void Node::boundingSphereRadius ( float &r ) const
{
  ::pfSphere sphere;
  ::pfNode *node = const_cast < ::pfNode * > ( _node.get() );
  node->getBound ( &sphere );
  r = sphere.radius;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the pfNode from the IUnknown.
//
///////////////////////////////////////////////////////////////////////////////

::pfNode *Node::getNode ( Usul::Interfaces::IUnknown *unknown )
{
  SAL::Interfaces::IPerformer::ValidQueryPtr iperf ( unknown );
  Node::ValidPfMemoryPtr mp ( iperf->getPfMemory() );
  Node::ValidPfNodePtr node ( ( mp->isOfType ( ::pfNode::getClassType() ) ) ? USUL_UNSAFE_CAST ( ::pfNode *, mp.get() ) : 0x0 );
  return node.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone the node.
//
///////////////////////////////////////////////////////////////////////////////

SAL::Interfaces::INode *Node::cloneNode()
{
  // Make the clone.
  Node::ValidPfNodePtr clone ( _node->clone ( 0 ) );

  // Make the appropriate node.
  INode::ValidQueryPtr node ( Factory::create ( clone ) );

  // Return the node.
  return node.release();
}
