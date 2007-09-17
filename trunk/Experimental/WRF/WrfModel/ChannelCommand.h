
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __WRF_CHANNEL_COMMAND_H__
#define __WRF_CHANNEL_COMMAND_H__

#include "Usul/Commands/Command.h"

class WRFDocument;

class ChannelCommand : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( ChannelCommand );

  ChannelCommand ( const std::string& name, unsigned int index, WRFDocument* doc );

protected:
  virtual ~ChannelCommand ();

  virtual void _execute ();
private:
  std::string _name;
  unsigned int _index;
  WRFDocument* _document;
};


#endif // __WRF_CHANNEL_COMMAND_H__
