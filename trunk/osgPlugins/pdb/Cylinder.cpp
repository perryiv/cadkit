///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#include "Cylinder.h"

#include "osg/Material"
#include "osg/Matrixf"
#include "osg/LineWidth"
#include "osg/ref_ptr"
#include "osg/Geometry"

#include <iostream>

#include <math.h>

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////
Cylinder::Cylinder(const osg::Vec3& v1, const osg::Vec3& v2) :
_point1(v1),
_point2(v2)
{
}

///////////////////////////////////////////////////////////////////////////////
//
//  Build the geometry for the cylinder
//  TODO fix normals
//  TODO have radius passed in.
//
///////////////////////////////////////////////////////////////////////////////
osg::Geometry* Cylinder::getGeometry(osg::Material *m, unsigned int steps) const
{
  float r=0.25f;

  osg::ref_ptr< osg::Geometry > geometry (new osg::Geometry);
  osg::ref_ptr< osg::Vec3Array > vertices (new osg::Vec3Array);
  
  //build a unit cylinder
  osg::Vec3 unPoint1 (0,0,0);
  osg::Vec3 unPoint2 (0,1,0);
  
  unPoint1.normalize();
  unPoint2.normalize();

  //get distance
  osg::Vec3 dist = _point2 - _point1;
  float d = dist.length();
  dist.normalize();

  //set up Matrix Transforms
  osg::Matrixf scale, rotate, translate, matrix;
  scale = matrix.scale( osg::Vec3(r, d, r) );
  rotate.makeRotate(unPoint2, dist);
  translate.setTrans(_point1);
  osg::Matrixf transform = scale * rotate * translate;

  float c = 3.14159 / 180.0;
  float x,y,z;
  //calculate the point for the unit cylinder then multiply it by the transform matrix
  for(unsigned int i = 0; i < steps; ++i)
  {
    float u = (float) i / (float) (steps - 1);
    float theta = u * 360.0;
    x = sin( c * theta ) + unPoint1[0];
    y = unPoint1[1];
    z = cos( c * theta ) + unPoint1[2];
    vertices->push_back( ( osg::Vec3(x, y, z) * transform ) );

    x = sin( c * theta ) + unPoint2[0];
    y = unPoint2[1];
    z = cos( c * theta ) + unPoint2[2];
    vertices->push_back( ( osg::Vec3(x, y, z) * transform ) );
  }

  osg::ref_ptr< osg::Vec3Array > normals ( new osg::Vec3Array );

  osg::Vec3 v1, v2, v3;

  //calculate the normals for the tri-strips
  for(unsigned int i = 0; i < vertices->size(); ++ i)
  {
    osg::Vec3 normal;
    v1 = vertices->at( (i+2) % (vertices->size() - 1) );
    v2 = vertices->at( (i+1) % (vertices->size() - 1) );
    v3 = vertices->at(i);
    normal = (v3-v1)^(v2-v1);
    normal.normalize();
    normals->push_back(normal);
  }
   
  geometry->setVertexArray ( vertices.get() );
  geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::TRIANGLE_STRIP, 0, vertices->size() ) );
  // TODO, make this an option. Display lists crash with really big files.
  geometry->setUseDisplayList ( false );

  osg::ref_ptr<osg::LineWidth> lw ( new osg::LineWidth );
  lw->setWidth ( 1 );

  osg::ref_ptr<osg::StateSet> state = geometry->getOrCreateStateSet();
  state->setAttribute ( lw.get() );
  state->setAttribute ( m );

  geometry->setNormalArray(normals.get());
  geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

  return geometry.release();
}

float Cylinder::_getDist(const osg::Vec3& v1, const osg::Vec3& v0) const
{
  float deltaX = v1[0] - v0[0];
  float deltaY = v1[1] - v0[1];
  float deltaZ = v1[2] - v0[2];
  return (float) sqrt((deltaX * deltaX) + (deltaY * deltaY) + (deltaZ * deltaZ));
}