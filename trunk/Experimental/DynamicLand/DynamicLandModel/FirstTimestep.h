
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DynamicLand_FIRST_TIMESTEP_COMMAND_H__
#define __DynamicLand_FIRST_TIMESTEP_COMMAND_H__

#include "Usul/Commands/Command.h"


class FirstTimestep : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( FirstTimestep );

  FirstTimestep ( Usul::Interfaces::IUnknown* caller );

protected:
  virtual ~FirstTimestep ();

  virtual void _execute ();
  virtual void _dummy ();


};


#endif // __DynamicLand_FIRST_TIMESTEP_COMMAND_H__
