
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MODELPRESENTATION_FIRST_SEQUENCE_COMMAND_H__
#define __MODELPRESENTATION_FIRST_SEQUENCE_COMMAND_H__

#include "Usul/Commands/Command.h"


class MpdFirstSequence : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( MpdFirstSequence );

  MpdFirstSequence ( Usul::Interfaces::IUnknown* caller );

protected:
  virtual ~MpdFirstSequence ();

  virtual void _execute ();


};


#endif // __MODELPRESENTATION_FIRST_SEQUENCE_COMMAND_H__
