
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_IMAGES_MORPHOLOGY_H__
#define __OSG_TOOLS_IMAGES_MORPHOLOGY_H__

#include "OsgTools/Export.h"

namespace osg { class Image; }

#include "OsgTools/Images/Matrix.h"

namespace OsgTools {
namespace Images {

  OSG_TOOLS_EXPORT void    erode   ( osg::Image& image, unsigned int r, unsigned int c );
  OSG_TOOLS_EXPORT void    dilate  ( osg::Image& image, unsigned int r, unsigned int c );
  OSG_TOOLS_EXPORT void    open    ( osg::Image& image, unsigned int r, unsigned int c );
  OSG_TOOLS_EXPORT void    close   ( osg::Image& image, unsigned int r, unsigned int c );

  OSG_TOOLS_EXPORT void    convolve  ( osg::Image& image, const Matrix<int>& mask );

  OSG_TOOLS_EXPORT void    smooth    ( osg::Image& image, const Matrix<int>& mask );

  template < class Mask > 
  OSG_TOOLS_EXPORT void    applyMask ( osg::Image& image, const Mask& mask );

}
}



#endif // __OSG_TOOLS_IMAGES_MORPHOLOGY_H__
