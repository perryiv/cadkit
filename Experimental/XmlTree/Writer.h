
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  XML writer class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _XML_TREE_WRITER_H_
#define _XML_TREE_WRITER_H_

#include "XmlTree/Export.h"

#include <string>


namespace XmlTree {

class Document;

class XML_TREE_EXPORT Writer
{
public:

  // Construction, destruction.
  Writer();
  ~Writer();

  // Write the document to file.
  void                    write ( const std::string &file, const Document *doc );

private:

  std::string _indent;
};


} // namespace XmlTree


#endif // _XML_TREE_WRITER_H_
