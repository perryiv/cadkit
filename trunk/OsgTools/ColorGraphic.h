///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Abstract class for creating Geodes using color instead of material.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _osg_tools_color_graphic_builder_
#define _osg_tools_color_graphic_builder_

#include "OsgTools/Export.h"

#include "osg/Vec4"

namespace osg
{
  class Geode;
};

namespace OsgTools
{
  /*
    Abstract class for deriving tools to create a geode with colors
  */
  class ColorGraphic
  {
  public:
    ColorGraphic(const osg::Vec4& c=osg::Vec4(1.0,0.0,0.0,1.0)): _color(c) {}
    ColorGraphic(const ColorGraphic& cg): _color(cg._color) {}
    virtual ~ColorGraphic() {}

    ColorGraphic& operator = (const ColorGraphic& cg)
    {
      _color = cg._color;
      return (*this);
    }

    void setColor(const osg::Vec4& c) { _color = c; }
    const osg::Vec4& getColor() const { return _color; }

    //virtual osg::Geode* operator()() = 0;

  private:
    osg::Vec4 _color;
  };

};

#endif
