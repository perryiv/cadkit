
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

#include "Usul/Documents/Manager.h"
#include "Usul/Interfaces/IDocumentCreate.h"
#include "Usul/Interfaces/GUI/IGUIDelegateNotify.h"
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
  // Do not lock the mutex. This function is re-entrant.

  Usul::Interfaces::IDocumentCreate::QueryPtr dc ( _component );
  if ( false == dc.valid () )
    return;

  // Create a document.
  Usul::Documents::Document::RefPtr document ( dc->createDocument ( this->caller () ) );

  // Make sure we got an active document.
  if ( document.valid () )
  {
    // Assign a default name.
    document->defaultFilename();

    // Find a delegate for the document.
    Usul::Documents::Manager::instance().delegate ( document.get() );

    // See if the caller wants to be notified when the document finishes loading.
    Usul::Interfaces::IGUIDelegateNotify::QueryPtr notify ( this->caller() );

    // Notify.
    if ( notify.valid() )
      notify->notifyDocumentFinishedLoading ( document );

    // Add the document to the manager.
    Usul::Documents::Manager::instance().add( document );
  }
}
