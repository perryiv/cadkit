
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_FUNCTIONS_GUID_H__
#define __USUL_FUNCTIONS_GUID_H__

#include "Usul/Export/Export.h"

#include <string>

namespace Usul {
namespace Functions {

  struct USUL_EXPORT GUID
  {
    static std::string generate();
  };
}
}

#endif // __USUL_FUNCTIONS_GUID_H__
