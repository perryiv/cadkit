
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Builders/Sign.h"

#include "osg/ref_ptr"
#include "osg/Group"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Shape"
#include "osg/ShapeDrawable"
#include "osg/AutoTransform"
#include "osg/Material"

#include "osgText/Text"

using namespace OsgTools::Builders;


Sign::Sign() : _text ( "Test" ),
_start ( 0.0, 0.0, 0.0 ),
_end ( 0.0, 1.0, 0.0 ),
_drawAnchor ( true ),
_drawPost ( true ),
_drawSign ( true )
{
}

Sign::~Sign()
{
}

osg::Node* Sign::operator() ()
{
  osg::ref_ptr < osg::Group > group ( new osg::Group );

  osg::ref_ptr< osg::AutoTransform > at ( new osg::AutoTransform );
  at->setAutoRotateMode ( osg::AutoTransform::ROTATE_TO_SCREEN );
  at->setPosition( _end );

  at->addChild( this->_buildText() );
  at->addChild( this->_buildSign() );
  at->addChild( this->_buildPost() );
  at->addChild( this->_buildAnchor() );

  group->addChild( at.get() );

  return group.release();
}


osg::Node*  Sign::_buildText()
{
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );

  osg::ref_ptr< osgText::Text > text ( new osgText::Text );

  text->setText( _text );

  osg::Vec3 position( _end );
  position.y() += 0.5;
  position.z() += 0.1;

  text->setCharacterSize( 0.5 );
  text->setPosition( position );
  text->setAlignment(osgText::Text::CENTER_CENTER);
  text->setAxisAlignment( osgText::Text::XY_PLANE );
  text->setCharacterSizeMode(osgText::Text::OBJECT_COORDS);
  
  geode->addDrawable( text.get() );

  return geode.release();
}


osg::Node*  Sign::_buildSign()
{
  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );

  float width ( 0.5 );
  float height ( 1.0 );

  osg::ref_ptr < osg::Geometry > geom ( new osg::Geometry );
  osg::ref_ptr < osg::Vec3Array > vertices ( new osg::Vec3Array ( 4 ) );
  (*vertices)[0] = osg::Vec3 ( _end.x() - width, _end.y(), _end.z() );
  (*vertices)[1] = osg::Vec3 ( _end.x() - width, _end.y() + height, _end.z() );
  (*vertices)[2] = osg::Vec3 ( _end.x() + width, _end.y() + height, _end.z() );
  (*vertices)[3] = osg::Vec3 ( _end.x() + width, _end.y(), _end.z() );

  geom->setVertexArray( vertices.get() );
  geom->addPrimitiveSet( new osg::DrawArrays ( GL_QUADS, 0, vertices->size() ) );

  geode->addDrawable ( geom.get() );

  osg::ref_ptr< osg::Material > mat ( new osg::Material );
  mat->setDiffuse ( osg::Material::FRONT, osg::Vec4 ( 0.0, 1.0, 0.0, 1.0 ) );

  geode->getOrCreateStateSet()->setAttribute ( mat.get() );
  geode->getOrCreateStateSet()->setMode ( GL_DEPTH_TEST, osg::StateAttribute::OFF );
  geode->getOrCreateStateSet()->setRenderingHint ( osg::StateSet::TRANSPARENT_BIN );

  return geode.release();
}

osg::Node*  Sign::_buildPost()
{
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );

  osg::ref_ptr < osg::Geometry > geom ( new osg::Geometry );
  osg::ref_ptr < osg::Vec3Array > vertices ( new osg::Vec3Array ( 2 ) );
  (*vertices)[0] = _start;
  (*vertices)[1] = _end;

  geom->setVertexArray( vertices.get() );
  geom->addPrimitiveSet( new osg::DrawArrays ( osg::PrimitiveSet::LINES, 0, vertices->size() ) );

  osg::ref_ptr < osg::Vec4Array > color ( new osg::Vec4Array ( 1 ) );
  color->at( 0 ).set( 1.0, 0.0, 0.0, 1.0 );

  geom->setColorArray( color.get() );
  geom->setColorBinding( osg::Geometry::BIND_OVERALL );

  geode->addDrawable( geom.get() );

  return geode.release();
}

osg::Node*  Sign::_buildAnchor()
{
  osg::ref_ptr< osg::Shape > sphere ( new osg::Sphere ( _start, 0.25 ) );
  osg::ref_ptr< osg::ShapeDrawable > sd ( new osg::ShapeDrawable ( sphere.get() ) );
  
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  geode->addDrawable ( sd.get( ) );

  return geode.release();
}
