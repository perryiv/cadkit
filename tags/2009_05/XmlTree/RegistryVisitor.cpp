
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Visitor class that builds an XML tree from Usul's registry.
//
///////////////////////////////////////////////////////////////////////////////

#include "XmlTree/RegistryVisitor.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Registry/Database.h"
#include "Usul/Scope/PushPop.h"
#include "Usul/Trace/Trace.h"

using namespace XmlTree;

USUL_IMPLEMENT_TYPE_ID ( RegistryVisitor );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RegistryVisitor::RegistryVisitor() : BaseClass(),
  _document ( new XmlTree::Document ),
  _stack()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

RegistryVisitor::~RegistryVisitor()
{
  USUL_TRACE_SCOPE;
  _document = 0x0;
  while ( false == _stack.empty() )
    _stack.pop();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the registry database.
//
///////////////////////////////////////////////////////////////////////////////

void RegistryVisitor::visit ( Usul::Registry::Database *db )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  if ( ( 0x0 != db ) && ( 0x0 != db->root() ) )
  {
    // Set document's name.
    XmlTree::Node::RefPtr node ( _document.get() );
    Usul::Scope::PushPop<NodeStack> pushPop ( _stack, node );
    _document->name ( db->root()->name() );

    // Visit document's children.
    BaseClass::visit ( db );

    // Because of the way the registry database has one node, the document 
    // always ends up with a single child node with the same name, and then 
    // multiple children under that. We put the single child's children 
    // directly under the document.
    if ( 1 == _document->children().size() )
    {
      XmlTree::Node::RefPtr child ( _document->children().front() );
      _document->children().clear();
      _document->children().assign ( child->children().begin(), child->children().end() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Visit the registry node.
//
///////////////////////////////////////////////////////////////////////////////

void RegistryVisitor::visit ( Usul::Registry::Node *n )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( 0x0 == n )
    return;

  // Default value for get function.  Needed to be on the stack for gcc 3.2.
  std::string t ( "" );

  // Make new node.
  XmlTree::Node::RefPtr node ( new XmlTree::Node ( n->name(), n->get ( t ) ) );

  // Add it to the current node.
  _stack.top()->children().push_back ( node.get() );

  // Make new node the current one.
  Usul::Scope::PushPop<NodeStack> pushPop ( _stack, node );

  // Call base class to visit the children.
  BaseClass::visit ( n );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the document.
//
///////////////////////////////////////////////////////////////////////////////

XmlTree::Document *RegistryVisitor::document()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _document.get();
}
