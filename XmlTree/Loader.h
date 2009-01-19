
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  XML loader class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _XML_TREE_LOADER_H_
#define _XML_TREE_LOADER_H_

#include "XmlTree/Export.h"

#include <string>
#include <vector>

namespace XmlTree {

class Document;

class XML_TREE_EXPORT Loader
{
public:

  // Construction, destruction.
  Loader();
  ~Loader();

  // Load contents of file and build the document.
  void load ( const std::string &, Document * );
  void load ( const std::istream &, Document * ) const;

  // Load from a file already loaded in memory.
  void loadFromMemory ( const std::string& buffer, Document * ) const;
};


} // namespace XmlTree


#endif // _XML_TREE_LOADER_H_
