
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ModelPresentation_TOOLS_COMMAND_H__
#define __ModelPresentation_TOOLS_COMMAND_H__

#include "Usul/Commands/Command.h"


class MpdTools : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( MpdTools );

  MpdTools ( Usul::Interfaces::IUnknown * caller, const std::string& text );

protected:
  virtual ~MpdTools ();

  virtual void _execute ();

private:
  std::string             _text;


};


#endif // __ModelPresentation_TOOLS_COMMAND_H__
