
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

///////////////////////////////////////////////////////////////////////////////
//
//  SgVisitor: The base visitor class.
//
///////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgVisitor.h"
#include "SgNode.h"

using namespace CadKit;

SG_IMPLEMENT_VISITOR(SgVisitor,SlRefBase);


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SgVisitor::SgVisitor() : SlRefBase ( 0 )
{
  SL_PRINT ( "SgVisitor::SgVisitor(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SgVisitor::~SgVisitor()
{
  SL_PRINT ( "SgVisitor::~SgVisitor(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reset the visitor. Call this in between scene traversals to ensure that
//  any accumulated states are reset. Here in the base class there is nothing 
//  to reset. I didn't want this to be a pure virtual because it isn't 
//  something that visitors should have to define.
//
///////////////////////////////////////////////////////////////////////////////

bool SgVisitor::reset()
{
  SL_ASSERT ( this );
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Push the node.
//
///////////////////////////////////////////////////////////////////////////////

void SgVisitor::pushNode ( SgNode *node )
{
  SL_ASSERT ( this );

  // This is not a list of ref-pointers, so we have to manually do it.
  _stack.push_back ( node );
  node->ref();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pop the top node.
//
///////////////////////////////////////////////////////////////////////////////

void SgVisitor::popNode()
{
  SL_ASSERT ( this );
  SL_ASSERT ( false == _stack.empty() );

  // This is not a list of ref-pointers, so we have to manually do it.
  _stack.back()->unref();
  _stack.pop_back();
}
