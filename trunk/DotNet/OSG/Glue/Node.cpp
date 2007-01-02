
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OSG/Glue/Node.h"

#include "Usul/Pointers/Intrusive.h"

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


Node::Node ( osg::Node *n ) : _node ( n )
{
  Usul::Pointers::reference ( _node );
}


Node::Node( System::IntPtr ptr ) : _node( 0x0 )
{
  this->nodePtr( ptr );
}


Node::~Node()
{
  this->clear();
}


Node::!Node()
{
  this->clear();
}


osg::Node* Node::node()
{
  return _node;
}


void Node::node ( osg::Node* node )
{
  Usul::Pointers::unreference ( _node );
  _node = node;
  Usul::Pointers::reference ( _node );
}


System::IntPtr Node::nodePtr()
{
  return System::IntPtr ( _node );
}


void Node::nodePtr ( System::IntPtr ptr )
{
  this->node( reinterpret_cast< osg::Node* > ( ptr.ToPointer() ) );
}


void Node::clear()
{
  Usul::Pointers::unreference ( _node );
  _node = 0x0;
}
