
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Mike Jackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class the returns time values in seconds or milliseconds.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_SYSTEM_CLOCK_CLASS_H_
#define _USUL_SYSTEM_CLOCK_CLASS_H_

#include "Usul/Export/Export.h"
#include "Usul/Types/Types.h"


namespace Usul {
namespace System {


struct USUL_EXPORT Clock
{
  static Usul::Types::Uint64          milliseconds();
  static Usul::Types::Uint64          seconds();
};


} // namespace System
} // namespace Usul


#endif // _USUL_SYSTEM_CLOCK_CLASS_H_
