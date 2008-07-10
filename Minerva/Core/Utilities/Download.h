
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Download file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_UTILITIES_DOWNLOAD_H__
#define __MINERVA_CORE_UTILITIES_DOWNLOAD_H__

#include "Minerva/Core/Export.h"

#include <string>

namespace Minerva {
namespace Core {
namespace Utilities {

  MINERVA_EXPORT bool download ( const std::string& href, std::string& filename );

}
}
}


#endif // __MINERVA_CORE_UTILITIES_DOWNLOAD_H__
