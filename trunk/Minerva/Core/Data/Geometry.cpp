
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Data/Geometry.h"

#include "Usul/Trace/Trace.h"

#include "osg/Node"
#include "osg/StateSet"

using namespace Minerva::Core::Data;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS( Geometry, Geometry::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Geometry::Geometry() : 
  BaseClass(),
  _altitudeMode ( CLAMP_TO_GROUND ),
  _color ( 0.0, 0.0, 0.0, 1.0 ),
  _offset( 0.0, 0.0, 0.0 ),
  _dirty ( false ),
  _extrude ( false ),
  _renderBin ( osg::StateSet::DEFAULT_BIN ),
  _extents()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Geometry::~Geometry()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* Geometry::queryInterface( unsigned long iid )
{
  switch ( iid )
  {
    case Usul::Interfaces::IUnknown::IID:
    case Usul::Interfaces::IBuildScene::IID:
      return static_cast<Usul::Interfaces::IBuildScene*> ( this );
    case Usul::Interfaces::ILayerExtents::IID:
      return static_cast<Usul::Interfaces::ILayerExtents*> ( this );
    default:
      return 0x0;
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the altitude mode.
//
///////////////////////////////////////////////////////////////////////////////

void Geometry::altitudeMode ( AltitudeMode mode )
{
  Guard guard ( this );
  _altitudeMode = mode;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the altitude mode.
//
///////////////////////////////////////////////////////////////////////////////

Geometry::AltitudeMode Geometry::altitudeMode () const
{
  Guard guard ( this );
  return _altitudeMode;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene branch.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Geometry::buildScene( const Options& options, Usul::Interfaces::IUnknown* caller )
{
  osg::ref_ptr<osg::Node> node ( this->_buildScene ( caller ) );
  
  if ( node.valid() )
  {
    osg::ref_ptr < osg::StateSet > ss ( node->getOrCreateStateSet () );
    
    // Set the render bin.
    ss->setRenderBinDetails( this->renderBin(), "RenderBin" );
    
    const unsigned int on ( osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );
    const unsigned int off ( osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE );
    
    const unsigned int blendMode ( this->transparent() ? on : off );
    
    ss->setMode ( GL_BLEND, blendMode );
    
    // Set render bin depending on alpha value.
    if( true == this->transparent() )
    {
      ss->setRenderingHint ( osg::StateSet::TRANSPARENT_BIN );
      ss->setRenderBinDetails ( osg::StateSet::TRANSPARENT_BIN, "DepthSortedBin" );
    }
  }
  
  this->dirty( false );
  
  return node.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the color.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec4f Geometry::color() const
{
  Guard guard ( this );
  return _color;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the color.
//
///////////////////////////////////////////////////////////////////////////////

void Geometry::color ( const Color& color )
{
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
//  Get the render bin.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Geometry::renderBin() const
{
  Guard guard ( this );
  return _renderBin;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set extrude flag.
//
///////////////////////////////////////////////////////////////////////////////

void Geometry::extrude ( bool b )
{
  Guard guard ( this );
  _extrude = b;
  this->dirty ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get extrude flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Geometry::extrude() const
{
  Guard guard ( this );
  return _extrude;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the extents.
//
///////////////////////////////////////////////////////////////////////////////

void Geometry::extents ( const Extents& e )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _extents = e;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the extents.
//
///////////////////////////////////////////////////////////////////////////////

Geometry::Extents Geometry::extents() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  return _extents;
}


///////////////////////////////////////////////////////////////////////////////
//
//   Get the min longitude (ILayerExtents).
//
///////////////////////////////////////////////////////////////////////////////

double Geometry::minLon() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _extents.minLon();
}


///////////////////////////////////////////////////////////////////////////////
//
//   Get the min latitude (ILayerExtents).
//
///////////////////////////////////////////////////////////////////////////////

double Geometry::minLat() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _extents.minLat();
}


///////////////////////////////////////////////////////////////////////////////
//
//   Get the max longitude (ILayerExtents).
//
///////////////////////////////////////////////////////////////////////////////

double Geometry::maxLon() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _extents.maxLon();
}


///////////////////////////////////////////////////////////////////////////////
//
//   Get the max latitude (ILayerExtents).
//
///////////////////////////////////////////////////////////////////////////////

double Geometry::maxLat() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _extents.maxLat();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the render bin.
//
///////////////////////////////////////////////////////////////////////////////

void Geometry::renderBin( unsigned int renderBin )
{
  Guard guard ( this );
  
  // Only change it if it's different.
  if ( renderBin != _renderBin )
  {
    _renderBin = renderBin;
    this->dirty( true );
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Is this data object transparent?
//
///////////////////////////////////////////////////////////////////////////////

bool Geometry::transparent() const
{
  return 1.0f != this->color()[3];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the offset.
//
///////////////////////////////////////////////////////////////////////////////

void Geometry::spatialOffset( const Point& value )
{
  Guard guard ( this->mutex() );
  if( false == _offset.equal ( value ) )
  {
    _offset = value;
    this->dirty( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the offset.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec3d Geometry::spatialOffset( ) const
{
  Guard guard ( this->mutex() );
  return _offset;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

void Geometry::dirty ( bool b )
{
  Guard guard ( this->mutex() );
  _dirty = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the dirty flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Geometry::dirty () const
{
  Guard guard ( this->mutex() );
  return _dirty;
}
