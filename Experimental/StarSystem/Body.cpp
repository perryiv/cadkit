
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Body class.
//
///////////////////////////////////////////////////////////////////////////////

#include "StarSystem/Body.h"
#include "StarSystem/Tile.h"
#include "StarSystem/Visitor.h"
#include "StarSystem/RasterGroup.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Math/MinMax.h"
#include "Usul/Trace/Trace.h"

#include "OsgTools/ShapeFactory.h"
#include "OsgTools/Group.h"

#include "ossim/base/ossimEllipsoid.h"

#include "osg/MatrixTransform"

#include <limits>

using namespace StarSystem;

STAR_SYSTEM_IMPLEMENT_NODE_CLASS ( Body );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Body::Body ( const Vec2d &r, Usul::Jobs::Manager& manager ) : BaseClass(),
  _transform ( new osg::MatrixTransform ),
  _ellipsoid ( new ossimEllipsoid ( r[Body::RADIUS_EQUATOR], r[Body::RADIUS_POLAR] ) ),
  _tile ( 0x0 ),
  _rasters ( new RasterGroup ),
  _manager ( manager ),
  _textureJobs (),
  _frame ( false ),
  _texturesPerFrame ( 0 ),
  _maxTexturesPerFrame ( 10 ),
  _maxLevel ( 50 ),
  _cacheTiles ( false )
{
  USUL_TRACE_SCOPE;

  // Not using smart pointers.
  _transform->ref();
  _rasters->ref();

  // Set ellipsoid radii.
  _ellipsoid->setAB ( r[Body::RADIUS_EQUATOR], r[Body::RADIUS_POLAR] );

  // Properties of the tile.
#if 1
  const osg::Vec2d mn ( -115, 31 );
  const osg::Vec2d mx ( -109, 37 );
#else
  const osg::Vec2d mn ( -180, -90 );
  const osg::Vec2d mx (    0,  90 );
#endif

  // Calculate split distance.
  const double lonRange ( mx[0] - mn[0] );
  const double latRange ( mx[1] - mn[1] );
  const double maxRange ( Usul::Math::maximum ( lonRange, latRange ) );
  const double multiplier ( 7 );
  const double splitDistance ( _ellipsoid->a() * multiplier * ( maxRange / 180 ) );

  // Make the tile and add it to the transform.
  _tile = new Tile ( 0, Tile::Extents ( mn, mx ), Tile::MeshSize ( 17, 17 ), Usul::Math::Vec4d ( 0.0, 1.0, 0.0, 1.0 ), splitDistance, this );
  _tile->ref();
  _transform->addChild ( _tile );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Body::~Body()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Body::_destroy ), "3973302267" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy
//
///////////////////////////////////////////////////////////////////////////////

