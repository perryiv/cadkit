
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to make a box.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Box.h"

#include "osg/Geode"
#include "osg/Geometry"
//#include "osg/ShadeModel"

using namespace OsgTools;


osg::Geode* Box::operator()() const
{
  // half lengths
  float width_2 = _width*0.5;
  float height_2 = _height*0.5;
  float depth_2 = _depth*0.5;

  osg::ref_ptr<osg::Geode> geode = new osg::Geode;

  osg::Vec3Array* vertices = new osg::Vec3Array;
  osg::Vec3Array* normals = new osg::Vec3Array;

  // front face
  vertices->push_back( osg::Vec3(-width_2,-height_2,depth_2) );
  vertices->push_back( osg::Vec3(width_2,-height_2,depth_2) );
  vertices->push_back( osg::Vec3(width_2,height_2,depth_2) );
  vertices->push_back( osg::Vec3(-width_2,height_2,depth_2) );
  normals->push_back( osg::Vec3(0.0,0.0,1.0) );

  // back face
  vertices->push_back( osg::Vec3(width_2,-height_2,-depth_2) );
  vertices->push_back( osg::Vec3(-width_2,-height_2,-depth_2) );
  vertices->push_back( osg::Vec3(-width_2,height_2,-depth_2) );
  vertices->push_back( osg::Vec3(width_2,height_2,-depth_2) );
  normals->push_back( osg::Vec3(0.0,0.0,-1.0) );

  // top face
  vertices->push_back( osg::Vec3(width_2,height_2,depth_2) );
  vertices->push_back( osg::Vec3(width_2,height_2,-depth_2) );
  vertices->push_back( osg::Vec3(-width_2,height_2,-depth_2) );
  vertices->push_back( osg::Vec3(-width_2,height_2,depth_2) );
  normals->push_back( osg::Vec3(0.0,1.0,0.0) );

  // bottom face
  vertices->push_back( osg::Vec3(width_2,-height_2,depth_2) );
  vertices->push_back( osg::Vec3(-width_2,-height_2,depth_2) );
  vertices->push_back( osg::Vec3(-width_2,-height_2,-depth_2) );
  vertices->push_back( osg::Vec3(width_2,-height_2,-depth_2) );
  normals->push_back( osg::Vec3(0.0,-1.0,0.0) );

  // left face
  vertices->push_back( osg::Vec3(-width_2,height_2,depth_2) );
  vertices->push_back( osg::Vec3(-width_2,height_2,-depth_2) );
  vertices->push_back( osg::Vec3(-width_2,-height_2,-depth_2) );
  vertices->push_back( osg::Vec3(-width_2,-height_2,depth_2) );
  normals->push_back( osg::Vec3(-1.0,0.0,0.0) );

  // right face
  vertices->push_back( osg::Vec3(width_2,height_2,-depth_2) );
  vertices->push_back( osg::Vec3(width_2,height_2,depth_2) );
  vertices->push_back( osg::Vec3(width_2,-height_2,depth_2) );
  vertices->push_back( osg::Vec3(width_2,-height_2,-depth_2) );
  normals->push_back( osg::Vec3(1.0,0.0,0.0) );

  osg::Vec4Array* colors = new osg::Vec4Array;
  colors->push_back( this->getColor() );

  osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
  geometry->setVertexArray( vertices );
  geometry->setColorArray( colors );
  geometry->setColorBinding( osg::Geometry::BIND_OVERALL );
  geometry->setNormalArray( normals );
  geometry->setNormalBinding( osg::Geometry::BIND_PER_PRIMITIVE );
  geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,
						0,vertices->size()) );

  geode->addDrawable( geometry.get() );
  return( geode.release() );
}
