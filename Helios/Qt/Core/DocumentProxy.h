
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  I can't get the QMetaObject::invokeMethod function to work without this class.  
//  I'm using it to pass a document as an agrument to a slot function.  
//
//  Document::RefPtr will compile, but won't work at run-time.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CADKIT_HELIOS_QT_CORE_DOCUMENT_PROXY_H__
#define __CADKIT_HELIOS_QT_CORE_DOCUMENT_PROXY_H__

#include "Usul/Documents/Document.h"


namespace CadKit {
namespace Helios {
namespace Core {

class DocumentProxy
{
public:

  /// Useful typedes.
  typedef Usul::Documents::Document Document;

  /// Default Constructor.
  /// This is for Q_DECLARE_METATYPE.  Do not use.
  DocumentProxy();

  /// Constructor.
  DocumentProxy ( Document::RefPtr doc );

  /// Copy Constructor.
  DocumentProxy ( const DocumentProxy& rhs );

  /// Assignment.
  DocumentProxy& operator = ( const DocumentProxy &rhs );

  /// Destructor.
  ~DocumentProxy();

  /// Get/Set the document.
  Document::RefPtr         getDocument() const;
  void                     setDocument ( Document::RefPtr document );

private:

  Document *_document;
};

}
}
}

#endif // __CADKIT_HELIOS_QT_CORE_DOCUMENT_PROXY_H__
