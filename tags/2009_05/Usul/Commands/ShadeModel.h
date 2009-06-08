
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_COMMANDS_SHADE_MODEL_H__
#define __USUL_COMMANDS_SHADE_MODEL_H__

#include "Usul/Export/Export.h"

#include "Usul/Commands/Command.h"
#include "Usul/Interfaces/IShadeModel.h"
#include "Usul/Interfaces/IUpdateCheck.h"

namespace Usul {
namespace Commands {

class USUL_EXPORT ShadeModel : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command            BaseClass;
  typedef Usul::Interfaces::IShadeModel      IShadeModel;
  typedef IShadeModel::Mode                  Mode;

  USUL_DECLARE_COMMAND ( ShadeModel );

  ShadeModel ( const std::string& name, Mode mode, Usul::Interfaces::IUnknown *caller = 0x0 );

protected:
  virtual ~ShadeModel();

  virtual void _execute();

  virtual bool updateCheck() const;

private:
  Mode _mode;
};

}
}

#endif // __USUL_COMMANDS_SHADE_MODEL_H__
