
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for a list of commands.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VIRTUAL_REALITY_VIEWER_INTERFACE_COMMAND_LIST_H_
#define _VIRTUAL_REALITY_VIEWER_INTERFACE_COMMAND_LIST_H_

#include "Usul/Interfaces/ICommand.h"


namespace VRV {
namespace Interfaces {


struct ICommandList : public Usul::Interfaces::IUnknown
{
  typedef Usul::Interfaces::ICommand ICommand;

  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICommandList );

  // Id for this interface.
  enum { IID = 3876275584u };

  // Get the number of commands.
  virtual unsigned long   getNumCommands() const = 0;

  // Get the command.
  virtual ICommand *      getCommand ( unsigned long which ) = 0;
};


}; // namespace Interfaces
}; // namespace VRV


#endif // _VIRTUAL_REALITY_VIEWER_INTERFACE_COMMAND_LIST_H_
