
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that represents the local host machine.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_SYSTEM_LOCAL_HOST_MACHINE_CLASS_H_
#define _USUL_SYSTEM_LOCAL_HOST_MACHINE_CLASS_H_

#include "Usul/Export/Export.h"

#include <string>


namespace Usul {
namespace System {


struct USUL_EXPORT Host
{
  // Get the machine name.
  static std::string      name();
};


} // namespace System
} // namespace Usul


#endif // _USUL_SYSTEM_LOCAL_HOST_MACHINE_CLASS_H_
