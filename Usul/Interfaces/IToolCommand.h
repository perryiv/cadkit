
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for excuting a command with FOX event
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACE_TOOL_COMMAND__
#define __USUL_INTERFACE_TOOL_COMMAND__

namespace FX { struct FXEvent; }

#include "Usul/Interfaces/IUnknown.h"

namespace Usul
{
  namespace Interfaces
  {
    class IToolCommand : public Usul::Interfaces::IUnknown
    {
    public:
      typedef Usul::Interfaces::IUnknown Unknown;

      // Smart-pointer definitions.
      USUL_DECLARE_QUERY_POINTERS ( IToolCommand );

      enum { IID = 1100623970u };

      virtual bool execute( Unknown* caller, const FX::FXEvent &event) = 0;

    };//class IToolCommand
  }; //namespace Interfaces
};//namespace Usul

#endif //__USUL_INTERFACE_TOOL_COMMAND__