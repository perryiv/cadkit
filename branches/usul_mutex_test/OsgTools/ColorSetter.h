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

#ifndef _osg_tools_color_setter_h_
#define _osg_tools_color_setter_h_

#include "OsgTools/Export.h"

#include "osg/Vec4"

namespace osg
{
  class ShapeDrawable;
  class Drawable;
  class Geometry;
};

namespace OsgTools
{

  class OSG_TOOLS_EXPORT ColorSetter
  {
  public:
    ColorSetter(): _color(1.0,0.0,0.0,1.0) {}
    ColorSetter(const ColorSetter& cs): _color(cs._color) {}
    ~ColorSetter() {}

    ColorSetter& operator =(const ColorSetter& cs);

    void operator ()(osg::ShapeDrawable* sd) const;
    void operator ()(osg::Geometry* g) const;

    void color(const osg::Vec4& c) { _color = c; }
    const osg::Vec4& color() const { return _color; }

  private:
    osg::Vec4 _color;
  };

};

#endif
