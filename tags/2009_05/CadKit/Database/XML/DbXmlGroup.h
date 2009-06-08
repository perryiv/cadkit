
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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

  DB_XML_DECLARE_DYNAMIC_NODE ( DbXmlGroup, 1032912327 );
};
};

#endif
