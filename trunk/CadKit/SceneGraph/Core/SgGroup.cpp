
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
//  SgGroup.cpp: container for SgNodes
//
/////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgGroup.h"

using namespace CadKit;

SG_IMPLEMENT_DYNAMIC_NODE(SgGroup,SgNode);


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgGroup::SgGroup() : SgNode()
{
  SL_PRINT ( "SgGroup::SgGroup(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

SgGroup::~SgGroup()
{
  SL_PRINT ( "SgGroup::~SgGroup(), this = %X\n", this );

  // When the vector of node "smart pointers" dies, it will automatically 
  // release the nodes.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Add a child node.
//
/////////////////////////////////////////////////////////////////////////////

void SgGroup::addChild ( SgNode *child )
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

SgNode *SgGroup::getChild ( const Index &index ) const
{
  SL_ASSERT ( this && index < this->getNumChildren() );
  return _nodes[index].getValue();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get i'th child of the given type.
//
/////////////////////////////////////////////////////////////////////////////

SgNode *SgGroup::getChildOfType ( const SlType *type, const Index &index ) const
{
  SL_ASSERT ( this && type && index < this->getNumChildren() );
  Index count = 0;
  SgNode *node;

  // Loop through all the nodes.
  for ( Nodes::const_iterator i = _nodes.begin(); i != _nodes.end(); ++i )
  {
    // Get the current child.
    node = i->getValue();
    SL_ASSERT ( node );

    // If it's the right type...
    if ( node->isOfType ( type ) )
    {
      // If this is the correct one then return it.
      if ( count == index ) 
        return node;

      // Increment the counter.
      ++count;
    }
  }

  // If we get this far then we didn't find it.
  return NULL;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get i'th child of the given exact type.
//
/////////////////////////////////////////////////////////////////////////////

SgNode *SgGroup::getChildOfExactType ( const SlType *type, const Index &index ) const
{
  SL_ASSERT ( this && type && index < this->getNumChildren() );
  Index count = 0;
  SgNode *node;

  // Loop through all the nodes.
  for ( Nodes::const_iterator i = _nodes.begin(); i != _nodes.end(); ++i )
  {
    // Get the current child.
    node = i->getValue();
    SL_ASSERT ( node );

    // If it's the right type...
    if ( node->isOfExactType ( type ) )
    {
      // If this is the correct one then return it.
      if ( count == index ) 
        return node;

      // Increment the counter.
      ++count;
    }
  }

  // If we get this far then we didn't find it.
  return NULL;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Insert the child.
//
/////////////////////////////////////////////////////////////////////////////

SgGroup::Index SgGroup::insertChild ( const Index &index, SgNode *child )
{
  SL_ASSERT ( this && child );

  // Get the number of children.
  Index numChildren = this->getNumChildren();

  // Trivial case.
  if ( 0 == numChildren || index >= numChildren )
  {
    this->addChild ( child );
    return numChildren;
  }

  // When we get to here the index should be in range.
  SL_ASSERT ( index < numChildren );

  // Get the iterator associated with the index.
  Nodes::iterator i = this->_getIterator ( index );

  // Stick it in the list. This will call the constructor for SlRefPtr 
  // which will reference the node.
  _nodes.insert ( i, child );

  // Return the index that it was inserted at.
  return index;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Remove all the children.
//
/////////////////////////////////////////////////////////////////////////////

void SgGroup::removeAllChildren()
{
  // Clear the list. Since the lise contains smart-pointers, the child 
  // nodes will automatically be released.
  _nodes.clear();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Remove all the children except the given one.
//
/////////////////////////////////////////////////////////////////////////////

void SgGroup::removeAllExcept ( const SgNode *notMe )
{
  SgNode *node;
  Nodes::iterator i = _nodes.begin();

  // Loop through all the nodes.
  while ( i != _nodes.end() )
  {
    // Get the current node.
    node = i->getValue();
    SL_ASSERT ( node );

    // If we're supposed to remove this one...
    if ( node != notMe )
    {
      // Erase this node from the list. This will either return the next 
      // valid iterator or _nodes.end() (if we just erased the last one).
      // The node pointer will automatically be released.
      i = _nodes.erase ( i );
    }

    // Otherwise, we skip this one.
    ++i;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
//  Remove the child.
//
/////////////////////////////////////////////////////////////////////////////

bool SgGroup::removeChild ( SgNode *child )
{
  SL_ASSERT ( this && child );

  // Loop through all the nodes.
  for ( Nodes::iterator i = _nodes.begin(); i != _nodes.end(); ++i )
  {
    // If the current child is the one we are supposed to remove...
    if ( i->getValue() == child )
    {
      // Erase this node from the list. We do not care about the 
      // iterator that returns because we are returing.
      // The node pointer will automatically be released.
      i = _nodes.erase ( i );

      // Success.
      return true;
    }
  }

  // If we get to here then the node is not a child.
  return false;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Remove the child.
//
/////////////////////////////////////////////////////////////////////////////

void SgGroup::removeChild ( const Index &index )
{
  SL_ASSERT ( this && index < this->getNumChildren() );

  // Get the iterator associated with the index.
  Nodes::iterator i = this->_getIterator ( index );

  // Erase this node from the list.
  // The node pointer will automatically be released.
  _nodes.erase ( i );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get the iterator associated with the index.
//
/////////////////////////////////////////////////////////////////////////////

SgGroup::Nodes::iterator SgGroup::_getIterator ( const Index &index )
{
  SL_ASSERT ( this && index < this->getNumChildren() );

  // Initialize the counter.
  Index count = 0;

  // Loop through all the nodes.
  for ( Nodes::iterator i = _nodes.begin(); i != _nodes.end(); ++i )
  {
    if ( count == index ) 
      return i;
    ++count;
  }

  // If we get to here then it didn't work.
  return _nodes.end();
}
