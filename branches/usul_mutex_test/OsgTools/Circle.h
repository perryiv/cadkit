
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

#ifndef _osg_tools_circle_builder_h_
#define _osg_tools_circle_builder_h_

#include "OsgTools/Export.h"

#include "osg/Array"

namespace OsgTools
{

  class OSG_TOOLS_EXPORT Circle
  {
  public:
    Circle(): _radius(1.0), _points(20) {}
    Circle(float r, unsigned int p): _radius(r), _points(p) {}

    void radius(float r) { _radius = r; }
    float radius() const { return _radius; }

    void points(unsigned int p) { _points = p; }
    unsigned int points() const { return _points; }

    osg::Vec3Array* operator()() const;

  private:
    float _radius;
    unsigned int _points;
  };

};

#endif
