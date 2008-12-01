
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

  // Download to given filename.
  MINERVA_EXPORT bool downloadToFile ( const std::string& href, const std::string& filename );
  MINERVA_EXPORT bool downloadToFile ( const std::string& href, const std::string& filename, unsigned int timeout );

  // Download.  Filename is populated where href is downloaded to.
  MINERVA_EXPORT bool download ( const std::string& href, std::string& filename );
  MINERVA_EXPORT bool download ( const std::string& href, std::string& filename, bool useCache );
}
}
}


#endif // __MINERVA_CORE_UTILITIES_DOWNLOAD_H__
