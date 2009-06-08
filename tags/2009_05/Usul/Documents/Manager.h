
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
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/GUI/IGUIDelegate.h"
#include "Usul/Interfaces/IActiveDocumentListener.h"
#include "Usul/Interfaces/IActiveViewListener.h"
#include "Usul/Interfaces/IView.h"

#include <string>
#include <vector>

namespace Usul {
namespace Documents {

class USUL_EXPORT Manager
{
public:

  // Typedefs.
  typedef Usul::Interfaces::IUnknown                      Unknown;
  typedef std::pair<std::string,std::string>              Filter;
  typedef std::vector<Filter>                             Filters;
  typedef Document::RefPtr                                DocumentPtr;
  typedef std::list< DocumentPtr >                        Documents;
  typedef Usul::Interfaces::IDocument                     IDocument;
  typedef Usul::Interfaces::IGUIDelegate                  Delegate;
  typedef Usul::Interfaces::IActiveDocumentListener       ActiveDocumentListener;
  typedef std::vector < ActiveDocumentListener::RefPtr >  ActiveDocumentListeners;
  typedef Usul::Interfaces::IView                         View;
  typedef Usul::Interfaces::IActiveViewListener           ActiveViewListener;
  typedef std::vector < ActiveViewListener::RefPtr >      ActiveViewListeners;
  typedef Usul::Threads::Mutex                            Mutex;
  typedef Usul::Threads::Guard < Mutex >                  Guard;

  // Struct that contains infomation on how to load a document.
  struct DocumentInfo
  {
    DocumentPtr         document;
    Delegate::QueryPtr  delegate;
    bool                loaded;
  };

  // Singleton.
  static Manager &      instance();
  
  /// Get/Set the active document.
  void                  activeDocument ( IDocument* );
  IDocument*            activeDocument ();

  /// Get/Set the active view.
  void                  activeView ( View * );
  View*                 activeView ();

  // Add document to internal list
  void                  add ( Document * );

  /// Is there a document that can open this file?
  bool                  canOpen ( const std::string& file ) const;

  // Close the document.  Does nothing if there are referenced windows
  void                  close ( Document* );

  // Create new document(s) that can open given extension. Pass the "check" flags as a selection criteria.
  Documents             create ( const std::string &ext, Unknown *caller = 0x0, bool checkOpen = true, bool checkSave = false );

  // Create new document with the given component
  Document *            create ( Unknown *component, Unknown *caller = 0x0 );

  // Find a delegate for the given document.
  void                  delegate ( Document *document );

  // Get the document list.
  Documents&            documents()       { return _documents; }
  const Documents&      documents() const { return _documents; }

  // Return all file-open filters.
  Filters               filtersOpen() const;

  // Find a document and delegate for the given filename.
  DocumentInfo          find ( const std::string& filename, Usul::Interfaces::IUnknown *caller = 0x0, bool checkOpen = true, bool checkSave = false );

  // Get the mutex. Use with caution.
  Mutex&                mutex() { return *_mutex; }

  // Deletes current instance. Clears cached data.
  static void           reset();

  /// Add/Remove IActiveDocumentListeners.
  void                  addActiveDocumentListener    ( ActiveDocumentListener* listener );
  void                  removeActiveDocumentListener ( ActiveDocumentListener* listener );

  /// Add/Remove IActiveViewListeners.
  void                  addActiveViewListener    ( ActiveViewListener* listener );
  void                  removeActiveViewListener ( ActiveViewListener* listener );

protected:

  // Find a delegate for the given document.
  Delegate*             _findDelegate ( Document * document );

private:

  Manager();
  Manager ( const Manager & );
  Manager &operator = ( const Manager & );
  ~Manager();

  mutable Mutex           *_mutex;  
  Documents                _documents;
  IDocument::RefPtr        _activeDocument;
  ActiveDocumentListeners  _activeDocumentListeners;
  View::RefPtr             _activeView;
  ActiveViewListeners      _activeViewListeners;

  static Manager *_manager;
};


} // namespace Documents
} // namespace Usul


#endif // _USUL_DOCUMENT_MANAGER_H_
