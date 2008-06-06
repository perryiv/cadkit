
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_IMAGES_IMAGE3D_H__
#define __OSG_TOOLS_IMAGES_IMAGE3D_H__

#include "OsgTools/Export.h"

namespace osg { template < class T > class ref_ptr; class Image; }

#include <vector>

namespace OsgTools {
namespace Images {

  typedef osg::ref_ptr< osg::Image > ImagePtr;
  typedef std::vector< ImagePtr >    ImageList;

  OSG_TOOLS_EXPORT osg::Image* image3d ( ImageList&, bool ensureProperTextureSize = false, double updateTime = 1000  );
}
}

#endif // __OSG_TOOLS_IMAGES_THRESHOLD_H__
