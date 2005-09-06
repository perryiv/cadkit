
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_FOX_TOGGLE_BUTTON_H__
#define __USUL_INTERFACES_FOX_TOGGLE_BUTTON_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {
    
struct IFoxToggleButton : public Usul::Interfaces::IUnknown
{
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IFoxToggleButton);

  enum { IID = 1101838475u };


}; // IFoxToggleButton
  
}; //namespace Interfaces
}; //namespace Usul

#endif // __USUL_INTERFACES_FOX_TOGGLE_BUTTON_H__

