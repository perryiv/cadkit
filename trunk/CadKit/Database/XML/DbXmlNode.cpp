
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
  CadKit::threadSafeIncrement ( _numNodes );
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
  CadKit::threadSafeIncrement ( _numNodes );
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
  CadKit::threadSafeIncrement ( _numNodes );

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
  CadKit::threadSafeDecrement ( _numNodes );
}
