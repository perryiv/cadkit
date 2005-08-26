
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that queries machine memory.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_SYSTEM_LOCAL_HOST_MACHINE_CLASS_H_
#define _USUL_SYSTEM_LOCAL_HOST_MACHINE_CLASS_H_

#include "Usul/Export/Export.h"
#include "Usul/Types/Types.h"

#include <string>


namespace Usul {
namespace System {


struct USUL_EXPORT Memory
{
  // Get the memory properties.
  static Usul::Types::Uint64       availablePhysical();
  static Usul::Types::Uint64       availableVirtual();
  static Usul::Types::Uint64       totalPhysical();
  static Usul::Types::Uint64       totalVirtual();
  static Usul::Types::Uint64       usedPhysical();
  static Usul::Types::Uint64       usedVirtual();

  // Return a formatted string.
  static std::string               formatPhysical();
};


} // namespace System
} // namespace Usul


#endif // _USUL_SYSTEM_LOCAL_HOST_MACHINE_CLASS_H_
