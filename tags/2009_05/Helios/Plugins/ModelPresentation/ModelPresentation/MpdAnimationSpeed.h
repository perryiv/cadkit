
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MODEPRESENTATION_ANIMATION_SPEED_COMMAND_H__
#define __MODEPRESENTATION_ANIMATION_SPEED_COMMAND_H__

#include "Usul/Commands/Command.h"


class MpdAnimationSpeed : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( MpdAnimationSpeed );

  MpdAnimationSpeed ( Usul::Interfaces::IUnknown* caller, unsigned int speed, const std::string &text );

protected:
  virtual ~MpdAnimationSpeed ();

  virtual void _execute ();
  virtual bool updateCheck () const;

private:
  unsigned int _speed;


};


#endif // __MODEPRESENTATION_ANIMATION_SPEED_COMMAND_H__
