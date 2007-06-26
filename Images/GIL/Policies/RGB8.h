
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Policy class for 8-bit RGB images.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMAGES_GIL_POLICIES_RGB_8_CLASS_H_
#define _IMAGES_GIL_POLICIES_RGB_8_CLASS_H_

#include <boost/gil/typedefs.hpp>


namespace Images
{
  namespace GIL
  {
    namespace Policies
    {
      struct RGB8
      {
        typedef boost::gil::rgb8_image_t ImageType;
        typedef ImageType::point_t Dimensions;
      };
    }
  }
}


#endif // _IMAGES_GIL_POLICIES_RGB_8_CLASS_H_
