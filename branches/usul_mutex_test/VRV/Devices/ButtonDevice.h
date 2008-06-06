
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
#include "Usul/Interfaces/IButtonPressSubject.h"
#include "Usul/Interfaces/IButtonPressListener.h"
#include "Usul/Interfaces/IButtonReleaseSubject.h"
#include "Usul/Interfaces/IButtonReleaseListener.h"
#include "Usul/Interfaces/IButtonID.h"

#include "gadget/Type/DigitalInterface.h"

#include <string>
#include <vector>


namespace VRV {
namespace Devices {

class VRV_EXPORT ButtonDevice : public Usul::Base::Object,
                                public Usul::Interfaces::IButtonPressSubject,
                                public Usul::Interfaces::IButtonReleaseSubject,
                                public Usul::Interfaces::IButtonID
{
public:

  // Useful typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef gadget::DigitalInterface DI;
  typedef Usul::Interfaces::IButtonPressListener IButtonPressListener;
  typedef Usul::Interfaces::IButtonReleaseListener IButtonReleaseListener;
  typedef std::vector<IButtonPressListener::RefPtr> ButtonPressListeners;
  typedef std::vector<IButtonReleaseListener::RefPtr> ButtonReleaseListeners;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ButtonDevice );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Constructor.
  ButtonDevice ( unsigned long mask, const std::string &name );
  ButtonDevice ( unsigned long mask, const std::string &vrj_name, const std::string &name );

  // Add the listener.
  virtual void          addButtonPressListener ( Usul::Interfaces::IUnknown * );
  virtual void          addButtonReleaseListener ( Usul::Interfaces::IUnknown * );

  // Remove all listeners.
  virtual void          clearButtonPressListeners();
  virtual void          clearButtonReleaseListeners();

  // Get the bit-mask for this button.
  unsigned long         mask() const;

  // Notify listeners if state changed.
  void                  notify();

  // Remove the listener.
  virtual void          removeButtonPressListener ( Usul::Interfaces::IUnknown * );
  virtual void          removeButtonReleaseListener ( Usul::Interfaces::IUnknown * );

  ///  Get the device state.
  unsigned long         state() const;

  /// Get the button ID.
  unsigned long         buttonID () const;

  /// Get the button name
  std::string           getButtonName() const;

protected:

  virtual ~ButtonDevice();

  void                  _notifyPressed();
  void                  _notifyReleased();

private:

  // Not copyable.
  ButtonDevice ( const ButtonDevice & );
  ButtonDevice& operator = ( const ButtonDevice & );

  DI _di;
  unsigned long _mask;
  ButtonPressListeners _pressed;
  ButtonReleaseListeners _released;
  std::string _buttonName;
};

} // namespace Devices
} // namespace VRV


#endif // _VRJGA_BUTTON_DEVICE_H_
