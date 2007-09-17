
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_COMMANDS_SHADE_MODEL_H__
#define __VRV_COMMANDS_SHADE_MODEL_H__

#include "VRV/Export.h"

#include "Usul/Commands/Command.h"
#include "Usul/Interfaces/IShadeModel.h"
#include "Usul/Interfaces/IUpdateCheck.h"

namespace VRV {
namespace Commands {

class VRV_EXPORT ShadeModel : public Usul::Commands::Command,
                              public Usul::Interfaces::IUpdateCheck
{
public:
  typedef Usul::Commands::Command BaseClass;
  typedef Usul::Interfaces::IShadeModel            IShadeModel;
  typedef IShadeModel::Mode                        Mode;

  USUL_DECLARE_COMMAND ( ShadeModel );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  ShadeModel ( const std::string& name, Mode mode, Usul::Interfaces::IUnknown *caller = 0x0 );

protected:
  virtual ~ShadeModel ();

  virtual void _execute ();

  virtual bool updateCheck () const;

private:
  Mode _mode;
};

}
}

#endif // __VRV_COMMANDS_SHADE_MODEL_H__
