
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_FOX_MESSAGE_HANDLER_H__
#define __USUL_INTERFACES_FOX_MESSAGE_HANDLER_H__

#include "Usul/Interfaces/IUnknown.h"

namespace FX 
{ 
class FXObject; 
typedef unsigned int FXuint;
typedef FXuint FXSelector;
};

namespace Usul
{
  namespace Interfaces
  {
    class IFoxMessageHandler : public Usul::Interfaces::IUnknown
    {
    public:
      typedef Usul::Interfaces::IUnknown Unknown;

      /// Smart-pointer definitions.
      USUL_DECLARE_QUERY_POINTERS ( IFoxMessageHandler );

      enum { IID = 1101837237u };

      virtual long handle( Unknown* caller, FX::FXObject *, FX::FXSelector, void * ) = 0;

    }; //class IFoxMessageHandler
  }; //namespace Interfaces
}; //namespace Usul

#endif // __USUL_INTERFACES_FOX_MESSAGE_HANDLER_H__

