
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Mike Jackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that represents a screen.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_SYSTEM_CLOCK_CLASS_H_
#define _USUL_SYSTEM_CLOCK_CLASS_H_

#include "Usul/Export/Export.h"
#include "Usul/Types/Types.h"

namespace Usul {
  namespace System {
    
    Usul::Types::Uint64 USUL_EXPORT milliseconds();
    
    Usul::Types::Uint64 USUL_EXPORT seconds();
  } // namespace System
} // namespace Usul


#endif // _USUL_SYSTEM_CLOCK_CLASS_H_
