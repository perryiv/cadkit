
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Group class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _STAR_SYSTEM_GROUP_CLASS_H_
#define _STAR_SYSTEM_GROUP_CLASS_H_

#include "Minerva/Core/TileEngine/Node.h"

#include <vector>


namespace Minerva {
namespace Core {
namespace TileEngine {


class MINERVA_EXPORT Group : public Node
{
public:

  // Useful typedefs.
  typedef Node BaseClass;
  typedef BaseClass::BuildOptions BuildOptions;
  typedef std::vector<Node::RefPtr> Nodes;

  // Helper macro for repeated code.
  MINERVA_DEFINE_NODE_CLASS ( Group );

  // Constructor
  Group();

  // Add a node.
  void                      add ( Node * );

  // Is the group empty?
  bool                      empty() const;

  // Set/get the nodes.
  void                      nodes ( Nodes & );
  const Nodes &             nodes() const;
  Nodes &                   nodes();

protected:

  // Use reference counting.
  virtual ~Group();

private:

  // No copying or assignment.
  Group ( const Group & );
  Group &operator = ( const Group & );

  void                      _destroy();

  Nodes _nodes;

  SERIALIZE_XML_DEFINE_MEMBERS ( Group );
};


} // namespace TileEngine
} // namespace Core
} // namespace Minerva


#endif // _STAR_SYSTEM_GROUP_CLASS_H_
