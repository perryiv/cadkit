
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Wrapper class for a multiple button device.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRJGA_BUTTON_DEVICES_GROUP_H_
#define _VRJGA_BUTTON_DEVICES_GROUP_H_

#include "VRV/Export.h"
#include "VRV/Devices/ButtonDevice.h"

#include "Usul/Base/Object.h"

#include <vector>

namespace VRV {
namespace Devices {

class VRV_EXPORT ButtonGroup : public Usul::Base::Object
{
public:
  
  // Useful typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef std::vector< ButtonDevice::RefPtr > Buttons;
  typedef Buttons::value_type value_type;
  typedef Buttons::iterator iterator;
  typedef unsigned long MaskType;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ButtonGroup );

  // Constructor.
  ButtonGroup();

  // Add a button device.
  void            add ( ButtonDevice * );

  iterator        begin () { return _buttons.begin (); }
  iterator        end ()   { return _buttons.end(); }

  // clear out the button group
  void            clear();

  // Return bitmask of buttons that are down.
  MaskType        down() const { return _down; }

  // Notify listeners if state changed.
  void            notify();

  // Return bitmask of buttons just pressed.
  MaskType        pressed() const { return _pressed; }

  // Return bitmask of buttons just released.
  MaskType        released() const { return _released; }

  // return the number of buttons
  unsigned int    size();

protected:

  virtual ~ButtonGroup();

  void            _update();

private:

  // Not copyable.
  ButtonGroup ( const ButtonGroup & );
  ButtonGroup& operator = ( const ButtonGroup & );

  Buttons _buttons;
  unsigned long _down;
  unsigned long _pressed;
  unsigned long _released;
};

} // namespace Devices
} // namespace VRV


#endif // _VRJGA_BUTTON_DEVICES_GROUP_H_
