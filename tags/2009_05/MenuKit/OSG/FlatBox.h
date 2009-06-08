
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _menukit_osg_flatbox_h_
#define _menukit_osg_flatbox_h_

#include "MenuKit/OSG/DrawableFunctor.h"

#include "osg/PrimitiveSet"
#include "osg/Geometry"

namespace MenuKit
{

  namespace OSG
  {

    class FlatBox : public DrawableFunctor
    {
    public:
      FlatBox():
        DrawableFunctor(1.0), _depth(-0.001), _width(1.0), _mode(osg::PrimitiveSet::QUADS)
      {}

      FlatBox(float h, float w, float d=-0.001f, osg::PrimitiveSet::Mode m=osg::PrimitiveSet::QUADS):
        DrawableFunctor(h), _depth(d), _width(w), _mode(m)
      {}

      FlatBox(const FlatBox& b):
        DrawableFunctor(b), _depth(b._depth), _width(b._width), _mode(b._mode)
      {}

      virtual ~FlatBox()
      {}

      void depth(float w) { _depth = w; }
      float depth() const { return _depth; }

      void mode(osg::PrimitiveSet::Mode m) { _mode = m; }
      osg::PrimitiveSet::Mode move() const { return _mode; }

      void width(float w) { _width = w; }
      float width() const { return _width; }

      virtual osg::Drawable* operator() ();

    private:
      float _depth, _width;
      osg::PrimitiveSet::Mode _mode;
    };

    // TODO: turn off double sided polygons
    inline osg::Drawable* FlatBox::operator() ()
    {
      float width_2  = 0.5f * _width;
      float height_2 = 0.5f * height();

      osg::Vec3Array* vertices = new osg::Vec3Array;
      vertices->push_back ( osg::Vec3 (  width_2,  height_2, _depth ) );
      vertices->push_back ( osg::Vec3 ( -width_2,  height_2, _depth ) );
      vertices->push_back ( osg::Vec3 ( -width_2, -height_2, _depth ) );
      vertices->push_back ( osg::Vec3 (  width_2, -height_2, _depth ) );

      osg::Vec3Array* normals = new osg::Vec3Array;
      normals->push_back( osg::Vec3(0.0,0.0,1.0) );

      osg::Vec4Array* colors = new osg::Vec4Array;
      colors->push_back( color() );

      osg::Geometry* patch = new osg::Geometry();
      patch->setVertexArray( vertices );
      patch->setNormalArray( normals );
      patch->setNormalBinding( osg::Geometry::BIND_OVERALL );
      patch->setColorArray( colors );
      patch->setColorBinding( osg::Geometry::BIND_OVERALL );
      patch->addPrimitiveSet( new osg::DrawArrays(_mode,
                                                  0,vertices->size()) );

      return( patch );
    }

  };

};

#endif
