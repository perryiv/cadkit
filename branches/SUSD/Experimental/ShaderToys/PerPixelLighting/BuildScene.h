
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __BUILD_SCENE_H__
#define __BUILD_SCENE_H__

#include "osg/Shape"
#include "osg/ShapeDrawable"
#include "osg/Geode"
#include "osg/Program"
#include "osg/Shader"
#include "osg/MatrixTransform"
#include "osg/Material"

osg::Node* perVertexLighting()
{
  osg::ref_ptr < osg::Sphere > sphere ( new osg::Sphere( osg::Vec3 ( 0.0, 0.0, 0.0 ), 1.0f ) );
  osg::ref_ptr < osg::ShapeDrawable > sd ( new osg::ShapeDrawable ( sphere.get() ) );

  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  geode->addDrawable ( sd.get() );
  return geode.release();
}

osg::Node* perPixelLighting()
{
  osg::ref_ptr < osg::Sphere > sphere ( new osg::Sphere( osg::Vec3 ( 0.0, 0.0, 0.0 ), 1.0f ) );
  osg::ref_ptr < osg::ShapeDrawable > sd ( new osg::ShapeDrawable ( sphere.get() ) );

  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  geode->addDrawable ( sd.get() );

  osg::ref_ptr < osg::StateSet > ss ( geode->getOrCreateStateSet() );
  
  osg::ref_ptr < osg::Program > program ( new osg::Program );
  program->setName ( "PerPixelLighting" );

  osg::ref_ptr < osg::Shader >  vertexShader ( new osg::Shader ( osg::Shader::VERTEX ) );
  vertexShader->setName( "VertexShader" );
  vertexShader->loadShaderSourceFromFile( "PerPixel.vert" );

  osg::ref_ptr < osg::Shader >  fragmentShader ( new osg::Shader ( osg::Shader::FRAGMENT ) );
  fragmentShader->setName( "FragmentShader" );
  fragmentShader->loadShaderSourceFromFile( "PerPixel.frag" );

  program->addShader( vertexShader.get() );
  program->addShader( fragmentShader.get() );
  ss->setAttributeAndModes( program.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );

  return geode.release();
}


osg::Node* buildScene()
{
  osg::ref_ptr< osg::Group > group ( new osg::Group );

  osg::ref_ptr< osg::MatrixTransform > mt1 ( new osg::MatrixTransform );
  osg::Matrix m1 ( osg::Matrix::translate( -5.0, 0.0, 0.0 ) );
  mt1->setMatrix ( m1 );
  //mt1->addChild( perVertexLighting() );

  osg::ref_ptr< osg::MatrixTransform > mt2 ( new osg::MatrixTransform );
  osg::Matrix m2 ( osg::Matrix::translate( 5.0, 0.0, 0.0 ) );
  mt2->setMatrix ( m2 );
  mt2->addChild( perPixelLighting() );

  group->addChild ( mt1.get() );
  group->addChild ( mt2.get() );

  osg::ref_ptr < osg::Material > mat ( new osg::Material );
  mat->setDiffuse ( osg::Material::FRONT, osg::Vec4 ( 1.0, 1.0, 0.0, 1.0 ) );

  osg::ref_ptr < osg::StateSet > ss ( group->getOrCreateStateSet () );
  //ss->setAttributeAndModes ( mat.get(), osg::StateAttribute::ON );

  return group.release();
}

#endif 
