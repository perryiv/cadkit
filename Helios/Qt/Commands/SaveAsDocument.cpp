
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Qt/Commands/SaveAsDocument.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Trace/Trace.h"

using namespace CadKit::Helios::Commands;

USUL_IMPLEMENT_COMMAND ( SaveAsDocument );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SaveAsDocument::SaveAsDocument ( IUnknown *caller ) : BaseClass ( caller )
{
  USUL_TRACE_SCOPE;
  this->text ( "&Save As..." );
  this->shortcut ( "" );
  this->statusTip ( "Save the active document as another document." );
  this->toolTip ( "Save the active document as another document." );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SaveAsDocument::~SaveAsDocument()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Save the active document.
//
///////////////////////////////////////////////////////////////////////////////

void SaveAsDocument::_execute()
{
  USUL_TRACE_SCOPE;

  Usul::Interfaces::IDocument::RefPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  if ( true == document.valid() )
  {
    document->saveAs ( this->caller() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we enabled?
//
///////////////////////////////////////////////////////////////////////////////

bool SaveAsDocument::updateEnable() const
{
  Usul::Interfaces::IDocument::RefPtr document ( Usul::Documents::Manager::instance().activeDocument() );

  return document.valid();
}

