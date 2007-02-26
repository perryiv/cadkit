
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

#include "LightShaders.h"

#include "Usul/File/Contents.h"
#include "Usul/Adaptors/Random.h"

#include "osg/Shape"
#include "osg/ShapeDrawable"
#include "osg/Geode"
#include "osg/Program"
#include "osg/Shader"

class Callback : public osg::NodeCallback
{
public:

  Callback() : osg::NodeCallback (), _random ( 0.0, 1.0 ), _param ( 0.0 ), _lastTime ( 0 ), _speed ( 0.5 )
  {
  }

  virtual void operator()( osg::Node* node, osg::NodeVisitor* nv)
  { 
    if( nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR )
    {
      const osg::FrameStamp *fs = nv->getFrameStamp();

      double duration ( fs->getReferenceTime() - _lastTime );

      if( duration > _speed )
      {
        osg::StateSet *ss = node->getOrCreateStateSet();
        osg::Uniform* uniform = ss->getOrCreateUniform("param", osg::Uniform::FLOAT);

        _param += .1;
        if( _param > 1.0 )
          _param = 0.0;
        
        //uniform->set( _random() );
        uniform->set( _param );

        _lastTime = fs->getReferenceTime();
      }
    }

    traverse(node,nv);
  }

private:
  Usul::Adaptors::Random< float > _random;
  float _param;
  double _lastTime;
  double _speed;
};

osg::Node* buildScene()
{
  osg::ref_ptr < osg::Sphere > sphere ( new osg::Sphere( osg::Vec3 ( 0.0, 0.0, 0.0 ), 1.0f ) );
  osg::ref_ptr < osg::ShapeDrawable > sd ( new osg::ShapeDrawable ( sphere.get() ) );

  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  geode->addDrawable ( sd.get() );

  osg::ref_ptr < osg::StateSet > ss ( geode->getOrCreateStateSet() );
  
  osg::ref_ptr < osg::Program > program ( new osg::Program );
  program->setName ( "Animate" );

  std::string contents;
  
  Usul::File::contents ( "Animate.vert", true, contents );

  std::string source;

  source.insert( source.size(), directionalLightVertSource );
  source.insert( source.size(), pointLightVertSource );
  source.insert( source.size(), spotLightVertSource );
  source.insert( source.end(), contents.begin(), contents.end() );

  source.resize( source.size() + 1 );
  source[ source.size() - 1 ] = '\0';

  osg::ref_ptr < osg::Shader >  vertexShader ( new osg::Shader ( osg::Shader::VERTEX, source ) );
  vertexShader->setName( "Animate_Vertex_Shader" );

  osg::ref_ptr < osg::Shader >  fragmentShader ( new osg::Shader ( osg::Shader::FRAGMENT ) );
  fragmentShader->setName( "Animate_Fragment_Shader" );
  fragmentShader->loadShaderSourceFromFile( "Animate.frag" );

  program->addShader( vertexShader.get() );
  program->addShader( fragmentShader.get() );
  ss->setAttributeAndModes( program.get(), osg::StateAttribute::ON );

  {
    osg::Uniform* uniform = ss->getOrCreateUniform("param", osg::Uniform::FLOAT);
    uniform->set( 0.0f );
  }

  geode->setUpdateCallback( new Callback );

  return geode.release();
}

#endif // __BUILD_SCENE_H__
