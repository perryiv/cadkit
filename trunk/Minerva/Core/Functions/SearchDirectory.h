
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Search a directory (and all sub-directories) and add all readable files to group.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_FUNCTIONS_SEARCH_DIRECTORY_H__
#define __MINERVA_CORE_FUNCTIONS_SEARCH_DIRECTORY_H__

#include "Minerva/Core/Export.h"

#include <string>

namespace Minerva { namespace Core { namespace Layers { class RasterGroup; } } }

namespace Minerva {
namespace Core {
namespace Functions {


MINERVA_EXPORT void searchDirectory ( Minerva::Core::Layers::RasterGroup& group, const std::string directory, bool showFoundLayers = false );
  

}
}
}


#endif // __MINERVA_CORE_FUNCTIONS_SEARCH_DIRECTORY_H__
