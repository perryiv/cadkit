
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Arrow.h"

#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Array"
#include "osg/MatrixTransform"
#include "osg/ShapeDrawable"
#include "osg/Shape"
#include "osg/Material"

#include "OsgTools/State/StateSet.h"

using namespace OsgTools::Builders;

Arrow::Arrow() : BaseClass()
{
}


osg::Node* Arrow::operator() () const
{
  osg::Vec3 start ( this->start() );
  osg::Vec3 end ( this->end() );
  osg::Vec4 color ( this->color() );

  osg::Vec3 vector ( start - end );

  double length ( vector.length() );

  double linePercent ( 0.75 );
  double conePercent ( 0.25 );

  osg::Vec3 origin ( 0.0, 0.0, 0.0 );
  osg::Vec3 x  ( length * linePercent, 0.0, 0.0 );

  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  osg::ref_ptr< osg::Geometry > geom ( new osg::Geometry );
  osg::ref_ptr < osg::Vec3Array > verts ( new osg::Vec3Array );
  verts->push_back ( origin );
  verts->push_back ( x );

  osg::ref_ptr< osg::Vec4Array > colors ( new osg::Vec4Array );
  colors->push_back ( color );

  geom->setColorArray( colors.get() );
  geom->setColorBinding ( osg::Geometry::BIND_PER_PRIMITIVE );

  geom->setVertexArray ( verts.get() );

  geom->addPrimitiveSet ( new osg::DrawArrays( osg::PrimitiveSet::LINES, 0, verts->size() ) );

  geode->addDrawable ( geom.get() );

  osg::ref_ptr< osg::StateSet > ss ( geom->getOrCreateStateSet() );

  // Set the state for the lines.
  OsgTools::State::StateSet::setLighting( ss.get(), false );
  OsgTools::State::StateSet::setLineWidth ( ss.get(), this->thickness() );

  // Radius and height for cone.
  const double radius ( 3.0 );
  const double height ( length * conePercent );

  osg::ref_ptr < osg::Cone > cone ( new osg::Cone( osg::Vec3 ( length * linePercent, 0.0, 0.0 ), radius, height ) );
  osg::Quat quat ( osg::inDegrees(90.0), osg::Vec3( 0.0, 1.0, 0.0 )  );

  cone->setRotation( quat );
  osg::ref_ptr< osg::ShapeDrawable > sd ( new osg::ShapeDrawable( cone.get() ) );
  osg::ref_ptr < osg::TessellationHints > hints ( new osg::TessellationHints );
  hints->setDetailRatio ( 0.25f );
  sd->setTessellationHints( hints.get() );

  osg::ref_ptr< osg::Material > mat ( new osg::Material );
  mat->setDiffuse ( osg::Material::FRONT, color );
  sd->getOrCreateStateSet()->setAttribute ( mat.get(), osg::StateAttribute::ON );
  OsgTools::State::StateSet::setLighting( sd->getOrCreateStateSet(), true );

  geode->addDrawable ( sd.get() );

  osg::ref_ptr < osg::MatrixTransform > mt ( new osg::MatrixTransform );

  vector.normalize();

  osg::Matrix matrix;
  matrix.makeRotate ( osg::Vec3 ( 1.0, 0.0, 0.0 ), vector );
  matrix.setTrans ( start );
  mt->setMatrix ( matrix );

  mt->addChild( geode.get() );

  return mt.release();
}

