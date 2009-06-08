
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  Interface for executing a command.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_COMMAND_H_
#define _USUL_INTERFACE_COMMAND_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct ICommand : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICommand );

  // Id for this interface.
  enum { IID = 3572469257u };

  /// Execute the command.
  virtual void execute ( Usul::Interfaces::IUnknown::RefPtr caller ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_COMMAND_H_
