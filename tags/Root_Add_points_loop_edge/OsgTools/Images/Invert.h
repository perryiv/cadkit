
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


namespace OsgTools
{
namespace Images {

OSG_TOOLS_EXPORT   void   invert ( osg::Image& image );

}
}

#endif // __OSG_TOOLS_IMAGES_HISTOGRAM_H__
