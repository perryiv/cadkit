
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_COMMANDS_RENDERING_PASSES_H__
#define __VRV_COMMANDS_RENDERING_PASSES_H__

#include "VRV/Export.h"

#include "Usul/Commands/Command.h"
#include "Usul/Interfaces/IUpdateCheck.h"

namespace VRV {
namespace Commands {

class VRV_EXPORT RenderingPasses : public Usul::Commands::Command,
                                   public Usul::Interfaces::IUpdateCheck
{
public:
  typedef Usul::Commands::Command BaseClass;

  USUL_DECLARE_COMMAND ( RenderingPasses );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  RenderingPasses ( const std::string& name, unsigned int passes, Usul::Interfaces::IUnknown *caller = 0x0 );

protected:
  virtual ~RenderingPasses ();

  virtual void _execute ();

  virtual bool updateCheck () const;

private:
  unsigned int _passes;
};

}
}

#endif // __VRV_COMMANDS_RENDERING_PASSES_H__
