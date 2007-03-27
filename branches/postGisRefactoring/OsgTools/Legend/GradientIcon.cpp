
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Legend/GradientIcon.h"

#include "osg/Geode"
#include "osg/Geometry"

using namespace OsgTools::Legend;

GradientIcon::GradientIcon() : BaseClass(),
_minColor(),
_maxColor()
{
}

GradientIcon::~GradientIcon()
{
}

void GradientIcon::minColor( const osg::Vec4& min )
{
  _minColor = min;
}

const osg::Vec4 & GradientIcon::minColor() const
{
  return _minColor;
}

void GradientIcon::maxColor( const osg::Vec4& max )
{
  _maxColor = max;
}

const osg::Vec4 & GradientIcon::maxColor() const
{
  return _maxColor;
}


osg::Node* GradientIcon::buildScene()
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

  osg::ref_ptr< osg::Vec4Array > colors ( new osg::Vec4Array );
  colors->push_back ( _minColor );
  colors->push_back ( _minColor );
  colors->push_back ( _maxColor );
  colors->push_back ( _maxColor );

  geometry->setColorArray ( colors.get() );
  geometry->setColorBinding ( osg::Geometry::BIND_PER_VERTEX );

  geometry->addPrimitiveSet ( new osg::DrawArrays ( GL_QUADS, 0, vertices->size() ) );

  geode->addDrawable( geometry.get() );

  return geode.release();
}
