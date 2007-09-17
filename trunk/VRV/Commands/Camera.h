
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_COMMANDS_CAMERA_H__
#define __VRV_COMMANDS_CAMERA_H__

#include "VRV/Export.h"

#include "Usul/Commands/Command.h"
#include "Usul/Interfaces/ICamera.h"

namespace VRV {
namespace Commands {

class VRV_EXPORT Camera : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;
  typedef Usul::Interfaces::ICamera            ICamera;
  typedef ICamera::CameraOption                CameraOption;

  USUL_DECLARE_COMMAND ( Camera );

  Camera ( const std::string& name, CameraOption option, Usul::Interfaces::IUnknown *caller = 0x0 );

protected:
  virtual ~Camera ();

  virtual void _execute ();

private:
  CameraOption _option;
};

}
}

#endif // __VRV_COMMANDS_CAMERA_H__
