
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

#include "osg/ShapeDrawable"
#include "osg/Geometry"

using namespace OsgTools;

void ColorSetter::operator ()(osg::ShapeDrawable* sd)
{
  sd->setColor( _color );
}

void ColorSetter::operator ()(osg::Geometry* g)
{
  //osg::Vec4Array* color_array = new osg::Vec4Array();
  //color_array->push_back( _color );
  //g->setColorArray( color_array );
  g->setColorBinding( osg::Geometry::BIND_OVERALL );
}
