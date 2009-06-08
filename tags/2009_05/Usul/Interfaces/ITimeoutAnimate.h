
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_TIMEOUT_ANIMATE_H__
#define __USUL_INTERFACES_TIMEOUT_ANIMATE_H__

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Quat; class Vec3f; typedef Vec3f Vec3; };

namespace Usul {
namespace Interfaces {

struct ITimeoutAnimate : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITimeoutAnimate );

  /// Id for this interface.
  enum { IID = 133794153u };

  // Animate from one position and orientation to another.
  virtual void startAnimation ( double timeout ) = 0;

}; 

}

}


#endif // __USUL_INTERFACES_TIMEOUT_ANIMATE_H__

