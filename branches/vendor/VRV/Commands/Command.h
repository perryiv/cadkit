
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all commands.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRV_COMMANDS_BASE_COMMAND_CLASS_H_
#define _VRV_COMMANDS_BASE_COMMAND_CLASS_H_

#include "VRV/Commands/CompileGuard.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/ICommand.h"


namespace VRV {
namespace Commands {


class Command : public Usul::Base::Referenced,
                public Usul::Interfaces::ICommand
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Command );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

protected:

  // Construction.
  Command();

  // Do not copy.
  Command ( const Command & );
  Command &operator = ( const Command & );

  // Use reference counting.
  virtual ~Command();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::ICommand
  //
  /////////////////////////////////////////////////////////////////////////////

  // Execute the command.
  virtual void            execute();

  /////////////////////////////////////////////////////////////////////////////

  // Execute the command.
  virtual void            _execute() = 0;
};


}; // namespace Commands
}; // namespace VRV


#endif // _VRV_COMMANDS_BASE_COMMAND_CLASS_H_
