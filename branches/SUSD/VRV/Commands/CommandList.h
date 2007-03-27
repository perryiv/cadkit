
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Command list.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRV_COMMANDS_COMMAND_LIST_CLASS_H_
#define _VRV_COMMANDS_COMMAND_LIST_CLASS_H_

#include "VRV/Commands/CompileGuard.h"

#include "VRV/Interfaces/ICommandList.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/ICommand.h"

#include <vector>


namespace VRV {
namespace Commands {


class CommandList : public Usul::Base::Referenced,
                    public VRV::Interfaces::ICommandList
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::ICommand ICommand;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( CommandList );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Construction.
  CommandList();

protected:

  // Do not copy.
  CommandList ( const CommandList & );
  CommandList &operator = ( const CommandList & );

  // Use reference counting.
  virtual ~CommandList();

  /////////////////////////////////////////////////////////////////////////////
  //
  //  VRV::Interfaces::ICommandList
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the number of commands.
  virtual unsigned long   getNumCommands() const;

  // Get the command.
  virtual ICommand *      getCommand ( unsigned long which );

private:

  std::vector<ICommand::ValidRefPtr> _commands;
};


}; // namespace Commands
}; // namespace VRV


#endif // _VRV_COMMANDS_COMMAND_LIST_CLASS_H_
