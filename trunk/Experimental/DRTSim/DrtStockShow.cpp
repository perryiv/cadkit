
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "DrtStockShow.h"

#include "Usul/Trace/Trace.h"
#include "Usul/Interfaces/IDrtCommands.h"

USUL_IMPLEMENT_COMMAND ( DrtStockShow );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DrtStockShow::DrtStockShow ( Usul::Interfaces::IUnknown * caller ) :
  BaseClass ( caller )
{
  USUL_TRACE_SCOPE;
  this->text ( "Show Stocks" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DrtStockShow::~DrtStockShow ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void DrtStockShow::_execute ()
{
  USUL_TRACE_SCOPE;
  //this->_dummy();
#if 1
  Usul::Interfaces::IDrtCommands::QueryPtr nav ( this->caller() );
  if ( nav.valid () )
  {
	if( true == nav->stockShow() )
		nav->stockShow( false );
	else
		nav->stockShow( true );
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the radio button.  Return true to have radio checked.
//
///////////////////////////////////////////////////////////////////////////////

bool DrtStockShow::updateCheck () const
{
  USUL_TRACE_SCOPE;

  // Implement me to update radio state.
  Usul::Interfaces::IDrtCommands::QueryPtr nav ( const_cast < Usul::Interfaces::IUnknown * > ( this->caller() ) );

  return nav->stockShow();
}
