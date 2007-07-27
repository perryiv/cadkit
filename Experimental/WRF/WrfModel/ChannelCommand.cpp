
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

ChannelCommand::ChannelCommand ( const std::string& name, unsigned int index, WRFDocument* doc ) :
  BaseClass ( 0x0 ),
  _name ( name ),
  _index ( index ),
  _document ( doc )
{
  this->text ( "Switch to " + name );
}

ChannelCommand::~ChannelCommand ()
{
}

void ChannelCommand::_execute ()
{
}
