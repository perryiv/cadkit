///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to set the color of osg types.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/ColorSetter.h"
#include "OsgTools/State/StateSet.h"

#include "osg/ShapeDrawable"
#include "osg/Geometry"

using namespace OsgTools;

ColorSetter& ColorSetter::operator =(const ColorSetter& cs)
{
  _color = cs._color;
  return *this;
}

void ColorSetter::operator ()(osg::ShapeDrawable* sd) const
{
  sd->setColor( _color );
}

void ColorSetter::operator ()(osg::Geometry* g) const
{
  osg::Vec4Array* color_array = new osg::Vec4Array();
  color_array->push_back( _color );
  g->setColorArray( color_array );
  g->setColorBinding( osg::Geometry::BIND_OVERALL );
 
  // Turn off lighting.
  OsgTools::State::StateSet::setLighting ( g->getOrCreateStateSet(), false );
}
