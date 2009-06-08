
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
  typedef XmlTree::Node NodeType;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Document );

  // Type id.
  USUL_DECLARE_TYPE_ID ( Document );

  // Construction
  Document();
  Document ( const std::string &name );

  // Get/set the header.
  const std::string &     header() const { return _header; }
  void                    header ( const std::string &h ) { _header = h; }

  // Load contents of file or stream.
  void                    load ( const std::string &file );
  void                    load ( const std::istream& in );

  // Load from a file already loaded in memory.
  void                    loadFromMemory ( const std::string& buffer );

  // Write document to file or stream.
  void                    write ( const std::string &file, bool includeHeader = true ) const;
  void                    write ( std::ostream &out, bool includeHeader = true ) const;

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
