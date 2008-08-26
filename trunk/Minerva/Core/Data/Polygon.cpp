
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Data/Polygon.h"

#include "OsgTools/Configure/OSG.h"
#include "OsgTools/Convert.h"
#include "OsgTools/DisplayLists.h"
#include "OsgTools/State/StateSet.h"
#include "OsgTools/Utilities/TranslateGeometry.h"

#include "Usul/Interfaces/IPlanetCoordinates.h"

#include "osg/Material"
#include "osg/MatrixTransform"
#include "osg/PolygonOffset"
#include "osg/Geode"
#include "osg/Geometry"

#include "osgUtil/Tessellator"
#include "osgUtil/SmoothingVisitor"

using namespace Minerva::Core::Data;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Polygon::Polygon ( ) :
  BaseClass(),
  _boundaries(),
  _polyStyle ( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Polygon::~Polygon()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the outer boundary.
//
///////////////////////////////////////////////////////////////////////////////

void Polygon::outerBoundary ( const Vertices& vertices )
{
  this->line ( vertices );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the outer boundary.
//
///////////////////////////////////////////////////////////////////////////////

const Polygon::Vertices& Polygon::outerBoundary() const
{
  return this->line();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add an inner boundary.
//
///////////////////////////////////////////////////////////////////////////////

void Polygon::addInnerBoundary ( const Vertices& vertices )
{
  Guard guard ( this->mutex() );
  _boundaries.push_back ( vertices );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the inner boundaries.
//
///////////////////////////////////////////////////////////////////////////////

const Polygon::Boundaries& Polygon::innerBoundaries() const
{
  Guard guard ( this->mutex() );
  return _boundaries;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Predicate for comparing vertices.  Should use CloseFloat for robustness?
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct VectorCompare
  {
    template < class T >
    bool operator() ( const T& lhs, const T& rhs ) const
    {
      return lhs.equal( rhs );
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build geometry from vertices.
//
///////////////////////////////////////////////////////////////////////////////

osg::Geometry* Polygon::_buildGeometry ( const Vertices& inVertices, Extents& e, Usul::Interfaces::IUnknown *caller )
{
  // Make sure we have vertices.
  if ( inVertices.empty() )
    return 0x0;
  
  Usul::Interfaces::IPlanetCoordinates::QueryPtr planet ( caller );
  Usul::Interfaces::IElevationDatabase::QueryPtr elevationDatabase ( caller );
  
  // Vertices and normals.
  osg::ref_ptr<osg::Vec3Array> vertices ( new osg::Vec3Array );
  osg::ref_ptr<osg::Vec3Array> normals  ( new osg::Vec3Array );
  osg::ref_ptr<osg::Vec4Array> colors  ( new osg::Vec4Array ( inVertices.size() ) );
  osg::Vec4f color ( Usul::Convert::Type<Color,osg::Vec4f>::convert ( this->fillColor() ) );
  std::fill ( colors->begin(), colors->end(), color );
  
  // Reserve enough rooms.
  vertices->reserve( inVertices.size() );
  normals->reserve( inVertices.size() );
  
  for ( Vertices::const_iterator iter = inVertices.begin(); iter != inVertices.end(); ++iter )
  {
    Vertices::value_type v0 ( *iter ), p0;
    
    // Expand the vertex.
    e.expand ( Extents::Vertex ( v0[0], v0[1] ) );
    
    if ( elevationDatabase.valid() )
      v0[2] = this->_elevation ( v0, elevationDatabase );
    
    if ( planet.valid() )
      planet->convertToPlanet ( v0, p0 );
    
    vertices->push_back ( osg::Vec3 ( p0[0], p0[1], p0[2] ) );
    
    Vertices::value_type n0 ( p0 ); n0.normalize();
    
    normals->push_back ( osg::Vec3 ( n0[0], n0[1], n0[2] ) );
  }
  
  osg::ref_ptr < osg::Geometry > geom ( new osg::Geometry );
  
  geom->setVertexArray ( vertices.get() );
  geom->setNormalArray ( normals.get() );
  geom->setNormalBinding ( osg::Geometry::BIND_PER_VERTEX );
  geom->setColorArray ( colors.get() );
  geom->setColorBinding ( osg::Geometry::BIND_PER_VERTEX );
  
  geom->addPrimitiveSet ( new osg::DrawArrays ( GL_POLYGON, 0, vertices->size() ) );
  
  // Make into triangles.
  osg::ref_ptr<osgUtil::Tessellator> tessellator ( new osgUtil::Tessellator );
  tessellator->retessellatePolygons ( *geom );
  
  // Make normals.
  osgUtil::SmoothingVisitor::smooth ( *geom );
  
  return geom.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert to planet coordinates.
//
///////////////////////////////////////////////////////////////////////////////

Polygon::Vertex Polygon::_convertToPlanetCoordinates ( const Polygon::Vertex& v, Usul::Interfaces::IPlanetCoordinates* planet, Usul::Interfaces::IElevationDatabase* elevation ) const
{
  Polygon::Vertex v0 ( v ), p0;
  
  if ( 0x0 != elevation )
    v0[2] = this->_elevation ( v0, elevation );
  
  if ( 0x0 != planet )
    planet->convertToPlanet ( v0, p0 );
  
  return p0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a quad from two points to the ground.
//
///////////////////////////////////////////////////////////////////////////////

osg::Geometry* Polygon::_extrudeToGround ( const Vertex& v0, const Vertex& v1, Usul::Interfaces::IUnknown *caller )
{
  Usul::Interfaces::IPlanetCoordinates::QueryPtr planet ( caller );
  Usul::Interfaces::IElevationDatabase::QueryPtr elevationDatabase ( caller );

  // Vertices and normals.
  osg::ref_ptr<osg::Vec3Array> vertices ( new osg::Vec3Array );
  osg::ref_ptr<osg::Vec3Array> normals  ( new osg::Vec3Array );
  osg::ref_ptr<osg::Vec4Array> colors  ( new osg::Vec4Array ( 4 ) );
  osg::Vec4f color ( Usul::Convert::Type<Color,osg::Vec4f>::convert ( this->fillColor() ) );
  std::fill ( colors->begin(), colors->end(), color );
  
  // Reserve enough rooms.
  vertices->reserve( 4 );
  normals->reserve( 4 );

  Vertex v2 ( v0 ); v2[2] = 0.0;
  Vertex v3 ( v1 ); v3[2] = 0.0;
  
  Vertex p0 ( this->_convertToPlanetCoordinates ( v0, planet, elevationDatabase ) );
  Vertex p1 ( this->_convertToPlanetCoordinates ( v1, planet, elevationDatabase ) );
  Vertex p2 ( this->_convertToPlanetCoordinates ( v2, planet, elevationDatabase ) );
  Vertex p3 ( this->_convertToPlanetCoordinates ( v3, planet, elevationDatabase ) );
  
  vertices->push_back ( osg::Vec3 ( p0[0], p0[1], p0[2] ) );
  vertices->push_back ( osg::Vec3 ( p2[0], p2[1], p2[2] ) );
  vertices->push_back ( osg::Vec3 ( p3[0], p3[1], p3[2] ) );
  vertices->push_back ( osg::Vec3 ( p1[0], p1[1], p1[2] ) );

  p0.normalize();
  p1.normalize();
  p2.normalize();
  p3.normalize();

  normals->push_back ( osg::Vec3 ( p0[0], p0[1], p0[2] ) );
  normals->push_back ( osg::Vec3 ( p2[0], p2[1], p2[2] ) );
  normals->push_back ( osg::Vec3 ( p3[0], p3[1], p3[2] ) );
  normals->push_back ( osg::Vec3 ( p1[0], p1[1], p1[2] ) );
  
  osg::ref_ptr < osg::Geometry > geom ( new osg::Geometry );
  
  geom->setVertexArray ( vertices.get() );
  geom->setNormalArray ( normals.get() );
  geom->setNormalBinding ( osg::Geometry::BIND_PER_VERTEX );
  geom->setColorArray ( colors.get() );
  geom->setColorBinding ( osg::Geometry::BIND_PER_VERTEX );
  
  geom->addPrimitiveSet ( new osg::DrawArrays ( GL_QUADS, 0, vertices->size() ) );
    
  // Make normals.
  osgUtil::SmoothingVisitor::smooth ( *geom );
  
  return geom.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a mesh.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Polygon::_buildPolygons( Usul::Interfaces::IUnknown* caller )
{  
  // Make new extents.
  Extents e;
  
  // Get the outer boundary.
  Vertices outerBoundary ( this->outerBoundary() );

  // Need at least 3 points to make a polygon.
  if ( outerBoundary.size() < 3 )
    return 0x0;

  // TODO: Handle inner boundaries.
  //Boundaries innerBoundaries ( polygon->innerBoundaries() );
  
  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  geode->addDrawable( this->_buildGeometry ( outerBoundary, e, caller ) );
  
  osg::ref_ptr < osg::StateSet > ss ( geode->getOrCreateStateSet () );
  
  // Make a polygon offset.
  osg::ref_ptr< osg::PolygonOffset > po ( new osg::PolygonOffset( 1.0f, 4.0f ) );
  ss->setMode ( GL_POLYGON_OFFSET_FILL, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
  ss->setAttribute( po.get(), osg::StateAttribute::ON | osg::StateAttribute::PROTECTED );
  
  // Turn off lighting.
  OsgTools::State::StateSet::setLighting  ( ss.get(), false );
  
  osg::Vec3 offset ( geode->getBound().center() );
  osg::ref_ptr<OsgTools::Utilities::TranslateGeometry> tg ( new OsgTools::Utilities::TranslateGeometry ( offset ) );
  tg->apply ( *geode );

  osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
  mt->setMatrix ( osg::Matrix::translate ( offset ) );
  mt->addChild ( geode.get() );

  // Extrude if we are suppose to.
  if ( true == this->extrude() )
  {
    for ( Vertices::const_iterator iter = outerBoundary.begin(); iter != outerBoundary.end() - 1; ++iter )
    {
      Vertex v0 ( *iter );
      Vertex v1 ( *(iter + 1 ) );

      osg::ref_ptr < osg::Geode > g ( new osg::Geode );
      g->addDrawable( this->_extrudeToGround ( v0, v1, caller ) );
      tg->apply ( *g );

      mt->addChild ( g.get() );
    }
  }
  
  this->extents ( e );
  
  return mt.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pre build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Polygon::_buildScene ( Usul::Interfaces::IUnknown* caller )
{
  // Clear what we have by making a new one.
  osg::ref_ptr < osg::Group > group ( new osg::Group );
  
  // Get the state set.
  osg::ref_ptr < osg::StateSet > ss ( group->getOrCreateStateSet () );
  
  if( this->showInterior() )
  {
    group->addChild( this->_buildPolygons( caller ) );
    
    // Turn off depth testing if we are clamping to ground.
    // This could probably be done better.
    if ( Geometry::CLAMP_TO_GROUND == this->altitudeMode() )
    {
      ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE );
    }
  }
  
  if( this->showBorder() )
  {
    group->addChild( BaseClass::_buildScene( this->borderColor(), caller ) );
  }
  
  // Set state set modes depending on alpha value.
  if( false == this->isSemiTransparent() )
  {
    // Set the render bin.
    ss->setRenderBinDetails( this->renderBin(), "RenderBin" );
  }
  
  // Turn off display lists.
  OsgTools::DisplayLists displayLists ( false );
  displayLists ( group.get() );
  
  this->dirty( false );
  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the draw border flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Polygon::showBorder() const
{
  PolyStyle::RefPtr style ( this->polyStyle() );
  return ( style.valid() ? style->outline() : true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the draw interior flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Polygon::showInterior() const
{
  PolyStyle::RefPtr style ( this->polyStyle() );
  return ( style.valid() ? style->fill() : true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the border color flag.
//
///////////////////////////////////////////////////////////////////////////////

PolyStyle::Color Polygon::borderColor() const
{
  return this->lineColor();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the PolyStyle.
//
///////////////////////////////////////////////////////////////////////////////

void Polygon::polyStyle ( PolyStyle * polyStyle )
{
  Guard guard ( this );
  _polyStyle = polyStyle;
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the PolyStyle.
//
///////////////////////////////////////////////////////////////////////////////

PolyStyle* Polygon::polyStyle() const
{
  Guard guard ( this );
  return _polyStyle.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the fill color flag.
//
///////////////////////////////////////////////////////////////////////////////

PolyStyle::Color Polygon::fillColor() const
{
  PolyStyle::RefPtr style ( this->polyStyle() );
  return ( style.valid() ? style->color() : Usul::Math::Vec4f ( 0.8, 0.8, 0.8, 1.0 ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is this geometry transparent?
//
///////////////////////////////////////////////////////////////////////////////

bool Polygon::isSemiTransparent() const
{
  return 1.0 != this->fillColor()[3];
}

