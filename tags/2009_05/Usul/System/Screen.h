
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that represents a screen.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_SYSTEM_SCREEN_CLASS_H_
#define _USUL_SYSTEM_SCREEN_CLASS_H_

#include "Usul/Export/Export.h"


namespace Usul {
namespace System {


struct USUL_EXPORT Screen
{
  // Get the screen width and height.
  static unsigned int           width();
  static unsigned int           height();
};


} // namespace System
} // namespace Usul


#endif // _USUL_SYSTEM_LOCAL_HOST_MACHINE_CLASS_H_
