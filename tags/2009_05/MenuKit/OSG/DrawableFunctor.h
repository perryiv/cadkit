
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _menukit_osg_drawablefunctor_h_
#define _menukit_osg_drawablefunctor_h_

#include "osg/Vec4"
#include "osg/Drawable"

namespace MenuKit
{

  namespace OSG
  {

    class DrawableFunctor
    {
    public:
      DrawableFunctor(float h=1.0): _color(1.0,0.0,0.0,1.0), _height(h) {}
      virtual ~DrawableFunctor() {}

      void color(const osg::Vec4& c) { _color = c; }
      const osg::Vec4& color() const { return _color; }

      void height(float h) { _height = h; }
      float height() const { return _height; }

      virtual osg::Drawable* operator() ()=0;

    private:
      float _height;
      osg::Vec4 _color;
    };

  };

};

#endif
