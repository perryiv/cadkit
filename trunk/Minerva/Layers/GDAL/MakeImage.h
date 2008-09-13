
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Make an osg::Image.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __GDAL_MAKE_IMAGE_H__
#define __GDAL_MAKE_IMAGE_H__

#include "osg/Image"

#include "gdal.h"

namespace Minerva {
namespace GDAL {

inline osg::Image* makeImage ( unsigned int width, unsigned int height, int bands, GDALDataType type, bool hasColorTable = false )
{
  // Only handle 1, 2, 3, and 4 bands.
  if ( bands < 1 || bands > 4 )
    return 0x0;
  
  osg::ref_ptr<osg::Image> result ( new osg::Image );
  
  // The pixel format.  Make sure the format always has an alpha.
  GLenum pixelFormat ( ( 3 == bands || 4 == bands || hasColorTable ) ? GL_RGBA : GL_LUMINANCE_ALPHA );
  
  GLenum dataType ( GL_UNSIGNED_BYTE );
  
  switch ( type )
  {
    case GDT_Byte:
      dataType = GL_UNSIGNED_BYTE;
      break;
    case GDT_UInt16:
      dataType = GL_UNSIGNED_SHORT;
      break;
    case GDT_Int16:
      dataType = GL_SHORT;
      break;
    case GDT_UInt32:
      dataType = GL_UNSIGNED_INT;
      break;
    case GDT_Int32:
      dataType = GL_INT;
      break;
    case GDT_Float32:
      dataType = GL_FLOAT;
      break;
    case GDT_Float64:
      dataType = GL_DOUBLE;
      break;
    default:
      return 0x0; // We don't handle this data type.
  }
  
  result->allocateImage ( width, height, 1, pixelFormat, dataType );
  
  return result.release();
}


}
}

#endif // __GDAL_MAKE_IMAGE_H__
