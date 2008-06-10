
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MODEPRESENTATION_FASTER_ANIMATION_SPEED_COMMAND_H__
#define __MODEPRESENTATION_FASTER_ANIMATION_SPEED_COMMAND_H__

#include "Usul/Commands/Command.h"


class MpdFasterAnimationSpeed : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( MpdFasterAnimationSpeed );

  MpdFasterAnimationSpeed ( Usul::Interfaces::IUnknown* caller, unsigned int multiplier, const std::string &text );

protected:
  virtual ~MpdFasterAnimationSpeed ();

  virtual void _execute ();

private:
  unsigned int _multiplier;


};


#endif // __MODEPRESENTATION_FASTER_ANIMATION_SPEED_COMMAND_H__
