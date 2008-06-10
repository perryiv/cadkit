
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ModelPresentation_STOP_ANIMATION_COMMAND_H__
#define __ModelPresentation_STOP_ANIMATION_COMMAND_H__

#include "Usul/Commands/Command.h"


class MpdStopAnimation : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( MpdStopAnimation );

  MpdStopAnimation ( Usul::Interfaces::IUnknown* caller );

protected:
  virtual ~MpdStopAnimation ();

  virtual void _execute ();


};


#endif // __ModelPresentation_STOP_ANIMATION_COMMAND_H__
