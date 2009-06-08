
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Mike Jackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Get the environment variable.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_SYSTEM_ENVIRONMENT_CLASS_H_
#define _USUL_SYSTEM_ENVIRONMENT_CLASS_H_

#include "Usul/Export/Export.h"

#include <string>


namespace Usul {
namespace System {


struct USUL_EXPORT Environment
{
  static std::string      get ( const std::string &name );
  static bool             has ( const std::string &name );
};


} // namespace System
} // namespace Usul


#endif // _USUL_SYSTEM_ENVIRONMENT_CLASS_H_
