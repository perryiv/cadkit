
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
//  TIF IO class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMAGES_GIL_IO_TIF_CLASS_H_
#define _IMAGES_GIL_IO_TIF_CLASS_H_

#include "Images/GIL/IO/Base.h"

#include <boost/gil/extension/io/tiff_io.hpp>

#include <tiffio.h>


namespace Images
{
  namespace GIL
  {
    namespace IO
    {
      template < class ImageType > struct TIF : Base < ImageType >
      {
        typedef Base<ImageType> BaseClass;
        typedef TIF<ImageType> ThisType;
        typedef boost::shared_ptr<ThisType> RefPtr;
        typedef typename BaseClass::Dimensions Dimensions;

        TIF ( bool noHandlers = true ) : BaseClass(),
          _originalErrorHandler   ( 0x0 ),
          _originalWarningHandler ( 0x0 ),
          _noHandlers             ( noHandlers )
        {
          // GIL handels errors by throwing exceptions.
          if ( _noHandlers )
          {
            _originalErrorHandler   = ::TIFFSetErrorHandler   ( 0x0 );
            _originalWarningHandler = ::TIFFSetWarningHandler ( 0x0 );
          }
        }

        virtual Dimensions dimensions ( const std::string &file )
        {
          return boost::gil::tiff_read_dimensions ( file );
        }

        virtual void read ( const std::string &file, ImageType &image )
        {
          boost::gil::tiff_read_and_convert_image ( file, image );
        }

        virtual void write ( const std::string &file, const ImageType &image, unsigned int )
        {
          boost::gil::tiff_write_view ( file, boost::gil::const_view ( image ) );
        }

      protected:

        virtual ~TIF()
        {
          if ( _noHandlers )
          {
            ::TIFFSetErrorHandler   ( _originalErrorHandler );
            ::TIFFSetWarningHandler ( _originalWarningHandler );
          }
        }

      private:

        TIFFErrorHandler _originalErrorHandler;
        TIFFErrorHandler _originalWarningHandler;
        bool _noHandlers;
      };
    }
  }
}


#endif // _IMAGES_GIL_IO_TIF_CLASS_H_
