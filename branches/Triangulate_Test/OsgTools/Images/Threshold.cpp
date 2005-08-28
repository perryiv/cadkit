
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Images/Threshold.h"

#include "osg/Image"

///////////////////////////////////////////////////////////////////////////////
//
//  Binarized data based on threshold
//
///////////////////////////////////////////////////////////////////////////////

void OsgTools::Images::threshold ( osg::Image& image, unsigned char th )
{
  for( int s = 0; s < image.s(); ++s )
  {
    for( int t = 0; t < image.t(); ++t )
    {
      for( int r = 0; r < image.r(); ++ r )
      {
        unsigned char* value ( image.data(s,t,r) );
        if( *value < th )
          *value = 0;
        else
          *value = 255;
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Threshold.
//
///////////////////////////////////////////////////////////////////////////////

void OsgTools::Images::threshold ( osg::Image& image, unsigned char t1, unsigned char t2 )
{
  for( int s = 0; s < image.s(); ++s )
  {
    for( int t = 0; t < image.t(); ++t )
    {
      for( int r = 0; r < image.r(); ++ r )
      {
        unsigned char* value ( image.data(s,t,r) );
        if( *value < t1 || *value > t2 )
          *value = 0;
        else
          *value = 255;
      }
    }
  }
}
