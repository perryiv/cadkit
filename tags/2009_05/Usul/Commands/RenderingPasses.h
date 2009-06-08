
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_COMMANDS_RENDERING_PASSES_H__
#define __USUL_COMMANDS_RENDERING_PASSES_H__

#include "Usul/Export/Export.h"

#include "Usul/Commands/Command.h"
#include "Usul/Interfaces/IUpdateCheck.h"

namespace Usul {
namespace Commands {

class USUL_EXPORT RenderingPasses : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;

  USUL_DECLARE_COMMAND ( RenderingPasses );

  RenderingPasses ( const std::string& name, unsigned int passes, Usul::Interfaces::IUnknown *caller = 0x0 );

protected:
  virtual ~RenderingPasses();

  virtual void _execute();

  virtual bool updateEnable() const;
  virtual bool updateCheck() const;

private:
  unsigned int _passes;
};

}
}

#endif // __USUL_COMMANDS_RENDERING_PASSES_H__
