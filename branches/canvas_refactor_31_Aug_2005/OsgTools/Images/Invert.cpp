
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Images/Invert.h"

#include "osg/Image"

#include <stdexcept>

void OsgTools::Images::invert( osg::Image& image )
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
            unsigned char value ( *image.data(s,t,r) );
            
            *image.data(s,t,r) = 255 - value; 
          }
        }
      }
    }
    break;
  default:
    throw std::runtime_error ( "Image format not supported." );
  };

}
