
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
//  Hud for the Minerva.
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Utilities/Hud.h"

#include "Usul/Bits/Bits.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Math/NaN.h"
#include "Usul/Strings/Format.h"

#include "OsgTools/Font.h"

#include "osg/Geode"
#include "osg/Geometry"
#include "osg/MatrixTransform"
#include "osg/Version"

using namespace Minerva::Core::Utilities;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Hud::Hud() :
  _camera ( new osg::Camera ),
  _feedback ( new osgText::Text ),
  _position ( new osgText::Text ),
  _date ( new osgText::Text ),
  _latLonHeight(),
  _requests ( 0 ),
  _running (),
  _compass ( new Compass ),
  _flags ( Hud::_ALL )
{
  _camera->setRenderOrder ( osg::Camera::POST_RENDER );
  _camera->setReferenceFrame ( osg::Camera::ABSOLUTE_RF );
  _camera->setClearMask( GL_DEPTH_BUFFER_BIT );
  _camera->setViewMatrix( osg::Matrix::identity() );
  _camera->setComputeNearFarMode ( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
  _camera->setCullingMode ( osg::CullSettings::NO_CULLING );
  
#ifndef _MSC_VER
  osg::ref_ptr<osgText::Font> font ( OsgTools::Font::defaultFont() );
  _feedback->setFont ( font.get() );
  _position->setFont ( font.get() );
  _date->setFont( font.get() );
#endif

#if OPENSCENEGRAPH_MAJOR_VERSION >= 2 && OPENSCENEGRAPH_MINOR_VERSION >= 3
	unsigned int textSize ( 25 );
#else
	unsigned int textSize ( 15 );
#endif

  _position->setCharacterSizeMode( osgText::Text::OBJECT_COORDS );
  _position->setCharacterSize( textSize );
  _position->setColor ( osg::Vec4 ( 1.0, 1.0, 1.0, 1.0 ) );
  _position->setPosition ( osg::Vec3 ( 5.0, 7.5, 0.0 ) );
  _position->setBackdropColor ( osg::Vec4 ( 0.0, 0.0, 0.0, 1.0 ) );
  _position->setBackdropType ( osgText::Text::DROP_SHADOW_BOTTOM_LEFT );
  
  _feedback->setCharacterSizeMode( osgText::Text::OBJECT_COORDS );
  _feedback->setCharacterSize( textSize );
  _feedback->setColor ( osg::Vec4 ( 1.0, 1.0, 1.0, 1.0 ) );
  _feedback->setPosition ( osg::Vec3 ( 5.0, 23, 0.0 ) );
  _feedback->setBackdropColor ( osg::Vec4 ( 0.0, 0.0, 0.0, 1.0 ) );
  _feedback->setBackdropType ( osgText::Text::DROP_SHADOW_BOTTOM_LEFT );
  _feedback->setLineSpacing ( 0.1 ); // Doesn't seem to have any effect.

  _date->setCharacterSizeMode( osgText::Text::SCREEN_COORDS );
  _date->setCharacterSize( textSize * 2 );
  _date->setColor ( osg::Vec4 ( 0.0, 0.0, 0.0, 1.0 ) );
  _date->setPosition ( osg::Vec3 ( 5.0, 23, 0.0 ) );
  _date->setBackdropColor ( osg::Vec4 ( 1.0, 1.0, 1.0, 1.0 ) );
  _date->setBackdropType ( osgText::Text::DROP_SHADOW_BOTTOM_LEFT );
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
  // Set the viewport and projection matrix.
  _camera->setViewport ( 0, 0, width, height );
  _camera->setProjectionMatrixAsOrtho ( 0, width, 0, height, -1000.0, 1000.0 );
  
  // Clear all children.
  _camera->removeChild ( 0, _camera->getNumChildren() );

  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  
#if 0
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

  geode->addDrawable ( geometry.get() );
#endif

  // Set the text's position.
  _date->setPosition     ( osg::Vec3 ( 5.0, height - 25.0, 0.0 ) );
  _feedback->setPosition ( osg::Vec3 ( 5.0, height - 50.0, 0.0 ) );
  
  const unsigned int queued ( this->requests() );
  const Strings executing   ( this->running() );
  const unsigned int total  ( queued + executing.size() );
  
  std::ostringstream out;
  if ( ( total > 0 ) )
  {
    out << "Queued: " << queued << ", Running: " << executing.size();
    for ( Strings::const_iterator i = executing.begin(); i != executing.end(); ++i )
      out << '\n' << *i;
  }
  _feedback->setText ( out.str() );
  
  typedef Usul::Convert::Type<double,std::string> ToString;
  _position->setText ( Usul::Strings::format ( "Lat: ", ToString::convert ( _latLonHeight[1] ), 
                                               " Lon: ", ToString::convert ( _latLonHeight[0] ), 
                                               " E: ", ToString::convert (  _latLonHeight[2] ) ) );

  // Update osgText objects.
  _feedback->update();
  _position->update();
  _date->update();
  
  const bool positionValid ( false == Usul::Math::nan ( _latLonHeight[0] ) && 
                             false == Usul::Math::nan ( _latLonHeight[1] ) && 
                             false == Usul::Math::nan ( _latLonHeight[2] ) );
  
  if ( this->showJobFeedback() )
    geode->addDrawable ( _feedback.get() );
  
  if ( this->showPointerPosition() && positionValid )
    geode->addDrawable ( _position.get() );

  if ( this->showDateFeedback() )
    geode->addDrawable ( _date.get() );
  
  // Turn off lighting.
  geode->getOrCreateStateSet()->setMode ( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );
  geode->getOrCreateStateSet()->setMode ( GL_BLEND, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
  
  _camera->addChild ( geode.get() );
  
  if ( this->showCompass() )
  {
    _compass->position ( osg::Vec3 ( width * 0.9, height * 0.9, 0.0 ) );
    _compass->updateCompass();
    _camera->addChild ( _compass.get() );
  }
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

void Hud::running ( const Strings &running )
{
  _running = running;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of running tasks.
//
///////////////////////////////////////////////////////////////////////////////

Hud::Strings Hud::running() const
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


///////////////////////////////////////////////////////////////////////////////
//
//  Set the compass shown flag.
//
///////////////////////////////////////////////////////////////////////////////

void Hud::showCompass ( bool b )
{
  _flags = Usul::Bits::set ( _flags, _SHOW_COMPASS, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the compass shown flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Hud::showCompass() const
{
  return Usul::Bits::has ( _flags, _SHOW_COMPASS );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the pointer position shown flag.
//
///////////////////////////////////////////////////////////////////////////////

void Hud::showPointerPosition ( bool b )
{
  _flags = Usul::Bits::set ( _flags, _SHOW_POINTER_POSITION, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the pointer position shown flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Hud::showPointerPosition() const
{
  return Usul::Bits::has ( _flags, _SHOW_POINTER_POSITION );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the queued jobs shown flag.
//
///////////////////////////////////////////////////////////////////////////////

void Hud::showJobFeedback ( bool b )
{
  _flags = Usul::Bits::set ( _flags, _SHOW_JOB_FEEDBACK, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the queued jobs shown flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Hud::showJobFeedback() const
{
  return Usul::Bits::has ( _flags, _SHOW_JOB_FEEDBACK );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set heading, pitch, roll.
//
///////////////////////////////////////////////////////////////////////////////

void Hud::hpr ( double heading, double pitch, double roll )
{
  _compass->heading ( heading );
  _compass->pitch( pitch );
  _compass->roll ( roll );
}



///////////////////////////////////////////////////////////////////////////////
//
//  Set the date feedback shown flag.
//
///////////////////////////////////////////////////////////////////////////////

void Hud::showDateFeedback ( bool b )
{
  _flags = Usul::Bits::set ( _flags, _SHOW_DATE_FEEDBACK, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the date feedback shown flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Hud::showDateFeedback() const
{
  return Usul::Bits::has ( _flags, _SHOW_DATE_FEEDBACK );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the date text.
//
///////////////////////////////////////////////////////////////////////////////

void Hud::dateFeedback ( const std::string& text )
{
  if ( _date.valid() )
    _date->setText ( text );
}
