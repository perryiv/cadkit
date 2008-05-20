
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Geometry/Line.h"
#include "Minerva/Core/Utilities/Resample.h"

#include "OsgTools/State/StateSet.h"
#include "OsgTools/Font.h"
#include "OsgTools/Utilities/TranslateGeometry.h"

#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IProjectCoordinates.h"
#include "Usul/Interfaces/IElevationDatabase.h"
#include "Usul/Interfaces/IPlanetCoordinates.h"

#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Depth"
#include "osg/Material"
#include "osg/MatrixTransform"
#include "osg/Version"

using namespace Minerva::Core::Geometry;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS( Line, Line::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Line::Line() : BaseClass(),
  _line (),
  _latLongPoints (),
  _width ( 1.0 ),
  _tessellate ( false )
{
  // Default render bin.
  this->renderBin ( 3 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Line::~Line()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert to lat long points.
//
///////////////////////////////////////////////////////////////////////////////

void Line::_convertToLatLong ( const Vertices& vertices, Vertices& latLongPoints )
{
  Usul::Interfaces::IProjectCoordinates::QueryPtr project ( Usul::Components::Manager::instance().getInterface( Usul::Interfaces::IProjectCoordinates::IID ) );

  if ( project.valid() )
  {
    latLongPoints.reserve ( vertices.size() );
    for( Vertices::const_iterator iter = vertices.begin(); iter != vertices.end(); ++iter )
    {
      Usul::Math::Vec3d point;
      project->projectToSpherical( *iter, this->srid(), point );     
      latLongPoints.push_back( point );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the lat long points.
//
///////////////////////////////////////////////////////////////////////////////

void Line::_buildLatLongPoints()
{
  Guard guard ( this );
  _latLongPoints.clear ();
  this->_convertToLatLong( _line, _latLongPoints );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the line data.
//
///////////////////////////////////////////////////////////////////////////////

const Line::Vertices& Line::lineData()
{
  Guard guard ( this->mutex() );
  if( _latLongPoints.empty() )
  {
    this->_convertToLatLong ( _line, _latLongPoints );
  }

  return _latLongPoints;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* Line::queryInterface( unsigned long iid )
{
  switch ( iid )
  {
  case Minerva::Interfaces::ILineData::IID:
    return static_cast < Minerva::Interfaces::ILineData* > ( this );
  default:
    return BaseClass::queryInterface( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the line data.
//
///////////////////////////////////////////////////////////////////////////////

void Line::line( const Vertices& data )
{
  Guard guard ( this->mutex() );
  _line = data;
  this->dirty( true );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the line data.
//
///////////////////////////////////////////////////////////////////////////////

const Line::Vertices& Line::line() const
{
  Guard guard ( this->mutex() );
  return _line;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the width.
//
///////////////////////////////////////////////////////////////////////////////

float Line::width () const
{
  Guard guard ( this );
  return _width;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the width.
//
///////////////////////////////////////////////////////////////////////////////

void Line::width ( float width )
{
  Guard guard ( this );
  _width = width;
  this->dirty( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene branch for the data object.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Line::_buildScene( Usul::Interfaces::IUnknown* caller )
{
  return this->_buildScene ( this->color(), caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene branch for the data object.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Line::_buildScene( const osg::Vec4& color, Usul::Interfaces::IUnknown* caller )
{
  Guard guard ( this );
  osg::ref_ptr< osg::Group > node ( new osg::Group );
  
  osg::ref_ptr < osg::StateSet > ss ( node->getOrCreateStateSet() );
    
  // Query for needed interfaces.
  Usul::Interfaces::IElevationDatabase::QueryPtr elevation ( caller );
  Usul::Interfaces::IPlanetCoordinates::QueryPtr planet ( caller );
    
  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  node->addChild( geode.get() );
  
  // Make new extents.
  Extents e;
  
  // Get the line data.
  Vertices data ( this->lineData() );
  
  Vertices sampledPoints;
  if ( this->tessellate() && Geometry::CLAMP_TO_GROUND == this->altitudeMode() )
    Minerva::Core::Utilities::resample( data, sampledPoints, 5, caller );
  else
    sampledPoints = data;
  
  // Make the osg::Vec3Array.
  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
  vertices->reserve ( sampledPoints.size() );
  
  if ( planet.valid() )
  {
    for ( Vertices::const_iterator iter = sampledPoints.begin(); iter != sampledPoints.end(); ++iter )
    {
      Vertices::value_type v ( *iter );
      
      // Expand the extents.
      e.expand ( Extents::Vertex ( v[0], v[1] ) );
      
      // Get the height.
      v[2] = this->_elevation ( v, elevation.get() );
      
      Usul::Math::Vec3d point;
      planet->convertToPlanet ( v, point );
      vertices->push_back ( osg::Vec3 ( point[0], point[1], point[2] ) );
    }
  }
  
  // Create the geometry
  osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );
  
  // Add the primitive set
  geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, 0, vertices->size() ) );
  
  // Set the vertices.
  geometry->setVertexArray( vertices.get() );
  
  // Set the colors.
  osg::ref_ptr < osg::Vec4Array > colors ( new osg::Vec4Array );
  colors->push_back( color );
  geometry->setColorArray( colors.get() );
  geometry->setColorBinding( osg::Geometry::BIND_OVERALL );
  
  geode->addDrawable ( geometry.get() );
  
  // Turn off lighting.
  OsgTools::State::StateSet::setLighting  ( ss.get(), false );
  OsgTools::State::StateSet::setLineWidth ( ss.get(), _width );
  
  // Set depth parameters.
  osg::ref_ptr<osg::Depth> depth ( new osg::Depth ( osg::Depth::LEQUAL, 0.0, 1.0, false ) );
  ss->setAttributeAndModes ( depth.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );
  
  ss->setRenderBinDetails( this->renderBin(), "RenderBin" );
  
  this->extents ( e );
  
  osg::Vec3 offset ( node->getBound().center() );
  osg::ref_ptr<OsgTools::Utilities::TranslateGeometry> tg ( new OsgTools::Utilities::TranslateGeometry ( offset ) );
  node->accept ( *tg );
  
  osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
  mt->setMatrix ( osg::Matrix::translate ( offset ) );
  mt->addChild ( node.get() );
  
  return mt.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set tessellate flag.
//
///////////////////////////////////////////////////////////////////////////////

void Line::tessellate ( bool b )
{
  Guard guard ( this->mutex() );
  _tessellate = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get tessellate flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Line::tessellate() const
{
  Guard guard ( this->mutex() );
  return _tessellate;
}
