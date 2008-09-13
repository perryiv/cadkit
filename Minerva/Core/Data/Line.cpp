
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

#include "OsgTools/Configure/OSG.h"
#include "OsgTools/State/StateSet.h"
#include "OsgTools/Font.h"
#include "OsgTools/Utilities/TranslateGeometry.h"

#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IElevationDatabase.h"
#include "Usul/Interfaces/IPlanetCoordinates.h"
#include "Usul/Threads/Safe.h"

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
  _vertices ( 0x0 ),
  _tessellate ( false ),
  _lineStyle ( 0x0 )
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
//  Set the line data.
//
///////////////////////////////////////////////////////////////////////////////

void Line::line ( const Vertices& data )
{
  Guard guard ( this->mutex() );
  
  _vertices = new Vertex3Array ( data );
  
  //_line = data;
  this->dirty( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the line data.
//
///////////////////////////////////////////////////////////////////////////////

Line::Vertex3Array::RefPtr Line::line() const
{
  Guard guard ( this->mutex() );
  return _vertices;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the width.
//
///////////////////////////////////////////////////////////////////////////////

float Line::width () const
{
  LineStyle::RefPtr style ( this->lineStyle() );
  return ( style.valid() ? style->width() : 1.0f );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene branch for the data object.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Line::_buildScene( Usul::Interfaces::IUnknown* caller )
{
  return this->_buildScene ( this->lineColor(), caller );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene branch for the data object.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Line::_buildScene ( const Color& color, Usul::Interfaces::IUnknown* caller )
{
  //Guard guard ( this ); Was causing deadlock!

  Vertex3Array::RefPtr data ( Usul::Threads::Safe::get ( this->mutex(), _vertices ) );

  // Get the line data.
  //Vertices data ( this->line() );

  // Make sure there are at least 2 points.
  if ( false == data.valid() || data->size() < 2 )
    return 0x0;

  // Query for needed interfaces.
  Usul::Interfaces::IElevationDatabase::QueryPtr elevation ( caller );
  Usul::Interfaces::IPlanetCoordinates::QueryPtr planet ( caller );

  // Make new extents.
  Extents e;
  
  // These points will be the ones that are worked on below.
  Vertex3Array::RefPtr sampledPoints ( data );
  
  // Resample the points if we should.
  if ( this->tessellate() && Geometry::CLAMP_TO_GROUND == this->altitudeMode() )
  {
    Vertices lineData ( data->size() ), sampled;
    
    // Copy in the data.
    {
      Vertex3Array::ReadLock guard ( data->mutex() );
      lineData.assign ( data->begin(), data->end() );
    }
    
    // Resampled the points.  This algorithm should be refactored to accept new Vector wrapper in Minerva.
    Minerva::Core::Algorithms::resample ( lineData, sampled, 5, caller );
    
    sampledPoints = new Vertex3Array ( sampled );
  }
  
  // Make the osg::Vec3Array.
  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
  vertices->reserve ( sampledPoints->size() );
  
  // Offset for all points to avoid floating point inacuracies.
  Usul::Math::Vec3d offset;
  
  if ( planet.valid() )
  {
    Vertex3Array::ReadLock guard ( sampledPoints->mutex() );
    
    Vertex temp ( *sampledPoints->begin() );
    
    // Get the height.
    temp[2] = this->_elevation ( temp, elevation.get() );
    
    // Convert to planet coordinates.
    planet->convertToPlanet ( temp, offset );
    
    for ( Vertices::const_iterator iter = sampledPoints->begin(); iter != sampledPoints->end(); ++iter )
    {
      Vertices::value_type v ( *iter );
      
      // Expand the extents.
      e.expand ( Extents::Vertex ( v[0], v[1] ) );
      
      // Get the height.
      v[2] = this->_elevation ( v, elevation.get() );
      
      Usul::Math::Vec3d point;
      planet->convertToPlanet ( v, point );
      
      // Apply the offset.
      point -= offset;
      
      vertices->push_back ( osg::Vec3 ( point[0], point[1], point[2] ) );
    }
  }
  
  // Create the geometry
  osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );

  // Add the primitive set
  geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINE_STRIP, 0, vertices->size() ) );

  // Set the vertices.
  geometry->setVertexArray ( vertices.get() );

  // Set the colors.
#if 0
  osg::ref_ptr < osg::Vec4Array > colors ( new osg::Vec4Array );
  colors->push_back ( osg::Vec4 ( color[0], color[1], color[2], color[3] ) );
  geometry->setColorArray( colors.get() );
  geometry->setColorBinding( osg::Geometry::BIND_OVERALL );
#else
  
  // Make an array of osg::Vec4ub.  This is to save space over a Vec4f.
  osg::Vec4ub c ( color[0] * 255, color[1] * 255, color[2] * 255, color[3] * 255 );
  osg::ref_ptr < osg::Vec4ubArray > colors ( new osg::Vec4ubArray ( vertices->size() ) );
  std::fill ( colors->begin(), colors->end(), c );
  geometry->setColorArray( colors.get() );
  geometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );
#endif

  geometry->setUseDisplayList ( false );

  // Make the geode
  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  
  // Add the geometry.
  geode->addDrawable ( geometry.get() );

  // Get the state set.
  osg::ref_ptr < osg::StateSet > ss ( geode->getOrCreateStateSet() );
  
  // Turn off lighting.
  OsgTools::State::StateSet::setLighting  ( ss.get(), false );
  OsgTools::State::StateSet::setLineWidth ( ss.get(), this->width() );

  // Set depth parameters.
  osg::ref_ptr<osg::Depth> depth ( new osg::Depth ( osg::Depth::LEQUAL, 0.0, 1.0, false ) );
  ss->setAttributeAndModes ( depth.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );

  ss->setRenderBinDetails ( this->renderBin(), "RenderBin" );

  this->extents ( e );

  // Make the matrix transform.
  osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
  mt->setMatrix ( osg::Matrixd::translate ( osg::Vec3d ( offset[0], offset[1], offset[2] ) ) );
  mt->addChild ( geode.get() );

  // Return the scene branch.
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


///////////////////////////////////////////////////////////////////////////////
//
//  Set the line style.
//
///////////////////////////////////////////////////////////////////////////////

void Line::lineStyle ( LineStyle * lineStyle )
{
  Guard guard ( this->mutex() );
  _lineStyle = lineStyle;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the line style.
//
///////////////////////////////////////////////////////////////////////////////

LineStyle* Line::lineStyle() const
{
  Guard guard ( this->mutex() );
  return _lineStyle.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this geometry transparent?
//
///////////////////////////////////////////////////////////////////////////////

bool Line::isSemiTransparent() const
{
  return 1.0f != this->lineColor()[3];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the line color.
//
///////////////////////////////////////////////////////////////////////////////

Line::Color Line::lineColor() const
{
  LineStyle::RefPtr lineStyle ( this->lineStyle() );
  return ( lineStyle.valid() ? lineStyle->color() : Usul::Math::Vec4f ( 1.0, 1.0, 1.0, 1.0 ) );
}
