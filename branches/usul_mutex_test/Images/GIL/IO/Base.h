
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
//  Base IO class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _IMAGES_GIL_IO_BASE_CLASS_H_
#define _IMAGES_GIL_IO_BASE_CLASS_H_

#include <boost/shared_ptr.hpp>
#include <boost/gil/utilities.hpp>

#include <string>


namespace Images
{
  namespace GIL
  {
    namespace IO
    {
      template < class ImageType > struct Base
      {
        typedef Base<ImageType> ThisType;
        typedef boost::shared_ptr<ThisType> RefPtr;
        typedef boost::gil::point2<std::ptrdiff_t> Dimensions;

        virtual Dimensions dimensions ( const std::string &file ) = 0;

        virtual void read  ( const std::string &file, ImageType &image ) = 0;
        virtual void write ( const std::string &file, const ImageType &image, unsigned int quality ) = 0;

        // Has to be public for boost::shared_ptr.
        virtual ~Base(){}

      protected:

        Base(){}

      private:

        // No copying or assignment.
        Base ( const Base & );
        Base &operator = ( const Base & );
      };
    }
  }
}


#endif // _IMAGES_GIL_IO_BASE_CLASS_H_
