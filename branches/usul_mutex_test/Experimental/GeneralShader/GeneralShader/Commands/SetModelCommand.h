
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __GENERAL_SHADER_SET_MODEL_COMMAND_H__
#define __GENERAL_SHADER_SET_MODEL_COMMAND_H__

#include "Usul/Commands/Command.h"


class SetModelCommand : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( SetModelCommand );

  SetModelCommand ( Usul::Interfaces::IUnknown * caller, const std::string name );

protected:
  virtual ~SetModelCommand ();

  virtual void _execute ();

  virtual bool updateCheck () const;

private:
  std::string             _name;


};


#endif // __GENERAL_SHADER_SET_MODEL_COMMAND_H__
