
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  Interface for undoing a command.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_UNDO_COMMAND_H_
#define _USUL_INTERFACE_UNDO_COMMAND_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IUndo : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IUndo );

  // Id for this interface.
  enum { IID = 3942385950u };

  /// Undo the command.
  virtual void undo() = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_UNDO_COMMAND_H_
