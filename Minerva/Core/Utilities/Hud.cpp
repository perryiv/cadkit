
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

#include "boost/format.hpp"

using namespace Minerva::Core::Utilities;

///////////////////////////////////////////////////////////////////////////////
//
//  Make a osgText::Text object.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  osgText::Text* makeText ( const osg::Vec3& position )
  {   
#if OPENSCENEGRAPH_MAJOR_VERSION >= 2 && OPENSCENEGRAPH_MINOR_VERSION >= 3
    unsigned int textSize ( 20 );
#else
    unsigned int textSize ( 15 );
#endif
    
    osg::ref_ptr<osgText::Text> text ( new osgText::Text );
    
    // Set the font.
    osg::ref_ptr<osgText::Font> font ( OsgTools::Font::defaultFont() );
    text->setFont ( font.get() );

    // Set text properties.    
    text->setCharacterSizeMode( osgText::Text::OBJECT_COORDS );
    text->setCharacterSize( textSize );
    text->setColor ( osg::Vec4 ( 1.0, 1.0, 1.0, 1.0 ) );
    text->setPosition ( position );
    text->setBackdropColor ( osg::Vec4 ( 0.0, 0.0, 0.0, 1.0 ) );
    text->setBackdropType ( osgText::Text::DROP_SHADOW_BOTTOM_LEFT );
  
    return text.release();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Hud::Hud() :
  _camera ( new osg::Camera ),
  _feedback ( Detail::makeText ( osg::Vec3 ( 5.0, 23, 0.0 ) ) ),
  _position ( Detail::makeText ( osg::Vec3 ( 5.0, 7.5, 0.0 ) ) ),
  _date ( Detail::makeText ( osg::Vec3 ( 5.0, 23, 0.0 ) ) ),
  _eyeAltitudeText ( Detail::makeText ( osg::Vec3 ( 0.0, 0.0, 0.0 ) ) ),
  _latLonHeight(),
  _requests ( 0 ),
  _running (),
  _compass ( new Compass ),
  _flags ( Hud::_ALL ),
  _eyeAltitude ( 0.0 ),
  _metaString()
{
  _camera->setRenderOrder ( osg::Camera::POST_RENDER );
  _camera->setReferenceFrame ( osg::Camera::ABSOLUTE_RF );
  _camera->setClearMask( GL_DEPTH_BUFFER_BIT );
  _camera->setViewMatrix( osg::Matrix::identity() );
  _camera->setComputeNearFarMode ( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
  _camera->setCullingMode ( osg::CullSettings::NO_CULLING );
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

  // Append meta-string to the "feedback" text.
  if ( ( true == this->showMetaString() ) && ( false == _metaString.empty() ) )
  {
    const std::string temp ( out.str() );
    if ( false == temp.empty() )
    {
      out << "\n\n";
    }
    out << _metaString;
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
  
  if ( this->showEyeAltitude() )
  {
    const bool showKilometers ( _eyeAltitude >= 1000.0 );
    const double display ( _eyeAltitude / ( showKilometers ? 1000.0 : 1.0 ) );
    const std::string units ( showKilometers ? "km" : "meters" );
    const std::string format ( showKilometers ? "Eye: %10.0f %s" : "Eye: %6.4f %s" );
    boost::format f ( format );
    f % display % units;
    _eyeAltitudeText->setText ( f.str() );
    _eyeAltitudeText->update();

    osg::BoundingBox bb ( _eyeAltitudeText->computeBound() );
    const float h ( bb.yMax() - bb.yMin() );
    const float w ( bb.xMax() - bb.xMin() );
    
    _eyeAltitudeText->setPosition ( osg::Vec3 ( width - w - 1, h / 2.0, 0 ) );
    geode->addDrawable ( _eyeAltitudeText.get() );
  }
  
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
//  Set the meta-data string.
//
///////////////////////////////////////////////////////////////////////////////

void Hud::metaString ( const std::string &s )
{
  _metaString = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the eye altitude.
//
///////////////////////////////////////////////////////////////////////////////

void Hud::eyeAltitude ( double altitude )
{
  _eyeAltitude = altitude;
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


///////////////////////////////////////////////////////////////////////////////
//
//  Set the eye altitude shown flag.
//
///////////////////////////////////////////////////////////////////////////////

void Hud::showEyeAltitude ( bool b )
{
  _flags = Usul::Bits::set ( _flags, _SHOW_EYE_ALTITUDE, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the eye altitude shown flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Hud::showEyeAltitude() const
{
  return Usul::Bits::has ( _flags, _SHOW_EYE_ALTITUDE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the meta-string shown flag.
//
///////////////////////////////////////////////////////////////////////////////

void Hud::showMetaString ( bool b )
{
  _flags = Usul::Bits::set ( _flags, _SHOW_META_STRING, b );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the meta-string shown flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Hud::showMetaString() const
{
  return Usul::Bits::has ( _flags, _SHOW_META_STRING );
}
