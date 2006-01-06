
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  XML document class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _XML_TREE_DOCUMENT_H_
#define _XML_TREE_DOCUMENT_H_

#include "XmlTree/Node.h"

#include <string>
#include <iosfwd>


namespace XmlTree {


class XML_TREE_EXPORT Document : public XmlTree::Node
{
public:

  // Typedefs.
  typedef XmlTree::Node BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Document );

  // Construction
  Document();
  Document ( const std::string &name );

  // Get/set the header.
  const std::string &     header() const { return _header; }
  void                    header ( const std::string &h ) { _header = h; }

  // Load contents of file.
  void                    load ( const std::string &file );

  // Write document to file or stream.
  void                    write ( const std::string &file );
  void                    write ( std::ostream &out );

protected:

  // Use reference counting.
  virtual ~Document();

private:

  // Do not copy.
  Document ( const Document & );
  Document &operator = ( const Document & );

  std::string _header;
};


} // namespace XmlTree


#endif // _XML_TREE_DOCUMENT_H_
