
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
//  DbXmlGroup.h: Base class for group nodes.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_XML_LIBRARY_GROUP_H_
#define _CADKIT_DATABASE_XML_LIBRARY_GROUP_H_

#include "DbXmlNode.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <list>
#endif


namespace CadKit
{
class DB_XML_API DbXmlGroup : public DbXmlNode
{
public:

  DbXmlGroup();
  DbXmlGroup ( const char *name );

  // Add the node to the group. You cannot add this instance to itself.
  void                      addChild ( DbXmlNode *child );

  // Get the i'th child.
  const DbXmlNode *         getChild ( const unsigned int &index ) const;
  DbXmlNode *               getChild ( const unsigned int &index );

  // Get the number of children.
  unsigned int              getNumChildren() const { return _nodes.size(); }

  // Remove the children. When the nodes are removed their reference count is
  // decremented. Any node that has it's reference decrement to zero is automatically
  // deleted.
  void                      removeAllChildren();

  // Traverse the children.
  bool                      traverse ( DbXmlVisitor &visitor );

protected:

  typedef std::list<DbXmlNode::Ptr> Nodes;
  Nodes _nodes;

  virtual ~DbXmlGroup();

  DB_XML_DECLARE_DYNAMIC_NODE(DbXmlGroup,0x00001016);
};
};

#endif
