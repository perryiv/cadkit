
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  DbXmlGroup.cpp: container for DbXmlNodes
//
/////////////////////////////////////////////////////////////////////////////

#include "DbXmlPrecompiled.h"
#include "DbXmlGroup.h"

using namespace CadKit;

DB_XML_IMPLEMENT_DYNAMIC_NODE(DbXmlGroup,DbXmlNode);


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

DbXmlGroup::DbXmlGroup() : DbXmlNode()
{
  SL_PRINT ( "In DbXmlGroup::DbXmlGroup(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

DbXmlGroup::DbXmlGroup ( const char *name ) : DbXmlNode ( name )
{
  SL_PRINT ( "In DbXmlGroup::DbXmlGroup(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

DbXmlGroup::~DbXmlGroup()
{
  SL_PRINT ( "In DbXmlGroup::~DbXmlGroup(), this = %X\n", this );

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
