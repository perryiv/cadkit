
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "CylinderFactory.h"

CylinderFactory::CylinderFactory() :
_cylinders()
{
}

osg::Geometry* CylinderFactory::create(unsigned int sides)
{
  if(sides >= _cylinders.size())
    _cylinders.resize(sides + 1);

  if(_cylinders.at(sides).valid())
    return _cylinders.at(sides)->getGeometry();

  _cylinders.at(sides) = new Cylinder(sides);

  return  _cylinders.at(sides)->getGeometry();
}