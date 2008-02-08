
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

#ifndef _USUL_SYSTEM_MEMORY_CLASS_H_
#define _USUL_SYSTEM_MEMORY_CLASS_H_

#include "Usul/Export/Export.h"
#include "Usul/Types/Types.h"

#include <string>

#ifdef __APPLE__
#include <sys/sysctl.h>
#include <stdio.h>
#include "mach/mach_host.h"
#include <mach/mach.h>
#include <mach/mach_types.h>
#endif

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

#ifdef __APPLE__
  static void                     DARWIN_getVMStat (vm_statistics_t vmstat);
  static int                      DARWIN_getPageSize();
  static long long int            DARWIN_getPhysicalMemory(); 
  static long long int            DARWIN_getUsedMemory();
  static long long int            DARWIN_getAvailableMemory();
#endif
  // Return a formatted string.
  static std::string               formatPhysical();
};


} // namespace System
} // namespace Usul


#endif // _USUL_SYSTEM_MEMORY_CLASS_H_
