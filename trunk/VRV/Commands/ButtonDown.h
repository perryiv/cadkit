
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all button-commands.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRV_COMMANDS_BASE_BUTTON_DOWN_COMMAND_CLASS_H_
#define _VRV_COMMANDS_BASE_BUTTON_DOWN_COMMAND_CLASS_H_

#include "VRV/Commands/ButtonCommand.h"

#include "VRV/Interfaces/IButtonDown.h"


namespace VRV {
namespace Commands {


class ButtonDown : public ButtonCommand,
                   public VRV::Interfaces::IButtonDown
{
public:

  // Typedefs.
  typedef ButtonCommand BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ButtonDown );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

protected:

  // Construction.
  ButtonDown ( unsigned long buttonId );

  // Do not copy.
  ButtonDown ( const ButtonDown & );
  ButtonDown &operator = ( const ButtonDown & );

  // Use reference counting.
  virtual ~ButtonDown();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  VRV::Interfaces::IButtonDown
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the id representing the button combination.
  virtual unsigned long   getButtonId() const;
};


}; // namespace Commands
}; // namespace VRV


#endif // _VRV_COMMANDS_BASE_BUTTON_DOWN_COMMAND_CLASS_H_
