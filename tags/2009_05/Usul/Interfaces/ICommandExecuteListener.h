
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ICOMMAND_EXECUTE_LISTENER_H__
#define __USUL_INTERFACES_ICOMMAND_EXECUTE_LISTENER_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul { namespace Commands { class Command; } }

namespace Usul {
namespace Interfaces {


struct ICommandExecuteListener : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICommandExecuteListener );

  /// Id for this interface.
  enum { IID = 1999857670u };

  virtual void                commandExecuteNotify ( Usul::Commands::Command* command ) = 0;

}; // struct ICommandExecuteListener


} // End namespace Interfaces
} // End namespace Usul

#endif // __USUL_INTERFACES_ICOMMAND_EXECUTE_LISTENER_H__
