
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
//  DbXmlVisitor: The base visitor class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_XML_LIBRARY_BASE_VISITOR_H_
#define _CADKIT_DATABASE_XML_LIBRARY_BASE_VISITOR_H_

#include "DbXmlApi.h"
#include "DbXmlVisitorMacros.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlRefBase.h"
# include <list>
#endif


namespace CadKit
{
class DbXmlNode;
class DbXmlGroup;
class DbXmlLeaf;

class DB_XML_API DbXmlVisitor : public SlRefBase
{
public:

  // Push/pop the node. These are called from each node's accept() function.
  // Normal usage does not require calling these, but they have to be public.
  void                  pushNode ( DbXmlNode * );
  void                  popNode();

  // Reset the visitor. Call this in between scene visits to ensure that
  // any accumulated states are reset.
  virtual bool          reset();

  // The visit functions.
  virtual bool          visit ( DbXmlNode & ) = 0;
  virtual bool          visit ( DbXmlGroup & ) = 0;
  virtual bool          visit ( DbXmlLeaf & ) = 0;

protected:

  typedef std::list<DbXmlNode *> Stack; // Compiler doesn't like SlRefPtr here.
  Stack _stack;

  DbXmlVisitor();
  virtual ~DbXmlVisitor();

  DB_XML_DECLARE_VISITOR ( DbXmlVisitor, 0x00001039 );
};

}; // namespace CadKit

#endif // _CADKIT_DATABASE_XML_LIBRARY_BASE_VISITOR_H_
