
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  Interface for seeing if the command can be redone.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_CAN_REDO_COMMAND_H_
#define _USUL_INTERFACE_CAN_REDO_COMMAND_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct ICanRedo : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICanRedo );

  // Id for this interface.
  enum { IID = 2453894265u };

  // See if the command can be redone.
  virtual bool canRedo() const = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_CAN_REDO_COMMAND_H_
