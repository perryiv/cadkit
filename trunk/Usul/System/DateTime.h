
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Mike Jackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class the returns time nicely formatted dates and times.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_SYSTEM_CLOCK_CLASS_H_
#define _USUL_SYSTEM_CLOCK_CLASS_H_

#include "Usul/Export/Export.h"
#include "Usul/Types/Types.h"

#include <string>


namespace Usul {
namespace System {


struct USUL_EXPORT DateTime
{
  static std::string        now();
  static std::string        format ( long ); // time_t
};


} // namespace System
} // namespace Usul


#endif // _USUL_SYSTEM_CLOCK_CLASS_H_
