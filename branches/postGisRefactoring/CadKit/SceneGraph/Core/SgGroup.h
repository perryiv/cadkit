
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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

  SgGroup();

  // Add the node to the group. You cannot add this instance to itself.
  void            addChild ( SgNode *child );

  // Get the i'th child.
  SgNode *        getChild ( const unsigned int &index ) const;

  // Get the i'th child of the given type.
  SgNode *        getChildOfType ( const SlType *type, const unsigned int &index ) const;

  // Get the i'th child of the given exact type.
  SgNode *        getChildOfExactType ( const SlType *type, const unsigned int &index ) const;

  // Get the number of children.
  unsigned int    getNumChildren() const { return _nodes.size(); }

  // Insert a child. If "index" is too small it will prepend, if it's too big it 
  // will append. It returns the position that the node was inserted.
  unsigned int    insertChild ( const unsigned int &index, SgNode *child );

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
  void            removeChild ( const unsigned int &index );

protected:

  typedef std::vector<SgNode::Ptr> Nodes;
  Nodes _nodes; // See note 3ce3a140-2edf-11d4-98a4-0040054c86c7.

  virtual ~SgGroup();

  Nodes::iterator _getIterator ( const unsigned int &index );

  SG_DECLARE_DYNAMIC_NODE ( SgGroup, 1033948674 );
};
};

#endif
