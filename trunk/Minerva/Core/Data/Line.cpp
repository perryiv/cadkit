
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Data/Line.h"
#include "Minerva/Core/Algorithms/Resample.h"
#include "Minerva/Core/Data/Transform.h"

#include "OsgTools/Configure/OSG.h"
#include "OsgTools/State/StateSet.h"
#include "OsgTools/Font.h"
#include "OsgTools/Utilities/TranslateGeometry.h"

#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IElevationDatabase.h"
#include "Usul/Interfaces/IPlanetCoordinates.h"

#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Depth"
#include "osg/Material"
#include "osg/MatrixTransform"
#include "osg/Version"

using namespace Minerva::Core::Data;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Line::Line() : BaseClass(),
  _line (),
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
//  Return the line data.
//
///////////////////////////////////////////////////////////////////////////////

Line::Vertices Line::_lineDataWgs84() const
{
  Guard guard ( this->mutex() );
  return _line;
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

osg::Node* Line::_buildScene( const Color& color, Usul::Interfaces::IUnknown* caller )
{
  //Guard guard ( this ); Was causing deadlock!

  osg::ref_ptr < osg::Group > node ( new osg::Group );
  osg::ref_ptr < osg::StateSet > ss ( node->getOrCreateStateSet() );
    
  // Query for needed interfaces.
  Usul::Interfaces::IElevationDatabase::QueryPtr elevation ( caller );
  Usul::Interfaces::IPlanetCoordinates::QueryPtr planet ( caller );
    
  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  node->addChild( geode.get() );
  
  // Make new extents.
  Extents e;
  
  // Get the line data.
  Vertices data ( this->_lineDataWgs84() );
  
  Vertices sampledPoints;
  if ( this->tessellate() && Geometry::CLAMP_TO_GROUND == this->altitudeMode() )
    Minerva::Core::Algorithms::resample( data, sampledPoints, 5, caller );
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
  colors->push_back( osg::Vec4 ( color[0], color[1], color[2], color[3] ) );
  geometry->setColorArray( colors.get() );
  geometry->setColorBinding( osg::Geometry::BIND_OVERALL );
  
  geode->addDrawable ( geometry.get() );
  
  // Turn off lighting.
  OsgTools::State::StateSet::setLighting  ( ss.get(), false );
  OsgTools::State::StateSet::setLineWidth ( ss.get(), this->width() );
  
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
  this->dirty ( true );
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
