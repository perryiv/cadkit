
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Utility class for writing the program state to file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_DIAGNOSTICS_WRITE_H_
#define _USUL_DIAGNOSTICS_WRITE_H_

#include "Usul/Export/Export.h"

#include <string>


namespace Usul {
namespace Diagnostics {


class USUL_EXPORT Write
{
public:

  // Write the state of the current process to file.
  static void writeProcessState ( const std::string &file );
};


} // namespace Diagnostics
} // namespace Usul


#endif // _USUL_DIAGNOSTICS_WRITE_H_
