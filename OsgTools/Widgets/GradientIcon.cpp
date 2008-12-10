
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Widgets/GradientIcon.h"

#include "osg/Geode"
#include "osg/Geometry"

using namespace OsgTools::Widgets;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

GradientIcon::GradientIcon() : BaseClass(),
  _minColor(),
  _maxColor()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

GradientIcon::~GradientIcon()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the minimum color.
//
///////////////////////////////////////////////////////////////////////////////

void GradientIcon::minColor( const osg::Vec4& min )
{
  _minColor = min;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the minimum color.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec4 & GradientIcon::minColor() const
{
  return _minColor;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the maximum color.
//
///////////////////////////////////////////////////////////////////////////////

void GradientIcon::maxColor( const osg::Vec4& max )
{
  _maxColor = max;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the maximum color.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec4 & GradientIcon::maxColor() const
{
  return _maxColor;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* GradientIcon::buildScene ( unsigned int depth )
{
  const SizeType width  ( this->size()[0] );
  const SizeType height ( this->size()[1] );
  
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );

  osg::ref_ptr < osg::Vec3Array > vertices ( new osg::Vec3Array );
  vertices->push_back ( osg::Vec3 ( 0.0, 0.0, 0.0 ) );
  vertices->push_back ( osg::Vec3 ( 0.0, height, 0.0 ) );
  vertices->push_back ( osg::Vec3 ( width, height, 0.0 ) );
  vertices->push_back ( osg::Vec3 ( width, 0.0, 0.0 ) );

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

  // Set the correct render order.
  Item::_setState ( geode->getOrCreateStateSet(), depth );

  return geode.release();
}
