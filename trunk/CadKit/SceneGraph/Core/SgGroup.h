
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
//  SgGroup.h: Base class for group nodes.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_GROUP_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_GROUP_H_

#include "SgNode.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <vector>
#endif


namespace CadKit
{
class SG_API SgGroup : public SgNode
{
public:

  typedef IndexGroup Index;

  SgGroup();

  // Add the node to the group. You cannot add this instance to itself.
  void            addChild ( SgNode *child );

  // Get the i'th child.
  SgNode *        getChild ( const Index &index ) const;

  // Get the i'th child of the given type.
  SgNode *        getChildOfType ( const SlType *type, const Index &index ) const;

  // Get the i'th child of the given exact type.
  SgNode *        getChildOfExactType ( const SlType *type, const Index &index ) const;

  // Get the number of children.
  Index           getNumChildren() const { return _nodes.size(); }

  // Insert a child. If "index" is too small it will prepend, if it's too big it 
  // will append. It returns the position that the node was inserted.
  Index           insertChild ( const Index &index, SgNode *child );

  // Remove the children. When the nodes are removed their reference count is
  // decremented. Any node that has it's reference decrement to zero is automatically
  // deleted.
  void            removeAllChildren();
  void            removeAllExcept ( const SgNode *notMe );

  // Remove the specified child, decrementing its reference number. If the child's
  // reference number goes to zero then it's automatically deleted. The first one 
  // returns false if it cannot find the specified child. The second one will assert
  // (in debug build) if the given index is out of range.
  bool            removeChild ( SgNode *child );
  void            removeChild ( const Index &index );

protected:

  typedef std::vector<SgNode::Ptr> Nodes;
  Nodes _nodes; // See note 3ce3a140-2edf-11d4-98a4-0040054c86c7.

  virtual ~SgGroup();

  Nodes::iterator _getIterator ( const Index &index );

  SG_DECLARE_DYNAMIC_NODE(SgGroup,0x00001016);
};
};

#endif
