
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _menukit_osg_arrow_h_
#define _menukit_osg_arrow_h_

#include "DrawableFunctor.h"

namespace MenuKit
{
  namespace OSG
  {
    class Arrow: public DrawableFunctor
    {
    public:
      Arrow(float h, float d): DrawableFunctor(h), _depth(d) {}
      virtual ~Arrow() {}

      virtual osg::Drawable* operator() ();

      void depth(float d) { _depth = d; }
      float depth() const { return _depth; }

    private:
      float _depth;
    };

    inline osg::Drawable* Arrow::operator () ()
    {
      osg::Vec3Array* vertices = new osg::Vec3Array();
      float h_2 = 0.5f * height();
      float h_4 = 0.5f * h_2;
      vertices->push_back( osg::Vec3(-h_4, h_2 , _depth) ); // top
      vertices->push_back( osg::Vec3(-h_4,-h_2 , _depth) ); // botton
      vertices->push_back( osg::Vec3(h_2, 0.0 , _depth) );  // pointing dir

      osg::Vec3Array* normals = new osg::Vec3Array();
      normals->push_back( osg::Vec3(0.0,0.0,1.0) );

      osg::Vec4Array* colors = new osg::Vec4Array();
      colors->push_back( color() );

      osg::Geometry* patch = new osg::Geometry();
      patch->setVertexArray( vertices );
      patch->setNormalArray( normals );
      patch->setNormalBinding( osg::Geometry::BIND_OVERALL );
      patch->setColorArray( colors );
      patch->setColorBinding( osg::Geometry::BIND_OVERALL );
      patch->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,vertices->size()) );

      return( patch );
    }
  };
};

#endif
