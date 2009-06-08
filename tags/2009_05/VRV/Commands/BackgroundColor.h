
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_COMMANDS_BACKGROUND_COLOR_H__
#define __VRV_COMMANDS_BACKGROUND_COLOR_H__

#include "VRV/Export.h"

#include "Usul/Commands/Command.h"
#include "Usul/Interfaces/IUpdateCheck.h"
#include "Usul/Math/Vector4.h"

namespace VRV {
namespace Commands {

class VRV_EXPORT BackgroundColor : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;

  USUL_DECLARE_COMMAND ( BackgroundColor );

  BackgroundColor ( const std::string& name, const Usul::Math::Vec4f& color, Usul::Interfaces::IUnknown *caller = 0x0 );

protected:
  virtual ~BackgroundColor ();

  virtual void _execute ();

  virtual bool updateCheck () const;

private:
  Usul::Math::Vec4f _color;
};

}
}

#endif // __VRV_COMMANDS_BACKGROUND_COLOR_H__
