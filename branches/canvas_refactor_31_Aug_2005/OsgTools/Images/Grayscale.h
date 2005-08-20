
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_IMAGES_GRAYSCALE_H__
#define __OSG_TOOLS_IMAGES_GRAYSCALE_H__

namespace osg { class Image; }

#include "OsgTools/Export.h"

namespace OsgTools {
  namespace Images {

  OSG_TOOLS_EXPORT bool        isGrayscale    ( const osg::Image& image );
  OSG_TOOLS_EXPORT osg::Image* rgbToGrayscale ( osg::Image* image );

  }
}

#endif // __OSG_TOOLS_IMAGES_GRAYSCALE_H__
