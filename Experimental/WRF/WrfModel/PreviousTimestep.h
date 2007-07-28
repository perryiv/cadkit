
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __WRF_PREVIOUS_TIMESTEP_H__
#define __WRF_PREVIOUS_TIMESTEP_H_

#include "WRFDocument.h"

#include "Usul/Commands/Command.h"


class PreviousTimestep : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( PreviousTimestep );

  PreviousTimestep ( WRFDocument* doc );

protected:
  virtual ~PreviousTimestep ();

  virtual void _execute ();
private:
  WRFDocument::RefPtr _document;
};


#endif // __WRF_PREVIOUS_TIMESTEP_H_
