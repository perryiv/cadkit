
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "DrtStartOver.h"

#include "Usul/Trace/Trace.h"
#include "Usul/Interfaces/IDrtCommands.h"

USUL_IMPLEMENT_COMMAND ( DrtStartOver );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

DrtStartOver::DrtStartOver ( Usul::Interfaces::IUnknown * caller ) :
  BaseClass ( caller )
{
  USUL_TRACE_SCOPE;
  this->text ( "Start Over" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

DrtStartOver::~DrtStartOver ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void DrtStartOver::_execute ()
{
  USUL_TRACE_SCOPE;
  //this->_dummy();
#if 1
  Usul::Interfaces::IDrtCommands::QueryPtr nav ( this->caller() );
  if ( nav.valid () )
  {
		nav->startOver();
  }
#endif
}

