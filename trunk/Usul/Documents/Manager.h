
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document manager class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_DOCUMENT_MANAGER_H_
#define _USUL_DOCUMENT_MANAGER_H_

#include "Usul/Export/Export.h"
#include "Usul/Documents/Document.h"

#include "Usul/Interfaces/IUnknown.h"

#include <string>
#include <vector>

namespace Usul {
namespace Documents {

class USUL_EXPORT Manager
{
public:

  // Typedefs.
  typedef Usul::Interfaces::IUnknown            Unknown;
  typedef std::pair<std::string,std::string>    Filter;
  typedef std::vector<Filter>                   Filters;
  typedef Document::RefPtr                      DocumentPtr;
  typedef std::list< DocumentPtr >              Documents;
  typedef Usul::Interfaces::IDocument           IDocument;

  // Singleton.
  static Manager &      instance();

  // Make this document active
  void                  active ( Document* );

  // Get the active document
  Document*             active ();

  // Add document to internal list
  void                  add ( Document * );

  // Close the document.  Does nothing if there are referenced windows
  void                  close ( Document* );

  // Create new document that can open given extension.
  Document *            create ( const std::string &ext, Unknown *caller = 0x0 );

  // Create new document with the given component
  Document *            create ( Unknown *component, Unknown *caller = 0x0 );

  // Find a delegate for the given document.
  void                  delegate ( Document *document );

  // Get the document list
  Documents&            documents()       { return _documents; }
  const Documents&      documents() const { return _documents; }

  // Return all file-open filters.
  Filters               filtersOpen() const;

  // Open new document.
  Document *            open ( const std::string &file, Unknown *caller = 0x0 );

  // Deletes current instance. Clears cached data.
  static void           reset();

  // Send the message to all documents.
  void                  sendMessage ( unsigned short message, const Document *skip = 0x0 );

private:

  Manager();
  Manager ( const Manager & );
  Manager &operator = ( const Manager & );
  ~Manager();

  
  Documents       _documents;
  DocumentPtr     _active;
  

  static Manager *_manager;
};


}; // namespace Documents
}; // namespace Usul


#endif // _USUL_DOCUMENT_MANAGER_H_
