
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MODELPRESENTATION_NEXT_SEQUENCE_H__
#define __MODELPRESENTATION_NEXT_SEQUENCE_H__

#include "Usul/Commands/Command.h"


class MpdNextSequence : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( MpdNextSequence );

  MpdNextSequence ( Usul::Interfaces::IUnknown* caller );

protected:
  virtual ~MpdNextSequence ();

  virtual void _execute ();


};


#endif // __MODELPRESENTATION_NEXT_SEQUENCE_H__
