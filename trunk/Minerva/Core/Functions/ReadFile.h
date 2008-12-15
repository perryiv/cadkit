
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_FUNCTIONS_READ_FILE_H__
#define __MINERVA_CORE_FUNCTIONS_READ_FILE_H__

#include "Minerva/Core/Export.h"

#include <string>

namespace Usul { namespace Interfaces { struct IUnknown; } }

namespace Minerva {
namespace Core {
namespace Functions {


MINERVA_EXPORT Usul::Interfaces::IUnknown* readFile ( const std::string& filename );


}
}
}


#endif // __MINERVA_CORE_FUNCTIONS_READ_FILE_H__
