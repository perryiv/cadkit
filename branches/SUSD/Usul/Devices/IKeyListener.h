
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DEVICES_IKEY_LISTENER_H__
#define __DEVICES_IKEY_LISTENER_H__

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Pointers/Pointers.h"

#include "Keys.h"

namespace Usul {
namespace Devices {


  struct IKeyListener : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IKeyListener );

  virtual void keyPressed ( unsigned int code ) = 0;
  virtual void keyReleased ( unsigned int code ) = 0;

};

} // namespace Usul
} // namespace Devices

#endif // __DEVICES_IKEY_LISTENER_H__
