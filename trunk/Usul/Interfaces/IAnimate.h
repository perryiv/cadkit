
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_ANIMATE_H_
#define _USUL_INTERFACE_ANIMATE_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IAnimate : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IAnimate );

  /// Id for this interface.
  enum { IID = 1102087119u };

  virtual void startAnimation() = 0;
  virtual void stopAnimation() = 0;
  
}; // class IAnimate


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_ANIMATE_H_
