
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
//  SgNode.cpp: Base class for all scene graph nodes.
//
/////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgNode.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlPrint.h"
# include "Standard/SlThread.h"
# ifndef _WIN32 // unix
#  define __USE_GNU  // For wcsdup().
#  include <wchar.h> // For wcsdup().
# endif
#endif

using namespace CadKit;

SG_IMPLEMENT_NODE(SgNode,SlRefBase);


/////////////////////////////////////////////////////////////////////////////
//
//  Unlike some object managing methodologies (like COM) that start objects 
//  with an initial reference count of one, nodes in a scene graph 
//  typically start with a reference count of zero. This puts the burden on 
//  the client to both reference and unreference the objects at all times. 
//  The "server" (in this case the "new" operator, and perhaps a group node 
//  that returns a child) will return a pointer to an unreferenced object.
//  This makes more sense for a scene graph.
//
/////////////////////////////////////////////////////////////////////////////

#define INITIAL_REF_COUNT 0


/////////////////////////////////////////////////////////////////////////////
//
//  Static data members.
//
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
long SgNode::_numNodes = 0;
#endif

#ifdef _DEBUG
typedef std::map<SgNode *, SgNode *> NodeMap;
NodeMap _nodeMap;
#endif

/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgNode::SgNode() : SlRefBase ( INITIAL_REF_COUNT ), 
  _flags ( SgNode::ACCEPT_VISITORS ),
  _reservedData ( 0 ),
  _clientData ( 0 ),
  _name ( NULL )
{
  SL_PRINT2 ( "SgNode::SgNode(), this = %X\n", this );
  #ifdef _DEBUG
  CadKit::threadSafeIncrement ( _numNodes );
  _nodeMap[this] = this;
  #endif
}


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgNode::SgNode ( const SgNode &node ) : SlRefBase ( INITIAL_REF_COUNT ), 
  _flags ( node._flags ),
  _reservedData ( node._reservedData ), 
  _clientData ( node._clientData ),
  _name ( NULL )
{
  SL_PRINT2 ( "SgNode::SgNode(), this = %X\n", this );
  #ifdef _DEBUG
  CadKit::threadSafeIncrement ( _numNodes );
  _nodeMap[this] = this;
  #endif

  // Set the name.
  this->setName ( node._name );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

SgNode::~SgNode()
{
  SL_PRINT2 ( "SgNode::~SgNode(), this = %X\n", this );
  #ifdef _DEBUG
  CadKit::threadSafeDecrement ( _numNodes );
  _nodeMap.erase ( _nodeMap.find ( this ) );
  #endif

  // Free the name is if it has been allocated.
  this->setName ( NULL );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Does the given node have the same visible properties?
//
/////////////////////////////////////////////////////////////////////////////

bool SgNode::isEqualVisibleProperties ( const SgNode &node ) const
{
  SL_ASSERT ( this );

  // The default implementation has to return true because child classes 
  // call their base class's function.
  return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the value of the node.
//
/////////////////////////////////////////////////////////////////////////////

void SgNode::setValue ( const SgNode &node )
{
  SL_ASSERT ( this );
  _reservedData = node._reservedData;
  _clientData = node._clientData;
  _flags = node._flags;
  this->setName ( node._name );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the name the node.
//
/////////////////////////////////////////////////////////////////////////////

void SgNode::setName ( const char *name )
{
  SL_ASSERT ( this );

  // Delete the old one if there is one.
  if ( _name )
    ::free ( _name );

  // Assign the new name.
  _name = ( name ) ? ::strdup ( name ) : NULL;
}
