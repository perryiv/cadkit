
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __IMAGES_ENUM_H__
#define __IMAGES_ENUM_H__


namespace Images {

// Possible channels
enum Channel
{
  RED = 0,
  GREEN,
  BLUE,
  ALPHA,
  GRAY = 0,
  GALPHA = 1
};

// Possible Pixel Formats
enum PixelFormat
{
  RGB = 0,
  RGBA,
  GRAYSCALE,
  GRAYSCALE_ALPHA
};

}


#endif // __IMAGES_ENUM_H__