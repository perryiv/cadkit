
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DynamicLand_START_ANIMATION_COMMAND_H__
#define __DynamicLand_START_ANIMATION_COMMAND_H__

#include "Usul/Commands/Command.h"


class StartAnimation : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( StartAnimation );

  StartAnimation ( Usul::Interfaces::IUnknown* caller );

protected:
  virtual ~StartAnimation ();

  virtual void _execute ();
  virtual void _dummy ();


};


#endif // __DynamicLand_START_ANIMATION_COMMAND_H__
