
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  File functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_FILE_FUNCTIONS_H_
#define _FOX_TOOLS_FILE_FUNCTIONS_H_

#include "FoxTools/Export/Export.h"

#include <list>
#include <string>


namespace FoxTools {
namespace Functions {


// Get a list of all files in the given directories that match the pattern.
// Typical patterns are *.txt, *.*, or *
FOX_TOOLS_EXPORT void files ( const std::list<std::string> dirs, const std::string &pattern, std::list<std::string> &fl );
FOX_TOOLS_EXPORT void files ( const std::string &dir,            const std::string &pattern, std::list<std::string> &fl );


}; // namespace Functions
}; // namespace FoxTools


#endif // _FOX_TOOLS_FILE_FUNCTIONS_H_
