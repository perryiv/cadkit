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

#include "ColorGraphic.h"
#include "OsgTools/Export.h"
#include "OsgTools/Declarations.h"

namespace OsgTools
{

  class OSG_TOOLS_EXPORT Box : public ColorGraphic
  {
  public:
    Box(): ColorGraphic(),
        _width(1.0), _height(1.0), _depth(1.0) {}
    Box(float w,float h,float d): ColorGraphic(),
        _width(w), _height(h), _depth(d) {}
    Box(const Box& b): ColorGraphic(b),
        _width(b._width), _height(b._height), _depth(b._depth) {}

    virtual ~Box() {}

    Box& operator = (const Box& b)
    {
      ColorGraphic::operator = (b);
      _width=b._width;
      _height=b._height;
      _depth=b._depth;
      return *this;
    }

    virtual osg::Geode* operator()() const;
    void set(float w,float h,float d) { _width = w; _height = h; _depth = d; }

    void setDepth(float d) { _depth = d; }
    float getDepth() const { return _depth; }

    void setHeight(float h) { _height = h; }
    float getHeight() const { return _height; }

    void setWidth(float w) { _width = w; }
    float getWidth() const { return _width; }

  private:
    float _width,_height,_depth;
  };

}; // namespace OsgTools

#endif // _OSG_TOOLS_BOX_H_
