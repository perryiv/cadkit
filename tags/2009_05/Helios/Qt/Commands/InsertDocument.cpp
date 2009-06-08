
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Commands/InsertDocument.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Trace/Trace.h"

using namespace CadKit::Helios::Commands;

USUL_IMPLEMENT_COMMAND ( InsertDocument );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

InsertDocument::InsertDocument ( IUnknown *caller ) : BaseClass ( caller )
{
  USUL_TRACE_SCOPE;
  this->text ( "&Insert..." );
  this->shortcut ( "Ctrl+I" );
  this->statusTip ( "Insert into the active document." );
  this->toolTip ( "Insert into the active document." );
  this->iconPath ( "" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

InsertDocument::~InsertDocument ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Insert into the active document.
//
///////////////////////////////////////////////////////////////////////////////

void InsertDocument::_execute ()
{
  USUL_TRACE_SCOPE;

  Usul::Interfaces::IDocument::RefPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  if( document.valid() )
  {
    document->insert( this->caller() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we enabled?
//
///////////////////////////////////////////////////////////////////////////////

bool InsertDocument::updateEnable () const
{
  Usul::Interfaces::IDocument::RefPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  return document.valid() && false == document->filtersInsert().empty();
}
