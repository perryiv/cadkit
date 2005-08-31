
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Generate a tiled image.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_IMAGES_TILED_IMAGE_H_
#define _OSG_TOOLS_IMAGES_TILED_IMAGE_H_

#include "OsgTools/Export.h"

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Image; }


namespace OsgTools
{
  namespace Images
  {
    OSG_TOOLS_EXPORT osg::Image *makeTiledImage ( unsigned int height, unsigned int width, Usul::Interfaces::IUnknown *caller );
  }
}


#endif // _OSG_TOOLS_IMAGES_TILED_IMAGE_H_
