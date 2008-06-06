
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _menukit_osg_backgroundbox_h_
#define _menukit_osg_backgroundbox_h_

#include "DrawableFunctor.h"

#include "osg/PrimitiveSet"

namespace MenuKit
{

  namespace OSG
  {

    class BackgroundBox : public DrawableFunctor
    {
    public:
      BackgroundBox(float h, float w, float d=-0.001f, osg::PrimitiveSet::Mode m=osg::PrimitiveSet::QUADS):
        DrawableFunctor(h), _width(w), _depth(d), _drawmode(m) {}
      ~BackgroundBox() {}

      void width(float w) { _width = w; }
      float width() const { return _width; }

      void depth(float w) { _depth = w; }
      float depth() const { return _depth; }

      void mode(osg::PrimitiveSet::Mode m) { _drawmode = m; }
      osg::PrimitiveSet::Mode move() const { return _drawmode; }

      virtual osg::Drawable* operator() ();

    private:
      float _width, _depth;
      osg::PrimitiveSet::Mode _drawmode;
    };

  };

};

#endif
