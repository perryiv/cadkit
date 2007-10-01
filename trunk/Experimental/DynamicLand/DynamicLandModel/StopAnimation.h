
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DynamicLand_STOP_ANIMATION_COMMAND_H__
#define __DynamicLand_STOP_ANIMATION_COMMAND_H__

#include "Usul/Commands/Command.h"


class StopAnimation : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( StopAnimation );

  StopAnimation ( Usul::Interfaces::IUnknown* caller );

protected:
  virtual ~StopAnimation ();

  virtual void _execute ();
  virtual void _dummy ();


};


#endif // __DynamicLand_STOP_ANIMATION_COMMAND_H__
