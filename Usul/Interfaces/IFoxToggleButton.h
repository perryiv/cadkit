
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

namespace Usul
{
  namespace Interfaces
  {
    class IFoxToggleButton : public Usul::Interfaces::IUnknown
    {
    public:
      typedef Usul::Interfaces::IUnknown Unknown;

      /// Smart-pointer definitions.
      USUL_DECLARE_QUERY_POINTERS ( IFoxRadioButton);

      enum { IID = 1101838475u };


    }; //class IFoxToggleButton
  }; //namespace Interfaces
}; //namespace Usul

#endif // __USUL_INTERFACES_FOX_TOGGLE_BUTTON_H__

