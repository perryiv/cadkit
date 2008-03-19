
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __GENERAL_SHADER_READ_SETTINGS_COMMAND_H__
#define __GENERAL_SHADER_READ_SETTINGS_COMMAND_H__

#include "Usul/Commands/Command.h"


class ReadSettingsCommand : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( ReadSettingsCommand );

  ReadSettingsCommand ( Usul::Interfaces::IUnknown * caller, const std::string &name, const std::string &workingDir );

protected:
  virtual ~ReadSettingsCommand ();

  virtual void _execute ();

private:
  std::string       _workingDir;


};


#endif // __GENERAL_SHADER_READ_SETTINGS_COMMAND_H__
