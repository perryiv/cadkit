
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_GET_ANIMATION_TIME_H_
#define _USUL_INTERFACE_GET_ANIMATION_TIME_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IGetAnimationTime : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IGetAnimationTime );

  /// Id for this interface.
  enum { IID = 1102087119u };

  virtual double getAnimationTime() = 0;
  
}; // class IGetAnimationTime


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_GET_ANIMATION_TIME_H_
