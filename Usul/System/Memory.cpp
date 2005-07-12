
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

#include "Usul/System/Memory.h"

#include <sstream>

#ifdef _MSC_VER
# include <windows.h> // For GetComputerName()
#endif

#ifdef __APPLE__
#include <sys/sysctl.h>
#include <sys/vmmeter.h>
#include <Usul/Types/Types.h>
#endif

using namespace Usul;
using namespace Usul::System;


///////////////////////////////////////////////////////////////////////////////
//
//  Get the memory.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Types::Uint64 Memory::totalPhysical()
{
#ifdef _MSC_VER

  ::MEMORYSTATUS status;
  ::GlobalMemoryStatus ( &status );
  return status.dwTotalPhys;
  
#elif __APPLE__

  int mib[2];
  size_t len;
  int physmem;
  mib[0] = CTL_HW;
  mib[1] = HW_PHYSMEM;
  len = sizeof(physmem);
  sysctl(mib, 2, &physmem, &len, NULL, 0);
  return static_cast< Usul::Types::Uint64 > ( physmem );
  
#else

  TODO FIX ME

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the memory.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Types::Uint64 Memory::totalVirtual()
{
#ifdef _MSC_VER

  ::MEMORYSTATUS status;
  ::GlobalMemoryStatus ( &status );
  return status.dwTotalVirtual;

#elif __APPLE__
    struct xsw_usage swapusage;
    int mib[2];
    size_t len;
    mib[0] = CTL_VM;
    mib[1] = VM_SWAPUSAGE;
    len = sizeof(swapusage);
    sysctl(mib, 2, &swapusage, &len, NULL, 0);
    return static_cast< Usul::Types::Uint64 > ( swapusage.xsu_total );

#else
  TODO  FIX ME
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the memory.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Types::Uint64 Memory::availablePhysical()
{
#ifdef _MSC_VER

  ::MEMORYSTATUS status;
  ::GlobalMemoryStatus ( &status );
  return status.dwAvailPhys;

#elif __APPLE__

  return (Memory::totalPhysical() - Memory::usedPhysical() );

#else

  TODO FIX ME
  
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the memory.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Types::Uint64 Memory::availableVirtual()
{
#ifdef _MSC_VER

  ::MEMORYSTATUS status;
  ::GlobalMemoryStatus ( &status );
  return status.dwAvailVirtual;

#elif __APPLE__

    struct xsw_usage swapusage;
    int mib[2];
    size_t len;
    mib[0] = CTL_VM;
    mib[1] = VM_SWAPUSAGE;
    len = sizeof(swapusage);
    sysctl(mib, 2, &swapusage, &len, NULL, 0);
    return static_cast< Usul::Types::Uint64 > ( swapusage.xsu_avail );
    
#else

 TODO FIX ME

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the memory.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Types::Uint64 Memory::usedPhysical()
{
#ifdef _MSC_VER

  return ( Memory::totalPhysical() - Memory::availablePhysical() );

#elif __APPLE__
  
  int mib[2];
  size_t len;
  int  usedmem;
  len = sizeof(usedmem);
  mib[0] = CTL_HW;
  mib[1] = HW_USERMEM;
  sysctl(mib, 2, &usedmem, &len, NULL, 0);
  return static_cast< Usul::Types::Uint64 > (usedmem);

#else
  
  FIX ME FOR OTHER PLATFORMS

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the memory.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Types::Uint64 Memory::usedVirtual()
{
  return ( Memory::totalVirtual() - Memory::availableVirtual() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return a formatted string.
//
///////////////////////////////////////////////////////////////////////////////

std::string Memory::formatPhysical()
{
  const Usul::Types::Uint64 toKB ( 1024 );
  std::ostringstream out;
  out << "Memory Used: " << Memory::usedPhysical()      / toKB 
      << ", Available: " << Memory::availablePhysical() / toKB 
      << ", Total: "     << Memory::totalPhysical()     / toKB;
  return out.str();
}
