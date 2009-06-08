
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for animation notifications.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_ANIMATION_NOTIFY_CALLBACK_H_
#define _USUL_INTERFACES_ANIMATION_NOTIFY_CALLBACK_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IAnimationNotify : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IAnimationNotify );

  /// Id for this interface.
  enum { IID = 2326457777u };

  // Called when the animation happens.
  virtual void                animationStarted() = 0;
  virtual void                animationStep ( unsigned int step, unsigned int totalSteps ) = 0;
  virtual void                animationStopped() = 0;
};


}
}

#endif // _USUL_INTERFACES_ANIMATION_NOTIFY_CALLBACK_H_
