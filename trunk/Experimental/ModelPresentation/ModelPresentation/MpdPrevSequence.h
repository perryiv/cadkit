
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MODELPRESENTATION_PREV_SEQUENCE_COMMAND_H__
#define __MODELPRESENTATION_PREV_SEQUENCE_COMMAND_H__

#include "Usul/Commands/Command.h"


class MpdPrevSequence : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( MpdPrevSequence );

  MpdPrevSequence ( Usul::Interfaces::IUnknown* caller );

protected:
  virtual ~MpdPrevSequence ();

  virtual void _execute ();


};


#endif // __MODELPRESENTATION_PREV_SEQUENCE_COMMAND_H__
