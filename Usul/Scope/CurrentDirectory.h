
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_SCOPED_WORKING_DIRECTORY_H_
#define _USUL_SCOPED_WORKING_DIRECTORY_H_

#include "Usul/Export/Export.h"

#include <string>


namespace Usul {
namespace Scope {


class USUL_EXPORT CurrentDirectory
{
public:

  CurrentDirectory ( const std::string& directory, bool allowThrow = true );
  ~CurrentDirectory();

private:

  std::string _oldCwd;
  bool _allowThrow;
};


} // namespace Scope
} // namespace Usul


#endif // _USUL_SCOPED_WORKING_DIRECTORY_H_
