///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#include "Cylinder.h"

#include "osg/Matrixf"

#include <math.h>

Cylinder::Cylinder() :
_point1(),
_point2()
{
}

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.  Creates a unit cylinder
//
///////////////////////////////////////////////////////////////////////////////
Cylinder::Cylinder(float r, unsigned int sides)
{
  _geometry = new osg::Geometry();
  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
  osg::ref_ptr< osg::Vec3Array > normals  ( new osg::Vec3Array );

  //build a unit cylinder
  osg::Vec3 unPoint1 (0,0,0);
  osg::Vec3 unPoint2 (0,1,0);

  _point1 = unPoint1;
  _point2 = unPoint2;

  _center[0] = (_point1[0] + _point2[0])/2.0;
  _center[1] = (_point1[1] + _point2[1])/2.0;
  _center[2] = (_point1[2] + _point2[2])/2.0;

  float c = 3.14159 / 180.0;
  float x,y,z;
  //calculate the point for the unit cylinder then multiply it by the transform matrix
  for(unsigned int i = 0; i < sides; ++i)
  {
    float u = (float) i / (float) (sides - 1);
    float theta = u * 360.0;
    x = r * sin( c * theta ) + unPoint1[0];
    y = unPoint1[1];
    z = r * cos( c * theta ) + unPoint1[2];
    vertices->push_back( osg::Vec3(x, y, z) );

    x = r * sin( c * theta ) + unPoint2[0];
    y = unPoint2[1];
    z = r * cos( c * theta ) + unPoint2[2];
    vertices->push_back( osg::Vec3(x, y, z) );
  }

  _geometry->setVertexArray ( vertices.get() );
  _geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::TRIANGLE_STRIP, 0, vertices->size() ) );
  // TODO, make this an option. Display lists crash with really big files.
  //_geometry->setUseDisplayList ( false );

  //osg::Vec3 v1, v2, v3;

  normals->reserve(vertices->size());

  //calculate the normals for the tri-strips
  for(osg::Vec3Array::iterator i = vertices->begin(); i != vertices->end(); ++ i)
  {
    
    osg::Vec3 normal = *i;
    
    /*v1 = vertices->at( (i+2) % (vertices->size() - 1) );
    v2 = vertices->at( (i+1) % (vertices->size() - 1) );
    v3 = vertices->at(i);
    normal = (v3-v1)^(v2-v1);
    */
    normal.normalize();
    normals->push_back(normal);
  }

  _geometry->setNormalArray(normals.get());
  _geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

}