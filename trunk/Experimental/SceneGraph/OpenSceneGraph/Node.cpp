
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

#include "SceneGraph/OpenSceneGraph/Node.h"
#include "SceneGraph/OpenSceneGraph/Factory.h"

#include "OsgTools/Convert/Matrix.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Strings/Format.h"
#include "Usul/Trace/Trace.h"

#include "osg/Group"

#include <algorithm>

using namespace SceneGraph::OSG;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Node, Node::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Node::Node ( NodePtr n ) : BaseClass(),
  _node ( n )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Node::~Node()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Node::_destroy ), "4048642426" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy this instance.
//
///////////////////////////////////////////////////////////////////////////////

void Node::_destroy()
{
  USUL_TRACE_SCOPE;
  _node = 0x0;
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
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::SceneGraph::IChild::IID:
    return static_cast < Usul::Interfaces::SceneGraph::IChild* > ( this );
  case Usul::Interfaces::SceneGraph::IViewAllMatrix::IID:
    return static_cast < Usul::Interfaces::SceneGraph::IViewAllMatrix* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of parents.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Node::getNumParents() const
{
  USUL_TRACE_SCOPE;
  NodePtr node ( this->node() );
  return ( ( true == node.valid() ) ? node->getNumParents() : 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the parent.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown::RefPtr Node::getParent ( unsigned int i ) const
{
  USUL_TRACE_SCOPE;
  NodePtr node ( this->node() );
  return Singleton::Factory::instance().createObject ( NodePtr ( ( true == node.valid() ) ? node->getParent ( i ) : 0x0 ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the node.
//
///////////////////////////////////////////////////////////////////////////////

Node::NodePtr Node::node()
{
  USUL_TRACE_SCOPE;
  ReadLock lock ( this );
  return _node;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the node.
//
///////////////////////////////////////////////////////////////////////////////

const Node::NodePtr Node::node() const
{
  USUL_TRACE_SCOPE;
  ReadLock lock ( this );
  return _node;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the matrix that will view the node.
//
///////////////////////////////////////////////////////////////////////////////

Node::Matrix Node::getViewAllMatrix ( double zScale ) const
{
  USUL_TRACE_SCOPE;

  NodePtr n ( this->node() );
  if ( false == n.valid() )
    return Matrix::getIdentity();

  // Get the bounding sphere of the group.
  osg::BoundingSphere bs ( n->getBound() );
  osg::Vec3d c ( bs.center() );

  // Push it back so we can see it.
  osg::Matrixd matrix;
  osg::Matrixd::value_type z ( zScale * bs.radius() + c[2] );
  matrix.makeTranslate ( -c[0], -c[1], -z );

  // Return the matrix.
  return Usul::Convert::Type<osg::Matrixd,Matrix>::convert ( matrix );
}
