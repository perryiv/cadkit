
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Node.h"

using namespace CadKit::OSG::Glue;


namespace Detail
{
  struct InitOSG
  {
    InitOSG()
    {
      osg::Referenced::setThreadSafeReferenceCounting ( true );
    }
  } _init_osg;
}


Node::Node() : _node( 0x0 )
{
}

Node::~Node()
{
  this->_unreference();
}

Node::!Node()
{
  this->_unreference();
}


osg::Node* Node::node()
{
  return _node;
}

void Node::node( osg::Node* node )
{
  this->_unreference();

  _node = node;
  _node->ref();
}

void Node::_unreference()
{
  if( 0x0 != _node )
  {
    _node->unref();
    _node = 0x0;
  }
}


System::IntPtr Node::nodePtr()
{
  return System::IntPtr ( _node );
}

void Node::nodePtr ( System::IntPtr ptr )
{
  this->node( reinterpret_cast< osg::Node* > ( ptr.ToPointer() ) );
}

