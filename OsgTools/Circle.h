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

namespace OsgTools
{

  template<class Vec2>
  class Circle
  {
  public:
    Circle(): _radius(10.0), _points(20) {}
    Circle(float r, unsigned int p): _radius(r), _points(p) {}

    void radius(float r) { _radius = r; }
    float radius() const { return _radius; }

    void points(unsigned int p) { _points = p; }
    unsigned int points() const { return p; }

    std::vector<Vec2> operator()() const
    {
      std::vector<osg::Vec2> pts;
      for(unsigned int i=0; i<_points; i++)
      {
        float angle = 2.0*osg::PI / _points;
        float x = _radius*cos( angle );
        float y = _radius*sin( angle );
        pts.push_back( Vec2(x,y) );
      }

      return pts;
    }

  private:
    float _radius;
    unsigned int _points;
  };

};

#endif
