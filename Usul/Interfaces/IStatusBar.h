
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for updating the status bar
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_STATUS_BAR_H_
#define _USUL_INTERFACE_STATUS_BAR_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>

namespace Usul {
namespace Interfaces {


struct IStatusBar : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IStatusBar );

  // Id for this interface.
  enum { IID = 1100880943u };

  //get the status bar text
  virtual const std::string& getStatusBarText () = 0;
  //set the status bar text
  virtual void setStatusBarText ( const std::string& ) = 0;

}; // class IStatusBar

}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_STATUS_BAR_H_
