
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
  _showBorder( false ),
  _showInterior ( true ),
  _borderColor ( 1.0, 1.0, 1.0, 1.0 )
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
  osg::Vec4f color ( Usul::Convert::Type<Color,osg::Vec4f>::convert ( this->color() ) );
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
//  Build a mesh.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Polygon::_buildPolygons( Usul::Interfaces::IUnknown* caller )
{  
  // Make new extents.
  Extents e;
  
  Vertices outerBoundary ( this->outerBoundary() );
  //Boundaries innerBoundaries ( polygon->innerBoundaries() );
  
  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  geode->addDrawable( this->_buildGeometry ( outerBoundary, e, caller ) );
  
  osg::ref_ptr < osg::StateSet > ss ( geode->getOrCreateStateSet () );
  
  // Set the render bin.
  //ss->setRenderBinDetails( this->renderBin(), "RenderBin" );
  
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
  if( false == this->transparent() )
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
//  Set the draw border flag.
//
///////////////////////////////////////////////////////////////////////////////

void Polygon::showBorder( bool b )
{
  Guard guard ( this );
  _showBorder = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the draw border flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Polygon::showBorder() const
{
  Guard guard ( this );
  return _showBorder;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the draw interior flag.
//
///////////////////////////////////////////////////////////////////////////////

void Polygon::showInterior( bool b )
{
  Guard guard ( this );
  _showInterior = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the draw interior flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Polygon::showInterior() const
{
  Guard guard ( this );
  return _showInterior;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the border color flag.
//
///////////////////////////////////////////////////////////////////////////////

void Polygon::borderColor ( const Color& color )
{
  Guard guard ( this );
  _borderColor = color;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the border color flag.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec4f Polygon::borderColor() const
{
  Guard guard ( this );
  return _borderColor;
}
