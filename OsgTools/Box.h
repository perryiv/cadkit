///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to make a box.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_BOX_H_
#define _OSG_TOOLS_BOX_H_

#include "Export.h"
#include "Declarations.h"

#include "osg/Vec4"

namespace OsgTools
{

  class OSG_TOOLS_EXPORT Box
  {
  public:
    Box():
        _width(1.0), _height(1.0), _depth(1.0), _color(osg::Vec4(1.0,0.0,0.0,1.0)) {}
    Box(float w,float h,float d,const osg::Vec4& c=osg::Vec4(1.0,0.0,0.0,1.0)):
        _width(w), _height(h), _depth(d), _color(c) {}
    Box(const Box& b):
        _width(b._width), _height(b._height), _depth(b._depth), _color(b._color) {}

    virtual ~Box() {}

    Box& operator = (const Box& b)
    {
      _width=b._width;
      _height=b._height;
      _depth=b._depth;
      _color=b._color;
      return *this;
    }

    virtual osg::Geode* operator()() const;
    void set(float w,float h,float d) { _width = w; _height = h; _depth = d; }

    void setColor(const osg::Vec4& c) { _color = c; }
    const osg::Vec4& getColor() const { return _color; }

    void setDepth(float d) { _depth = d; }
    float getDepth() const { return _depth; }

    void setHeight(float h) { _height = h; }
    float getHeight() const { return _height; }

    void setWidth(float w) { _width = w; }
    float getWidth() const { return _width; }

  private:
    float _width,_height,_depth;
    osg::Vec4 _color;
  };

}; // namespace OsgTools

#endif // _OSG_TOOLS_BOX_H_
