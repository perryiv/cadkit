
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VTKTOOLS_CONVERT_IMAGE_DATA_H__
#define __VTKTOOLS_CONVERT_IMAGE_DATA_H__

#include "VTKTools/Export/Export.h"

#include "osg/ref_ptr"

#include <vector>

class vtkImageData;

namespace osg { class Image; }

namespace VTKTools {
namespace Convert {

class VTK_TOOLS_EXPORT ImageData
{
public:

  typedef osg::ref_ptr< osg::Image > ImagePtr;
  typedef std::vector< ImagePtr > ImageList;

  static void osgImageToImageData ( const osg::Image &image, vtkImageData &data );

  static void osgImageToImageData ( const ImageList &imageList, vtkImageData &data );

  static void imageDataToOsgImage ( vtkImageData& data, osg::Image &image );
};

}
}

#endif //__VTKTOOLS_CONVERT_IMAGE_DATA_H__

