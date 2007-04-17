
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_SYSTEM_DIRECTORY_H__
#define __USUL_SYSTEM_DIRECTORY_H__

#include "Usul/Export/Export.h"

#include <string>

namespace Usul {
namespace System {

class USUL_EXPORT Directory
{
public:

  /// Get/Set the current working directory.
  static void          cwd ( const std::string& directory );
  static std::string   cwd ();
};

}
}


#endif // __USUL_SYSTEM_DIRECTORY_H__
