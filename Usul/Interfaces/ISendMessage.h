
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_SEND_MESSAGE_H__
#define __USUL_INTERFACES_SEND_MESSAGE_H__

#include "Usul/Interfaces/IUnknown.h"

namespace FX { class FXWindow; }

namespace Usul {
namespace Interfaces {

struct ISendMessage : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ISendMessage );

  /// Possible messages.
  enum
  {
    ID_FIRST = 1000,
    ID_UPDATE_TITLES,
    ID_CLEAR_SCENE,
    ID_BUILD_SCENE,
    ID_RENDER_SCENE,
    ID_CLOSE,
    ID_DISPLAY_LISTS_UPDATE,
    ID_LAST
  };

  /// Id for this interface.
  enum { IID = 3448455701u };

  virtual void sendMessage ( unsigned short message, const Usul::Interfaces::IUnknown *skip = 0x0 ) = 0;
};

}
}

#endif // __USUL_INTERFACES_SEND_MESSAGE_H__

