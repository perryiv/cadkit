
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

#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IElevationDatabase.h"
#include "Usul/Interfaces/IPlanetCoordinates.h"

#include "osg/BlendFunc"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Depth"
#include "osg/Hint"
#include "osg/Material"
#include "osg/MatrixTransform"
#include "osg/Version"

#include <limits>

using namespace Minerva::Core::Data;

USUL_IMPLEMENT_TYPE_ID ( Line );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Line::Line() : BaseClass(),
  _line(),
  _tessellate ( false ),
  _lineStyle ( 0x0 ),
  _useShader ( false )
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
  _line = data;
  this->dirty( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the line data.
//
///////////////////////////////////////////////////////////////////////////////

Line::Vertices Line::line() const
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

  // Get the line data.
  Vertices data ( this->line() );
  
  // Make sure there are at least 2 points.
  if ( data.size() < 2 )
    return 0x0;
  
  // Fit the line to the terrain if required.
  Vertices sampledPoints;
  if ( this->tessellate() && Geometry::CLAMP_TO_GROUND == this->altitudeMode() )
    Minerva::Core::Algorithms::resample( data, sampledPoints, 5, caller );
  else
    sampledPoints = data;
  
  // Query for needed interfaces.
  Usul::Interfaces::IElevationDatabase::QueryPtr elevation ( caller );
  Usul::Interfaces::IPlanetCoordinates::QueryPtr planet ( caller );

  // Store the converted values here.
  Vertices convertedPoints;
  convertedPoints.reserve ( sampledPoints.size() );
  
  // Make new extents.
  Extents e;
  
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
      convertedPoints.push_back ( point );
    }
  }
  else
  {
    convertedPoints.swap ( sampledPoints );
  }
  
  // Make the osg::Vec3Array.
  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
  vertices->reserve ( sampledPoints.size() );
  
  // Move all the points so the line starts at (0,0,0).
  Vertices::value_type offset ( convertedPoints.at ( 0 ) );
  for ( Vertices::const_iterator iter = convertedPoints.begin(); iter != convertedPoints.end(); ++iter )
  {
    Vertices::value_type v ( *iter );
    Vertices::value_type point ( v - offset );
    vertices->push_back ( osg::Vec3f ( point[0], point[1], point[2] ) );
  }

  // Create the geometry
  osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );

  // Add the primitive set
  geometry->addPrimitiveSet( new osg::DrawArrays ( osg::PrimitiveSet::LINE_STRIP, 0, vertices->size() ) );

  // Set the vertices.
  geometry->setVertexArray ( vertices.get() );

  // Display lists seems to be the fastest.
  geometry->setUseDisplayList ( true );
  geometry->setUseVertexBufferObjects ( false );

  if ( false == this->useShader() )
  {
    // Set the color.
    osg::ref_ptr < osg::Vec4Array > colors ( new osg::Vec4Array );
    colors->push_back ( osg::Vec4 ( color[0], color[1], color[2], color[3] ) );
    geometry->setColorArray ( colors.get() );
    geometry->setColorBinding ( osg::Geometry::BIND_OVERALL );
  }
    
  // Set our new extents.
  this->extents ( e );
  
  // Make the geode.
  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  geode->addDrawable ( geometry.get() );

  // Get the state set.
  osg::ref_ptr < osg::StateSet > ss ( geode->getOrCreateStateSet() );
  
  // Set needed state.
  this->_setState ( ss.get(), color );

  // Make the MatrixTransform.
  osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
  mt->setMatrix ( osg::Matrixd::translate ( osg::Vec3d ( offset[0], offset[1], offset[2] ) ) );
  mt->addChild ( geode.get() );
  
  return mt.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Simple shaders to work around problems in per-tile lighting issues.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  static const char* VERTEX_SHADER_SOURCE = 
  { 
    "void main(void)\n"
    "{\n"
    "   gl_Position = ftransform();\n"
    "}\n"
  };

  static const char* FRAGMENT_SHADER_SOURCE = 
  {
    "uniform vec4 Color;\n"
    "void main(void)\n"
    "{\n"
    "   gl_FragColor = vec4( Color );\n"
    "}\n"
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set proper state.
//
///////////////////////////////////////////////////////////////////////////////

void Line::_setState ( osg::StateSet* ss, const Color& color ) const
{
  // Return now if state isn't valid.
  if ( 0x0 == ss )
    return;
  
  // Turn off lighting.
  OsgTools::State::StateSet::setLighting  ( ss, false );
  OsgTools::State::StateSet::setLineWidth ( ss, this->width() );
  
  const osg::StateAttribute::GLModeValue on ( osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );
  const osg::StateAttribute::GLModeValue off ( osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF );

  if ( this->useShader() )
  {
    osg::ref_ptr< osg::Program > program ( new osg::Program ); 
    ss->setAttribute ( program.get() );
    ss->addUniform ( new osg::Uniform ( "Color", osg::Vec4 ( color[0], color[1], color[2], color[3] ) ) );

    program->addShader ( new osg::Shader ( osg::Shader::VERTEX, Detail::VERTEX_SHADER_SOURCE ) );
    program->addShader ( new osg::Shader ( osg::Shader::FRAGMENT, Detail::FRAGMENT_SHADER_SOURCE )  );
  }
  else
  {
    // Set the line parameters.
    ss->setMode ( GL_LINE_SMOOTH, on );
    ss->setMode ( GL_BLEND, on );
    
    // Add a blend function.
    osg::ref_ptr<osg::BlendFunc> blend ( new osg::BlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) );
    ss->setAttributeAndModes ( blend.get(), on );
    
    // Set the hint.
    osg::ref_ptr<osg::Hint> hint ( new osg::Hint ( GL_LINE_SMOOTH_HINT, GL_NICEST ) );
    ss->setAttributeAndModes ( hint.get(), on );
  }

  // Set the render bin.
  ss->setRenderBinDetails ( this->renderBin(), "RenderBin" );
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


///////////////////////////////////////////////////////////////////////////////
//
//  Call to notify of an intersection.
//
///////////////////////////////////////////////////////////////////////////////

void Line::intersectNotify ( double x, double y, double z, double lon, double lat, double elev, 
                             Unknown::RefPtr tile, Unknown::RefPtr body, Unknown::RefPtr caller, Closest &answer )
{
  typedef IIntersectNotify::Path Path;
  typedef IIntersectNotify::Point Point;
  typedef IIntersectNotify::PointAndDistance PointAndDistance;

  // Assuming that guarding is less expensive than copying.
  Guard guard ( this );

  // Initialize for the loop.
  Vertex lle ( lon, lat, elev );
  bool append ( false );

  // Loop through the vertices.
  for ( Vertices::const_iterator i = _line.begin(); i != _line.end(); ++i )
  {
    // Get the distance to the point.
    Vertex point ( *i );
    const double dist ( point.distanceSquared ( lle ) );
    if ( dist < answer.second.second )
    {
      answer.second.first.resize ( 3 );
      answer.second.first[0] = point[0];
      answer.second.first[1] = point[1];
      answer.second.first[2] = point[2];
      answer.second.second = dist;
      append = true;
    }
  }

  // Append us to the answer if we should.
  if ( true == append )
  {
    answer.first.push_back ( Usul::Interfaces::IUnknown::QueryPtr ( this ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get flag to use a shader.
//
///////////////////////////////////////////////////////////////////////////////

bool Line::useShader() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _useShader;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set flag to use a shader.
//
///////////////////////////////////////////////////////////////////////////////

void Line::useShader ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _useShader = b;
}
