
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that represents the system's last error.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_SYSTEM_LAST_ERROR_CLASS_H_
#define _USUL_SYSTEM_LAST_ERROR_CLASS_H_

#include "Usul/Export/Export.h"

#include <string>


namespace Usul {
namespace System {


struct USUL_EXPORT LastError
{
  // Typedefs
#ifdef _WIN32
  typedef unsigned long Number;
#else
  typedef int Number;
#endif

  // Initialize the last error.
  static void             init();

  // Get the last error message.
  static std::string      message();

  // Get the number of the last error.
  static Number           number();
};


}; // namespace System
}; // namespace Usul


#endif // _USUL_SYSTEM_LAST_ERROR_CLASS_H_
