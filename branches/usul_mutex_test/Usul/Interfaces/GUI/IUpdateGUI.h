
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  Interface for updating the GUI.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_UPDATE_GUI_H_
#define _USUL_INTERFACE_UPDATE_GUI_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IUpdateGUI : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IUpdateGUI );

  /// Id for this interface.
  enum { IID = 1927013417u };

  virtual void              updateGUI() = 0;
  
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_UPDATE_GUI_H_

