
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgNode.cpp: Base class for all scene graph nodes.
//
///////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgNode.h"

#include "Standard/SlPrint.h"
#include "Standard/SlThread.h"

using namespace CadKit;

SG_IMPLEMENT_NODE ( SgNode, SlRefBase );


///////////////////////////////////////////////////////////////////////////////
//
//  Unlike some object managing methodologies (like COM) that start objects 
//  with an initial reference count of one, nodes in a scene graph 
//  typically start with a reference count of zero. This puts the burden on 
//  the client to both reference and unreference the objects at all times. 
//  The "server" (in this case the "new" operator) will return a pointer to 
//  an unreferenced object. This makes more sense for a scene graph.
//
///////////////////////////////////////////////////////////////////////////////

#define INITIAL_REF_COUNT 0


///////////////////////////////////////////////////////////////////////////////
//
//  Static data members.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
long SgNode::_numNodes = 0;
#endif

#ifdef _DEBUG
#include <map>
typedef std::map<SgNode *, SgNode *> NodeMap;
NodeMap _nodeMap;
#endif

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SgNode::SgNode() : SlRefBase ( INITIAL_REF_COUNT ), 
  _flags ( SgNode::ACCEPT_VISITORS ),
  _reservedData ( 0 ),
  _clientData ( 0 ),
  _name ( NULL ),
  _renderTime ( 0 )
{
  SL_PRINT2 ( "SgNode::SgNode(), this = %X\n", this );
  #ifdef _DEBUG
  CadKit::Threads::safeIncrement ( _numNodes );
  _nodeMap[this] = this;
  #endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SgNode::SgNode ( const SgNode &node ) : SlRefBase ( INITIAL_REF_COUNT ), 
  _flags ( node._flags ),
  _reservedData ( node._reservedData ), 
  _clientData ( node._clientData ),
  _name ( NULL ),
  _renderTime ( node._renderTime )
{
  SL_PRINT2 ( "SgNode::SgNode(), this = %X\n", this );
  #ifdef _DEBUG
  CadKit::Threads::safeIncrement ( _numNodes );
  _nodeMap[this] = this;
  #endif

  // Set the name.
  this->setName ( node._name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SgNode::~SgNode()
{
  SL_PRINT2 ( "SgNode::~SgNode(), this = %X\n", this );
  #ifdef _DEBUG
  CadKit::Threads::safeDecrement ( _numNodes );
  _nodeMap.erase ( _nodeMap.find ( this ) );
  #endif

  // Free the name is if it has been allocated.
  this->setName ( NULL );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does the given node have the same visible properties?
//
///////////////////////////////////////////////////////////////////////////////

bool SgNode::isEqualVisibleProperties ( const SgNode &node ) const
{
  SL_ASSERT ( this );

  // The default implementation has to return true because child classes 
  // call their base class's function.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the value of the node.
//
///////////////////////////////////////////////////////////////////////////////

void SgNode::setValue ( const SgNode &node )
{
  SL_ASSERT ( this );
  _reservedData = node._reservedData;
  _clientData = node._clientData;
  _flags = node._flags;
  this->setName ( node._name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the name of the node.
//
///////////////////////////////////////////////////////////////////////////////

void SgNode::setName ( const char *name )
{
  SL_ASSERT ( this );

  // Delete the old one if there is one.
  if ( _name )
    ::free ( _name );

  // Assign the new name.
#ifdef _WIN32
  _name = ( name ) ? ::_strdup ( name ) : NULL;
#else
  _name = ( name ) ? ::strdup ( name ) : NULL;
#endif
}
