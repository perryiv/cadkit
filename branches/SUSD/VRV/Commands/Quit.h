
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The quit-command.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRV_COMMANDS_QUIT_COMMAND_H_
#define _VRV_COMMANDS_QUIT_COMMAND_H_

#include "VRV/Commands/ButtonDown.h"


namespace VRV {
namespace Commands {


class Quit : public ButtonDown
{
public:

  // Typedefs.
  typedef ButtonDown BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Quit );

  // Construction.
  Quit();

protected:

  // Do not copy.
  Quit ( const Quit & );
  Quit &operator = ( const Quit & );

  // Use reference counting.
  virtual ~Quit();

  // Execute the command.
  virtual void            _execute();
};


}; // namespace Commands
}; // namespace VRV


#endif // _VRV_COMMANDS_QUIT_COMMAND_H_
