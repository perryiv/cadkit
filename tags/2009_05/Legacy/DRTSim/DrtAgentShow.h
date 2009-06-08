
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DRT_SIM_DOCUMENT_AGENTSHOW_COMMAND_H__
#define __DRT_SIM_DOCUMENT_AGENTSHOW_COMMAND_H__

#include "Usul/Commands/Command.h"


class DrtAgentShow : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( DrtAgentShow );

  DrtAgentShow ( Usul::Interfaces::IUnknown* caller );

protected:
  virtual ~DrtAgentShow ();

  virtual void _execute ();

  virtual bool updateCheck () const;

};


#endif // __DRT_SIM_DOCUMENT_AGENTSHOW_COMMAND_H__
