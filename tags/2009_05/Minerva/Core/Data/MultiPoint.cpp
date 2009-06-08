
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Data/MultiPoint.h"

#include "OsgTools/State/StateSet.h"
#include "OsgTools/Convert.h"

#include "Usul/Interfaces/IElevationDatabase.h"
#include "Usul/Interfaces/IPlanetCoordinates.h"
#include "Usul/Threads/Safe.h"
#include "Usul/Trace/Trace.h"

#include "osg/Geometry"
#include "osg/Point"
#include "osg/Geode"
#include "osg/MatrixTransform"

using namespace Minerva::Core::Data;

USUL_IMPLEMENT_TYPE_ID ( MultiPoint );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MultiPoint::MultiPoint() : BaseClass(),
  _points(),
  _size ( 1.0 ),
  _color ( 0.0, 0.0, 0.0, 1.0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MultiPoint::~MultiPoint()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append a single point.
//
///////////////////////////////////////////////////////////////////////////////

void MultiPoint::append ( const Vertex& point )
{
  Guard guard ( this->mutex() );
  _points.push_back ( point );
  
  // Dirty.
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append multiple points.
//
///////////////////////////////////////////////////////////////////////////////

void MultiPoint::append ( const Vertices& v )
{
  Guard guard ( this->mutex() );
  _points.insert ( _points.end(), v.begin(), v.end() );
  
  // Dirty.
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the point.
//
///////////////////////////////////////////////////////////////////////////////

void MultiPoint::points ( const Vertices& points )
{
  Guard guard ( this->mutex() );
  _points = points;
  
  // Dirty.
  this->dirty( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the point.
//
///////////////////////////////////////////////////////////////////////////////

MultiPoint::Vertices MultiPoint::points() const
{
  Guard guard ( this->mutex() );
  return _points;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the size.
//
///////////////////////////////////////////////////////////////////////////////

float MultiPoint::size() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

void MultiPoint::size ( float size )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _size = size;
  this->dirty( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene branch for the data object.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* MultiPoint::_buildScene ( Usul::Interfaces::IUnknown * caller )
{
  USUL_TRACE_SCOPE;
  
  // Get the point data.
  Vertices data ( this->points() );

  if ( true == data.empty() )
    return 0x0;
  
  // Query for needed interfaces.
  Usul::Interfaces::IElevationDatabase::QueryPtr elevation ( caller );
  Usul::Interfaces::IPlanetCoordinates::QueryPtr planet ( caller );
  
  // Store the converted values here.
  Vertices convertedPoints;
  convertedPoints.reserve ( data.size() );
  
  // Make new extents.
  Extents e;
  
  if ( planet.valid() )
  {
    for ( Vertices::const_iterator iter = data.begin(); iter != data.end(); ++iter )
    {
      Vertices::value_type v ( *iter );
      
      // Expand the extents.
      e.expand ( Extents::Vertex ( v[0], v[1] ) );
      
      // Get the height.
      v[2] = this->_elevation ( v, elevation.get() );
      
      Usul::Math::Vec3d point;
      planet->convertToPlanet ( v, point );
      convertedPoints.push_back ( point );
    }
  }
  else
  {
    convertedPoints.swap ( data );
  }
  
  // Set our new extents.
  this->extents ( e );
  
  // Make the osg::Vec3Array.
  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
  vertices->reserve ( data.size() );
  
  // Move all the points so that the first point starts at (0,0,0).
  Vertices::value_type offset ( convertedPoints.at ( 0 ) );
  for ( Vertices::const_iterator iter = convertedPoints.begin(); iter != convertedPoints.end(); ++iter )
  {
    Vertices::value_type v ( *iter );
    Vertices::value_type point ( v - offset );
    vertices->push_back ( osg::Vec3f ( point[0], point[1], point[2] ) );
  }
    
  // Make the geomtry.
  osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );
  
  geometry->setVertexArray( vertices.get() );
  
  osg::ref_ptr < osg::Vec4Array > colors ( new osg::Vec4Array ( data.size() ) );
  osg::Vec4f color ( Usul::Convert::Type<Color,osg::Vec4f>::convert ( this->color() ) );
  std::fill ( colors->begin(), colors->end(), color );
  geometry->setColorArray ( colors.get() );
  geometry->setColorBinding ( osg::Geometry::BIND_PER_VERTEX );
  
  osg::ref_ptr < osg::Vec3Array > normals ( new osg::Vec3Array ( data.size() ) );
  std::fill ( normals->begin(), normals->end(), osg::Vec3 ( 0.0f,-1.0f,0.0f ) );
  geometry->setNormalArray( normals.get() );
  geometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );
  
  osg::ref_ptr < osg::Point > ps ( new osg::Point );
  ps->setSize( Usul::Threads::Safe::get ( this->mutex(), _size ) );
	
  osg::ref_ptr < osg::StateSet > ss ( geometry->getOrCreateStateSet() );
  ss->setAttributeAndModes( ps.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );
  
  // Turn off lighting.
  OsgTools::State::StateSet::setLighting ( ss.get(), false );
  
  geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::POINTS, 0, vertices->size() ) );
  
  // Make the geode.
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  geode->addDrawable ( geometry.get() );
  
  osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
  mt->setMatrix ( osg::Matrixd::translate ( offset[0], offset[1], offset[2] ) );
  mt->addChild ( geode.get() );
  
  // Set the render bin.
  ss->setRenderBinDetails ( this->renderBin(), "RenderBin" );
  
  // Return the MatixTransform.
  return mt.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the color.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec4f MultiPoint::color() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _color;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the color.
//
///////////////////////////////////////////////////////////////////////////////

void MultiPoint::color ( const Color& color )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  
  if ( false == color.equal ( _color ) )
  {
    // Set the internal color.
    _color = color;
    this->dirty( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this geometry transparent?
//
///////////////////////////////////////////////////////////////////////////////

bool MultiPoint::isSemiTransparent() const
{
  USUL_TRACE_SCOPE;
  return 1.0 != this->color()[3];
}
