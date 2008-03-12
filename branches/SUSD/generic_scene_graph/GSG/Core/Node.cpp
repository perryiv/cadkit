
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

#include "GSG/Core/Precompiled.h"
#include "GSG/Core/Node.h"
#include "GSG/Core/Visitor.h"
#include "GSG/Core/PushPop.h"

using namespace GSG;

GSG_IMPLEMENT_ACCEPT_NODE ( Node );


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

Node::Node() : Referenced(),
  _bound()
{
}


/////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
/////////////////////////////////////////////////////////////////////////////

Node::Node ( const Node &n ) : Referenced ( n ),
  _bound ( n._bound )
{
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

Node::~Node()
{
}


/////////////////////////////////////////////////////////////////////////////
//
//  Update this node.
//
/////////////////////////////////////////////////////////////////////////////

void Node::update()
{
  Lock lock ( this );

  // Calculate the bounding sphere.
  this->calculateBoundingSphere();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get the bounding sphere.
//
/////////////////////////////////////////////////////////////////////////////

const BoundingSphere &Node::boundingSphere() const
{
  Lock lock ( this );
  return _bound;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the bounding sphere.
//
/////////////////////////////////////////////////////////////////////////////

void Node::boundingSphere ( const BoundingSphere &b )
{
  Lock lock ( this );
  _bound = b;
}