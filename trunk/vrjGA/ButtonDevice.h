
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

#include "vrjGA/vrjGA.h"
#include "vrjGA/Referenced.h"

#include "gadget/Type/DigitalInterface.h"

#include <string>


namespace vrjGA {


class VRJ_GA_EXPORT ButtonDevice : public Referenced
{
public:

  // Useful typedefs.
  typedef Referenced BaseClass;
  typedef gadget::DigitalInterface DI;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ButtonDevice );

  // Constructor.
  ButtonDevice ( unsigned long mask, const std::string &name );

  // Get the bit-mask for this button.
  unsigned long         mask() const { return _mask; }

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


}; // namespace vrjGA


#endif // _VRJGA_BUTTON_DEVICE_H_
