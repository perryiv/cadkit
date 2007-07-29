
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Wrapper class for a single gadget::DigitalInterface.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRJGA_BUTTON_DEVICE_H_
#define _VRJGA_BUTTON_DEVICE_H_

#include "VRV/Export.h"

#include "Usul/Base/Object.h"

#include "gadget/Type/DigitalInterface.h"

#include <string>


namespace VRV {
namespace Devices {

class VRV_EXPORT ButtonDevice : public Usul::Base::Object
{
public:

  // Useful typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef gadget::DigitalInterface DI;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ButtonDevice );

  // Constructor.
  ButtonDevice ( unsigned long mask, const std::string &name );

  // Get the bit-mask for this button.
  unsigned long         mask() const { return _mask; }

  // Notify listeners if state changed.
  void                  notify();

  // Get the device state.
  unsigned long         state() const;

protected:

  virtual ~ButtonDevice();

private:

  // Not copyable.
  ButtonDevice ( const ButtonDevice & );
  ButtonDevice& operator = ( const ButtonDevice & );

  DI _di;
  unsigned long _mask;
};

} // namespace Devices
} // namespace VRV


#endif // _VRJGA_BUTTON_DEVICE_H_
