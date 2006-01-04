
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
  Document ( const std::string & );

  // Get/set the header.
  const std::string &     header() const { return _header; }
  void                    header ( const std::string &h ) { _header = h; }

  // Load contents of file.
  void                    load ( const std::string & );

  // Write document to file.
  void                    write ( const std::string & );

protected:

  // Use reference counting.
  virtual ~Document();

private:

  // Do not copy.
  Document ( const Document & );
  Document &operator = ( const Document & );

  std::string _header;
};


// Header for the document.
namespace Constants { const std::string HEADER ( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" ); }


} // namespace XmlTree


#endif // _XML_TREE_DOCUMENT_H_
