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
//  TODO get it working correctly
//
///////////////////////////////////////////////////////////////////////////////
osg::Geometry* Cylinder::getGeometry(osg::Material *m, float step) const
{
  osg::ref_ptr< osg::Geometry > geometry (new osg::Geometry);
  osg::ref_ptr< osg::Vec3Array > vertices (new osg::Vec3Array);

  /*
  //build a unit cylinder
  osg::Vec3 unPoint1 (0,0,0);
  osg::Vec3 unPoint2 (1,0,0);
  */

  float c = 3.14159 / 180.0;
  float x,z,r=0.25f;
  for(float theta = 0.0; theta <= 360; theta += step) {
    /* 
    x = sin(c * theta) + unPoint1[0];
    z = cos(c * theta) + unPoint1[2];
    vertices->push_back( osg::Vec3(x, unPoint1[1], z) );

    x = sin(c * theta) + unPoint2[0];
    z = cos(c * theta) + unPoint2[2];
    vertices->push_back( osg::Vec3(x, unPoint2[1], z) );
    */
    
    x = r * sin(c * theta) + _point1[0];
    z = r * cos(c * theta) + _point1[2];
    vertices->push_back( osg::Vec3(x, _point1[1], z) );

    x = r * sin(c * theta) + _point2[0];
    z = r * cos(c * theta) + _point2[2];
    vertices->push_back( osg::Vec3(x, _point2[1], z) );
  }

  /*
  //get distance
  osg::Vec3 dist = _point2 - _point1;
  float d = dist.length();
  dist.normalize();

  //set up Matrix Transforms
  osg::Matrixf scale, rotate, translate, matrix;
  scale = matrix.scale( osg::Vec3(d, r, r));
  rotate.makeRotate(unPoint2, dist);
  translate = matrix.translate(_point1);

  //multiply each point by the matricies
  for(osg::Vec3Array::iterator i = vertices->begin(); i != vertices->end(); ++i)
  {
    *i = translate.preMult(*i);
    *i = rotate.preMult(*i);
    *i = scale.preMult(*i);
  }
  */
  
  geometry->setVertexArray ( vertices.get() );
  geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::TRIANGLE_STRIP, 0, vertices->size() ) );
  // TODO, make this an option. Display lists crash with really big files.
  geometry->setUseDisplayList ( false );

  osg::ref_ptr<osg::LineWidth> lw ( new osg::LineWidth );
  lw->setWidth ( 1 );

  osg::ref_ptr<osg::StateSet> state = geometry->getOrCreateStateSet();
  state->setAttribute ( lw.get() );
  state->setAttribute ( m );

  return geometry.release();
}

float Cylinder::_getDist(const osg::Vec3& v1, const osg::Vec3& v0) const
{
  float deltaX = v1[0] - v0[0];
  float deltaY = v1[1] - v0[1];
  float deltaZ = v1[2] - v0[2];
  return (float) sqrt((deltaX * deltaX) + (deltaY * deltaY) + (deltaZ * deltaZ));
}