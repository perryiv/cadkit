
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ModelPresentation_NEXT_COMMAND_H__
#define __ModelPresentation_NEXT_COMMAND_H__

#include "Usul/Commands/Command.h"


class MpdNextCommand : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( MpdNextCommand );

  MpdNextCommand ( Usul::Interfaces::IUnknown* caller );

protected:
  virtual ~MpdNextCommand ();

  virtual void _execute ();


};


#endif // __ModelPresentation_NEXT_COMMAND_H__
