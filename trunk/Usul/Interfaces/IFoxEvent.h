
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_FOX_EVENT_H__
#define __USUL_INTERFACES_FOX_EVENT_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul
{
  namespace Interfaces
  {
    class IFoxEvent : public Usul::Interfaces::IUnknown
    {
    public:
      typedef Usul::Interfaces::IUnknown Unknown;

      /// Smart-pointer definitions.
      USUL_DECLARE_QUERY_POINTERS ( IFoxEvent );

      enum { IID = 1100798022 };

      virtual void execute( Unknown* caller, const FX::FXEvent &event) = 0;

    }; //class IFoxEvent
  }; //namespace Interfaces
}; //namespace Usul

#endif // __USUL_INTERFACES_FOX_EVENT_H__