
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
# include "Standard/SlTString.h"
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
  #endif

  // Set the name.
  this->_setName ( node._name );
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
  #endif

  // Free the name is if it has been allocated.
  this->_setName ( NULL );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the name.
//
/////////////////////////////////////////////////////////////////////////////

void SgNode::_setName ( const void *name )
{
  SL_ASSERT ( this );

  // Delete the current one if it's not null.
  if ( _name )
    ::free ( _name );

  // Initialize (every time).
  _name = NULL;

  // If the given name is not valid then just return.
  if ( NULL == name )
    return;

  // If we are in unicode mode...
  if ( this->hasNodeFlags ( SgNode::UNICODE_NAME ) )
  {
    SlTString<wchar_t> temp ( static_cast<const wchar_t *>(name) );
    _name = (void *) temp.getArrayCopy();
  }

  // Otherwise...
  else
    _name = ::strdup ( static_cast<const char *>(name) );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the name as an ascii string.
//
/////////////////////////////////////////////////////////////////////////////

void SgNode::setNameA ( const char *name )
{
  this->removeNodeFlags ( SgNode::UNICODE_NAME );
  this->_setName ( (const void *) name );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the name as a unicode string.
//
/////////////////////////////////////////////////////////////////////////////

void SgNode::setNameW ( const wchar_t *name )
{
  this->addNodeFlags ( SgNode::UNICODE_NAME );
  this->_setName ( (const void *) name );
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
  this->_setName ( node._name );
}