void Body::_destroy()
{
  USUL_TRACE_SCOPE;

  Usul::Pointers::unreference ( _transform ); _transform = 0x0;
  Usul::Pointers::unreference ( _tile ); _tile = 0x0;
  Usul::Pointers::unreference ( _rasters ); _rasters = 0x0;
  delete _ellipsoid; _ellipsoid = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Node *Body::scene() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _transform;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *Body::scene()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _transform;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the center.
//
///////////////////////////////////////////////////////////////////////////////

void Body::center ( const Vec3d &c )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _transform->setMatrix ( osg::Matrix::translate ( c[0], c[1], c[2] ) );
  _tile->dirty();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the center.
//
///////////////////////////////////////////////////////////////////////////////

Body::Vec3d Body::center() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  const osg::Vec3d c ( _transform->getMatrix().getTrans() );
  return Vec3d ( c[0], c[1], c[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the maximum radius.
//
///////////////////////////////////////////////////////////////////////////////

double Body::maxRadius() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return ( ( 0x0 == _ellipsoid ) ? 1 : Usul::Math::maximum ( _ellipsoid->a(), _ellipsoid->b() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the radii.
//
///////////////////////////////////////////////////////////////////////////////

Body::Vec2d Body::radii() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return ( ( 0x0 == _ellipsoid ) ? Vec2d ( 1, 1 ) : Vec2d ( _ellipsoid->a(), _ellipsoid->b() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the radii.
//
///////////////////////////////////////////////////////////////////////////////

void Body::radii ( const Body::Vec2d &r )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _ellipsoid->setA ( r[Body::RADIUS_EQUATOR] );
  _ellipsoid->setB ( r[Body::RADIUS_POLAR] );
  _tile->dirty();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Append raster data.
//
///////////////////////////////////////////////////////////////////////////////

void Body::rasterAppend ( RasterLayer * layer )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( 0x0 != layer )
  {
    _rasters->append ( layer );
    _tile->dirty( true, Tile::TEXTURE, true, layer->extents() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert lat, lon, height to x,y,z.
//
///////////////////////////////////////////////////////////////////////////////

void Body::latLonHeightToXYZ ( double lat, double lon, double elevation, osg::Vec3f& point ) const
{
  USUL_TRACE_SCOPE;

  typedef osg::Vec3f::value_type ValueType;

  double x ( 0 ), y ( 0 ), z ( 0 );
  _ellipsoid->latLonHeightToXYZ ( lat, lon, elevation, x, y, z );
  point.set ( static_cast<ValueType> ( x ), static_cast<ValueType> ( y ), static_cast<ValueType> ( z ) );
}

 
///////////////////////////////////////////////////////////////////////////////
//
//  Computes the "geodetic" radius for a given latitude in degrees.
//
///////////////////////////////////////////////////////////////////////////////

double Body::geodeticRadius( double latitude ) const
{
  USUL_TRACE_SCOPE;

  return _ellipsoid->geodeticRadius ( latitude );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the thread pool for this body.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Jobs::Manager& Body::jobManager()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _manager;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification that a renderer just rendered.
//
///////////////////////////////////////////////////////////////////////////////

void Body::preRender ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  BaseClass::preRender ( caller );

  Guard guard ( this );
  _frame = true;
  _texturesPerFrame = 0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Notification that a renderer is about to render.
//
///////////////////////////////////////////////////////////////////////////////

void Body::postRender ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  BaseClass::postRender ( caller );

  Guard guard ( this );
  _frame = false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Request texture.
//
///////////////////////////////////////////////////////////////////////////////

int Body::textureRequest ( const Extents &extents, unsigned int level )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  CutImageJob::RefPtr job ( new CutImageJob ( extents, level, _rasters ) );
  _manager.addJob ( job );
  _textureJobs.insert ( TextureJobs::value_type ( job->id(), job ) );
  return job->id();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Cancel request.
//
///////////////////////////////////////////////////////////////////////////////

void Body::textureRequestCancel ( int id )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  TextureJobs::iterator iter = _textureJobs.find ( id );

  if ( iter != _textureJobs.end() )
  {
    CutImageJob::RefPtr job ( iter->second );
    _manager.cancel ( job );
    _textureJobs.erase ( iter );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the texture.
//
///////////////////////////////////////////////////////////////////////////////

osg::Texture2D* Body::texture ( int id )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  TextureJobs::iterator iter = _textureJobs.find ( id );
  osg::ref_ptr < osg::Texture2D > texture ( 0x0 );

  if ( iter != _textureJobs.end() && _frame && _texturesPerFrame < _maxTexturesPerFrame )
  {
    CutImageJob::RefPtr job ( iter->second );
    if ( job->isDone() && false == job->canceled() )
    {
      texture = job->texture();
      _textureJobs.erase ( iter );
      _texturesPerFrame++;
    }
  }

  return texture.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the maximum level.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Body::maxLevel() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _maxLevel;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the maximum level.
//
///////////////////////////////////////////////////////////////////////////////

void Body::maxLevel ( unsigned int level )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _maxLevel = level;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the flag for caching tiles.
//
///////////////////////////////////////////////////////////////////////////////

bool Body::cacheTiles() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _cacheTiles;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flag for caching tiles.
//
///////////////////////////////////////////////////////////////////////////////

void Body::cacheTiles ( bool state )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _cacheTiles = state;
}
