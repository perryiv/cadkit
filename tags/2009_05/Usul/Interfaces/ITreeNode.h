
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
//  Interface for a node in a tree (directed graph).
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_TREE_NODE_H_
#define _USUL_INTERFACES_TREE_NODE_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct ITreeNode : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITreeNode );

  // Id for this interface.
  enum { IID = 2304172919u };

  // Get the number of children.
  virtual unsigned int                  getNumChildNodes() const = 0;

  // Get the child node.
  virtual ITreeNode::RefPtr             getChildNode ( unsigned int which ) = 0;

  // Set/get the name.
  virtual void                          setTreeNodeName ( const std::string & ) = 0;
  virtual std::string                   getTreeNodeName() const = 0;
};


} // end namespace Interfaces
} // end namespace Usul


#endif // _USUL_INTERFACES_TREE_NODE_H_
