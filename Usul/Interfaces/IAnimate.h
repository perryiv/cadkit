
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ANIMATE_H__
#define __USUL_INTERFACES_ANIMATE_H__

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Quat; class Vec3f; }

namespace Usul {
namespace Interfaces {


struct IAnimate : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IAnimate );

  /// Id for this interface.
  enum { IID = 3673709138u };

  // Animate from current position to given position
  virtual void animate ( const osg::Vec3f& center, float distance, const osg::Quat& rotation, Usul::Interfaces::IUnknown *caller = 0x0 ) = 0;
}; 


}
}

#endif // __USUL_INTERFACES_ANIMATE_H__

