
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

#include "osg/Node"

using namespace SAL;
using namespace SAL::OSG;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Node, Node::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Node::Node ( osg::Node *n ) : BaseClass(),
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
  case SAL::Interfaces::IOSG::IID:
    return static_cast<SAL::Interfaces::IOSG*>(this);
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
  _node->setName ( name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the pointer to the base class for all OSG objects.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Referenced *Node::osgReferenced() const
{
  return _node;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the pointer to the base class for all OSG objects.
//
///////////////////////////////////////////////////////////////////////////////

osg::Referenced *Node::osgReferenced()
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
  const osg::BoundingSphere &sphere = _node->getBound();
  const osg::Vec3 &c = sphere.center();
  center.set ( c[0], c[1], c[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the radius.
//
///////////////////////////////////////////////////////////////////////////////

void Node::boundingSphereRadius ( float &r ) const
{
  r = _node->getBound().radius();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone the node.
//
///////////////////////////////////////////////////////////////////////////////

SAL::Interfaces::INode *Node::cloneNode()
{
  // Make the clone.
  osg::ref_ptr<osg::Object> object ( _node->clone ( osg::CopyOp::DEEP_COPY_ALL ) );
  Node::ValidOsgNodePtr clone ( dynamic_cast < osg::Node * > ( object.get() ) );

  // Make the appropriate node.
  INode::ValidQueryPtr node ( Factory::create ( clone ) );

  // Return the node.
  return node.release();
}