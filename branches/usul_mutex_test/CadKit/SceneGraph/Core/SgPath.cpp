
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgPath.h: A path through the scene graph tree.
//
///////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgPath.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlPrint.h"
# include "Standard/SlAssert.h"
#endif

using namespace CadKit;

#define INITIAL_REF_COUNT 0

SL_IMPLEMENT_DYNAMIC_CLASS ( SgPath, SlRefBase );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor. 
//
///////////////////////////////////////////////////////////////////////////////

SgPath::SgPath() : SlRefBase ( INITIAL_REF_COUNT ),
  _nodes ( new Nodes )
{
  SL_PRINT2 ( "SgPath::SgPath(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor. 
//
///////////////////////////////////////////////////////////////////////////////

SgPath::SgPath ( const SgPath &path ) : SlRefBase ( INITIAL_REF_COUNT ),
  _nodes ( new Nodes )
{
  SL_PRINT2 ( "SgPath::SgPath(), this = %X\n", this );
  _nodes->assign ( path._nodes->begin(), path._nodes->end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor. 
//
///////////////////////////////////////////////////////////////////////////////

SgPath::~SgPath()
{
  SL_PRINT2 ( "SgPath::~SgPath(), this = %X\n", this );

  // We don't have to do anything here because the vector's elements are 
  // "reference pointers". Their destructors will release the nodes.
  if ( _nodes )
    delete _nodes;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the list and unref the nodes. 
//
//  Keep the addRef/Release because all nodes in the path already have a 
//  reference count of at least 1. You don't want nodes in the path to die 
//  based on something the client does (before the path is deleted).
//
///////////////////////////////////////////////////////////////////////////////

void SgPath::clear()
{
  SL_ASSERT ( this );

  // Clear the list. This will automatically release the nodes.
  _nodes->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append the node to the path and ref the node.
//
//  Keep the addRef/Release because all nodes in the path already have a 
//  reference count of at least 1. You don't want nodes in the path to die 
//  based on something the client does (before the path is deleted).
//
///////////////////////////////////////////////////////////////////////////////

void SgPath::append ( SgNode *node ) 
{
  SL_ASSERT ( this && node );

  // Stick it at the end. This will automatically reference.
  _nodes->push_back ( node );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the requested node.
//
///////////////////////////////////////////////////////////////////////////////

SgNode *SgPath::getNode ( const unsigned int &index ) const
{
  SL_ASSERT ( this && index < this->getNumNodes() );

  // Initialize the counter.
  unsigned int count = 0;

  // Loop through all the nodes.
  for ( Nodes::const_iterator i = _nodes->begin(); i != _nodes->end(); ++i )
  {
    if ( index == count ) 
      return i->getValue();
    ++count;
  }

  // Should never get this far.
  SL_ASSERT ( 0 );
  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the node is in the path.
//
///////////////////////////////////////////////////////////////////////////////

bool SgPath::hasNode ( const SgNode *node ) const
{
  SL_ASSERT ( this && node );

  // Loop through all the nodes.
  for ( Nodes::const_iterator i = _nodes->begin(); i != _nodes->end(); ++i )
  {
    // Return true if the current node is the same as the given node.
    if ( node == i->getValue() ) 
      return true;
  }

  // We didn't find the node.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the type is in the path.
//
///////////////////////////////////////////////////////////////////////////////

bool SgPath::hasNodeOfType ( const SlType *type ) const
{
  SL_ASSERT ( this && type );

  // Loop through all the nodes.
  for ( Nodes::const_iterator i = _nodes->begin(); i != _nodes->end(); ++i )
  {
    // Return true if the current node is the correct type.
    if ( i->getValue()->isOfType ( type ) ) 
      return true;
  }

  // We didn't find a node of the correct type.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the type is in the path.
//
///////////////////////////////////////////////////////////////////////////////

bool SgPath::hasNodeOfExactType ( const SlType *type ) const
{
  SL_ASSERT ( this && type );

  // Loop through all the nodes.
  for ( Nodes::const_iterator i = _nodes->begin(); i != _nodes->end(); ++i )
  {
    // Return true if the current node is the correct type.
    if ( i->getValue()->isOfExactType ( type ) ) 
      return true;
  }

  // We didn't find a node of the correct type.
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value of this path from the given one.
//
///////////////////////////////////////////////////////////////////////////////

void SgPath::setValue ( const SgPath &path )
{
  // Clear the path (deleting the links and unrefing the nodes).
  this->clear();

  // Loop through all of the given path's nodes.
  for ( Nodes::const_iterator i = path._nodes->begin(); i != path._nodes->end(); ++i )
  {
    // Append the current node our list. This will automatically reference.
    _nodes->push_back ( *i );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the first occurance of a node of the given type.
//
///////////////////////////////////////////////////////////////////////////////

SgNode *SgPath::getFirstOfType ( const SlType *type ) const
{
  SL_ASSERT ( this && type );

  // Loop through all the nodes.
  for ( Nodes::const_iterator i = _nodes->begin(); i != _nodes->end(); ++i )
  {
    // If the node is the right type then return it.
    if ( i->getValue()->isOfType ( type ) ) 
      return i->getValue();
  }

  // Didn't find a node with the right type.
  return NULL;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the last occurance of a node of the given type.
//
///////////////////////////////////////////////////////////////////////////////

SgNode *SgPath::getLastOfType ( const SlType *type ) const
{
  SL_ASSERT ( this && type );

  // Loop through all the nodes.
  SL_ASSERT ( 0 ); // Make sure this works. Had to hack this in because gcc and VC++ didn't like me using const_reverse_iterator.
  for ( Nodes::const_iterator i = _nodes->end(); i != _nodes->begin(); --i )
  {
    // If the node is the right type then return it.
    if ( i->getValue()->isOfType ( type ) ) 
      return (SgNode *) (i->getValue());
  }

  // Didn't find a node with the right type.
  return NULL;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the first node.
//
///////////////////////////////////////////////////////////////////////////////

SgNode *SgPath::getHead() const
{
  SL_ASSERT ( this );
  return _nodes->front().getValue();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the last node.
//
///////////////////////////////////////////////////////////////////////////////

SgNode *SgPath::getTail() const
{
  SL_ASSERT ( this );
  return _nodes->back().getValue();
}


#ifdef CADKIT_DEFINE_SG_PATH_WRITE_TO_OSTREAM


///////////////////////////////////////////////////////////////////////////////
//
//  For debugging.
//
///////////////////////////////////////////////////////////////////////////////

void SgPath::write ( ::ostream &out ) const
{
  SL_ASSERT ( this );

  // Initialize the counter.
  unsigned int count = 0;

  // Loop through all the nodes.
  for ( Nodes::const_iterator i = _nodes->begin(); i != _nodes->end(); ++i )
  {
    // Print the node pointer.
    out << count++ << " " << (*i) << ::endl;
  }
}


#endif // CADKIT_DEFINE_SG_PATH_WRITE_TO_OSTREAM


#ifdef CADKIT_DEFINE_SG_PATH_WRITE_TO_STD_OSTREAM


///////////////////////////////////////////////////////////////////////////////
//
//  For debugging.
//
///////////////////////////////////////////////////////////////////////////////

void SgPath::write ( std::ostream &out ) const
{
  SL_ASSERT ( this );

  // Initialize the counter.
  unsigned int count = 0;

  // Loop through all the nodes.
  for ( Nodes::const_iterator i = _nodes->begin(); i != _nodes->end(); ++i )
  {
    // Print the node pointer.
    out << count++ << " " << (*i) << ::endl;
  }
}


#endif // CADKIT_DEFINE_SG_PATH_WRITE_TO_STD_OSTREAM
