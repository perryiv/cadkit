
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Visitor class that builds an XML tree from Usul's registry.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _XML_TREE_REGISTRY_VISITOR_H_
#define _XML_TREE_REGISTRY_VISITOR_H_

#include "XmlTree/Document.h"

#include "Usul/Registry/Visitor.h"

#include <stack>
#include <string>


namespace XmlTree {


class XML_TREE_EXPORT RegistryVisitor : public Usul::Registry::Visitor
{
public:

  // Typedefs.
  typedef Usul::Registry::Visitor BaseClass;
  typedef BaseClass::Mutex Mutex;
  typedef BaseClass::Guard Guard;
  typedef std::stack < XmlTree::Node::RefPtr > NodeStack;

  // Type information.
  USUL_DECLARE_TYPE_ID ( RegistryVisitor );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( RegistryVisitor );

  // Construction
  RegistryVisitor();

  // Visit the object.
  virtual void              visit ( Usul::Registry::Database * );
  virtual void              visit ( Usul::Registry::Node * );

  // Get the document.
  XmlTree::Document *       document();

protected:

  // Use reference counting.
  virtual ~RegistryVisitor();

private:

  // No copying or assignment.
  RegistryVisitor &operator = ( const RegistryVisitor & );
  RegistryVisitor ( const RegistryVisitor & );

  XmlTree::Document::RefPtr _document;
  NodeStack _stack;
};


} // namespace XmlTree


#endif // _XML_TREE_REGISTRY_VISITOR_H_
