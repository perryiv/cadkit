
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Remove the file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FILE_REMOVE_H_
#define _USUL_FILE_REMOVE_H_

#include "Usul/Export/Export.h"

#include <string>


namespace Usul {
namespace File {


// Remove the file. Provided as a convenience.
USUL_EXPORT void remove ( const std::string &file );


}; // namespace File
}; // namespace Usul


#endif // _USUL_FILE_REMOVE_H_