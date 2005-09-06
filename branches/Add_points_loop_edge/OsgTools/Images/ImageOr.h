
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __IMAGES_IMAGE_OR_H__
#define __IMAGES_IMAGE_OR_H__

#include "OsgTools/Export.h"

namespace osg { class Image; }

namespace OsgTools {
namespace Images {

  OSG_TOOLS_EXPORT osg::Image* imageOr ( const osg::Image& , const osg::Image& );

}
}

#endif // __IMAGES_IMAGE_OR_H__
