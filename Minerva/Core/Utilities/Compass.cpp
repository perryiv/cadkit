///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Utilities/Compass.h"

#include "Usul/App/Application.h"
#include "Usul/Predicates/FileExists.h"

#include "OsgTools/State/StateSet.h"

#include "osg/MatrixTransform"
#include "osg/Math"
#include "osg/Texture2D"
#include "osg/Image"
#include "osg/Geometry"
#include "osg/Geode"
#include "osg/PolygonMode"

#include "osgDB/ReadFile"

using namespace Minerva::Core::Utilities;


///////////////////////////////////////////////////////////////////////////////
//
//  Build a triangle fan object to use as a compass face
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{

  osg::Geode* buildTriangleFan( osg::Image* image, float zoff, float radius, unsigned int numSlices, unsigned int render_level )
  {
    osg::ref_ptr< osg::StateSet > stateset ( new osg::StateSet );

    if ( 0x0 != image )
    {
      osg::ref_ptr< osg::Texture2D > texture ( new osg::Texture2D ); 
      texture->setImage ( image );
      stateset->setTextureAttributeAndModes ( 0, texture.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );      
    }

    stateset->setMode ( GL_BLEND, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
    stateset->setMode ( GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );    
    stateset->setRenderingHint ( osg::StateSet::TRANSPARENT_BIN );
    stateset->setRenderBinDetails ( render_level, "RenderBin" );

    osg::ref_ptr< osg::Vec3Array > fan ( new osg::Vec3Array );
    osg::ref_ptr< osg::Vec2Array > fan_tex ( new osg::Vec2Array );

    fan->reserve ( numSlices + 1 );
    fan_tex->reserve ( numSlices + 1 );

    fan->push_back( osg::Vec3 ( 0.0f, 0.0f, zoff ) );
    fan_tex->push_back( osg::Vec2 ( 0.5f, 0.5f ) );

    for ( unsigned int x = 0; x < numSlices + 1; ++x )
    {	
      const float angle ( float ( x ) * ( ( 2 * osg::PI ) / numSlices ) );
      fan->push_back ( osg::Vec3 ( radius * ( cos( angle ) ), radius * ( sin ( angle ) ), zoff ) );
      fan_tex->push_back ( osg::Vec2 ( 0.5f + ( 0.5f * ( cos( angle ) ) ), 0.5f + ( 0.5f * ( sin ( angle ) ) ) ) );
    }

    // Make a new geometry.
    osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry ); 

    // Set the colors.
    //geometry->setColorArray ( _colors.get() );
    //geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    // Set the vertices.
    geometry->setVertexArray ( fan.get() );

    // set the texture coordinates.
    geometry->setTexCoordArray ( 0, fan_tex.get() );

    // Set the one normal.
    osg::ref_ptr< osg::Vec3Array > normal ( new osg::Vec3Array );
    normal->push_back ( osg::Vec3 ( 0.0f, 0.0f, 1.0f ) );
    geometry->setNormalArray ( normal.get() );
    geometry->setNormalBinding ( osg::Geometry::BIND_OVERALL );

    // Draw a triangle fan.
    geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::TRIANGLE_FAN, 0, fan->size() ) );

    // Set the state set.
    geometry->setStateSet ( stateset.get() );

    // Make the geode.
    osg::ref_ptr< osg::Geode > geode ( new osg::Geode() );
    geode->addDrawable ( geometry.get() );

    return geode.release();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Compass::Compass() : BaseClass(),
  _pos(),
  _scale ( 1.0f ),
  _ring ( Usul::App::Application::instance().iconDirectory() + "/compassring.png" ),
  _interior ( Usul::App::Application::instance().iconDirectory() + "/compassrose.png" ),
  _numslices ( 32 ),
  _radius ( 50 ),
  _compassRingObject ( new osg::MatrixTransform ),
  _compassInteriorObject ( new osg::MatrixTransform ),
  _heading ( 0.0f ),
  _pitch ( 0.0f),
  _roll( 0.0f ),
  _colors ( new osg::Vec4Array )
{
  _colors->push_back( osg::Vec4 ( 1.0, 1.0, 1.0, 1.0 ) );
  
  osg::ref_ptr<osg::Image> imageRing ( osgDB::readImageFile ( _ring ) );
  osg::ref_ptr<osg::Image> imageInterior ( osgDB::readImageFile ( _interior ) );

  if( true == imageRing.valid() && true == imageInterior.valid() )
  {
    _compassRingObject->addChild ( Detail::buildTriangleFan ( imageRing.get(), 0.0f, _radius, _numslices, 1001 ) );
    _compassInteriorObject->addChild ( Detail::buildTriangleFan ( imageInterior.get(), 0.0f, _radius, _numslices, 1001 ) );
  }
  else
  {
    this->setNodeMask ( 0x0 );
  }

  this->_buildCompass();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Compass::~Compass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Traverse.
//
///////////////////////////////////////////////////////////////////////////////

void Compass::traverse(osg::NodeVisitor& nv)
{
  BaseClass::traverse ( nv );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the heading of the compass.
//
///////////////////////////////////////////////////////////////////////////////

float Compass::heading() const
{
  return _heading;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the heading of the compass.
//
///////////////////////////////////////////////////////////////////////////////

void Compass::heading( float r )
{
  _heading = r;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the pitch of the compass.
//
///////////////////////////////////////////////////////////////////////////////

float Compass::pitch() const
{
   return _pitch;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the pitch of the compass.
//
///////////////////////////////////////////////////////////////////////////////

void Compass::pitch ( float p )
{
   _pitch = p;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the roll of the compass.
//
///////////////////////////////////////////////////////////////////////////////

float Compass::roll() const
{
   return _roll;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the roll of the compass.
//
///////////////////////////////////////////////////////////////////////////////

void Compass::roll(float r)
{
   _roll = r;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the position on the screen of the compass
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec3& Compass::position() const
{
  return _pos;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the position on the screen of the compass by absolute postion
//
///////////////////////////////////////////////////////////////////////////////

void Compass::position( const osg::Vec3& p )
{
  _pos = p;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the xyz scale value of the compass
//
///////////////////////////////////////////////////////////////////////////////

float Compass::scale() const
{
  return _scale;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the xyz scale value of the compass
//
///////////////////////////////////////////////////////////////////////////////

void Compass::scale(float s)
{
  _scale = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Rebuilds compass based upon animation show/hide parameters.
//	Call this method to rebuild after changing parameters in a set method
//
///////////////////////////////////////////////////////////////////////////////

void Compass::updateCompass()
{
  const double h ( this->heading() );
  const double p ( this->pitch() );
  const double r ( this->roll() );
  
  if ( _compassRingObject.valid () )
  {
    _compassRingObject->setMatrix ( osg::Matrix::scale ( this->scale(),
                                       this->scale(),
                                       this->scale()) * 
                 osg::Matrix::rotate ( osg::inDegrees ( h ),
                                      0.0f, 0.0f, 1.0f ) *
                 osg::Matrix::translate ( this->position() ) );
  }
  
#if 1
  if ( _compassInteriorObject.valid () )
  {
    _compassInteriorObject->setMatrix ( osg::Matrix::scale ( this->scale(),
                                                          this->scale(),
                                                          this->scale()) * 
                                         osg::Matrix::rotate ( osg::inDegrees ( 0.0 ),
                                                              0.0f, 0.0f, 1.0f ) *
                                     osg::Matrix::rotate ( osg::inDegrees ( p ),
                                                          1.0f, 0.0f, 0.0f ) *
                                     osg::Matrix::rotate ( osg::inDegrees ( r ),
                                                          0.0f, 0.0f, 1.0f ) * // make it about z as well
                                     osg::Matrix::translate ( this->position() ) );
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the compass.
//
///////////////////////////////////////////////////////////////////////////////

void Compass::_buildCompass()
{
  // Get the state set.
  osg::ref_ptr < osg::StateSet > ss ( this->getOrCreateStateSet() );
  ss->setRenderBinDetails ( 1000, "RenderBin" );
  ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );

  // Ensure it's always drawed filled.
  osg::ref_ptr<osg::PolygonMode> mode ( new osg::PolygonMode ( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL ) );
  ss->setAttributeAndModes ( mode.get(), osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );
  
  // Turn off lighting.
  OsgTools::State::StateSet::setLighting ( this, false );
  
  // Remove what we have.
  this->removeChildren ( 0, this->getNumChildren() ); 
  
  // Add the compass.
  osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
  mt->setReferenceFrame ( osg::Transform::ABSOLUTE_RF );
  
  mt->addChild ( _compassInteriorObject.get() );
  mt->addChild ( _compassRingObject.get() );
  
  this->addChild ( mt.get() );
}
