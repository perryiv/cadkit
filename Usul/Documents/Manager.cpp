
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
#include "Usul/Interfaces/IGUIDelegate.h"

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
_active ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Manager::~Manager()
{
  _active = 0x0;

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
//  Create new document that can open given extension.
//
///////////////////////////////////////////////////////////////////////////////

Document *Manager::create ( const std::string &ext, Unknown *caller )
{
  // Typedefs.
  typedef Usul::Components::Manager PluginManager;
  typedef PluginManager::UnknownSet PluginSet;
  typedef PluginSet::iterator PluginItr;
  typedef Usul::Interfaces::IDocumentCreate::ValidQueryPtr CreatorPtr;

  // Ask for plugins that open documents.
  PluginSet plugins ( PluginManager::instance().getInterfaces ( Usul::Interfaces::IDocumentCreate::IID ) );

  // Initialize up here because we care about scope and order of destruction.
  Document::RefPtr doc ( 0x0 );

  // Loop through the plugins.
  for ( PluginItr i = plugins.begin(); i != plugins.end(); ++i )
  {
    // Create the empty document.
    CreatorPtr creator ( (*i).get() );
    doc = creator->createDocument ( caller );

    // Can the document open the given file?
    if ( doc.valid() && doc->canOpen ( ext ) )
      break;

    // Otherwise, reset document pointer.
    else
      doc = 0x0;
  }

  // Return document pointer, which may still be null.
  return doc.release();
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
//  Open new document.
//
///////////////////////////////////////////////////////////////////////////////

Document *Manager::open ( const std::string &file, Unknown *caller )
{
  // Create document.
  Document::RefPtr doc ( this->create ( file, caller ) );
  if ( !doc.valid() )
    return 0x0;

  // Open the file.
  doc->open ( file, caller );

  // Return the document.
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
  all.push_back ( Filters::value_type ( "All Files (*.*).", "*.*" ) );

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

void Manager::active ( Document *document )
{
  // Return now if document is already active
  if( document == _active.get() )
    return;

  // Notify the active document that it is no longer active.
  if( _active )
    _active->noLongerActive( document->typeName() );
  
  // Get the old type.
  std::string oldType ( _active ? _active->typeName() : "" );

  // Set the active document
  _active = document;

  // Notify the document that it is now active.
  if( _active )
    _active->nowActive( oldType );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the active document
//
///////////////////////////////////////////////////////////////////////////////

Document* Manager::active()
{
  return _active.get();
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

  // Unset the active document if it is the one that is closing
  if ( document == _active.get() )
    _active = 0x0;

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
  // Typedefs.
  typedef Usul::Components::Manager PluginManager;
  typedef PluginManager::UnknownSet PluginSet;
  typedef PluginSet::iterator PluginItr;

  // Ask for plugins that open documents.
  PluginSet plugins ( PluginManager::instance().getInterfaces ( Usul::Interfaces::IGUIDelegate::IID ) );

  // Loop through the plugins.
  for ( PluginItr i = plugins.begin(); i != plugins.end(); ++i )
  {
    Usul::Interfaces::IGUIDelegate::ValidQueryPtr gd ( i->get() );

    // Use the typeName as the token
    if( gd->doesHandle( document->typeName() ) )
    {
      // Set the delegate
      document->delegate ( gd.get() );
    }
  }
}


