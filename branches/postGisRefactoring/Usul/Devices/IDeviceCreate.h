
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for creating a new device
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_DEVICES_DEVICE_CREATE_H_
#define _USUL_DEVICES_DEVICE_CREATE_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Devices {

struct IDevice;

struct IDeviceCreate : public Usul::Interfaces::IUnknown
{

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDeviceCreate );

  /// Id for this interface.
  enum { IID = 3155481028u };

  /// Initialize a new document
  virtual IDevice*          createDevice ( Usul::Interfaces::IUnknown *caller = 0x0 ) = 0;

};


} // namespace Devices
} // namespace Usul


#endif // _USUL_DEVICES_DEVICE_CREATE_H_
