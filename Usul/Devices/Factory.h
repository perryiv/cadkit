
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DEVICES_FACTORY_H__
#define __DEVICES_FACTORY_H__

#include "Usul/Export/Export.h"

#include "IDevice.h"

#include <vector>

namespace Usul {
namespace Devices {

class USUL_EXPORT Factory
{
public:

  typedef std::vector< IDevice::ValidRefPtr > Devices;

  static Factory& instance();

  Devices devices( Usul::Interfaces::IUnknown *caller = 0x0 );

private:
  Factory();
  ~Factory();

  static Factory* _instance;
};

}
}

#endif //__DEVICES_FACTORY_H__
