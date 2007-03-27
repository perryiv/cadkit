
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  DbXmlGroup.cpp: container for DbXmlNodes
//
/////////////////////////////////////////////////////////////////////////////

#include "DbXmlPrecompiled.h"
#include "DbXmlGroup.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlPrint.h"
# include "Standard/SlAssert.h"
#endif

using namespace CadKit;

DB_XML_IMPLEMENT_DYNAMIC_NODE(DbXmlGroup,DbXmlNode);


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

DbXmlGroup::DbXmlGroup() : DbXmlNode()
{
  SL_PRINT2 ( "In DbXmlGroup::DbXmlGroup(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

DbXmlGroup::DbXmlGroup ( const char *name ) : DbXmlNode ( name )
{
  SL_PRINT2 ( "In DbXmlGroup::DbXmlGroup(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

DbXmlGroup::~DbXmlGroup()
{
  SL_PRINT2 ( "In DbXmlGroup::~DbXmlGroup(), this = %X\n", this );

  // When the vector of node "smart pointers" dies, it will automatically 
  // release the nodes.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Add a child node.
//
/////////////////////////////////////////////////////////////////////////////

void DbXmlGroup::addChild ( DbXmlNode *child )
{
  SL_ASSERT ( this && child && child != this );

  // Add it to the end of the list, node will automatically be referenced.
  _nodes.push_back ( child );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get the child.
//
/////////////////////////////////////////////////////////////////////////////

const DbXmlNode *DbXmlGroup::getChild ( const unsigned int &index ) const
{
  SL_ASSERT ( this && index < this->getNumChildren() );

  // Loop through all the nodes.
  unsigned int count = 0;
  for ( Nodes::const_iterator i = _nodes.begin(); i != _nodes.end(); ++i )
    if ( count == index )
      return *i;

  // If we get to here then it didn't work.
  return NULL;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get the child.
//
/////////////////////////////////////////////////////////////////////////////

DbXmlNode *DbXmlGroup::getChild ( const unsigned int &index )
{
  SL_ASSERT ( this && index < this->getNumChildren() );

  // Loop through all the nodes.
  unsigned int count = 0;
  for ( Nodes::iterator i = _nodes.begin(); i != _nodes.end(); ++i )
    if ( count == index )
      return *i;

  // If we get to here then it didn't work.
  return NULL;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Remove all the children.
//
/////////////////////////////////////////////////////////////////////////////

void DbXmlGroup::removeAllChildren()
{
  // Clear the list. Since the lise contains smart-pointers, the child 
  // nodes will automatically be released.
  _nodes.clear();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Traverse the children.
//
/////////////////////////////////////////////////////////////////////////////

bool DbXmlGroup::traverse ( DbXmlVisitor &visitor )
{
  // Loop through all the nodes.
  for ( Nodes::iterator i = _nodes.begin(); i != _nodes.end(); ++i )
    if ( false == (*i)->accept ( visitor ) )
      return false;

  // It worked.
  return true;
}
