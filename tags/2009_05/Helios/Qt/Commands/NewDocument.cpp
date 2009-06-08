
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Command to open a document.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Commands/NewDocument.h"
#include "Helios/Qt/Core/Constants.h"

#include "Usul/Components/Manager.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Interfaces/IDocumentCreate.h"
#include "Usul/Interfaces/IInitNewDocument.h"
#include "Usul/Interfaces/GUI/IGUIDelegateNotify.h"
#include "Usul/Threads/Named.h"
#include "Usul/Trace/Trace.h"

using namespace CadKit::Helios::Commands;

USUL_IMPLEMENT_COMMAND ( NewDocument );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

NewDocument::NewDocument ( IUnknown *caller, IUnknown *component, const std::string& name ) : BaseClass ( caller ),
_component ( component )
{
  USUL_TRACE_SCOPE;
  this->text ( name );
  this->statusTip ( "Create new " + name + " document" );
  this->toolTip ( this->statusTip() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

NewDocument::NewDocument ( const NewDocument& rhs ) : BaseClass ( rhs ),
_component ( rhs._component )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
///////////////////////////////////////////////////////////////////////////////

NewDocument& NewDocument::operator = ( const NewDocument& rhs ) 
{
  USUL_TRACE_SCOPE;
  BaseClass::operator = ( rhs );
  _component = rhs._component;
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

NewDocument::~NewDocument()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the command. This function is re-entrant.
//
///////////////////////////////////////////////////////////////////////////////

void NewDocument::_execute()
{
  USUL_TRACE_SCOPE;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );
  // Do not lock the mutex. This function is re-entrant.

  Usul::Interfaces::IDocumentCreate::QueryPtr dc ( _component );
  if ( false == dc.valid () )
    return;

  // Create a document.
  Usul::Documents::Document::RefPtr document ( dc->createDocument ( this->caller () ) );

  // Make sure we got an active document.
  if ( false == document.valid() )
    return;

  // Find a delegate for the document.
  Usul::Documents::Manager::instance().delegate ( document.get() );
  
  // Make sure a delegate was found.
  if ( 0x0 == document->delegate() )
    Usul::Exceptions::Thrower < std::runtime_error > ( "Error 3380055569: Could not find delegate for: ", document->typeName() );

  // Assign a default name.
  document->defaultFilename();

  // Try to initialize the document.
  NewDocument::_initNewDocument ( IUnknown::QueryPtr ( document ), this->caller() );

  // See if the caller wants to be notified when the document finishes loading.
  Usul::Interfaces::IGUIDelegateNotify::QueryPtr notify ( this->caller().get() );

  // Notify.
  if ( notify.valid() )
    notify->notifyDocumentFinishedLoading ( document );

  // Add the document to the manager.
  Usul::Documents::Manager::instance().add ( document );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Try to initialize the new document.
//
///////////////////////////////////////////////////////////////////////////////

void NewDocument::_initNewDocument ( IUnknown *document, IUnknown *caller )
{
  USUL_TRACE_SCOPE_STATIC;
  USUL_THREADS_ENSURE_GUI_THREAD ( return );

  // Get all plugins that initialize documents.
  typedef Usul::Interfaces::IInitNewDocument IInitNewDocument;
  typedef Usul::Components::Manager::UnknownSet UnknownSet;
  UnknownSet unknowns ( Usul::Components::Manager::instance().getInterfaces ( IInitNewDocument::IID ) );
  for ( UnknownSet::iterator i = unknowns.begin(); i != unknowns.end(); ++i )
  {
    IInitNewDocument::QueryPtr initializer ( (*i).get() );
    if ( true == initializer.valid() )
    {
      // Ask the plugin if it does our type of document.
      if ( true == initializer->handlesDocumentType ( document ) )
      {
        // Initialize the document.
        initializer->initNewDocument ( document, caller );

        // Return now!
        return;
      }
    }
  }
}
