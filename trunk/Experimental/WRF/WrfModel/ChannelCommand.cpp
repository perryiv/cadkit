
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "ChannelCommand.h"

#include "Usul/Trace/Trace.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ChannelCommand::ChannelCommand ( const std::string& name, unsigned int index, WRFDocument* doc ) :
  BaseClass ( 0x0 ),
  _name ( name ),
  _index ( index ),
  _document ( doc )
{
  USUL_TRACE_SCOPE;
  this->text ( "Switch to " + name );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ChannelCommand::~ChannelCommand ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void ChannelCommand::_execute ()
{
  USUL_TRACE_SCOPE;
  if ( _document.valid () )
    _document->currentChannel ( _index );
}
