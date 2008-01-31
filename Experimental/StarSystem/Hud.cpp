
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

using namespace StarSystem;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Hud::Hud() :
_camera ( new osg::Camera ),
_feedback ( new osgText::Text ),
_position ( new osgText::Text ),
_latLonHeight(),
_requests ( 0 ),
_running ( 0 )
{
  _camera->setRenderOrder ( osg::Camera::POST_RENDER );
  _camera->setReferenceFrame ( osg::Camera::ABSOLUTE_RF );
  _camera->setClearMask( GL_DEPTH_BUFFER_BIT );
  _camera->setViewMatrix( osg::Matrix::identity() );
  
#ifndef _MSC_VER
  osg::ref_ptr<osgText::Font> font ( OsgTools::Font::defaultFont() );
  _feedback->setFont ( font.get() );
  _position->setFont ( font.get() );
#endif
  _position->setCharacterSizeMode( osgText::Text::OBJECT_COORDS );
  _position->setCharacterSize( 15 );
  _position->setColor ( osg::Vec4 ( 1.0, 1.0, 1.0, 1.0 ) );
  _position->setPosition ( osg::Vec3 ( 5.0, 7.5, 0.0 ) );
  
  _feedback->setCharacterSizeMode( osgText::Text::OBJECT_COORDS );
  _feedback->setCharacterSize( 15 );
  _feedback->setColor ( osg::Vec4 ( 1.0, 1.0, 1.0, 1.0 ) );
  _feedback->setPosition ( osg::Vec3 ( 5.0, 23, 0.0 ) );
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
  _camera->setProjectionMatrixAsOrtho ( 0, width, 0, height, -10.0, 10.0 );
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
  
  _feedback->setText ( out );
  _feedback->update();
  
  _position->setText ( Usul::Strings::format ( "Lat: ", _latLonHeight[1], " Lon: ", _latLonHeight[0], " E: ", _latLonHeight[2] ) );
  _position->update();
  
  geode->addDrawable ( _feedback.get() );
  geode->addDrawable ( _position.get() );
  
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


///////////////////////////////////////////////////////////////////////////////
//
//  Set the pointer position.
//
///////////////////////////////////////////////////////////////////////////////

void Hud::position ( double lat, double lon, double height )
{
  _latLonHeight[0] = lon;
  _latLonHeight[1] = lat;
  _latLonHeight[2] = height;
}
