
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

#include "Manager.h"

#include "Usul/Errors/Assert.h"
#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IDocumentCreate.h"
#include "Usul/Interfaces/GUI/IGUIDelegate.h"
#include "Usul/Interfaces/IDocumentSelect.h"

#include <algorithm>

using namespace Usul;
using namespace Usul::Documents;


///////////////////////////////////////////////////////////////////////////////
//
//  Static instance.
//
///////////////////////////////////////////////////////////////////////////////

Manager *Manager::_manager ( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Manager::Manager() :
_documents (),
_activeDocument ( 0x0 ),
_activeDocumentListeners (),
_activeView ( 0x0 ),
_activeViewListeners ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Manager::~Manager()
{
  _activeDocument = 0x0;
  _activeDocumentListeners.clear();

  _activeView = 0x0;
  _activeViewListeners.clear();

  // Sanity check.  Should be empty when manager is destroyed
  USUL_ASSERT ( _documents.empty() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Singleton.
//
///////////////////////////////////////////////////////////////////////////////

Manager &Manager::instance()
{
  if ( 0x0 == _manager )
    _manager = new Manager;
  return *_manager;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deletes current instance. Clears cached data.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::reset()
{
  delete _manager;
  _manager = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create new document(s) that can open given extension.
//
///////////////////////////////////////////////////////////////////////////////

Manager::Documents Manager::create ( const std::string &ext, Unknown *caller )
{
  // Typedefs.
  typedef Usul::Components::Manager PluginManager;
  typedef PluginManager::UnknownSet PluginSet;
  typedef PluginSet::iterator PluginItr;
  typedef Usul::Interfaces::IDocumentCreate::ValidQueryPtr CreatorPtr;

  // Ask for plugins that open documents.
  PluginSet plugins ( PluginManager::instance().getInterfaces ( Usul::Interfaces::IDocumentCreate::IID ) );

  Documents documentList;

  // Loop through the plugins.
  for ( PluginItr i = plugins.begin(); i != plugins.end(); ++i )
  {
    // Create the empty document.
    CreatorPtr creator ( (*i).get() );
    Document::RefPtr doc ( creator->createDocument ( caller ) );

    // Can the document open the given file?
    if ( doc.valid() && doc->canOpen ( ext ) )
      documentList.push_back( doc.get() );
  }

  // Return the document list, which may be empty;
  return documentList;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create new document with the given component
//
///////////////////////////////////////////////////////////////////////////////

Document* Manager::create ( Unknown *component, Unknown *caller )
{
  // Typedefs
  typedef Usul::Interfaces::IDocumentCreate::ValidQueryPtr CreatorPtr;

  // Initialize up here because we care about scope and order of destruction.
  Document::RefPtr doc ( 0x0 );

  {
    // Create the empty document.
    CreatorPtr creator ( component );
    doc = creator->createDocument ( caller );
  }

  // Return document pointer, which may still be null.
  return doc.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return all file-open filters.
//
///////////////////////////////////////////////////////////////////////////////

Manager::Filters Manager::filtersOpen() const
{
  // Typedefs.
  typedef Usul::Components::Manager PluginManager;
  typedef PluginManager::UnknownSet PluginSet;
  typedef PluginSet::iterator PluginItr;
  typedef Usul::Interfaces::IDocumentCreate::ValidQueryPtr CreatorPtr;

  // Ask for plugins that open documents.
  PluginSet plugins ( PluginManager::instance().getInterfaces ( Usul::Interfaces::IDocumentCreate::IID ) );

  // Initialize.
  Filters all;

  // Add all files filter.
  all.push_back ( Filters::value_type ( "All Files (*.*)", "*.*" ) );

  // Loop through the plugins.
  for ( PluginItr i = plugins.begin(); i != plugins.end(); ++i )
  {
    // Create the empty document.
    CreatorPtr creator ( (*i).get() );
    DocumentPtr doc ( creator->createDocument() );

    // Add to filters if possible.
    if ( doc.valid() )
    {
      const Filters current ( doc->filtersOpen() );
      all.insert ( all.end(), current.begin(), current.end() );
    }
  }

  // Return the filters.
  return all;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make this document active.  May be null.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::active ( IDocument *document )
{
  // Return now if document is already active
  if( document == _activeDocument.get() )
    return;

  // Save the old document.
  IDocument::RefPtr oldDoc ( _activeDocument );

  // Set the active document
  _activeDocument = document;

  // Notify any listeners we may have.
  for ( ActiveDocumentListeners::iterator iter = _activeDocumentListeners.begin(); iter != _activeDocumentListeners.end(); ++iter )
    (*iter)->activeDocumentChanged ( Usul::Interfaces::IUnknown::QueryPtr ( oldDoc.get() ), Usul::Interfaces::IUnknown::QueryPtr ( document ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the active document
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IDocument* Manager::active()
{
  return _activeDocument.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the active view
//
///////////////////////////////////////////////////////////////////////////////

void Manager::activeView ( View * view )
{
  // Return now if the view is already active.
  if( view == _activeView.get () )
    return;

  // Save the old view.
  View::RefPtr oldView ( _activeView );

  // Set the active view.
  _activeView = view;

  IDocument::RefPtr document ( view ? view->document() : 0x0 );

  // Set the active document.
  this->active ( document );

  // Notify any listeners we may have.
  for ( ActiveViewListeners::iterator iter = _activeViewListeners.begin(); iter != _activeViewListeners.end(); ++iter )
    (*iter)->activeViewChanged ( oldView.get(), view );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the active view.
//
///////////////////////////////////////////////////////////////////////////////

Manager::View* Manager::activeView ()
{
  return _activeView.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the document.  Does nothing if there are referenced windows
//
///////////////////////////////////////////////////////////////////////////////

void Manager::close ( Document *document )
{
  // Return now if the document still has windows open
  if ( document->numWindows() > 0 )
    return;

  Usul::Interfaces::IDocument::QueryPtr doc ( document );

  // Unset the active document if it is the one that is closing
  if ( doc.get() == _activeDocument.get() )
  {
    // Set the active document to null.  Call this instead of setting it directy.
    this->active ( 0x0 );
  }

  // Remove the document from the list
  _documents.remove ( document );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add document to internal list
//
///////////////////////////////////////////////////////////////////////////////

void Manager::add ( Document *document )
{
  _documents.push_back( document );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Send a message to all documents.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::sendMessage ( unsigned short message, const Document *skip )
{
  for ( Documents::iterator i = _documents.begin(); i != _documents.end(); ++i )
  {
    Document::RefPtr doc ( *i );
    if ( doc.valid() && ( skip != doc.get() ) )
    {
      doc->notify ( message );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find a delegate for the given document.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::delegate ( Document *document )
{
  Delegate::QueryPtr gd ( this->_findDelegate ( document ) );
  if( gd.valid() )
  {
    // Set the delegate
    document->delegate ( gd.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find a delegate for the given document.
//
///////////////////////////////////////////////////////////////////////////////

Manager::Delegate* Manager::_findDelegate ( Document * document )
{
  // Typedefs.
  typedef Usul::Components::Manager PluginManager;
  typedef PluginManager::UnknownSet PluginSet;
  typedef PluginSet::iterator PluginItr;

  // Make sure we have a valid document.
  if ( 0x0 != document )
  {
    // Ask for plugins that open documents.
    PluginSet plugins ( PluginManager::instance().getInterfaces ( Usul::Interfaces::IGUIDelegate::IID ) );

    // Loop through the plugins.
    for ( PluginItr i = plugins.begin(); i != plugins.end(); ++i )
    {
      Usul::Interfaces::IGUIDelegate::ValidQueryPtr gd ( i->get() );

      // Use the typeName as the token
      if( gd->doesHandle( document->typeName() ) )
        return gd.get();
    }
  }

  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Find a document and delegate for the given filename.
//
///////////////////////////////////////////////////////////////////////////////

Manager::DocumentInfo Manager::find ( const std::string& filename, Usul::Interfaces::IUnknown *caller )
{
  DocumentInfo info;

  bool loaded ( false );

  // The document to open.
  Document::RefPtr document ( 0x0 );

  // Check to see if there is a document already open with this filename.
  for( Documents::iterator i = _documents.begin(); i != _documents.end(); ++ i )
  {
    // Is a document open with this file name?
    if( (*i)->fileName() == filename )
    {
      document = (*i).get();
      loaded = true;
    }
  }

  // If we didn't find a document already open...
  if( 0x0 == document )
  {
    // Create the document.
    Documents documents ( this->create ( filename ) );

    // Make sure...
    if ( documents.empty() )
    {
      std::ostringstream message;
      message << "Error 2565795720: Failed to create document for file: " << filename;
      throw std::runtime_error ( message.str() );
    }

    Usul::Interfaces::IDocumentSelect::QueryPtr select ( caller );

    if ( select.valid() && documents.size() > 1 )
    {
      // Get the document from the user.
      document = select->selectDocument( documents );
    }
    else
    {
      // Just use the front if that's all we have, or if we don't have an interface to select a document.
      document = documents.front();
    }
  }

  // Set the members.
  info.document = document;
  info.delegate = this->_findDelegate ( document );
  info.loaded   = loaded;

  // Return what we found.
  return info;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add ActiveDocumentListener.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::addActiveDocumentListener    ( ActiveDocumentListener* listener )
{
  _activeDocumentListeners.push_back ( listener );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove ActiveDocumentListener.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::removeActiveDocumentListener ( ActiveDocumentListener* listener )
{
  _activeDocumentListeners.erase ( 
    std::remove_if ( _activeDocumentListeners.begin(), 
                     _activeDocumentListeners.end(), 
                     Usul::Interfaces::IActiveDocumentListener::RefPtr::IsEqual ( listener ) ),
    _activeDocumentListeners.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add ActiveViewListener.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::addActiveViewListener    ( ActiveViewListener* listener )
{
  _activeViewListeners.push_back ( listener );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove ActiveViewListener.
//
///////////////////////////////////////////////////////////////////////////////

void Manager::removeActiveViewListener ( ActiveViewListener* listener )
{
  _activeViewListeners.erase ( 
    std::remove_if ( _activeViewListeners.begin(), 
                     _activeViewListeners.end(), 
                     Usul::Interfaces::IActiveViewListener::RefPtr::IsEqual ( listener ) ),
    _activeViewListeners.end() );
}
