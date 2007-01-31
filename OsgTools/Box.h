
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

#include "OsgTools/Export.h"
#include "OsgTools/Declarations.h"

#include "osg/Vec4"


namespace OsgTools {


class OSG_TOOLS_EXPORT Box
{
public:
  Box(): _width(1.0), _height(1.0), _depth(1.0), _color() {;}
  Box(float w,float h,float d): _width(w), _height(h), _depth(d), _color() {;}
  Box(const Box& b): _width(b._width), _height(b._height), _depth(b._depth), _color(b._color) {;}

  ~Box() {;}

  Box& operator = (const Box& b) {_width=b._width;_height=b._height;_depth=b._depth;_color=b._color; return *this; }

  osg::Geode* operator()() const;
  void set(float w,float h,float d) { _width = w; _height = h; _depth = d; }
  void setColor(const osg::Vec4& c) { _color = c; }

private:
  float _width,_height,_depth;
  osg::Vec4 _color;
};


}; // namespace OsgTools


#endif // _OSG_TOOLS_BOX_H_
