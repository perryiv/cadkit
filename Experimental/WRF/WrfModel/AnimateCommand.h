
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __WRF_ANIMATE_COMMAND_H__
#define __WRF_ANIMATE_COMMAND_H__

#include "Usul/Commands/Command.h"


class AnimateCommand : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( AnimateCommand );

   AnimateCommand ( bool animate, Usul::Interfaces::IUnknown * caller );

protected:
  virtual ~AnimateCommand ();

  virtual void _execute ();
private:
  bool _animate;
  Usul::Interfaces::IUnknown::QueryPtr _document;
};


#endif // __WRF_ANIMATE_COMMAND_H__
