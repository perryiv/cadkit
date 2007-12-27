
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Hud for the StarSystem.
//
///////////////////////////////////////////////////////////////////////////////

#include "StarSystem/Hud.h"

#include "Usul/Strings/Format.h"

#include "OsgTools/Font.h"

#include "osg/Geode"
#include "osg/Geometry"

#include "osgText/Text"

using namespace StarSystem;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Hud::Hud() :
_camera ( new osg::Camera ),
_requests ( 0 ),
_running ( 0 )
{
  _camera->setRenderOrder ( osg::Camera::POST_RENDER );
  _camera->setReferenceFrame ( osg::Camera::ABSOLUTE_RF );
  _camera->setClearMask( GL_DEPTH_BUFFER_BIT );
  _camera->setViewMatrix( osg::Matrix::identity() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Hud::buildScene()
{
  return _camera.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Hud::updateScene ( unsigned int width, unsigned int height )
{
  _camera->setViewport ( 0, 0, width, height );
  _camera->setProjectionMatrixAsOrtho2D ( 0, width, 0, height );
  _camera->removeChild ( 0, _camera->getNumChildren() );
  
  osg::ref_ptr<osg::Vec3Array> vertices ( new osg::Vec3Array );
  vertices->push_back ( osg::Vec3 ( 0.0,   0.0,  -1.0 ) );
  vertices->push_back ( osg::Vec3 ( width, 0.0,  -1.0 ) );
  vertices->push_back ( osg::Vec3 ( width, 20.0, -1.0 ) );
  vertices->push_back ( osg::Vec3 ( 0.0,   20.0, -1.0 ) );
  
  osg::ref_ptr<osg::Vec4Array> colors ( new osg::Vec4Array );
  colors->push_back ( osg::Vec4 ( 0.0, 0.0, 0.6, 0.4 ) );
  
  osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );
  geometry->setVertexArray ( vertices.get() );
  
  geometry->setColorArray ( colors.get() );
  geometry->setColorBinding ( osg::Geometry::BIND_OVERALL );
  
  geometry->addPrimitiveSet ( new osg::DrawArrays ( GL_QUADS, 0, vertices->size() ) );
  
  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  //geode->addDrawable ( geometry.get() );
  
  const unsigned int queued    ( this->requests() );
  const unsigned int executing ( this->running() );
  const unsigned int total     ( queued + executing );
  
  const std::string out ( ( total > 0 ) ? ( Usul::Strings::format ( "Queued: ", queued, ", Running: ", executing ) ) : "" );
  
  osg::ref_ptr < osgText::Text > text ( new osgText::Text );
  text->setText ( out );
  text->setPosition ( osg::Vec3 ( 5.0, 7.5, 0.0 ) );
  text->setFont ( OsgTools::Font::defaultFont() );
  text->setCharacterSizeMode( osgText::Text::OBJECT_COORDS );
  text->setCharacterSize( 15 );
  text->setColor ( osg::Vec4 ( 1.0, 1.0, 1.0, 1.0 ) );
  
  geode->addDrawable ( text.get() );
  
  // Turn off lighting.
  geode->getOrCreateStateSet()->setMode ( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );
  geode->getOrCreateStateSet()->setMode ( GL_BLEND, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
  
  _camera->addChild ( geode.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of requests.
//
///////////////////////////////////////////////////////////////////////////////

void Hud::requests ( unsigned int requests )
{
  _requests = requests;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of requests.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Hud::requests() const
{
  return _requests;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of running tasks.
//
///////////////////////////////////////////////////////////////////////////////

void Hud::running ( unsigned int running )
{
  _running = running;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of running tasks.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Hud::running() const
{
  return _running;
}
