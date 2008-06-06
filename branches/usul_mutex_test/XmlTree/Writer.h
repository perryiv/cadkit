
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
#include <iosfwd>


namespace XmlTree {

class Document;

class XML_TREE_EXPORT Writer
{
public:

  // Construction, destruction.
  Writer();
  ~Writer();

  // Write the document to file or stream.
  void                    write ( const Document *doc, const std::string &file, bool includeHeader = true );
  void                    write ( const Document *doc, std::ostream &out, bool includeHeader = true );

private:

  std::string _indent;
};


} // namespace XmlTree


#endif // _XML_TREE_WRITER_H_
