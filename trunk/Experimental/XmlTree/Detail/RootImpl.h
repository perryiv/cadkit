
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  XML tree root class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _XML_TREE_ROOT_IMPLEMENTATION_H_
#define _XML_TREE_ROOT_IMPLEMENTATION_H_

#include "boost/shared_ptr.hpp"

#include "xercesc/dom/DOM.hpp"

#include <string>


namespace XmlTree {
namespace Detail {


class RootImpl
{
public:

  // Typedefs.
  typedef boost::shared_ptr<xercesc::DOMNode> NodePtr;

  // Construction, destruction.
  RootImpl();
  ~RootImpl();

  // Clear the tree.
  void                                clear();

  // Load contents of file.
  void                                load ( const std::string & );

protected:

  std::string                         _format ( const xercesc::DOMException &, const std::string & );

private:

  // Do not copy.
  RootImpl ( const RootImpl & );
  RootImpl &operator = ( const RootImpl & );

  NodePtr _document;
};


} // namespace Detail
} // namespace XmlTree


#endif // _XML_TREE_ROOT_IMPLEMENTATION_H_
