
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Node.h"

#include "Factory.h"

using namespace CadKit::OSG::Glue;

Node::Node() : _key ( 0 )
{
  long l = System::DateTime::Now.Ticks;
  _key = (unsigned int)(l & 0xFFFFFFFF);

  Factory::instance().create( _key );
}

Node::~Node()
{
  Factory::instance().remove( _key );
}

Node::!Node()
{
  Factory::instance().remove( _key );
}


osg::Node* Node::node()
{
  return Factory::instance().node( _key );
}

void Node::node( osg::Node* node )
{
  Factory::instance().node( _key, node );
}

unsigned int Node::nodePtr()
{
  return reinterpret_cast < unsigned int > ( Factory::instance().node( _key ) );
}

void Node::nodePtr ( unsigned int ptr )
{
  Factory::instance().node ( _key, reinterpret_cast < osg::Node* > ( ptr ) );
}

