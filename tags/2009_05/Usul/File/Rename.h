
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_FILE_RENAME_H__
#define __USUL_FILE_RENAME_H__

#include "Usul/Export/Export.h"

#include <string>

namespace Usul {
namespace File {

USUL_EXPORT void rename ( const std::string& oldFilename, const std::string& newFilename, bool allowThrow = false, std::ostream *out = 0x0 );

}
}

#endif // __USUL_FILE_RENAME_H__
