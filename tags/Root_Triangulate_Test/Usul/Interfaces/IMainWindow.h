
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting the main window
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_MAIN_WINDOW_H_
#define _USUL_INTERFACE_MAIN_WINDOW_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IMainWindow : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IMainWindow );

  /// Id for this interface.
  enum { IID = 1232733632u };

  virtual Usul::Interfaces::IUnknown* getMainWindow() = 0;
};


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_MAIN_WINDOW_H_
