
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that abstracts the system sleep function.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_SYSTEM_SLEEP_CLASS_H_
#define _USUL_SYSTEM_SLEEP_CLASS_H_

#include "Usul/Export/Export.h"
#include "Usul/Types/Types.h"


namespace Usul {
namespace System {


struct USUL_EXPORT Sleep
{
  static void milliseconds ( unsigned long );
  static void seconds ( unsigned long );
};


} // namespace System
} // namespace Usul


#endif // _USUL_SYSTEM_SLEEP_CLASS_H_
