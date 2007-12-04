
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DRT_SIM_DOCUMENT_TRANSSHOW_COMMAND_H__
#define __DRT_SIM_DOCUMENT_TRANSSHOW_COMMAND_H__

#include "Usul/Commands/Command.h"


class DrtTransShow : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( DrtTransShow );

  DrtTransShow ( Usul::Interfaces::IUnknown* caller );

protected:
  virtual ~DrtTransShow ();

  virtual void _execute ();

  virtual bool updateCheck () const;

};


#endif // __DRT_SIM_DOCUMENT_TRANSSHOW_COMMAND_H__
