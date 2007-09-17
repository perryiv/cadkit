
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Experimental/WRF/WrfModel/ChannelCommand.h"
#include "Experimental/WRF/WrfModel/WRFDocument.h"

#include "Usul/Trace/Trace.h"

USUL_IMPLEMENT_COMMAND ( ChannelCommand );

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

  if ( 0x0 != _document )
    _document->ref ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ChannelCommand::~ChannelCommand ()
{
  USUL_TRACE_SCOPE;

  if ( 0x0 != _document )
    _document->unref ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void ChannelCommand::_execute ()
{
  USUL_TRACE_SCOPE;
  if ( 0x0 != _document )
    _document->currentChannel ( _index );
}
