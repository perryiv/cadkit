///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#include "Cylinder.h"

#include "osg/Material"

#include <math.h>

Cylinder::Cylinder(const osg::Vec3& v1, const osg::Vec3& v2) :
_point1(v1),
_point2(v2)
{
}

osg::Geometry* Cylinder::getGeometry(osg::Material *m, float step) const
{
  osg::ref_ptr< osg::Geometry > geometry (new osg::Geometry);
  osg::ref_ptr< osg::Vec3Array > vertices (new osg::Vec3Array);

  float c = 3.14159 / 180.0;
  float x,z,r=0.25f;
  for(float theta = 0.0; theta < 360; theta += step) {
      x = r * sin(c * theta) + _point1[0];
      z = r * cos(c * theta) + _point1[2];
      vertices->push_back( osg::Vec3(x, _point1[1], z) );

      x = r * sin(c * (theta + step)) + _point1[0];
      z = r * cos(c * (theta + step)) + _point1[2];
      vertices->push_back( osg::Vec3(x, _point1[1], z) );

      x = r * sin(c * (theta + step)) + _point2[0];
      z = r * cos(c * (theta + step)) + _point2[2];
      vertices->push_back( osg::Vec3(x, _point2[1], z) );
      
      x = r * sin(c * theta) + _point2[0];
      z = r * cos(c * theta) + _point2[2];
      vertices->push_back( osg::Vec3(x, _point2[1], z) );
  }
  
  geometry->setVertexArray ( vertices.get() );
  geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::QUADS, 0, vertices->size() ) );

  osg::ref_ptr<osg::Vec4Array> color ( new osg::Vec4Array );
  color->resize ( 1 );
  (*color)[0] = osg::Vec4(1.0, 1.0, 1.0, 1.0);

  geometry->setColorArray ( color.get() );
  geometry->setColorBinding ( osg::Geometry::BIND_PER_PRIMITIVE_SET );

  osg::ref_ptr<osg::LineWidth> lw ( new osg::LineWidth );
  lw->setWidth ( 1 );

  osg::ref_ptr<osg::StateSet> state = geometry->getOrCreateStateSet();
  state->setAttribute ( lw.get() );
  state->setAttribute ( m );
  //state->setMode ( GL_LIGHTING, osg::StateAttribute::OFF );

  return geometry.release();
}