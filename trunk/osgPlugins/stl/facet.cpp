
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#include "facet.h"

#include "osg/Geometry"

Facet::Facet() : 
_numVec(0)
{
}

void Facet::setVector(const osg::Vec3 &v)
{
  switch (_numVec)
  {
  case 0:
    _v1 = v;
    break;
  case 1:
    _v2 = v;
    break;
  case 2:
    _v3 = v;
    break;
  default:
    return;
  }
  _numVec++;
}

osg::Geometry* Facet::getGeometry()
{
  osg::ref_ptr<osg::Vec3Array> vertices ( new osg::Vec3Array ( ) );
  osg::ref_ptr<osg::Vec3Array> normals  ( new osg::Vec3Array ( ) );

  vertices->push_back(_v1);
  vertices->push_back(_v2);
  vertices->push_back(_v3);

  normals->push_back(_normal);

  // Make a new geometry.
//  osg::ref_ptr<osg::Geometry> geometry ( new osg::Geometry );
  osg::Geometry *geometry = new osg::Geometry(); 
  geometry->setVertexArray ( vertices.get() );
  geometry->setNormalArray ( normals.get() );
  geometry->setNormalBinding ( osg::Geometry::BIND_PER_PRIMITIVE );
  geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::TRIANGLES, 0, vertices->size() ) );

  //return geometry.get();
  return geometry;
}
