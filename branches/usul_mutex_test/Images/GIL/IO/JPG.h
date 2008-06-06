
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
//  JPG IO class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMAGES_GIL_IO_JPG_CLASS_H_
#define _IMAGES_GIL_IO_JPG_CLASS_H_

#include "Images/GIL/IO/Base.h"

#include <boost/gil/extension/io/jpeg_io.hpp>


namespace Images
{
  namespace GIL
  {
    namespace IO
    {
      template < class ImageType > struct JPG : Base < ImageType >
      {
        typedef Base<ImageType> BaseClass;
        typedef JPG<ImageType> ThisType;
        typedef boost::shared_ptr<ThisType> RefPtr;
        typedef typename BaseClass::Dimensions Dimensions;

        JPG() : BaseClass()
        {
        }

        virtual Dimensions dimensions ( const std::string &file )
        {
          return boost::gil::jpeg_read_dimensions ( file );
        }

        virtual void read ( const std::string &file, ImageType &image )
        {
          boost::gil::jpeg_read_and_convert_image ( file, image );
        }

        virtual void write ( const std::string &file, const ImageType &image, unsigned int quality )
        {
          boost::gil::jpeg_write_view ( file, boost::gil::const_view ( image ), static_cast<int> ( quality ) );
        }

      protected:

        virtual ~JPG()
        {
        }
      };
    }
  }
}


#endif // _IMAGES_GIL_IO_JPG_CLASS_H_
