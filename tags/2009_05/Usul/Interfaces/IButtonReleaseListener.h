
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for a button release.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_BUTTON_RELEASE_LISTENER_H_
#define _USUL_INTERFACES_BUTTON_RELEASE_LISTENER_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IButtonReleaseListener : public Usul::Interfaces::IUnknown
{
  // Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IButtonReleaseListener );

  // Id for this interface.
  enum { IID = 2967760305u };

  // Called when button is released.
  virtual bool            buttonReleaseNotify ( Usul::Interfaces::IUnknown * ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACES_BUTTON_RELEASE_LISTENER_H_
