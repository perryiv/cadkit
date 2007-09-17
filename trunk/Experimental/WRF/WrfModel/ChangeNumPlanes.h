
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __WRF_CHANGE_NUM_PLANES_COMMAND_H__
#define __WRF_CHANGE_NUM_PLANES_COMMAND_H__

#include "Usul/Commands/Command.h"

class WRFDocument;

class ChangeNumPlanes : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( ChangeNumPlanes );

  ChangeNumPlanes ( double multiplier, WRFDocument* doc );

protected:
  virtual ~ChangeNumPlanes ();

  virtual void _execute ();
private:
  double _multiplier;
  WRFDocument* _document;
};


#endif // __WRF_CHANGE_NUM_PLANES_COMMAND_H__
