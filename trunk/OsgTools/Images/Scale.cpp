

///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Images/Scale.h"

#include "osg/Image"

void OsgTools::Images::scale( osg::Image& image, unsigned int scale )
{

  switch ( image.getPixelFormat() )
  {
  case(GL_ALPHA):
  case(GL_LUMINANCE):
  case(GL_LUMINANCE_ALPHA):
    {
      for( int s = 0; s < image.s(); ++s )
      {
        for( int t = 0; t < image.t(); ++t )
        {
          for( int r = 0; r < image.r(); ++ r )
          {
            int value ( *image.data(s,t,r) * scale );

            if( value > 255  )
              value = 255;
            
            *image.data(s,t,r) = (char) value; 
          }
        }
      }
    }
    break;
  default:
    throw std::runtime_error ( "Image format not supported." );
  };

}
