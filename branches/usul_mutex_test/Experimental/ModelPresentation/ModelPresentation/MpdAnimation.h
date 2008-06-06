
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ModelPresentation_START_ANIMATION_COMMAND_H__
#define __ModelPresentation_START_ANIMATION_COMMAND_H__

#include "Usul/Commands/Command.h"


class MpdAnimation : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( MpdAnimation );

  MpdAnimation ( Usul::Interfaces::IUnknown* caller );

  

protected:
  virtual ~MpdAnimation ();

  virtual void _execute ();
  
  virtual bool updateCheck () const;



};


#endif // __ModelPresentation_START_ANIMATION_COMMAND_H__
