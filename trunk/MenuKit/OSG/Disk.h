#ifndef _menukit_osg_disk_h_
#define _menukit_osg_disk_h_

#include "DrawableFunctor.h"

namespace MenuKit
{
  namespace OSG
  {
    class Disk: public DrawableFunctor
    {
    public:
      Disk(float h,float d=0.0,unsigned int p=80):
        DrawableFunctor(h),_depth(d),_points(p) {}
      virtual ~Disk() {}

      virtual osg::Drawable* operator() ();

      void depth(float d) { _depth = d; }
      float depth() const { return _depth; }

      void points(unsigned int p) { _points = p; }
      unsigned int points() const { return _points; }

    private:
      unsigned int _points;
      float _depth;
    };

    inline osg::Drawable* Disk::operator() ()
    {
      float radius = 0.5*height();
      osg::Vec3Array* vertices = new osg::Vec3Array();
      osg::PrimitiveSet::Mode drawmode = osg::PrimitiveSet::TRIANGLE_FAN;

      // define the circle
      float da = 2.0*osg::PI / float(_points);
      for(unsigned int i=0; i<_points; i++)
      {
        vertices->push_back( osg::Vec3(0.0,0.0,_depth) );

        float angle = float(i)*da;
        float x = radius*cosf(angle);
        float y = radius*sinf(angle);
        vertices->push_back( osg::Vec3(x,y,_depth) );

        float nextangle;
        if( i==(_points-1) )
          nextangle = 0.0;
        else
          nextangle = float(i+1)*da;
        float nx = radius*cosf(nextangle);
        float ny = radius*sinf(nextangle);
        vertices->push_back( osg::Vec3(nx,ny,_depth) );
      }

      osg::Vec4Array* colors = new osg::Vec4Array();
      colors->push_back( color() );

      osg::Vec3Array* normals = new osg::Vec3Array();
      normals->push_back( osg::Vec3(0.0,0.0,1.0) );

      osg::Geometry* disk = new osg::Geometry();
      disk->setVertexArray( vertices );
      disk->setNormalArray( normals );
      disk->setColorArray( colors );
      disk->setNormalBinding( osg::Geometry::BIND_OVERALL );
      disk->setColorBinding( osg::Geometry::BIND_OVERALL );
      disk->addPrimitiveSet( new osg::DrawArrays(drawmode,0,vertices->size()) );

      return( disk );
    }
  };
};

#endif
