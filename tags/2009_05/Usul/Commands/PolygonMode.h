
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_COMMANDS_POLYGON_MODE_H__
#define __USUL_COMMANDS_POLYGON_MODE_H__

#include "Usul/Export/Export.h"

#include "Usul/Commands/Command.h"
#include "Usul/Interfaces/IPolygonMode.h"
#include "Usul/Interfaces/IUpdateCheck.h"

namespace Usul {
namespace Commands {

class USUL_EXPORT PolygonMode : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;
  typedef Usul::Interfaces::IPolygonMode            IPolygonMode;
  typedef IPolygonMode::Mode                        Mode;

  USUL_DECLARE_COMMAND ( PolygonMode );

  PolygonMode ( const std::string& name, Mode mode, Usul::Interfaces::IUnknown *caller = 0x0 );

protected:
  virtual ~PolygonMode();

  virtual void _execute();

  virtual bool updateCheck() const;

private:
  Mode _mode;
};

}
}

#endif // __USUL_COMMANDS_POLYGON_MODE_H__
