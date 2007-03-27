
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

#include "vrjGA/ButtonDevice.h"

#include <vector>


namespace vrjGA {


class VRJ_GA_EXPORT ButtonGroup : public Referenced
{
public:

  // Useful typedefs.
  typedef Referenced BaseClass;

  /// Smart-pointer definitions.
  VRJGA_DECLARE_POINTER ( ButtonGroup );

  // Constructor.
  ButtonGroup();

  // Add a button device.
  void            add ( ButtonDevice * );

  // Return bitmask of buttons that are down.
  unsigned long   down() const { return _down; }

  // Return bitmask of buttons just pressed.
  unsigned long   pressed() const { return _pressed; }

  // Return bitmask of buttons just released.
  unsigned long   released() const { return _released; }

  // Update the internal state.
  void            update();

protected:

  virtual ~ButtonGroup();

private:

  // Not copyable.
  ButtonGroup ( const ButtonGroup & );
  ButtonGroup& operator = ( const ButtonGroup & );

  typedef std::vector<ButtonDevice::Ptr> Buttons;
  Buttons _buttons;
  unsigned long _down;
  unsigned long _pressed;
  unsigned long _released;
};


}; // namespace vrjGA


#endif // _VRJGA_BUTTON_DEVICES_GROUP_H_
