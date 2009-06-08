
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_COMMANDS_RENDER_LOOP_H__
#define __USUL_COMMANDS_RENDER_LOOP_H__

#include "Usul/Export/Export.h"

#include "Usul/Commands/Command.h"
#include "Usul/Interfaces/IUpdateCheck.h"

namespace Usul {
namespace Commands {

class USUL_EXPORT RenderLoop : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;

  USUL_DECLARE_COMMAND ( RenderLoop );

  RenderLoop ( const std::string& name, Usul::Interfaces::IUnknown *caller = 0x0 );

protected:
  virtual ~RenderLoop();

  virtual void _execute();

  virtual bool updateEnable() const;
  virtual bool updateCheck() const;

};

}
}

#endif // __USUL_COMMANDS_RENDER_LOOP_H__
