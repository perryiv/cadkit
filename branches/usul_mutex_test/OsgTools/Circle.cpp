
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Help defining a Circle.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Circle.h"

#include "osg/Math"
#include "osg/Vec3"

using namespace OsgTools;

osg::Vec3Array* Circle::operator ()() const
{
  osg::Vec3Array* pts = new osg::Vec3Array();
  for(unsigned int i=0; i<_points; i++)
  {
    float angle = float(i)*(float(2.0*osg::PI) / float(_points));
    float x = _radius*cosf( angle );
    float y = _radius*sinf( angle );
    pts->push_back( osg::Vec3(x,y,0.0) );
  }

  return pts;
}
