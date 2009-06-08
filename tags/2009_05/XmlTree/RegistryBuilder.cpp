
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that traverses an XML tree and builds Usul's registry.
//
///////////////////////////////////////////////////////////////////////////////

#include "XmlTree/RegistryBuilder.h"
#include "XmlTree/Document.h"

#include "Usul/Registry/Database.h"
#include "Usul/Trace/Trace.h"

using namespace XmlTree;

USUL_IMPLEMENT_TYPE_ID ( RegistryBuilder );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RegistryBuilder::RegistryBuilder() : BaseClass()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

RegistryBuilder::~RegistryBuilder()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to add nodes to the registry.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void traverse ( const XmlTree::Node *xn, Usul::Registry::Node *rn )
  {
    if ( 0x0 == xn || 0x0 == rn )
      return;

    typedef XmlTree::Node::Children::const_iterator Itr;
    for ( Itr i = xn->children().begin(); i != xn->children().end(); ++i )
    {
      XmlTree::Node::RefPtr child ( i->get() );
      if ( true == child.valid() )
      {
        Usul::Registry::Node &node ( (*rn)[child->name()] );
        node.set ( child->value() );
        Helper::traverse ( child.get(), &node );
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the registry.
//
///////////////////////////////////////////////////////////////////////////////

void RegistryBuilder::build ( const XmlTree::Node *node, Usul::Registry::Node &reg )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( 0x0 != node )
  {
    // Traverse the tree.
    Helper::traverse ( node, &reg );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the registry.
//
///////////////////////////////////////////////////////////////////////////////

void RegistryBuilder::build ( const XmlTree::Node *node, Usul::Registry::Database &reg )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( 0x0 != node )
  {
    // Set registry's name.
    reg.root()->name ( node->name() );

    // Traverse the tree.
    Helper::traverse ( node, reg.root() );
  }
}
