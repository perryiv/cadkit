
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ModelPresentation_MENU_COMMAND_H__
#define __ModelPresentation_MENU_COMMAND_H__

#include "Usul/Commands/Command.h"


class MpdMenuCommand : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( MpdMenuCommand );

  MpdMenuCommand ( Usul::Interfaces::IUnknown * caller, const std::string text, unsigned int set, unsigned int group );

protected:
  virtual ~MpdMenuCommand ();

  virtual void _execute ();

  virtual bool updateCheck () const;

private:
  std::string             _text;
  unsigned int            _set;
  unsigned int            _group;


};


#endif // __ModelPresentation_MENU_COMMAND_H__
