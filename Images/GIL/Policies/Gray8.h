
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
//  Policy class for 8-bit gray-scale images.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMAGES_GIL_POLICIES_GRAY_8_CLASS_H_
#define _IMAGES_GIL_POLICIES_GRAY_8_CLASS_H_

#include <boost/gil/typedefs.hpp>


namespace Images
{
  namespace GIL
  {
    namespace Policies
    {
      struct Gray8
      {
        typedef boost::gil::gray8_image_t ImageType;
        typedef ImageType::point_t Dimensions;
      };
    }
  }
}


#endif // _IMAGES_GIL_POLICIES_GRAY_8_CLASS_H_
