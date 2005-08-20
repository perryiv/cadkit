
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_IMAGES_HISTOGRAM_H__
#define __OSG_TOOLS_IMAGES_HISTOGRAM_H__

#include "OsgTools/Export.h"

namespace osg { class Image; }

#include <vector>

namespace OsgTools
{
namespace Images {

typedef std::vector< unsigned int > Histogram;

OSG_TOOLS_EXPORT   Histogram   calculateHistogram ( const osg::Image& image );

}
}

#endif // __OSG_TOOLS_IMAGES_HISTOGRAM_H__
