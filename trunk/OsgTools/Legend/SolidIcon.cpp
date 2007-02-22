
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Legend/SolidIcon.h"

#include "osg/Geode"
#include "osg/Geometry"

using namespace OsgTools::Legend;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SolidIcon::SolidIcon() : BaseClass(),
_material( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SolidIcon::~SolidIcon()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the material.
//
///////////////////////////////////////////////////////////////////////////////

void SolidIcon::material ( osg::Material * material )
{
  _material = material;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the material.
//
///////////////////////////////////////////////////////////////////////////////

osg::Material * SolidIcon::material()
{
  return _material.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the material.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Material * SolidIcon::material() const
{
  return _material.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* SolidIcon::buildScene()
{
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );

  osg::ref_ptr < osg::Vec3Array > vertices ( new osg::Vec3Array );
  vertices->push_back( osg::Vec3 ( 0.0, 0.0, 0.0 ) );
  vertices->push_back( osg::Vec3 ( 0.0, this->height(), 0.0 ) );
  vertices->push_back( osg::Vec3 ( this->width(), this->height(), 0.0 ) );
  vertices->push_back( osg::Vec3 ( this->width(), 0.0, 0.0 ) );

  geometry->setVertexArray ( vertices.get() );

  osg::ref_ptr < osg::Vec3Array > normals ( new osg::Vec3Array );
  normals->push_back ( osg::Vec3 ( 0.0, 0.0, 1.0 ) );

  geometry->setNormalArray ( normals.get() );
  geometry->setNormalBinding ( osg::Geometry::BIND_OVERALL );

  geometry->addPrimitiveSet ( new osg::DrawArrays ( GL_QUADS, 0, vertices->size() ) );

  geode->addDrawable( geometry.get() );

  osg::ref_ptr< osg::StateSet > ss ( geode->getOrCreateStateSet() );
  ss->setAttribute( _material.get(), osg::StateAttribute::ON );

  return geode.release();
}
