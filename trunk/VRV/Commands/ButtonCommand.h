
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

#ifndef _VRV_COMMANDS_BASE_BUTTON_COMMAND_CLASS_H_
#define _VRV_COMMANDS_BASE_BUTTON_COMMAND_CLASS_H_

#include "VRV/Commands/Command.h"


namespace VRV {
namespace Commands {


class ButtonCommand : public Command
{
public:

  // Typedefs.
  typedef Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ButtonCommand );

protected:

  // Construction.
  ButtonCommand ( unsigned long buttonId );

  // Do not copy.
  ButtonCommand ( const ButtonCommand & );
  ButtonCommand &operator = ( const ButtonCommand & );

  // Use reference counting.
  virtual ~ButtonCommand();

  // Get the id representing the button combination.
  unsigned long       _getButtonId() const { return _buttonId; }

private:

  unsigned long _buttonId;
};


}; // namespace Commands
}; // namespace VRV


#endif // _VRV_COMMANDS_BASE_BUTTON_COMMAND_CLASS_H_
