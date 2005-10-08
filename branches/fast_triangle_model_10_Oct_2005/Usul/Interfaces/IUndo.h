
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for undoing something.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_UNDO_H_
#define _USUL_INTERFACE_UNDO_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IUndo : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IUndo );

  // Id for this interface.
  enum { IID = 1032702528 };

  // Undo whatever it was that was done.
  virtual void undo();
};


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_UNDO_H_
