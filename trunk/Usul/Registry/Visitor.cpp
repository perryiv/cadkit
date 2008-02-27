
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base visitor class for the registry.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Registry/Visitor.h"
#include "Usul/Registry/Database.h"
#include "Usul/Trace/Trace.h"

using namespace Usul::Registry;

USUL_IMPLEMENT_TYPE_ID ( Visitor );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Visitor::Visitor() : BaseClass()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Visitor::~Visitor()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the registry database.
//
///////////////////////////////////////////////////////////////////////////////

void Visitor::visit ( Usul::Registry::Database *db )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  if ( 0x0 != db )
  {
    this->visit ( db->root() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the registry node.
//
///////////////////////////////////////////////////////////////////////////////

void Visitor::visit ( Usul::Registry::Node *node )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  if ( 0x0 != node )
  {
    Guard guard2 ( node );
    typedef Usul::Registry::Node::Kids Kids;
    for ( Kids::iterator i = node->begin(); i != node->end(); ++i )
    {
      Usul::Registry::Node::RefPtr node ( i->second );
      if ( true == node.valid() )
      {
        node->accept ( this );
      }
    }
  }
}