
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

#include "xercesc/dom/DOM.hpp"

#include <string>


namespace XmlTree {
namespace Detail {


class RootImpl
{
public:

  // Construction, destruction.
  RootImpl();
  ~RootImpl();

  // Clear the tree.
  void                                clear();

  // Load contents of file.
  void                                load ( const std::string & );

  // Set the value of the node. Create it if needed.
  void                                node ( const std::string &path, char delim, unsigned int value );
  void                                node ( const std::string &path, char delim, int value );
  void                                node ( const std::string &path, char delim, bool value );
  void                                node ( const std::string &path, char delim, const std::string &value );

  // Write tree to file.
  void                                write ( const std::string & );

protected:

  void                                _ensureDocument();

  std::string                         _format ( const xercesc::DOMException &, const std::string & );

private:

  // Do not copy.
  RootImpl ( const RootImpl & );
  RootImpl &operator = ( const RootImpl & );

  xercesc::DOMDocument *_document;
};


} // namespace Detail
} // namespace XmlTree


#endif // _XML_TREE_ROOT_IMPLEMENTATION_H_
