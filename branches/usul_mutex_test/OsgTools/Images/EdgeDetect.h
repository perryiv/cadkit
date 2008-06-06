
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_IMAGES_EDGE_DETECT_H__
#define __OSG_TOOLS_IMAGES_EDGE_DETECT_H__

#include "OsgTools/Export.h"

namespace osg { class Image; }

namespace OsgTools {
namespace Images {

template < class T > class Matrix;

OSG_TOOLS_EXPORT osg::Image* edgeDetect ( const osg::Image& image, const Matrix<int>& maskX, const Matrix<int>& maskY );

}
}

#endif // __OSG_TOOLS_IMAGES_EDGE_DETECT_H__
