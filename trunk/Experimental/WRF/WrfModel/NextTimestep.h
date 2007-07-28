
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __WRF_NEXT_TIMESTEP_COMMAND_H__
#define __WRF_NEXT_TIMESTEP_COMMAND_H__

#include "WRFDocument.h"

#include "Usul/Commands/Command.h"


class NextTimestep : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( NextTimestep );

  NextTimestep ( WRFDocument* doc );

protected:
  virtual ~NextTimestep ();

  virtual void _execute ();
private:
  WRFDocument::RefPtr _document;
};


#endif // __WRF_NEXT_TIMESTEP_COMMAND_H__
