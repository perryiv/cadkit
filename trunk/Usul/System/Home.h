
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Mike Jackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Get user's home directory.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_SYSTEM_HOME_DIR_CLASS_H_
#define _USUL_SYSTEM_HOME_DIR_CLASS_H_

#include "Usul/Export/Export.h"

#include <string>


namespace Usul {
namespace System {


struct USUL_EXPORT Home
{
  // Returns empty string if it failed.
  static std::string      dir ( bool wantSlash = false );
};


} // namespace System
} // namespace Usul


#endif // _USUL_SYSTEM_HOME_DIR_CLASS_H_
