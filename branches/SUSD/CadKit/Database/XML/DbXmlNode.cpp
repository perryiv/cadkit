
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  DbXmlNode.cpp: Base class for all nodes.
//
/////////////////////////////////////////////////////////////////////////////

#include "DbXmlPrecompiled.h"
#include "DbXmlNode.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlPrint.h"
# include "Standard/SlThread.h"
#endif

using namespace CadKit;

DB_XML_IMPLEMENT_NODE(DbXmlNode,SlRefBase);


/////////////////////////////////////////////////////////////////////////////
//
//  Unlike some object managing methodologies (like COM) that start objects 
//  with an initial reference count of one, nodes in a graph 
//  typically start with a reference count of zero. This puts the burden on 
//  the client to both reference and unreference the objects at all times. 
//  The "server" (in this case the "new" operator, and perhaps a group node 
//  that returns a child) will return a pointer to an unreferenced object.
//  This makes more sense for a graph.
//
/////////////////////////////////////////////////////////////////////////////

#define INITIAL_REF_COUNT 0


/////////////////////////////////////////////////////////////////////////////
//
//  Defaults.
//
/////////////////////////////////////////////////////////////////////////////

#define DEFAULT_NAME "untitled_node"


/////////////////////////////////////////////////////////////////////////////
//
//  Static data members.
//
/////////////////////////////////////////////////////////////////////////////

long DbXmlNode::_numNodes = 0;


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

DbXmlNode::DbXmlNode() : SlRefBase ( INITIAL_REF_COUNT ), 
  _name ( DEFAULT_NAME )
{
  SL_PRINT2 ( "In DbXmlNode::DbXmlNode(), this = %X\n", this );
  CadKit::Threads::safeIncrement ( _numNodes );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

DbXmlNode::DbXmlNode ( const char *name ) : SlRefBase ( INITIAL_REF_COUNT ), 
  _name ( ( name ) ? name : DEFAULT_NAME )
{
  SL_PRINT2 ( "In DbXmlNode::DbXmlNode(), this = %X\n", this );
  CadKit::Threads::safeIncrement ( _numNodes );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
/////////////////////////////////////////////////////////////////////////////

DbXmlNode::DbXmlNode ( const DbXmlNode &node ) : SlRefBase ( INITIAL_REF_COUNT ), 
  _name ( node._name )
{
  SL_PRINT2 ( "In DbXmlNode::DbXmlNode(), this = %X\n", this );
  CadKit::Threads::safeIncrement ( _numNodes );

  // Do not use the copy constructor. What does it mean to copy a tree? 
  // Does the new group have referenced pointers of the children, or copies 
  // of the children? Until this is resolved do not try to make a copy 
  // this way.
  SL_ASSERT ( 0 );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

DbXmlNode::~DbXmlNode()
{
  SL_PRINT2 ( "In DbXmlNode::~DbXmlNode(), this = %X\n", this );
  CadKit::Threads::safeDecrement ( _numNodes );
}
