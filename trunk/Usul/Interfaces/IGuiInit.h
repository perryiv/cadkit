
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for initializing the gui.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_GUI_INIT_H_
#define _USUL_INTERFACE_GUI_INIT_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IGuiInit : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IGuiInit );

  // Id for this interface.
  enum { IID = 1175417607u };

  // Call to have component implement its gui.
  virtual void        guiInit ( IUnknown *caller ) = 0;
};


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_GUI_INIT_H_
