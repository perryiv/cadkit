
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
//  PNG IO class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMAGES_GIL_IO_PNG_CLASS_H_
#define _IMAGES_GIL_IO_PNG_CLASS_H_

#include "Images/GIL/IO/Base.h"

// Has to be in '<>' for VC++ 8.
// Has to come before this boost header.
// Not sure why...
#include <png.h>

#include <boost/gil/extension/io/png_io.hpp>


namespace Images
{
  namespace GIL
  {
    namespace IO
    {
      template < class ImageType > struct PNG : Base < ImageType >
      {
        typedef Base<ImageType> BaseClass;
        typedef PNG<ImageType> ThisType;
        typedef boost::shared_ptr<ThisType> RefPtr;
        typedef typename BaseClass::Dimensions Dimensions;

        PNG() : BaseClass()
        {
        }

        virtual Dimensions dimensions ( const std::string &file )
        {
          return boost::gil::png_read_dimensions ( file );
        }

        virtual void read ( const std::string &file, ImageType &image )
        {
          boost::gil::png_read_and_convert_image ( file, image );
        }

        virtual void write ( const std::string &file, const ImageType &image, unsigned int )
        {
          boost::gil::png_write_view ( file, boost::gil::const_view ( image ) );
        }

      protected:

        virtual ~PNG()
        {
        }
      };
    }
  }
}


#endif // _IMAGES_GIL_IO_PNG_CLASS_H_
