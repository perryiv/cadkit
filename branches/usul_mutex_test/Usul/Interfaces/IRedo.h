
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  Interface for redoing a command.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_REDO_COMMAND_H_
#define _USUL_INTERFACE_REDO_COMMAND_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IRedo : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IRedo );

  // Id for this interface.
  enum { IID = 2197577025u };

  /// Redo the command.
  virtual void redo() = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_REDO_COMMAND_H_
