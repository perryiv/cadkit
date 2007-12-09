
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

#include "osg/MatrixTransform"

#include <limits>
#include <stdexcept>

using namespace StarSystem;

STAR_SYSTEM_IMPLEMENT_NODE_CLASS ( Body );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Body::Body ( LandModel *model, Usul::Jobs::Manager &manager, const MeshSize &ms, double splitDistance ) : BaseClass(),
  _transform ( new osg::MatrixTransform ),
  _landModel ( model ),
  _rasters ( new RasterGroup ),
  _manager ( manager ),
  _textureJobs (),
  _frame ( false ),
  _texturesPerFrame ( 0 ),
  _maxTexturesPerFrame ( 10 ),
  _maxLevel ( 50 ),
  _cacheTiles ( false ),
  _splitDistance ( splitDistance ),
  _meshSize ( ms )
{
  USUL_TRACE_SCOPE;

  // Must not be null.
  if ( false == _landModel.valid() )
    throw std::invalid_argument ( "Error 3062013877: null land model given to body" );

  // Not using smart pointers.
  _transform->ref();
  _rasters->ref();
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
  Usul::Pointers::unreference ( _rasters ); _rasters = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to dirty the tiles.
//
///////////////////////////////////////////////////////////////////////////////

namespace StarSystem
{
  namespace Helper
  {
    inline void dirtyTiles ( osg::Group *group, bool state, unsigned int flags, bool dirtyChildren, const Body::Extents& extents )
    {
      USUL_TRACE_SCOPE_STATIC;

      const unsigned int numChildren ( ( 0x0 == group ) ? 0 : group->getNumChildren() );
      for ( unsigned int i = 0; i < numChildren; ++i )
      {
        osg::ref_ptr<Tile> tile ( dynamic_cast < Tile * > ( group->getChild ( i ) ) );
        if ( true == tile.valid() )
        {
          tile->dirty ( state, flags, dirtyChildren, extents );
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a tile.
//
///////////////////////////////////////////////////////////////////////////////

void Body::addTile ( const Extents &extents )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Make the tile.
  const Usul::Math::Vec4d textureCoords ( 0.0, 1.0, 0.0, 1.0 );
  const MeshSize meshSize ( this->meshSize ( extents ) );
  osg::ref_ptr<Tile> tile ( new Tile ( 0, extents, meshSize, textureCoords, _splitDistance, this ) );

  // Add tile to the trnsform.
  _transform->addChild ( tile.get() );
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
//  Append raster data.
//
///////////////////////////////////////////////////////////////////////////////

void Body::rasterAppend ( RasterLayer * layer )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  if ( 0x0 != layer )
  {
    // Append the layer to the existing group.
    _rasters->append ( layer );

    // Dirty the tiles.
    Helper::dirtyTiles ( _transform, true, Tile::TEXTURE, true, layer->extents() );
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
  _landModel->latLonHeightToXYZ ( lat, lon, elevation, point );
}

 
///////////////////////////////////////////////////////////////////////////////
//
//  Computes the "geodetic" radius for a given latitude in degrees.
//
///////////////////////////////////////////////////////////////////////////////

double Body::geodeticRadius ( double latitude ) const
{
  USUL_TRACE_SCOPE;
  return _landModel->elevation ( 0, latitude );
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


///////////////////////////////////////////////////////////////////////////////
//
//  Return the mesh size for the extents.
//
///////////////////////////////////////////////////////////////////////////////

Body::MeshSize Body::meshSize ( const Body::Extents &extents )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return ( ( true == _landModel.valid() ) ? _landModel->meshSize ( extents, _meshSize ) : _meshSize );
}
